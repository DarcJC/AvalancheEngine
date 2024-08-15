#include "resource.h"
#include <utility>

namespace avalanche::core {

    ResourceHandle ResourceHandle::new_handle() {
        static std::atomic<uint64_t> counter{1};
        return ResourceHandle{counter.fetch_add(1, std::memory_order_acq_rel)};
    }
    ResourceHandle ResourceHandle::null_handle() { return ResourceHandle{0}; }

    ResourceHandle::ResourceHandle(const ResourceHandle &other) = default;
    ResourceHandle &ResourceHandle::operator=(const ResourceHandle &other) = default;

    uint64_t ResourceHandle::raw_value() const { return m_value; }

    bool ResourceHandle::operator==(const ResourceHandle &other) const { return other.m_value == m_value; }

    bool ResourceHandle::operator!=(const ResourceHandle &other) const {
        return !(other == *this);
    }

    ResourceHandle::ResourceHandle(const uint64_t in_value)
        : m_value(in_value)
    {}

}

