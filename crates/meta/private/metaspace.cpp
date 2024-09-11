#include "metaspace.h"
#include "class.h"

#include <cassert>
#include <map>
#include <ranges>
#include <set>
#include <string>
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

    class MetaSpaceImpl final : public MetaSpace {
        std::set<MetaSpace*> m_children_spaces;
        std::unordered_map<std::string, Class*, FNV1aHash> m_owned_classes;
        std::map<std::string, MetaSpace*> m_quick_path_to_class;
    public:
        MetaSpaceProxy create() override { return MetaSpaceProxy{*this}; }

        ~MetaSpaceImpl() override {
            for (const auto& value: m_owned_classes | std::views::values) {
                delete value;
            }
        }

        MetaSpace* allocate_meta_space() override {
            MetaSpace *new_meta_space = new MetaSpaceImpl();
            m_children_spaces.insert(new_meta_space);
            return new_meta_space;
        }

        void deallocate_meta_space(MetaSpace* space) override {
            // TODO: perform unregister
            // Delete the space owned by current space
            if (const auto node_handle = m_children_spaces.extract(space)) {
                delete space;
            }
        }

        void register_class(Class* new_class) override {
            if (nullptr == new_class) return;
            const std::string& name = new_class->full_name_str();
            assert(!m_owned_classes.contains(name));

            m_owned_classes.insert_or_assign(name, new_class);
        }

        void unregister_class(Class* new_class) override {
            if (nullptr == new_class) return;
            const std::string& name = new_class->full_name_str();
            assert(m_owned_classes.contains(name));

            m_owned_classes.extract(name);
        }

        Class* find_class(const std::string_view name) override {
            const std::string class_name(name);
            if (const auto it = m_owned_classes.find(class_name); it != m_owned_classes.end()) {
                return it->second;
            }

            if (const auto it = m_quick_path_to_class.find(class_name); it != m_quick_path_to_class.end()) {
                if (m_children_spaces.contains(it->second)) {
                    if (const auto result = it->second->find_class(name); nullptr != result) {
                        return result;
                    }
                }
                m_quick_path_to_class.erase(class_name);
            }

            for (MetaSpace* child : m_children_spaces) {
                if (const auto result = child->find_class(name); nullptr != result) {
                    m_quick_path_to_class.insert_or_assign(class_name, child);
                    return result;
                }
            }

            return nullptr;
        }
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
