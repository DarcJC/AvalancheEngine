#pragma once

#include "avalanche_core_export.h"
#include "polyfill.h"
#include "delegate.h"
#include <cstdint>
#include <compare>  // for the <=> compare
#include <atomic>
#include <functional>

namespace avalanche::core {

    template <typename>
    class ResourceHandle final {
    public:
        AVALANCHE_NO_DISCARD static ResourceHandle new_handle(bool trigger_delegate = true) {
            static std::atomic<uint64_t> counter{1};
            return ResourceHandle{counter.fetch_add(1, std::memory_order_acq_rel), trigger_delegate};
        }

        AVALANCHE_NO_DISCARD static ResourceHandle null_handle() {
            return ResourceHandle{0, false};
        }

        static multicast_delegate<const ResourceHandle&> get_create_observer() {
            static multicast_delegate<const ResourceHandle&> HandleCreateDelegate;
            return HandleCreateDelegate;
        }

        static multicast_delegate<const ResourceHandle&> get_free_observer() {
            static multicast_delegate<const ResourceHandle&> HandleFreeDelegate;
            return HandleFreeDelegate;
        }

        ResourceHandle() : ResourceHandle(null_handle()) {}

        ResourceHandle(const ResourceHandle& other) AVALANCHE_NOEXCEPT
            : m_value(other.m_value)
        {
            get_create_observer().invoke(*this);
        }

        ResourceHandle& operator=(const ResourceHandle& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                if (this->m_value != other.m_value) {
                    reset();
                    m_value = other.m_value;
                    get_create_observer().invoke(*this);
                }
            }
            return *this;
        }

        ResourceHandle(ResourceHandle&& other) AVALANCHE_NOEXCEPT
            : m_value(std::exchange(other.m_value, 0))
        {}
        ResourceHandle& operator=(ResourceHandle&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                reset();
                m_value = other.m_value;
            }
            return *this;
        }

        ~ResourceHandle() {
            reset();
        }

        AVALANCHE_NO_DISCARD uint64_t raw_value() const {
            return m_value;
        }

        bool operator==(const ResourceHandle& other) const {
            return other.m_value == m_value;
        }
        bool operator!=(const ResourceHandle& other) const {
            return !(other == *this);
        }

        AVALANCHE_NO_DISCARD bool is_valid() const {
            return m_value != 0;
        }
        explicit operator bool() const AVALANCHE_NOEXCEPT {
            return is_valid();
        }

        void reset() {
            if (is_valid()) {
                get_free_observer().invoke(*this);
            }
            m_value = 0;
        }

    private:
        explicit ResourceHandle(uint64_t in_value, bool trigger_delegate = true)
            : m_value(in_value) {
            if (trigger_delegate) AVALANCHE_LIKELY_BRANCH {
                get_create_observer().invoke(*this);
            }
        }

        uint64_t m_value = 0;
    };
}

#define DEFINE_RESOURCE_HANDLE(NAME, DLL_EXPORT) struct resource_handle_tag_##NAME {}; template class DLL_EXPORT avalanche::core::ResourceHandle<resource_handle_tag_##NAME>; \
    template<> struct std::hash<avalanche::core::ResourceHandle<resource_handle_tag_##NAME>> {\
        std::size_t operator()(const avalanche::core::ResourceHandle<resource_handle_tag_##NAME> &handle) const AVALANCHE_NOEXCEPT {\
            return std::hash<uint64_t>{}(handle.raw_value());\
        }\
    };
