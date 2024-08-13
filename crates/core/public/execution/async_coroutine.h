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

    template<typename Ret>
    class coroutine_state;

    /////////////////////////
    template <typename Ret>
    class coroutine {
    public:
        struct promise_base;
        struct promise_type;
        struct awaiter_type;
        struct final_awaiter;
        using handle_type = std::coroutine_handle<promise_type>;
        using state_type = coroutine_state<Ret>;

        explicit coroutine(handle_type coroutine_handle)
            : m_state(make_atomic_shared<state_type>(coroutine_handle))
        {}

        coroutine(const coroutine& other) : m_state(other.m_state) {}
        coroutine& operator=(const coroutine& other) {
            if (this != &other) {
                m_state = other.m_state;
            }
            return *this;
        }

        coroutine(coroutine&& other) AVALANCHE_NOEXCEPT : m_state(std::move(other.m_state)) {}
        coroutine& operator=(coroutine&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                AVALANCHE_MAYBE_UNUSED coroutine temporary(other);
                std::swap(m_state, other.m_state);
            }
            return *this;
        }

        state_type* operator->() {
            return m_state.template get<state_type>();
        }

        shared_ptr<state_type, true> get_state() {
            return m_state;
        }

        awaiter_type operator co_await() AVALANCHE_NOEXCEPT {
            return awaiter_type{m_state};
        }

    private:
        shared_ptr<state_type, true> m_state;
    };

    template <typename Ret>
    struct coroutine<Ret>::promise_base {

        /**
         * @brief We won't handle the exception inside the coroutine.
         */
        static void unhandled_exception() {
            throw;
        }

        /**
         * @brief Always suspending and return coroutine<Ret> to invoker.
         *
         * Suspend at
         *
         * coroutine<void> foo() {
         *     co_return;        ^ suspended here
         * }
         *
         * The function body isn't executed yet.
         */
        static constexpr std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT {
            return std::suspend_always{};
        }

        /**
         * @brief Coroutine has finished, and we suspend at
         *
         * coroutine<void> foo() {
         *     co_return;
         * }
         * ^
         * suspended here
         *
         * to resume the continuation if existed.
         */
        final_awaiter final_suspend() AVALANCHE_NOEXCEPT {
            return final_awaiter { coroutine_state_value };
        }

        /**
         * @brief The caller coroutine.
         *
         * coroutine<void> bar() {
         *     co_await foo();
         *     ^ This will invoke
         *       `(bar's promise).await_transform(foo())`
         *       or(and)
         *       `foo().operator co_await((bar's handle))`
         * }
         *
         * The `(bar's coroutine)` was returned to caller first as the result of expression `bar()`.
         *
         * We save `(bar's coroutine)` as the continuation of `foo()` so we can resume `bar()` after `foo()` completed.
         *
         */
        std::coroutine_handle<> continuation = nullptr;

        /**
         * @brief We use a shared state to allow tracking coroutine that moving across threads.
         */
        shared_ptr<state_type, true> coroutine_state_value;
    };

    template <typename Ret>
    struct coroutine<Ret>::promise_type : coroutine<Ret>::promise_base {
        coroutine<Ret> get_return_object() AVALANCHE_NOEXCEPT {
            coroutine res { handle_type::from_promise(*this) };
            this->coroutine_state_value = res.m_state;
            return res;
        }

        void return_value(Ret&& value) {
            this->coroutine_state_value.set_result(std::move(value));
        }
    };

    template <>
    struct coroutine<void>::promise_type : coroutine<void>::promise_base {
        coroutine<void> get_return_object() AVALANCHE_NOEXCEPT {
            coroutine res{ handle_type::from_promise(*this) };
            this->coroutine_state_value = res.m_state;
            return res;
        }

        static void return_void() {}
    };

    template <typename Ret>
    struct coroutine<Ret>::awaiter_type {
        shared_ptr<state_type, true> awaiting_coroutine_state;

        /**
         * @brief Always returning false to step into `await_suspend()`
         */
        AVALANCHE_CONSTEXPR static bool await_ready() AVALANCHE_NOEXCEPT {
            return false;
        }

        /**
         * @brief
         * @param parent_handle The handle of coroutine awaiting current `awaiter`
         * @return true to suspend handle, false to resume handle
         */
        decltype(auto) await_suspend(std::coroutine_handle<> parent_handle) AVALANCHE_NOEXCEPT {
            handle_type awaiting_handle = awaiting_coroutine_state->get_handle();
            auto& promise = awaiting_handle.promise();
            promise.continuation = parent_handle;  // Set awaiting coroutine's continuation to parent scope handle

            awaiting_coroutine_state->submit_to_executor(awaiting_coroutine_state);

            return !awaiting_coroutine_state->is_ready();
        }

        decltype(auto) await_resume() AVALANCHE_NOEXCEPT {
            return awaiting_coroutine_state->get_result();
        }
    };

    template <typename Ret>
    struct coroutine<Ret>::final_awaiter {
        shared_ptr<state_type, true> awaiting_coroutine_state;

        AVALANCHE_CONSTEXPR static bool await_ready() AVALANCHE_NOEXCEPT {
            return false;
        }

        decltype(auto) await_suspend(std::coroutine_handle<> current_handle) AVALANCHE_NOEXCEPT {
            handle_type awaiting_handle = awaiting_coroutine_state->get_handle();
            auto& promise = awaiting_handle.promise();

            if (awaiting_coroutine_state->set_ready()) {
                if (promise.continuation) {
                    promise.continuation.resume();
                }
            }
        }

        void await_resume() AVALANCHE_NOEXCEPT {}
    };
    /////////////////////////

    /////////////////////////

    template <typename T>
    struct bool_if_void_else_type {
        using type = T;
        using ret = const T&;
    };

    template <>
    struct bool_if_void_else_type<void> {
        using type = bool;
        using ret = bool;
    };

    template <typename Ret>
    class coroutine_state : public promise_state_base {
    public:
        using promise_type = typename coroutine<Ret>::promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        explicit coroutine_state(handle_type handle)
            : m_handle(handle)
            , m_is_ready(false)
            , m_executor(threaded_coroutine_executor::get_global_executor())
        {}

        ~coroutine_state() override {
            if (m_handle) {
                m_handle.destroy();
            }
        }

        handle_type get_handle() {
            return m_handle;
        }

        std::coroutine_handle<> get_erased_handle() override {
            return m_handle;
        }

        bool set_ready() override {
            return m_is_ready.exchange(true, std::memory_order_acq_rel);
        }

        bool done() override {
            return m_handle.done();
        }

        void resume() override {
            m_handle.resume();
        }

        bool is_ready() override {
            return m_is_ready.load(std::memory_order_acquire);
        }

        void set_result(typename bool_if_void_else_type<Ret>::type &&result) { m_coroutine_result = std::move(result); }

        typename bool_if_void_else_type<Ret>::ret get_result() {
            return m_coroutine_result.value();
        }

        void submit_to_executor(shared_ptr<promise_state_base, true> task) const AVALANCHE_NOEXCEPT {
            m_executor.push_coroutine(std::move(task));
        }

    private:
        handle_type m_handle;
        std::atomic<bool> m_is_ready;
        optional<typename bool_if_void_else_type<Ret>::type> m_coroutine_result{};
        coroutine_executor_base& m_executor;
    };
    /////////////////////////


}

    template <typename T>
    using async = detail::async::coroutine<T>;

    template <typename T>
    void launch(T&& in) {
        in->submit_to_executor(in.get_state().template clone<promise_state_base>());
    }

}
