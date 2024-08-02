#pragma once

#include <memory>
#include <initializer_list>
#include <tuple>
#include "container/impl/red_black_tree.hpp"

namespace avalanche {

    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class map_base {
    public:
        using key_type = Key;
        using key_reference_type = key_type&;
        using key_const_reference_type = const key_type&;
        using value_type = Value;
        using size_type = size_t;
        using value_reference_type = value_type&;
        using value_const_reference_type = const value_type&;
        using hash_impl = Hash;
        using hash_key_size = size_t;
        using rb_tree_impl = RBTreeMap<hash_key_size, Value>;

    private:
        hash_impl m_hasher {};
        rb_tree_impl m_rb_tree{};

    public:
        map_base() : m_hasher(hash_impl{}), m_rb_tree(rb_tree_impl{}) {}
        explicit map_base(hash_impl impl) : m_hasher(std::move(impl)) {}
        map_base(std::initializer_list<std::pair<key_type, value_type>> initializers) : map_base() {
            for (auto& [key, value] : initializers) {
                m_rb_tree.insert(m_hasher(key), value);
            }
        }

        AVALANCHE_NO_DISCARD size_type size() const AVALANCHE_NOEXCEPT {
            return m_rb_tree.size();
        }

        AVALANCHE_NO_DISCARD bool is_empty() const AVALANCHE_NOEXCEPT {
            return m_rb_tree.empty();
        }

        void clear() AVALANCHE_NOEXCEPT {
            m_rb_tree.clear();
        }

        void reset() AVALANCHE_NOEXCEPT {
            clear();
        }

        value_const_reference_type get(const key_type& key) const {
            return m_rb_tree.get(m_hasher(key));
        }

        value_reference_type get(const key_type& key) {
            return m_rb_tree.get(m_hasher(key));
        }

        bool contains(const key_type& key) const {
            return m_rb_tree.contains(m_hasher(key));
        }

        value_type get_or_default(const key_type& key, const value_type& default_value) const {
            return contains(key) ? get(key) : default_value;
        }

        template <typename X = key_type, typename Y = value_type>
        requires std::convertible_to<const X&, key_const_reference_type> && std::convertible_to<Y, value_type>
        void insert(const X& key, Y&& value) {
            m_rb_tree.insert(m_hasher(static_cast<key_const_reference_type>(key)), std::forward<Y>(value));
        }

        template <typename X = key_type, typename Y = value_type>
        requires std::convertible_to<const X&, const key_type&> && std::convertible_to<const Y&, const value_type&>
        bool insert_if_not_exist(const X& key, const Y& value) {
        }
    };

    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    using map = map_base<Key, Value, Hash>;
}
