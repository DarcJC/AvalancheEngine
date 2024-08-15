#include "resource.h"
#include <utility>

namespace avalanche::core {

    static inline multicast_delegate<const ResourceHandle&> HandleCreateDelegate;
    static inline multicast_delegate<const ResourceHandle&> HandleFreeDelegate;

    ResourceHandle ResourceHandle::new_handle() {
        static std::atomic<uint64_t> counter{1};
        return ResourceHandle{counter.fetch_add(1, std::memory_order_acq_rel)};
    }
    ResourceHandle ResourceHandle::null_handle() { return ResourceHandle{0}; }

    ResourceHandle::ResourceHandle(const ResourceHandle &other) AVALANCHE_NOEXCEPT = default;
    ResourceHandle& ResourceHandle::operator=(const ResourceHandle &other) AVALANCHE_NOEXCEPT = default;

    ResourceHandle::ResourceHandle(ResourceHandle&& other) AVALANCHE_NOEXCEPT : ResourceHandle(static_cast<const ResourceHandle&>(other)) {}
    ResourceHandle& ResourceHandle::operator=(ResourceHandle&& other) AVALANCHE_NOEXCEPT {
        *this = static_cast<const ResourceHandle&>(other);
        return *this;
    }

    ResourceHandle::~ResourceHandle() {
        HandleFreeDelegate.invoke(*this);
    }

    uint64_t ResourceHandle::raw_value() const { return m_value; }

    bool ResourceHandle::operator==(const ResourceHandle &other) const { return other.m_value == m_value; }

    bool ResourceHandle::operator!=(const ResourceHandle &other) const {
        return !(other == *this);
    }

    ResourceHandle::ResourceHandle(const uint64_t in_value)
        : m_value(in_value) {
        HandleCreateDelegate.invoke(*this);
    }

}

