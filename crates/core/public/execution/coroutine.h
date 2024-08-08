#pragma once

#include "polyfill.h"
#include <concepts>
#include <type_traits>
#include <coroutine>
#include <utility>


namespace avalanche::core::execution {

    template <typename=void>
    class coroutine_context {
    public:
        class promise_base;
        class promise_type;
        class awaiter_type;
        using handle_type = std::coroutine_handle<promise_type>;
        using untyped_handle_type = std::coroutine_handle<void>;

        coroutine_context(coroutine_context&&) AVALANCHE_NOEXCEPT
            : m_coro_handle(std::exchange(m_coro_handle, {})) {}

        coroutine_context& operator=(coroutine_context&&) AVALANCHE_NOEXCEPT {
            m_coro_handle = std::exchange(m_coro_handle, {});
            return *this;
        }

        ~coroutine_context() AVALANCHE_NOEXCEPT{
            if (m_coro_handle) {
                m_coro_handle.destroy();
            }
        }

        awaiter_type operator co_await() && AVALANCHE_NOEXCEPT {
            return awaiter_type(m_coro_handle);
        }

        void resume() {
            if (m_coro_handle) {
                m_coro_handle.resume();
            }
        }

    private:
        explicit coroutine_context(const handle_type handle) AVALANCHE_NOEXCEPT
            : m_coro_handle(handle) {}

        handle_type m_coro_handle;
    };

    template <typename T>
    class coroutine_context<T>::promise_base {
    public:
        using Outer = coroutine_context;

        constexpr std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT { return {}; }

        void unhandled_exception() {
            // Rethrow to our terminate handler
            throw;
        }

        struct final_awaiter {
            bool await_ready() const AVALANCHE_NOEXCEPT { return false; }
            decltype(auto) await_suspend(handle_type handle) AVALANCHE_NOEXCEPT {
                auto& promise = handle.promise();
                // The coroutine is now suspended at the final-suspend point.
                // Lookup its continuation in the promise and resume it symmetrically.
                return promise.continuation;
            }
            void await_resume() const AVALANCHE_NOEXCEPT {}
        };

        final_awaiter final_suspend() AVALANCHE_NOEXCEPT {
            return {};
        }

        untyped_handle_type continuation{};
    };

    template <typename T>
    class coroutine_context<T>::promise_type : public coroutine_context<T>::promise_base {
    public:
        using Super = coroutine_context<T>::promise_base;

        typename Super::Outer get_return_object() AVALANCHE_NOEXCEPT {
            return Outer(handle_type::from_promise(*this));
        }

        void return_value(T&& value) {
        }
    };

    template <>
    class coroutine_context<void>::promise_type : public coroutine_context<void>::promise_base {
    public:
        using Super = coroutine_context<void>::promise_base;

        Super::Outer get_return_object() AVALANCHE_NOEXCEPT {
            return Super::Outer(handle_type::from_promise(*this));
        }

        void return_void() {}
    };

    template <typename T>
    class coroutine_context<T>::awaiter_type {
    public:
        bool await_ready() AVALANCHE_NOEXCEPT {
            return false;
        }

        decltype(auto) await_suspend(untyped_handle_type handle) AVALANCHE_NOEXCEPT {
            auto& promise = m_coro_handle.promise();

            // Store the continuation in the task's promise so that the final_suspend()
            // knows to resume this coroutine when the task completes.
            promise.continuation = handle;
            // Then we tail-resume the task's coroutine, which is currently suspended
            // at the initial-suspend-point (i.e. at the open curly brace), by returning
            // its handle from await_suspend().
            return m_coro_handle;
        }

        void await_resume() noexcept {}

    private:
        explicit awaiter_type(handle_type handle) AVALANCHE_NOEXCEPT : m_coro_handle(handle) {}

        handle_type m_coro_handle;

        friend coroutine_context<T>;
    };

    struct sync_coroutine_context {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        handle_type coro_handle;

        explicit sync_coroutine_context(handle_type handle) AVALANCHE_NOEXCEPT : coro_handle(handle) {}

        sync_coroutine_context(sync_coroutine_context&& other) AVALANCHE_NOEXCEPT : coro_handle(std::exchange(other.coro_handle, {})) {}
        ~sync_coroutine_context() AVALANCHE_NOEXCEPT {
            if (coro_handle) {
                coro_handle.destroy();
            }
        }

        struct promise_type {
            using Outer = sync_coroutine_context;

            Outer get_return_object() AVALANCHE_NOEXCEPT {
                return Outer(handle_type::from_promise(*this));
            }

            std::suspend_never initial_suspend() AVALANCHE_NOEXCEPT { return {}; }
            std::suspend_always final_suspend() AVALANCHE_NOEXCEPT { return {}; }

            void return_void() AVALANCHE_NOEXCEPT {}

            void unhandled_exception() { throw; }
        };

        template <typename T>
        static sync_coroutine_context start(T&& coro) AVALANCHE_NOEXCEPT {
            co_await std::forward<T>(coro);
        }

        bool done() const AVALANCHE_NOEXCEPT {
            return coro_handle.done();
        }
    };

}
