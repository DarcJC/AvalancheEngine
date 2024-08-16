#pragma once

#include "avalanche_core_export.h"
#include "polyfill.h"
#include "delegate.h"
#include <cstdint>
#include <compare>  // for the <=> compare
#include <atomic>
#include <functional>

namespace avalanche::core {

    class AVALANCHE_CORE_API ResourceHandle {
    public:
        AVALANCHE_NO_DISCARD static ResourceHandle new_handle(bool trigger_delegate = true);
        AVALANCHE_NO_DISCARD static ResourceHandle null_handle();

        ResourceHandle(const ResourceHandle& other) AVALANCHE_NOEXCEPT;
        ResourceHandle& operator=(const ResourceHandle& other) AVALANCHE_NOEXCEPT;

        ResourceHandle(ResourceHandle&&) AVALANCHE_NOEXCEPT;
        ResourceHandle& operator=(ResourceHandle&& other) AVALANCHE_NOEXCEPT;

        ~ResourceHandle();

        AVALANCHE_NO_DISCARD uint64_t raw_value() const;

        bool operator==(const ResourceHandle& other) const;
        bool operator!=(const ResourceHandle& other) const;

        AVALANCHE_NO_DISCARD bool is_valid() const;
        explicit operator bool() const AVALANCHE_NOEXCEPT ;

        void reset();

        /**
         * @brief Forbid new operator as I don't want a vtable.
         */
        static void* operator new(size_t) = delete;

    private:
        explicit ResourceHandle(uint64_t in_value, bool trigger_delegate = true);

        uint64_t m_value = 0;
    };

    AVALANCHE_CORE_API extern multicast_delegate<const ResourceHandle&> HandleCreateDelegate;
    AVALANCHE_CORE_API extern multicast_delegate<const ResourceHandle&> HandleFreeDelegate;

    using handle_t = ResourceHandle;
}


template <>
struct std::hash<avalanche::core::handle_t> {
    std::size_t operator()(const avalanche::core::handle_t& handle) const AVALANCHE_NOEXCEPT {
        return std::hash<uint64_t>{}(handle.raw_value());
    }
};
