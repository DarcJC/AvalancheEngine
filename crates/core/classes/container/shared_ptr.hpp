#pragma once

#include <cstdint>
#include <utility>
#include <type_traits>
#include "logger.h"
#include "container/allocator.hpp"

namespace avalanche {

    template <typename T>
    class enable_shared_from_this;

    template <typename, typename RefCounter = size_t>
    struct control_block {
        RefCounter shared_count = 1; // Initially one reference when shared_ptr is created
        RefCounter weak_count = 0;   // No weak pointers initially

        control_block() : shared_count(1), weak_count(0) {}
    };

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    class shared_ptr_base {
    public:
        using value_type = T;
        using pointer_type = value_type*;
        using reference_type = value_type&;
        using const_pointer_type = const pointer_type;
        using allocator_type = Allocator;
        using size_type = size_t;

    private:
        pointer_type m_ptr = nullptr;
        control_block<T, RefCounter>* m_ctrl_block = nullptr;
        allocator_type m_allocator{};

        void release_reference_count() {
            if (m_ctrl_block) {
                if (--(m_ctrl_block->shared_count) == 0) {
                    if (m_ptr) {
                        m_allocator.destroy(m_ptr);
                        m_allocator.deallocate(m_ptr, 1);
                    }
                    if (m_ctrl_block->weak_count == 0) {
                        delete m_ctrl_block;
                    }
                }
            }
        }

        void set_enable_shared_from_this() {
            if constexpr (std::is_base_of_v<enable_shared_from_this<T>, T>) {
                m_ptr->weak_this = *this;
            }
        }

    public:
        shared_ptr_base(nullptr_t = nullptr) : m_ptr(nullptr), m_ctrl_block(nullptr) {}

        explicit shared_ptr_base(pointer_type ptr) : m_ptr(ptr), m_ctrl_block(new control_block<T>()) {
            set_enable_shared_from_this();
        }

        shared_ptr_base(const shared_ptr_base& other) : m_ptr(other.m_ptr), m_ctrl_block(other.m_ctrl_block) {
            if (m_ctrl_block) {
                ++(m_ctrl_block->shared_count);
            }
        }

        shared_ptr_base(shared_ptr_base&& other) : shared_ptr_base() {
            swap(*this, other);
        }

        ~shared_ptr_base() {
            release_reference_count();
        }

        shared_ptr_base& operator=(const shared_ptr_base& other) {
            if (this != &other) {
                release_reference_count();
                m_ptr = other.m_ptr;
                m_ctrl_block = other.m_ctrl_block;
                if (m_ctrl_block) {
                    ++(m_ctrl_block->shared_count);
                }
                set_enable_shared_from_this();
            }
            return *this;
        }

        shared_ptr_base& operator=(shared_ptr_base&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                swap(*this, other);
            }
            return *this;
        }

        friend void swap(shared_ptr_base& lhs, shared_ptr_base& rhs) {
            using std::swap;
            swap(lhs.m_ptr, rhs.m_ptr);
            swap(lhs.m_ctrl_block, rhs.m_ctrl_block);
            swap(lhs.m_allocator, rhs.m_allocator);
        }

        pointer_type get() const {
            return m_ptr;
        }

        reference_type operator*() const {
            AVALANCHE_CHECK(m_ptr, "Trying deref a invalid shared_ptr");
            return *m_ptr;
        }

        pointer_type operator->() const {
            AVALANCHE_CHECK(m_ptr, "Trying visit a nullptr inside shared_ptr");
            return m_ptr;
        }

        size_t use_count() const {
            return m_ctrl_block ? m_ctrl_block->shared_count : 0;
        }
    };

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    using shared_ptr = shared_ptr_base<T, Allocator, RefCounter>;

    template <typename T, typename RefCounter = size_t>
    class weak_ptr_base {
    public:
        using value_type = T;
        using reference_type = value_type&;
        using pointer_type = value_type*;

    private:
        pointer_type m_ptr = nullptr;
        control_block<T, RefCounter>* m_ctrl_block = nullptr;

    public:
        weak_ptr_base(nullptr_t = nullptr) : m_ptr(nullptr), m_ctrl_block(nullptr) {}

        explicit weak_ptr_base(const shared_ptr<T, RefCounter>& shared) : m_ptr(shared.get()), m_ctrl_block(shared.m_ctrl_block) {
            if (m_ctrl_block) {
                ++(m_ctrl_block->weak_count);
            }
        }

        weak_ptr_base(const weak_ptr_base& other) : m_ptr(other.m_ptr), m_ctrl_block(other.m_ctrl_block) {
            if (m_ctrl_block) {
                ++(m_ctrl_block->weak_count);
            }
        }

        weak_ptr_base& operator=(const weak_ptr_base& other) {
            if (this != &other) {
                if (m_ctrl_block && --(m_ctrl_block->weak_count) == 0 && m_ctrl_block->shared_count == 0) {
                    delete m_ctrl_block;
                }
                m_ptr = other.m_ptr;
                m_ctrl_block = other.m_ctrl_block;
                if (m_ctrl_block) {
                    ++(m_ctrl_block->weak_count);
                }
            }
            return *this;
        }

        ~weak_ptr_base() {
            if (m_ctrl_block && --(m_ctrl_block->weak_count) == 0 && m_ctrl_block->shared_count == 0) {
                delete m_ctrl_block;
            }
        }

        shared_ptr<T> lock() const AVALANCHE_NOEXCEPT {
            if (m_ctrl_block && m_ctrl_block->shared_count > 0) {
                return shared_ptr<T>(m_ptr, m_ctrl_block);
            }
            return nullptr;
        }

        AVALANCHE_NO_DISCARD bool expired() const AVALANCHE_NOEXCEPT {
            return !m_ctrl_block || m_ctrl_block->shared_count == 0;
        }

        AVALANCHE_NO_DISCARD size_t use_count() const AVALANCHE_NOEXCEPT {
            return m_ctrl_block ? m_ctrl_block->shared_count : 0;
        }

        pointer_type operator->() const {
            AVALANCHE_CHECK(m_ptr, "Trying visit a nullptr inside weak_ptr");
            AVALANCHE_CHECK(m_ctrl_block && m_ctrl_block->shared_count > 0, "Trying visit a freed instance (RC=0)");
            return lock().get();
        }

        reference_type operator*() const {
            AVALANCHE_CHECK(m_ptr, "Trying deref a invalid weak_ptr");
            AVALANCHE_CHECK(m_ctrl_block && m_ctrl_block->shared_count > 0, "Trying visit a freed instance (RC=0)");
            return *lock();
        }
    };

    template <typename T, typename RefCounter = size_t>
    using weak_ptr = weak_ptr_base<T, RefCounter>;

    template <typename T>
    class enable_shared_from_this {
    protected:
        enable_shared_from_this() = default;
        enable_shared_from_this(const enable_shared_from_this&) = default;
        enable_shared_from_this& operator=(const enable_shared_from_this&) = default;

    public:
        shared_ptr<T> shared_from_this() {
            return weak_this.lock();
        }

        shared_ptr<const T> shared_from_this() const {
            return weak_this.lock();
        }

        template <typename U, AllocatorType Allocator, typename RefCounter>
        friend class shared_ptr_base;

    private:
        mutable weak_ptr<T> weak_this;
    };
}
