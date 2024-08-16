#pragma once

#include "container/allocator.hpp"
#include "container/exception.hpp"
#include "polyfill.h"

namespace avalanche {
    template <typename T, AllocatorType Allocator = default_allocator<T>>
    class optional {
    public:
        using value_type = T;
        using value_reference_type = value_type&;
        using value_right_reference_type = value_type&&;
        using value_const_reference_type = const value_type&;
        using value_pointer_type = value_type*;
        using value_const_pointer_type = const value_type*;
        using allocator_impl = Allocator;

    private:
        allocator_impl allocator{};

        alignas(std::max_align_t) void* m_data[sizeof(value_type)] {};
        bool m_has_value = false;

        value_pointer_type data() {
            return reinterpret_cast<value_pointer_type>(m_data);
        }

        value_const_pointer_type data() const {
            return reinterpret_cast<value_const_pointer_type>(m_data);
        }

    public:
        optional() : m_has_value(false) {}
        optional(nullptr_t) : optional() {}

        optional(value_right_reference_type value) : m_has_value(true) {
            allocator.construct(data(), std::forward<T>(value));
        }
        optional(value_const_reference_type value) : m_has_value(true) {
            allocator.construct(data(), value);
        }

        template <typename U = value_type>
        requires std::convertible_to<U, value_type>
        optional(const optional<U>& other) : m_has_value(other.m_has_value) {
            if (m_has_value) {
                allocator.construct(data(), *other.data());
            }
        }

        template <typename U = value_type>
        requires std::convertible_to<U, value_type>
        optional& operator=(const optional<U>& other) {
            if (this != &other) {
                reset();
                if (other.m_has_value) {
                    allocator.construct(data(), *other.data());
                    m_has_value = true;
                }
            }
            return *this;
        }

        template <typename U = T>
        optional& operator=(U&& value) {
            reset();
            m_has_value = true;
            allocator.construct(data(), std::forward<U>(value));
            return *this;
        }

        ~optional() AVALANCHE_NOEXCEPT {
            reset();
        }

        value_reference_type value() {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return *data();
        }

        value_const_reference_type value() const {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return *data();
        }

        void reset() {
            if (m_has_value) {
                allocator.destroy(data());
                m_has_value = false;
            }
        }

        value_type value_or(value_const_reference_type default_value) const {
            if (m_has_value) {
                return *data();
            } else {
                return default_value;
            }
        }

        operator bool() const {
            return m_has_value;
        }

        value_pointer_type operator->() {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return data();
        }

        value_const_pointer_type operator->() const {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return data();
        }

        value_reference_type operator*() {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return *data();
        }

        value_const_reference_type operator*() const {
            if (!m_has_value) {
                raise_exception(invalid_access());
            }
            return *data();
        }
    };

    template <typename T>
    optional<T> make_optional(T&& value) {
        return optional<T>(std::forward<T>(value));
    }
}
