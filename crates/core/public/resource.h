#pragma once

#include "avalanche_core_export.h"
#include <cstdint>
#include <compare>  // for the <=> compare
#include <atomic>

namespace avalanche::core {

    enum class ResourceType : uint8_t {
        Invalid = 0,
        Rendering,
        Geometry,
        Other,
    };

    class AVALANCHE_CORE_API ResourceHandle {
    public:
        using size_type = uint64_t;

        ResourceHandle(nullptr_t = nullptr);
        ResourceHandle(const ResourceHandle &);
        ResourceHandle(ResourceHandle &&) AVALANCHE_NOEXCEPT;
        ResourceHandle& operator=(const ResourceHandle &);
        ResourceHandle& operator=(ResourceHandle &&) AVALANCHE_NOEXCEPT;

        void swap(ResourceHandle& other) AVALANCHE_NOEXCEPT;
        AVALANCHE_NO_DISCARD ResourceType type() const AVALANCHE_NOEXCEPT;
        AVALANCHE_NO_DISCARD size_type operator*() const AVALANCHE_NOEXCEPT;

    private:
        ResourceHandle(ResourceType resource_type, size_type value);

        /**
         * High 0 - 7 bits is resource type
         * Lower 0 - 47 bits is an atomic increase
         */
        size_type m_resource_identifier;

        template <ResourceType ResourceType>
        friend class ResourceHandleAllocator;
    };

    template <ResourceType TyRes>
    class ResourceHandleAllocator {
    private:
        std::atomic<ResourceHandle::size_type> m_current_atomic = 0;

    public:
        ResourceHandle allocate_resource_handle() AVALANCHE_NOEXCEPT {
            return { TyRes, m_current_atomic++ };
        }

        ResourceHandle allocate_resource_handle(ResourceType resource_type) AVALANCHE_NOEXCEPT {
            return { resource_type, m_current_atomic++ };
        }
    };

    AVALANCHE_CORE_API std::strong_ordering operator<=>(const ResourceHandle& lhs, const ResourceHandle& rhs) AVALANCHE_NOEXCEPT;

}
