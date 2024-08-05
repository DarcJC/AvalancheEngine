#pragma once

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <functional>
#include <concepts>
#include "polyfill.h"
#include "container/allocator.hpp"
#include "container/exception.hpp"

namespace avalanche {

    template <typename T, AllocatorType Allocator = default_allocator<T>>
    class vector_base {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference_type = value_type&;
        using const_reference_type = const value_type&;
        using pointer_type = typename std::allocator_traits<Allocator>::pointer;
        using const_pointer_type = typename std::allocator_traits<Allocator>::const_pointer;
        using iterator_type = pointer_type;
        using const_iterator_type = const_pointer_type;
        using reverse_iterator_type = std::reverse_iterator<iterator_type>;
        using const_reverse_iterator_type = std::reverse_iterator<const_iterator_type>;

        static constexpr size_type npos = static_cast<size_type>(-1);

    private:
        allocator_type m_allocator{};
        pointer_type m_data = nullptr;
        size_type m_capacity = 0;
        size_type m_length = 0;

        void check_index(size_type index) const {
            if (index >= m_length)
                raise_exception(out_of_range());
        }

        void resize_internal(size_type new_capacity = 0) {
            if (new_capacity == 0) {
                new_capacity = m_capacity ? m_capacity * 2 : 8;
            }

            pointer_type new_data = m_allocator.allocate(new_capacity);
            AVALANCHE_CHECK(m_length <= m_capacity, "vector conatiner internal error: length > capacity");
            for (size_type i = 0; i < m_length; ++i) {
                AVALANCHE_CHECK(m_data != nullptr, "vector conatiner internal error: invalid memory access");
                m_allocator.construct(new_data + i, std::move(m_data[i]));
                m_allocator.destroy(m_data + i);
            }

            if (m_data) {
                m_allocator.deallocate(m_data, m_capacity);
            }

            m_data = new_data;
            m_capacity = new_capacity;
        }

    public:
        vector_base() : m_data(nullptr), m_length(0), m_capacity(0) {}
        explicit vector_base(const allocator_type& allocator) : m_allocator(allocator), m_data(nullptr), m_length(0), m_capacity(0) {}
        explicit vector_base(size_type default_capacity) : vector_base() {
            resize_internal(default_capacity);
        }

        template <typename U = value_type>
        requires(std::is_convertible_v<U, T>)
        vector_base(std::initializer_list<U> initializers) : m_length(0), m_allocator(allocator_type{}) {
            resize_internal(initializers.size());

            for (const U& value : initializers) {
                m_allocator.construct(m_data + (m_length++), value);
            }
        }

        ~vector_base() {
            clear();
            if (m_data) {
                m_allocator.deallocate(m_data, m_capacity);
            }
        }

        vector_base(const vector_base& other) {
            resize_internal(m_capacity);
            for (size_type i = 0; i < other.m_length; ++i) {
                m_allocator.construct(m_data + i, other.m_data[i]);
            }
            m_length = other.m_length;
        }

        vector_base(vector_base&& other) AVALANCHE_NOEXCEPT
            : m_allocator(std::move(other.m_allocator))
            , m_data(other.m_data)
            , m_length(other.m_length)
            , m_capacity(other.m_capacity)
        {
            other.m_data = nullptr;
            other.m_length = 0;
            other.m_capacity = 0;
        }

        vector_base& operator=(const vector_base& other) {
            if (this != &other) {
                clear();
                resize_internal(other.m_capacity);
                for (size_type i = 0; i < other.m_length; ++i) {
                    m_allocator.construct(m_data + i, other.m_data[i]);
                }
                m_length = other.m_length;
            }
            return *this;
        }

        vector_base& operator=(vector_base&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                clear();
                m_allocator = std::move(other.m_allocator);
                m_data = other.m_data;
                m_capacity = other.m_capacity;
                m_length = other.m_length;
                other.m_data = nullptr;
                other.m_length = 0;
                other.m_capacity = 0;
            }
            return *this;
        }

        template <typename U = value_type>
        requires std::convertible_to<const U&, value_type>
        size_type add_item(const U& value) {
            if (m_length == m_capacity) {
                resize_internal();
            }
            m_allocator.construct(m_data + m_length, value);
            return m_length++;
        }

        template <typename U = value_type>
        requires std::convertible_to<U&&, value_type>
        size_type add_item(U&& value) {
            if (m_length == m_capacity) {
                resize_internal();
            }
            m_allocator.construct(m_data + m_length, std::forward<U>(value));
            return m_length++;
        }

        template <typename U = value_type>
        requires std::convertible_to<U&&, value_type>
        size_type push_back(const U& value) {
            return add_item(value);
        }

        template <typename U = value_type>
        requires std::convertible_to<U&&, value_type>
        size_type push_back(U&& value) {
            return add_item(std::forward<U>(value));
        }

        template <typename... Args>
        size_type emplace_back(Args&&... vals) {
            if (m_length == m_capacity) {
                resize_internal();
            }
            m_allocator.construct(m_data + m_length, std::forward<Args>(vals)...);
            return m_length++;
        }

        template <typename... Args>
        void emplace_at(size_type index, Args&&... vals) {
            check_index(index);
            if (m_length == m_capacity) {
                resize_internal();
            }
            for (size_type i = m_length; i > index; --i) {
                m_allocator.construct(m_data + i, std::move(m_data[i - 1]));
                m_allocator.destroy(m_data + i - 1);
            }
            m_allocator.construct(m_data + index, std::forward<Args>(vals)...);
            ++m_length;
        }

        void clear() {
            for (size_type i = 0; i < m_length; ++i) {
                m_allocator.destroy(m_data + i);
            }
            m_length = 0;
        }

        template <typename U = value_type>
        requires std::equality_comparable_with<const U&, const_reference_type>
        void remove(const U& value) {
            while (true) {
                size_type pos = find(value);
                if (pos == npos) {
                    break;
                }
                remove_at(pos);
            }
        }

        size_type remove_last() {
            if (m_length > 0) {
                m_allocator.destroy(m_data + (--m_length));
            }
            return m_length;
        }

        void remove_at(size_type index) {
            check_index(index);
            m_allocator.destroy(m_data + index);
            for (size_type i = index; i < m_length - 1; ++i) {
                m_allocator.construct(m_data + i, std::move(m_data[i + 1]));
                m_allocator.destroy(m_data + i + 1);
            }
            --m_length;
        }

        template <typename U = value_type>
        requires(std::is_convertible_v<U, value_type>)
        void insert_at(size_type index, const U& value) {
            check_index(index);
            if (m_length == m_capacity) {
                resize_internal();
            }
            for (size_type i = m_length; i > index; --i) {
                m_allocator.construct(m_data + i, std::move(m_data[i - 1]));
                m_allocator.destroy(m_data + i - 1);
            }
            m_allocator.construct(m_data + index, value);
            ++m_length;
        }

        iterator_type begin() {
            return m_data;
        }

        iterator_type end() {
            return m_data + m_length;
        }

        const_iterator_type begin() const {
            return m_data;
        }

        const_iterator_type end() const {
            return m_data + m_length;
        }

        reference_type at(size_type index) {
            check_index(index);
            return m_data[index];
        }

        const_reference_type at(size_type index) const {
            check_index(index);
            return m_data[index];
        }

        reference_type operator[](size_type index) {
            return at(index);
        }

        const_reference_type operator[](size_type index) const {
            return at(index);
        }

        pointer_type data() {
            return m_data;
        }

        const_pointer_type data() const {
            return m_data;
        }

        AVALANCHE_NO_DISCARD size_type length() const {
            return m_length;
        }

        AVALANCHE_NO_DISCARD size_type size() const {
            return length();
        }

        AVALANCHE_NO_DISCARD size_type allocated_size() const {
            return m_capacity;
        }

        AVALANCHE_NO_DISCARD size_type capacity() const {
            return allocated_size();
        }

        template <typename U = value_type>
        requires std::equality_comparable_with<const U&, const value_type&>
        size_type find(const U& value) const {
            for (size_type i = 0; i < m_length; ++i) {
                if (m_data[i] == value) {
                    return i;
                }
            }
            return npos;
        }

        template <typename U = value_type>
        requires std::equality_comparable_with<const U&, const_reference_type>
        bool contains(const U& value) {
            return find(value) != npos;
        }

        template <typename U = value_type>
        requires std::convertible_to<const U&, value_type>
        bool contains(const U& value) {
            return find(value_type(value)) != npos;
        }

        void reverse() {
            for (size_type i = 0; i < m_length / 2; ++i) {
                std::swap(m_data[i], m_data[m_length - i - 1]);
            }
        }

        void ensure_capacity(size_type new_capacity) {
            if (m_capacity >= new_capacity) {
                return;
            }
            resize_internal(new_capacity);
        }

        void sort(std::function<bool(const_reference_type,const_reference_type)> comparator) {
            std::sort(begin(), end(), comparator);
        }

        void swap(vector_base& other) AVALANCHE_NOEXCEPT {
            std::swap(m_length, other.m_length);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_data, other.m_data);
            std::swap(m_allocator, other.m_allocator);
        }

        AVALANCHE_NO_DISCARD bool is_valid_index(size_type index) const {
            return index < m_length;
        }

        void set_size_uninitialized(size_type new_size) {
            if (new_size > m_capacity) {
                resize_internal(new_size);
            }
            m_length = new_size;
        }

        void set_size_defaulted(size_type new_size) {
            set_size_uninitialized(new_size);
            for (size_type i = 0; i < m_length; ++i) {
                m_allocator.construct(m_data + i);
            }
        }

        AVALANCHE_NO_DISCARD bool is_empty() const {
            return m_length == 0;
        }

        reference_type front_item() {
            AVALANCHE_CHECK(!is_empty(), "Function front_item() is unavailable to use while vector is empty");
            return at(0);
        }

        const_reference_type front_item() const {
            AVALANCHE_CHECK(!is_empty(), "Function front_item() is unavailable to use while vector is empty");
            return at(0);
        }

        reference_type last_item() {
            AVALANCHE_CHECK(!is_empty(), "Function last_item() is unavailable to use while vector is empty");
            return at(m_length - 1);
        }

        const_reference_type last_item() const {
            AVALANCHE_CHECK(!is_empty(), "Function last_item() is unavailable to use while vector is empty");
            return at(m_length - 1);
        }

    };

    template <typename T, typename Allocator = default_allocator<T>>
    using vector = vector_base<T, Allocator>;

}
