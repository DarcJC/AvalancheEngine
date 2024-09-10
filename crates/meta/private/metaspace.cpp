#include "metaspace.h"

#include <cassert>
#include <set>
#include <unordered_map>


namespace avalanche {

    constexpr uint32_t FNV1aHash::hash_32_fnv1a(const std::string_view str) noexcept {
        uint32_t hash = detail::FNV1aInternal<uint32_t>::val;
        for (const unsigned char c : str) {
            hash = hash ^ c;
            hash *= detail::FNV1aInternal<uint32_t>::prime;
        }
        return hash;
    }

    constexpr uint64_t FNV1aHash::hash_64_fnv1a(const std::string_view str) noexcept {
        uint64_t hash = detail::FNV1aInternal<uint64_t>::val;
        for (const unsigned char c : str) {
            hash = hash ^ c;
            hash *= detail::FNV1aInternal<uint64_t>::prime;
        }
        return hash;
    }

    size_t FNV1aHash::operator()(std::string_view str) const noexcept {
        if constexpr (sizeof(size_t) == sizeof(uint32_t)) {
            return hash_32_fnv1a(str);
        } else if constexpr (sizeof(size_t) == sizeof(uint64_t)) {
            return hash_64_fnv1a(str);
        } else {
            assert(false);
        }
    }

    class MetaSpaceImpl : public MetaSpace {
        std::set<MetaSpace *> m_children_spaces;
        std::unordered_map<std::string_view, Class*, FNV1aHash> m_owned_classes;

        MetaSpaceProxy create() override { return MetaSpaceProxy{*this}; }

        MetaSpace *allocate_meta_space() override {
            MetaSpace *new_meta_space = new MetaSpaceImpl();
            m_children_spaces.insert(new_meta_space);
            return new_meta_space;
        }

        void deallocate_meta_space(MetaSpace *space) override {
            // TODO: perform unregister
            // Delete the space owned by current space
            if (const auto node_handle = m_children_spaces.extract(space)) {
                delete space;
            }
        }

        void register_class(Class *new_class) override {}

        void unregister_class(Class *new_class) override {}
    };

    MetaSpace &MetaSpace::get() {
        static MetaSpaceImpl impl{};
        return impl;
    }

    MetaSpace::~MetaSpace() = default;

    MetaSpaceProxy::MetaSpaceProxy(MetaSpace &parent_space) :
        m_parent_space(parent_space), m_value(parent_space.allocate_meta_space()) {}

    MetaSpaceProxy::~MetaSpaceProxy() { m_parent_space.deallocate_meta_space(m_value); }

    MetaSpace* MetaSpaceProxy::operator->() const noexcept { return m_value; }

} // namespace avalanche
