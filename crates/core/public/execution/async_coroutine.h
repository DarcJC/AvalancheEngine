#pragma once

#include "polyfill.h"
#include "container/optional.hpp"
#include "container/vector_queue.hpp"
#include "execution/executor.h"
#include "container/shared_ptr.hpp"
#include "container/allocator.hpp"
#include <type_traits>
#include <coroutine>
#include <utility>
#include <concepts>
#include <atomic>
#include <functional>
#include <mutex>


namespace avalanche::core::execution {

namespace detail::async {

    template <typename T>
    struct type_reference_but_void {
        using type = T&;
    };

    template <>
    struct type_reference_but_void<void> {
        using type = void;
    };

    template <typename T>
    class coroutine_state;

    template <typename T = void>
    class coroutine_context {
    public:
        class promise_base;
        class promise_type;
        class awaiter_type;
        using handle_type = std::coroutine_handle<promise_type>;
        using untyped_handle_type = std::coroutine_handle<void>;
        using callback_type = std::function<void(typename type_reference_but_void<T>::type)>;
        using self_type = coroutine_context;
        using state_type = coroutine_state<T>;
        using shared_state_type = avalanche::atomic_shared_ptr<state_type>;
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
            return awaiter_type(m_coro_handle);
        }

        self_type set_executor(coroutine_executor_base& executor) {
            if (m_executor != &executor) {
                m_executor = &executor;
                m_coro_handle.promise().m_executor = m_executor;
                m_coro_handle.promise().inherit_executor = false;
            }
            return std::move(*this);
        }

        /**
         * @brief Launch a coroutine context in a **non-coroutine** context.
         * Don't launch after co_await, unless you know what are you doing
         */
        shared_state_type launch() AVALANCHE_NOEXCEPT {
            handle_type h = m_coro_handle;
            shared_state_type state = require_shared_state();
            m_executor->push_coroutine(h);
            return state;
        }

        /**
         * @brief Release the control of coroutine. Caller require to destroy it. Ignoring the returning might leads to memory leak.
         * @return The coroutine handle
         */
        handle_type unsafe_release() AVALANCHE_NOEXCEPT {
            AVALANCHE_CHECK(m_coro_handle, "Trying to release coroutine that was already released");
            return std::exchange(m_coro_handle, {});
        }

        shared_state_type require_shared_state() {
            AVALANCHE_CHECK(m_coro_handle && !m_coro_handle.promise().m_state, "Invalid coroutine context. Check if you require_shared_state() twice.");
            auto& promise = m_coro_handle.promise();
            shared_state_type state = avalanche::make_atomic_shared<coroutine_state<T>>(unsafe_release());
            promise.m_state = state;
            return state;
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
    class coroutine_state : public avalanche::enable_atomic_shared_from_this<coroutine_state<T>> {
    public:
        using handle_type = typename coroutine_context<T>::handle_type;
        using callback_type = typename coroutine_context<T>::callback_type;
        explicit coroutine_state(handle_type handle)
            : is_finished(false)
            , m_handle(handle) {}

        ~coroutine_state() AVALANCHE_NOEXCEPT {
            if (m_handle) {
                m_handle.destroy();
            }
        }

        coroutine_state(const coroutine_state&) = delete;
        coroutine_state& operator=(const coroutine_state&) = delete;
        coroutine_state(coroutine_state&& other) = delete;
        coroutine_state& operator=(coroutine_state&& other) = delete;

        void then(callback_type callback) {
            AVALANCHE_CHECK(m_handle, "Trying to perform then operation on an invalid coroutine_state");
            if (is_finished) {
                if AVALANCHE_CONSTEXPR (std::is_void_v<T>) {
                    callback();
                } else {
                    auto& promise = m_handle.promise();
                    T& result_value = promise.result.value();
                    callback(result_value);
                }
            } else {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_callbacks.emplace_back(std::move(callback));
            }
        }

        void notify_finished() {
            if (!is_finished.exchange(true, std::memory_order_acq_rel)) {
                std::lock_guard<std::mutex> lock(m_mutex);

                if AVALANCHE_CONSTEXPR (std::is_void_v<T>) {
                    while (!m_callbacks.queue_is_empty()) {
                        callback_type cb = std::move(m_callbacks.queue_pop_front());
                        cb();
                    }
                } else {
                    auto& promise = m_handle.promise();
                    T& result_value = promise.result.value();
                    while (!m_callbacks.queue_is_empty()) {
                        callback_type cb = std::move(m_callbacks.queue_pop_front());
                        cb(result_value);
                    }
                }

                m_callbacks.clear();
            }
        }

    private:
        std::mutex m_mutex;
        std::atomic<bool> is_finished;
        handle_type m_handle;
        vector_queue<callback_type> m_callbacks;
    };

    template <typename T>
    class coroutine_context<T>::promise_base {
    public:
        using Outer = coroutine_context;
        using shared_state = typename Outer::shared_state_type;
        using weak_shared_state = avalanche::atomic_weak_ptr<coroutine_state<T>>;

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

                // Notify callbacks
                // if (shared_state ptr = promise.m_state.lock()) {
                //     ptr->notify_finished();
                // }
            }
            void await_resume() const AVALANCHE_NOEXCEPT {}

            coroutine_executor_base* m_executor;
        };

        final_awaiter final_suspend() AVALANCHE_NOEXCEPT {
            return { m_executor };
        }

        // TODO: memory pool
        // static void* operator new(std::size_t size) {
        //     return ::operator new(size);
        // }
        // static void operator delete(void* ptr, std::size_t size) {
        //     ::operator delete(ptr);
        // }

        untyped_handle_type continuation{};
        std::atomic<bool> ready = false;
        bool inherit_executor = true;
        coroutine_executor_base* m_executor = nullptr;
        weak_shared_state m_state{};
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

        decltype(auto) await_suspend(handle_type handle) AVALANCHE_NOEXCEPT {
            auto& promise = m_coro_handle.promise();

            // Inheritance parent executor
            if (handle) {
                if (promise.inherit_executor) {
                    promise.m_executor = handle.promise().m_executor;
                }
            }

            // Store the continuation in the task's promise so that the final_suspend()
            // knows to resume this coroutine when the task completes.
            promise.continuation = handle;

            // Then we tail-resume the task's coroutine, which is currently suspended
            // at the initial-suspend-point (i.e. at the open curly brace), by returning
            // its handle from await_suspend().
            promise.m_executor->push_coroutine(m_coro_handle);

            return !promise.ready.exchange(true, std::memory_order_acq_rel);
        }

        void await_resume() noexcept {}

    private:
        explicit awaiter_type(handle_type handle) AVALANCHE_NOEXCEPT : m_coro_handle(handle) {}

        handle_type m_coro_handle;

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
