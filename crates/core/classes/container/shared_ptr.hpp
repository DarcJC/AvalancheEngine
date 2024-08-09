#pragma once

#include <cstdint>
#include <atomic>
#include <utility>
#include <type_traits>
#include <iostream>
#include "logger.h"
#include "container/allocator.hpp"

namespace avalanche {

    template <typename T>
    class enable_shared_from_this;

    template <typename T>
    class enable_atomic_shared_from_this;

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    class shared_ptr_base;

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    using shared_ptr = shared_ptr_base<T, Allocator, RefCounter>;

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    class weak_ptr_base;

    template <typename T, AllocatorType Allocator = default_allocator<T>, typename RefCounter = size_t>
    using weak_ptr = weak_ptr_base<T, Allocator, RefCounter>;

    template <typename, typename RefCounter = size_t>
    struct control_block {
        RefCounter shared_count = 1; // Initially one reference when shared_ptr is created
        RefCounter weak_count = 0;   // No weak pointers initially

        control_block() : shared_count(1), weak_count(0) {}
    };

    template <typename T, AllocatorType Allocator, typename RefCounter>
    class shared_ptr_base {
    public:
        using value_type = T;
        using pointer_type = value_type*;
        using reference_type = value_type&;
        using const_pointer_type = const pointer_type;
        using allocator_type = Allocator;
        using size_type = size_t;
        using control_block_type = control_block<T, RefCounter>;
        static constexpr bool is_atomic_counter = std::is_same_v<RefCounter, std::atomic<size_t>>;

    private:
        pointer_type m_ptr = nullptr;
        control_block_type* m_ctrl_block = nullptr;
        allocator_type m_allocator{};

        void release_reference_count() {
            if (m_ctrl_block) {
                size_type count;
                if (is_atomic_counter) {
                    count = m_ctrl_block->shared_count.fetch_sub(1, std::memory_order_relaxed) - 1;
                } else {
                    count = --(m_ctrl_block->shared_count);
                }
                if (count == 0) {
                    if (m_ptr) {
                        m_allocator.destroy(m_ptr);
                        m_allocator.deallocate(m_ptr, 1);
                    }
                    if (is_atomic_counter) {
                        std::atomic_thread_fence(std::memory_order_acquire);
                        if (m_ctrl_block->weak_count.load(std::memory_order_relaxed) == 0) {
                            delete m_ctrl_block;
                        }
                    } else {
                        if (m_ctrl_block->weak_count == 0) {
                            delete m_ctrl_block;
                        }
                    }
                }
            }
        }

        void increase_shared_count() {
            if (m_ctrl_block) {
                if (is_atomic_counter) {
                    m_ctrl_block->shared_count.fetch_add(1, std::memory_order_acq_rel);
                } else {
                    ++(m_ctrl_block->shared_count);
                }
            }
        }

        void set_enable_shared_from_this() {
            if constexpr (std::is_base_of_v<enable_shared_from_this<T>, T> || std::is_base_of_v<enable_atomic_shared_from_this<T>, T>) {
                m_ptr->weak_this = to_weak_ptr();
            }
        }

    public:
        shared_ptr_base(nullptr_t = nullptr) : m_ptr(nullptr), m_ctrl_block(nullptr) {}

        explicit shared_ptr_base(pointer_type ptr) : m_ptr(ptr), m_ctrl_block(new control_block_type()) {
            set_enable_shared_from_this();
        }

        shared_ptr_base(const shared_ptr_base& other) : m_ptr(other.m_ptr), m_ctrl_block(other.m_ctrl_block) {
            if (m_ctrl_block) {
                increase_shared_count();
            }
        }

        shared_ptr_base(shared_ptr_base&& other) AVALANCHE_NOEXCEPT : shared_ptr_base() {
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
                    increase_shared_count();
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

        friend void swap(shared_ptr_base& lhs, shared_ptr_base& rhs) AVALANCHE_NOEXCEPT {
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

        AVALANCHE_NO_DISCARD size_type use_count() const {
            if constexpr (std::is_integral_v<RefCounter>) {
                return m_ctrl_block ? m_ctrl_block->shared_count : 0;
            } else {
                return m_ctrl_block ? m_ctrl_block->shared_count.load(std::memory_order_acquire) : 0;
            }
        }

        operator bool() const AVALANCHE_NOEXCEPT {
            return use_count() > 0;
        }

        weak_ptr<value_type, allocator_type, RefCounter> to_weak_ptr() const {
            return weak_ptr_base(*this);
        }

        explicit operator weak_ptr<value_type, allocator_type, RefCounter>() const AVALANCHE_NOEXCEPT {
            return to_weak_ptr();
        }

    private:
        shared_ptr_base(pointer_type ptr, control_block_type* ctrl_block) : m_ptr(ptr), m_ctrl_block(ctrl_block) {
            AVALANCHE_CHECK(ctrl_block, "Invalid control block");
            increase_shared_count();
        }

        friend class weak_ptr_base<value_type, allocator_type, RefCounter>;
    };

    template <typename T, AllocatorType Allocator, typename RefCounter>
    class weak_ptr_base {
    public:
        using value_type = T;
        using reference_type = value_type&;
        using pointer_type = value_type*;
        using allocator_type = Allocator;
        using reference_counter_type = RefCounter;
        static constexpr bool is_atomic_counter = std::is_same_v<RefCounter, std::atomic<size_t>>;

    private:
        pointer_type m_ptr = nullptr;
        control_block<T, RefCounter>* m_ctrl_block = nullptr;

        void release_weak_ref() {
            if (m_ctrl_block) {
                if AVALANCHE_CONSTEXPR (is_atomic_counter) {
                    auto weak_count = m_ctrl_block->weak_count.fetch_sub(1, std::memory_order_release) - 1;

                    if (weak_count == 0) {
                        std::atomic_thread_fence(std::memory_order_acquire);

                        if (m_ctrl_block->shared_count.load(std::memory_order_relaxed) == 0) {
                            delete m_ctrl_block;
                        }
                    }
                } else {
                     if (--(m_ctrl_block->weak_count) == 0 && (m_ctrl_block->shared_count) == 0) {
                        delete m_ctrl_block;
                     }
                }
            }
        }

        void increase_weak_count() {
            if (m_ctrl_block) {
                if AVALANCHE_CONSTEXPR (is_atomic_counter) {
                    m_ctrl_block->weak_count.fetch_add(1, std::memory_order_acq_rel);
                } else {
                    ++(m_ctrl_block->weak_count);
                }
            }
        }

    public:
        weak_ptr_base(nullptr_t = nullptr) : m_ptr(nullptr), m_ctrl_block(nullptr) {}

        explicit weak_ptr_base(const shared_ptr<value_type, allocator_type, reference_counter_type>& shared) : m_ptr(shared.get()), m_ctrl_block(shared.m_ctrl_block) {
            if (m_ctrl_block) {
                increase_weak_count();
            }
        }

        weak_ptr_base(const weak_ptr_base& other) : m_ptr(other.m_ptr), m_ctrl_block(other.m_ctrl_block) {
            if (m_ctrl_block) {
                increase_weak_count();
            }
        }

        weak_ptr_base& operator=(const weak_ptr_base<value_type, allocator_type, RefCounter>& other) {
            if (this != &other) {
                release_weak_ref();
                m_ptr = other.m_ptr;
                m_ctrl_block = other.m_ctrl_block;
                if (m_ctrl_block) {
                    increase_weak_count();
                }
            }
            return *this;
        }

        weak_ptr_base& operator=(const shared_ptr<value_type, allocator_type, RefCounter>& other) {
            weak_ptr_base(other).swap(*this);
            return *this;
        }

        ~weak_ptr_base() {
            release_weak_ref();
        }

        shared_ptr<value_type, allocator_type, reference_counter_type> lock() const AVALANCHE_NOEXCEPT {
            if AVALANCHE_CONSTEXPR (is_atomic_counter) {
                if (m_ctrl_block) {
                    auto old_count = m_ctrl_block->shared_count.load(std::memory_order_acquire);
                    while (old_count != 0) {
                        if (m_ctrl_block->shared_count.compare_exchange_weak(old_count, old_count + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                            return {m_ptr, m_ctrl_block};
                        }
                    }
                }
            } else {
                if (m_ctrl_block && m_ctrl_block->shared_count > 0) {
                    return {m_ptr, m_ctrl_block};
                }
            }
            return nullptr;
        }

        AVALANCHE_NO_DISCARD bool expired() const AVALANCHE_NOEXCEPT {
            return !m_ctrl_block || m_ctrl_block->shared_count == 0;
        }

        AVALANCHE_NO_DISCARD size_t use_count() const AVALANCHE_NOEXCEPT {
            return m_ctrl_block ? m_ctrl_block->shared_count : 0;
        }

        void swap(weak_ptr_base& other) AVALANCHE_NOEXCEPT {
            using std::swap;
            swap(m_ptr, other.m_ptr);
            swap(m_ctrl_block, other.m_ctrl_block);
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

        operator bool() const AVALANCHE_NOEXCEPT {
            return !expired();
        }

    };

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

    template <typename T>
    class enable_atomic_shared_from_this {
    protected:
        enable_atomic_shared_from_this() = default;
        enable_atomic_shared_from_this(const enable_atomic_shared_from_this&) = default;
        enable_atomic_shared_from_this& operator=(const enable_atomic_shared_from_this&) = default;

        using shared_type = shared_ptr<T, default_allocator<T>, std::atomic<size_t>>;
        using shared_const_type = shared_ptr<const T, default_allocator<T>, std::atomic<size_t>>;
        using weak_type = weak_ptr<T, default_allocator<T>, std::atomic<size_t>>;

    public:
        shared_type shared_from_this() {
            return weak_this.lock();
        }

        shared_const_type shared_from_this() const {
            return weak_this.lock();
        }

        template <typename U, AllocatorType Allocator, typename RefCounter>
        friend class shared_ptr_base;

    private:
        mutable weak_type weak_this;
    };

    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        using allocator_type = default_allocator<T>;
        allocator_type allocator{};
        T* allocated_memory = allocator.allocate(1, nullptr);
        allocator.construct(allocated_memory, std::forward<Args>(args)...);
        return shared_ptr_base<T, default_allocator<T>>(allocated_memory);
    }

    template <typename T>
    using atomic_shared_ptr = shared_ptr_base<T, default_allocator<T>, std::atomic<size_t>>;

    template <typename T>
    using atomic_weak_ptr = weak_ptr_base<T, default_allocator<T>, std::atomic<size_t>>;

    template <typename T, typename... Args>
    atomic_shared_ptr<T> make_atomic_shared(Args&&... args) {
        using allocator_type = default_allocator<T>;
        allocator_type allocator{};
        T* allocated_memory = allocator.allocate(1, nullptr);
        allocator.construct(allocated_memory, std::forward<Args>(args)...);
        return atomic_shared_ptr<T>(allocated_memory);
    }
}
