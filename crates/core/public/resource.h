#pragma once

#include "avalanche_core_export.h"
#include "polyfill.h"
#include <cstdint>
#include <compare>  // for the <=> compare
#include <atomic>
#include <functional>

namespace avalanche::core {
    class AVALANCHE_CORE_API ResourceHandle {
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
}


template <>
struct std::hash<avalanche::core::handle_t> {
    std::size_t operator()(const avalanche::core::handle_t& handle) const AVALANCHE_NOEXCEPT {
        return std::hash<uint64_t>{}(handle.raw_value());
    }
};
