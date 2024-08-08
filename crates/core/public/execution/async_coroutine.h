#pragma once

#include "polyfill.h"
#include "container/optional.hpp"
#include "container/vector_queue.hpp"
#include "execution/executor.h"
#include <type_traits>
#include <coroutine>
#include <utility>
#include <concepts>
#include <atomic>
#include <functional>


namespace avalanche::core::execution {

namespace detail::async {

    template <typename T>
    struct no_sucking_void {
        using type = T&;
    };

    template <>
    struct no_sucking_void<void> {
        using type = void;
    };

    template <typename T = void>
    class coroutine_context {
    public:
        class promise_base;
        class promise_type;
        class awaiter_type;
        using handle_type = std::coroutine_handle<promise_type>;
        using untyped_handle_type = std::coroutine_handle<void>;
        using callback_type = std::function<void(typename no_sucking_void<T>::type)>;
        using self_type = coroutine_context;
        static coroutine_executor_base* get_default_executor() {
            static coroutine_executor_base* executor = &threaded_coroutine_executor::get_global_executor();
            return executor;
        }

        coroutine_context(coroutine_context&& other) AVALANCHE_NOEXCEPT
            : m_coro_handle(std::exchange(other.m_coro_handle, {}))
            , m_executor(other.m_executor)
        {}

        coroutine_context& operator=(coroutine_context&& other) AVALANCHE_NOEXCEPT {
            m_coro_handle = std::exchange(other.m_coro_handle, {});
            std::swap(other.m_executor, m_executor);
            return *this;
        }

        ~coroutine_context() AVALANCHE_NOEXCEPT{
            if (m_coro_handle) {
                m_coro_handle.destroy();
            }
        }

        awaiter_type operator co_await() && AVALANCHE_NOEXCEPT {
            return awaiter_type(m_coro_handle, *m_executor);
        }

        self_type set_executor(coroutine_executor_base& executor) {
            m_executor = &executor;
            m_coro_handle.promise().m_executor = m_executor;
            return std::move(*this);
        }

        /**
         * @brief Launch a coroutine context in a **non-coroutine** context.
         * Don't launch after co_await, unless you know what are you doing
         */
        self_type launch() AVALANCHE_NOEXCEPT {
            m_executor->push_coroutine(m_coro_handle);
            return std::move(*this);
        }

        /**
         * @brief Release the control of coroutine. Caller require to destroy it. Ignoring the returning might leads to memory leak.
         * @return The coroutine handle
         */
        handle_type unsafe_release() AVALANCHE_NOEXCEPT {
            return std::exchange(m_coro_handle, {});
        }

        self_type then(callback_type callback) {
            AVALANCHE_CHECK(m_coro_handle, "Using invalid coroutine context");
            if (m_coro_handle.done()) {
            }
            return std::move(*this);
        }

    protected:
        explicit coroutine_context(const handle_type handle) AVALANCHE_NOEXCEPT
            : m_coro_handle(handle)
            , m_executor(get_default_executor()) {
            handle.promise().m_executor = m_executor;
        }

        handle_type m_coro_handle;
        coroutine_executor_base* m_executor;
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
            void await_suspend(handle_type handle) AVALANCHE_NOEXCEPT {
                auto& promise = handle.promise();

                // The coroutine is now suspended at the final-suspend point.
                // Lookup its continuation in the promise and resume it symmetrically.
                if (promise.ready.exchange(true, std::memory_order_acq_rel)) {
                    if (promise.continuation) {
                        m_executor->push_coroutine(promise.continuation);
                    }
                }
            }
            void await_resume() const AVALANCHE_NOEXCEPT {}

            coroutine_executor_base* m_executor;
        };

        final_awaiter final_suspend() AVALANCHE_NOEXCEPT {
            return { m_executor };
        }

        untyped_handle_type continuation{};
        std::atomic<bool> ready = false;
        coroutine_executor_base* m_executor = nullptr;
    };

    template <typename T>
    class coroutine_context<T>::promise_type : public coroutine_context<T>::promise_base {
    public:
        using Super = coroutine_context<T>::promise_base;

        typename Super::Outer get_return_object() AVALANCHE_NOEXCEPT {
            return Super::Outer(handle_type::from_promise(*this));
        }

        void return_value(T&& value) {
            result = std::forward<T>(value);
        }

        optional<T> result{};
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
            m_executor.push_coroutine(m_coro_handle);

            return !promise.ready.exchange(true, std::memory_order_acq_rel);
        }

        void await_resume() noexcept {}

    private:
        explicit awaiter_type(handle_type handle, coroutine_executor_base& executor) AVALANCHE_NOEXCEPT : m_coro_handle(handle), m_executor(executor) {}

        handle_type m_coro_handle;
        coroutine_executor_base& m_executor;

        friend coroutine_context<T>;
    };

}

    template <typename T = void>
    using async_coroutine = detail::async::coroutine_context<T>;

    template <typename T = void>
    using async = async_coroutine<T>;

    using async_void = async<void>;
    using async_bool = async<bool>;

    template <typename T>
    void launch_async(async_coroutine<T>&& coro) {
        coro.launch();
    }

    template <typename T>
    void launch_sync(async_coroutine<T>&& coro) {
        coro.set_executor(sync_coroutine_executor::get_global_executor());
        coro.launch();
    }

}
