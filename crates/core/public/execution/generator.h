#pragma once

#include "polyfill.h"
#include "container/optional.hpp"
#include <concepts>
#include <coroutine>
#include <type_traits>

namespace avalanche::core::execution {
    /**
     * https://zh.cppreference.com/w/cpp/coroutine/coroutine_handle
     * @brief a generator wrapper
     * @tparam T type
     */
    template <std::movable T>
    class generator {
    public:
        using value_type = T;

        struct promise_type;
        using coroutine_handle = std::coroutine_handle<promise_type>;

        class Iter;
        using iterator = Iter;

        struct promise_type {
            generator<value_type> get_return_object() {
                return generator{coroutine_handle::from_promise(*this)};
            }
            static std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT { return {}; }
            static std::suspend_always final_suspend() AVALANCHE_NOEXCEPT { return {}; }
            static void return_void() {}
            std::suspend_always yield_value(value_type value) AVALANCHE_NOEXCEPT {
                current_value = std::move(value);
                return {};
            }
            // Make co_await a generator expression leads to compile time error
            void await_transform() = delete;
            [[noreturn]] static void unhandled_exception() { throw; }

            optional<value_type> current_value;
        };

        explicit generator(const coroutine_handle& handle) : m_coroutine_handle(handle) {}
        generator() = default;
        ~generator() {
            reset();
        }

        generator(const generator&) = delete;
        generator& operator=(const generator&) = delete;

        generator(generator&& other) AVALANCHE_NOEXCEPT : m_coroutine_handle(other.m_coroutine_handle) {
            // We can't use move here as coroutine_handle is trivial copiable
            other.m_coroutine_handle = {};
        }
        generator& operator=(generator&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                reset();
                m_coroutine_handle = other.m_coroutine_handle;
                other.m_coroutine_handle = {};
            }
            return *this;
        }

        void reset() {
            if (m_coroutine_handle) {
                m_coroutine_handle.destroy();
            }
        }

        class Iter {
        public:
            void operator++() {
                m_coroutine_handle.resume();
            }
            const T& operator*() const {
                return *m_coroutine_handle.promise().current_value;
            }
            bool operator==(std::default_sentinel_t) const {
                return !m_coroutine_handle || m_coroutine_handle.done();
            }
            explicit Iter(const coroutine_handle coroutine_handle) : m_coroutine_handle(coroutine_handle) {}

        private:
            coroutine_handle m_coroutine_handle;
        };

        Iter begin() {
            if (m_coroutine_handle) {
                m_coroutine_handle.resume();
            }
            return Iter(m_coroutine_handle);
        }

        std::default_sentinel_t end() { return {}; }

    private:
        coroutine_handle m_coroutine_handle;
    };

    template <std::integral T>
    generator<T> range(T start_included, const T last_excluded) {
        while (start_included < last_excluded) {
            co_yield start_included++;
        }
    }

}
