#include "resource.h"

namespace avalanche::core {

    static inline multicast_delegate<const ResourceHandle&> HandleCreateDelegate;
    static inline multicast_delegate<const ResourceHandle&> HandleFreeDelegate;

    ResourceHandle ResourceHandle::new_handle(const bool trigger_delegate/* = true*/) {
        static std::atomic<uint64_t> counter{1};
        return ResourceHandle{counter.fetch_add(1, std::memory_order_acq_rel), trigger_delegate};
    }

    ResourceHandle ResourceHandle::null_handle() { return ResourceHandle{0}; }

    ResourceHandle::ResourceHandle(const ResourceHandle &other) AVALANCHE_NOEXCEPT
        : m_value(other.m_value)
    {
        HandleCreateDelegate.invoke(*this);
    }

    ResourceHandle& ResourceHandle::operator=(const ResourceHandle &other) AVALANCHE_NOEXCEPT {
        if (this != &other) {
            if (this->m_value != other.m_value) {
                reset();
                m_value = other.m_value;
                HandleCreateDelegate.invoke(*this);
            }
        }
        return *this;
    }

    ResourceHandle::ResourceHandle(ResourceHandle&& other) AVALANCHE_NOEXCEPT
        : m_value(std::exchange(other.m_value, 0))
    {}

    ResourceHandle& ResourceHandle::operator=(ResourceHandle&& other) AVALANCHE_NOEXCEPT {
        if (this != &other) {
            reset();
            m_value = other.m_value;
            HandleCreateDelegate.invoke(*this);
        }
        return *this;
    }

    ResourceHandle::~ResourceHandle() {
        reset();
    }

    uint64_t ResourceHandle::raw_value() const { return m_value; }

    bool ResourceHandle::operator==(const ResourceHandle &other) const { return other.m_value == m_value; }

    bool ResourceHandle::operator!=(const ResourceHandle &other) const { return !(other == *this); }

    bool ResourceHandle::is_valid() const { return m_value != 0; }

    ResourceHandle::operator bool() const AVALANCHE_NOEXCEPT { return is_valid(); }

    void ResourceHandle::reset() {
        if (is_valid()) {
            HandleFreeDelegate.invoke(*this);
        }
        m_value = 0;
    }

    ResourceHandle::ResourceHandle(const uint64_t in_value, const bool trigger_delegate/* = true*/)
        : m_value(in_value) {
        if (trigger_delegate) AVALANCHE_LIKELY_BRANCH {
            HandleCreateDelegate.invoke(*this);
        }
    }

}

