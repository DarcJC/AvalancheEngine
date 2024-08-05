#include "resource.h"
#include <utility>

namespace avalanche::core {

    ResourceHandle::ResourceHandle(nullptr_t) : m_resource_identifier(0) {
    }

    ResourceHandle::ResourceHandle(const ResourceHandle &) = default;

    ResourceHandle::ResourceHandle(ResourceHandle&& other) AVALANCHE_NOEXCEPT : m_resource_identifier(other.m_resource_identifier) {
        other.m_resource_identifier = 0;
    }

    ResourceHandle & ResourceHandle::operator=(const ResourceHandle& other) {
        ResourceHandle(other).swap(*this);
        return *this;
    }

    ResourceHandle & ResourceHandle::operator=(ResourceHandle&& other) AVALANCHE_NOEXCEPT {
        ResourceHandle(std::move(other)).swap(*this);
        return *this;
    }

    void ResourceHandle::swap(ResourceHandle &other) AVALANCHE_NOEXCEPT {
        std::swap(m_resource_identifier, other.m_resource_identifier);
    }

    ResourceType ResourceHandle::type() const AVALANCHE_NOEXCEPT {
        return static_cast<ResourceType>(m_resource_identifier >> 56);
    }

    ResourceHandle::size_type ResourceHandle::operator*() const noexcept {
        return m_resource_identifier;
    }

    ResourceHandle::ResourceHandle(ResourceType resource_type, const size_type value) {
        constexpr size_type mask = 0x00FFFFFFFFFFFFFF;
        const size_type cleared = value & mask;
        m_resource_identifier = (static_cast<size_type>(resource_type) << 56) | cleared;
    }

    std::strong_ordering operator<=>(const ResourceHandle &lhs, const ResourceHandle &rhs) noexcept {
        return *lhs <=> *rhs;
    }
}

