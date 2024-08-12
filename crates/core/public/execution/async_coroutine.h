#pragma once

#include "avalanche_core_export.h"
#include "polyfill.h"
#include "container/optional.hpp"
#include "container/vector_queue.hpp"
#include "execution/executor.h"
#include "container/shared_ptr.hpp"
#include <type_traits>
#include <coroutine>
#include <utility>
#include <concepts>
#include <atomic>
#include <functional>
#include <mutex>


namespace avalanche::core::execution {

namespace detail::async {

    template <typename Ret>
    class promise;

    template <typename Ret>
    class coroutine : public std::coroutine_handle<promise<Ret>>, public promise_state_base {
    public:
        using promise_type = promise<Ret>;
        using coroutine_handle_type = std::coroutine_handle<promise<Ret>>;

        explicit coroutine(coroutine_handle_type handle)
            : std::coroutine_handle<promise<Ret>>(handle)
            , m_executor(threaded_coroutine_executor::get_global_executor())
        {}
        ~coroutine() override {
            this->destroy();
        }

        std::coroutine_handle<> get_erased_handle() override {
            return *this;
        }

        void set_ready() override {
            m_is_ready.store(true, std::memory_order_release);
        }

        bool done() override {
            return m_is_ready.load(std::memory_order_acquire);
        }

        void resume() override {
            coroutine_handle_type::resume();
        }

        struct awaiter_context;

        awaiter_context operator co_await() && AVALANCHE_NOEXCEPT {
            return awaiter_context{ *this };
        }

    private:
        coroutine_executor_base& m_executor;
        std::atomic<bool> m_is_ready;
    };

    template <typename Ret>
    class promise_base {
    public:
        using state_type = coroutine<Ret>;
        using handle_type = state_type::coroutine_handle_type;

        shared_ptr<state_type> get_return_object() {
            m_state_object = make_shared<state_type>(state_type::from_promise(*this));
            return m_state_object;
        }

        /**
         * Suspending here to return the decltype(get_return_object()) to caller.
         * e.g.
         *
         * ```
         * promise_state<void> foo() { co_return; }
         *
         * shared_ptr<state_type> promise = foo();
         * ```
         */
        static std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT { return {}; }

        /**
         * We don't allow exception in coroutine for now.
         * It is a trivial job to ensure safety after an exception occurred.
         */
        void unhandled_exception() {
            throw;
        }

        void set_next(state_type::coroutine_handle_type handle) {
            m_continuation = handle;
        }

        shared_ptr<state_type> get_state() {
            return m_state_object;
        }

        struct final_awaiter {
            bool await_ready() const AVALANCHE_NOEXCEPT { return false; }
            decltype(auto) await_suspend(handle_type handle) {
                auto& promise = handle.promise();

                if (promise.m_continuation) {
                    auto state = promise.m_continuation.promise().get_state();
                    state->m_executor.push_coroutine(promise.get_state());
                }

                return true;
            }
        };

    private:
        state_type::coroutine_handle_type m_continuation = nullptr;
        shared_ptr<state_type> m_state_object;

        friend class coroutine<Ret>;
    };

    template <typename Ret>
    class promise : public promise_base<Ret> {
    public:
        void return_value(Ret&& value) {
            m_result = std::move(value);
        }

        optional<Ret> m_result;
    };

    template <>
    class promise<void> : public promise_base<void> {
    public:
        void return_void() {}
    };

    template <typename Ret>
    struct coroutine<Ret>::awaiter_context {
        coroutine<Ret>& current_coroutine_context;

        /**
         * Always goto await_suspend
         */
        bool await_ready() AVALANCHE_NOEXCEPT {
            return false;
        }

        decltype(auto) await_suspend(coroutine_handle_type parent_coroutine_handle) {
            // Setup continuation chain
            current_coroutine_context.promise().set_next(parent_coroutine_handle);

            // Submit to queue
            current_coroutine_context.m_executor.push_coroutine(current_coroutine_context.promise().get_state());

            return !current_coroutine_context.done();
        }

        decltype(auto) await_resume() noexcept {
            if AVALANCHE_CONSTEXPR (!std::is_void_v<Ret>) {
                return current_coroutine_context.promise().m_result.value();
            }
        }
    };

}

}
