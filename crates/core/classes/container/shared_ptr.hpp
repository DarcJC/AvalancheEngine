#pragma once

#include <cstdint>
#include <atomic>
#include <utility>
#include <type_traits>
#include "logger.h"
#include "container/allocator.hpp"

namespace avalanche {

    template <bool IsAtomic = false>
    class reference_counter {
    public:
        using inner_type = size_t;
        using counter_type = std::conditional_t<IsAtomic, std::atomic<inner_type>, inner_type>;

        explicit reference_counter(inner_type default_count = 0) : m_count(default_count) {}

        /**
         * @brief Add ref
         */
        inner_type increase() {
            if AVALANCHE_CONSTEXPR (IsAtomic) {
                return m_count.fetch_add(1, std::memory_order_acquire) + 1;
            } else {
                return ++m_count;
            }
        }

        inner_type decrease() {
            if AVALANCHE_CONSTEXPR (IsAtomic) {
                return m_count.fetch_sub(1, std::memory_order_release) + 1;
            } else {
                return --m_count;
            }
        }

        AVALANCHE_NO_DISCARD inner_type count() const {
            if AVALANCHE_CONSTEXPR (IsAtomic) {
                return m_count.load(std::memory_order_acquire);
            } else {
                return m_count;
            }
        }

        AVALANCHE_NO_DISCARD bool is_zero() const {
            return count() == 0;
        }

    private:
        counter_type m_count;
    };

    namespace detail::shared_ptr {
        template <bool IsAtomic = false>
        struct control_block {
        public:
            using inner_type = typename reference_counter<IsAtomic>::inner_type;

            control_block() : m_weak(0), m_shared(0) {}

            inner_type increase_shared() {
                return m_shared.increase();
            }

            inner_type decrease_shared() {
                return m_shared.decrease();
            }

            inner_type increase_weak() {
                return m_weak.increase();
            }

            inner_type decrease_weak() {
                return m_weak.decrease();
            }

            bool should_object_been_destroy() {
                return m_shared.is_zero();
            }

            bool should_control_block_been_destroy() {
                return m_shared.is_zero() && m_weak.is_zero();
            }

            reference_counter<IsAtomic> m_weak{};
            reference_counter<IsAtomic> m_shared{};
        };
    }

    template <typename T, bool IsAtomic = false>
    class shared_ptr {
        using control_block_type = detail::shared_ptr::control_block<IsAtomic>;
        using control_block_pointer = control_block_type*;
    public:
        using value_type = T;
        using value_pointer = value_type*;
        static constexpr bool is_atomic_rc = IsAtomic;

        shared_ptr() : m_value(nullptr), m_control_block(nullptr) {}
        explicit shared_ptr(std::nullptr_t) : shared_ptr() {}
        explicit shared_ptr(value_pointer ptr) : shared_ptr(ptr, new control_block_type()) {}

        shared_ptr(const shared_ptr& other) : shared_ptr(other.m_value, other.m_control_block) {}
        shared_ptr& operator=(const shared_ptr& other) {
            if (this != &other) {
                reset();
                if (other.m_value && other.m_control_block) {
                    other.m_control_block->increase_shared();
                }
                m_value = other.m_value;
                m_control_block = other.m_control_block;
            }
            return *this;
        }

        shared_ptr(shared_ptr&& other) AVALANCHE_NOEXCEPT : shared_ptr(std::exchange(other.m_value, nullptr), std::exchange(other.m_control_block, nullptr)) {}
        shared_ptr& operator=(shared_ptr&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                shared_ptr temp(other);
                std::swap(other.m_value, m_value);
                std::swap(other.m_control_block, m_control_block);
            }
            return *this;
        }

        ~shared_ptr() {
            reset();
        }

        void reset() {
            if (m_control_block) {
                if (m_control_block->decrease_shared() == 0) {
                    delete m_value;
                    m_value = nullptr;
                    if (m_control_block->m_weak.count() == 0) {
                        delete m_control_block;
                        m_control_block = nullptr;
                    }
                }
            }
        }

        template <typename U = T>
        requires std::is_base_of_v<U, T>
        shared_ptr<U, is_atomic_rc> clone() const {
            return shared_ptr<U, is_atomic_rc>(m_value, m_control_block);
        }

        template <typename U = T>
        requires std::is_convertible_v<T*, U*>
        U* get() {
            return m_value;
        }

        template <typename U = T>
        requires std::is_convertible_v<T*, U*>
        U* get() const {
            return m_value;
        }

        AVALANCHE_NO_DISCARD bool is_valid() const {
            return m_control_block != nullptr && m_value && !m_control_block->should_object_been_destroy();
        }

        explicit operator bool() const {
            return is_valid();
        }

        value_pointer operator->() const {
            return get();
        }

    private:
        shared_ptr(value_pointer ptr, control_block_pointer cb) : m_value(ptr), m_control_block(cb) {
            if (m_value && m_control_block) {
                m_control_block->increase_shared();
            }
        }

        value_pointer m_value = nullptr;
        control_block_pointer m_control_block = nullptr;

        template <typename, bool>
        friend class weak_ptr;
    };

    template <typename T, bool IsAtomic = false>
    class weak_ptr {
        using control_block_type = detail::shared_ptr::control_block<IsAtomic>;
        using control_block_pointer = control_block_type*;
    public:
        using value_type = T;
        using value_pointer = value_type*;

        explicit weak_ptr(const shared_ptr<T, IsAtomic>& other) : weak_ptr(other.m_value, other.m_control_block) {}

        weak_ptr(const weak_ptr& other) : weak_ptr(other.m_value, other.m_control_block) {}
        weak_ptr& operator=(const weak_ptr& other) {
            if (&other != this) {
                reset();
                if (other.m_value && other.m_control_block) {
                    other.m_control_block->increase_weak();
                }
                m_value = other.m_value;
                m_control_block = other.m_control_block;
            }
            return *this;
        }

        weak_ptr(weak_ptr&& other) AVALANCHE_NOEXCEPT : weak_ptr(std::exchange(other.m_value, nullptr), std::exchange(other.m_control_block, nullptr)) {}
        weak_ptr& operator=(weak_ptr&& other) AVALANCHE_NOEXCEPT {
            if (&other != this) {
                weak_ptr temp(other);
                std::swap(other.m_value, m_value);
                std::swap(other.m_control_block, m_control_block);
            }
            return *this;
        }

        ~weak_ptr() {
            reset();
        }

        void reset() {
            m_value = nullptr;
            if (m_control_block) {
                if (m_control_block->decrease_weak() == 0) {
                    delete m_control_block;
                    m_control_block = nullptr;
                }
            }
        }

        shared_ptr<T, IsAtomic> lock() {
            return { m_value, m_control_block };
        }

    private:
        weak_ptr(value_pointer ptr, control_block_pointer cb) : m_value(ptr), m_control_block(cb) {
            if (m_value && m_control_block) {
                m_control_block->increase_weak();
            }
        }

        value_pointer m_value = nullptr;
        control_block_pointer m_control_block = nullptr;

        template <typename, bool>
        friend class shared_ptr;
    };

    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T> { new T(std::forward<Args>(args)...) };
    }

    template <typename T, typename... Args>
    shared_ptr<T, true> make_atomic_shared(Args&&... args) {
        return shared_ptr<T, true>{ new T(std::forward<Args>(args)...) };
    }

}
