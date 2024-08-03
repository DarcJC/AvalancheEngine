#pragma once

#include <memory>
#include <cstdint>
#include <initializer_list>
#include "container/vector.hpp"
#include "container/utility.hpp"
#include "container/optional.hpp"

namespace avalanche {

    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class hash_map {
    public:
        using key_type = Key;
        using key_reference_type = key_type&;
        using key_const_reference_type = const key_type&;
        using value_type = Value;
        using value_pointer_type = value_type*;
        using value_const_pointer_type = const value_type*;
        using size_type = size_t;
        using value_reference_type = value_type&;
        using value_const_reference_type = const value_type&;
        using hash_impl = Hash;
        using hash_key_size = size_t;
        using node_type = optional<pair<key_type, value_type>>;
        using bucket_type = vector<node_type>;
        using bucket_iterator = typename bucket_type::iterator_type;
        using load_factor_type = float;

    private:
        hash_impl m_hasher {};
        bucket_type m_buckets{};
        size_type m_size = 0;
        load_factor_type m_load_factor_to_scale = 0.75;
        static constexpr size_type default_size = 8;

        size_type index_of_key(key_const_reference_type key) const {
            return m_hasher(key) % m_buckets.size();
        }

        void rehash() {
            size_type new_size = m_buckets.size() * 2;
            bucket_type new_buckets(new_size);
            for (const node_type& node : m_buckets) {
                if (node) {
                    size_type index = m_hasher(node->first) % new_size;
                    while (new_buckets[index]) {
                        index = (index + 1) % new_size;
                    }
                    new_buckets[index] = node;
                }
            }
            m_buckets.swap(new_buckets);
        }

    public:
        hash_map() : m_buckets(default_size) {}

        template <typename U = value_type>
        requires std::is_default_constructible_v<U> && std::convertible_to<U, value_type>
        bool insert_defaulted_if_absent(key_const_reference_type key) {
            if (!contains(key)) {
                insert_or_assign(key, U{});
                return true;
            }
            return false;
        }

        void insert_or_assign(key_const_reference_type key, value_const_reference_type value) {
            if (m_size >= m_buckets.size() * m_load_factor_to_scale) { // Scale while load coefficient >= 0.75
                rehash();
            }

            size_type index = index_of_key(key);
            while (m_buckets[index]) {
                if (m_buckets[index]->first == key) {
                    m_buckets[index]->second = value; // update
                    return;
                }
                index = (index + 1) % m_buckets.size();
            }
            m_buckets[index] = make_pair(key, value); // insert
            ++m_size;
        }

        value_const_pointer_type find(key_const_reference_type key) const {
            size_type index = index_of_key(key);
            size_type count = 0;
            while (m_buckets[index] && count < m_buckets.size()) {
                if (m_buckets[index]->first == key) {
                    return &m_buckets[index]->second;
                }
                index = (index + 1) % m_buckets.size();
                ++count;
            }
            return nullptr;
        }

        bool contains(key_const_reference_type key) const {
            return find(key);
        }

        void set_load_factor(load_factor_type new_factor) {
            AVALANCHE_CHECK(new_factor < 0.9 && new_factor > 0.05, "Load factor must only set to less than 0.9 and lager than 0.05");
            m_load_factor_to_scale = new_factor;
        }
    };

}
