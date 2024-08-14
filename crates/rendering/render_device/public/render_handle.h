#pragma once


#include <cstdint>
#include "polyfill.h"


namespace avalanche::rendering {
    class ResourceHandle {
    public:
        AVALANCHE_NO_DISCARD static ResourceHandle new_handle();
        AVALANCHE_NO_DISCARD static ResourceHandle null_handle();

        ResourceHandle(const ResourceHandle& other);
        ResourceHandle& operator=(const ResourceHandle& other);

        ResourceHandle(ResourceHandle&&) = delete;
        ResourceHandle& operator=(ResourceHandle&& other) = delete;

        AVALANCHE_NO_DISCARD uint64_t raw_value() const;

        bool operator==(const ResourceHandle& other) const;
        bool operator!=(const ResourceHandle& other) const;

    private:
        explicit ResourceHandle(uint64_t in_value);

        uint64_t m_value = 0;
    };

    using handle_t = ResourceHandle;
} // namespace avalanche::rendering
