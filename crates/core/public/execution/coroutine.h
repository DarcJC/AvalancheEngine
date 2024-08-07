#pragma once

#include "polyfill.h"
#include "container/optional.hpp"
#include <concepts>
#include <type_traits>
#include <coroutine>
#include <utility>


namespace avalanche::core::execution {

    template <typename T, typename AwaiterType>
    class coroutine {
    public:
        struct promise_type;
        using coroutine_handle_type = std::coroutine_handle<promise_type>;
        using awaiter_type = AwaiterType;

        explicit coroutine(coroutine_handle_type coroutine_handle) : m_handle(coroutine_handle) {}
        ~coroutine() {
            reset();
        }

        coroutine(const coroutine&) = delete;
        coroutine& operator=(const coroutine&) = delete;

        coroutine(coroutine &&other) AVALANCHE_NOEXCEPT {
            swap(other);
        }
        coroutine& operator=(coroutine &&other) AVALANCHE_NOEXCEPT {
            coroutine(std::move(other)).swap(*this);
            return *this;
        }

        void reset() {
            if (m_handle) {
                m_handle.destroy();
            }
        }

        void swap(coroutine &other) AVALANCHE_NOEXCEPT {
            using std::swap;
            swap(m_handle, other.m_handle);
        }

        void resume() const {
            AVALANCHE_CHECK(m_handle, "Trying to resume a non-existent coroutine handle");
            m_handle.resume();
        }

        coroutine_handle_type release() {
            AVALANCHE_CHECK(m_handle, "Trying to release a non-existent coroutine handle");
            coroutine_handle_type old = m_handle;
            m_handle = nullptr;
            return old;
        }

        awaiter_type operator co_await() {
            return awaiter_type(m_handle);
        }

    private:
        coroutine_handle_type m_handle;
    };

    template <typename AwaiterType>
    class coroutine<void, AwaiterType> {
    public:
        struct promise_type;
        using coroutine_handle_type = std::coroutine_handle<promise_type>;
        using awaiter_type = AwaiterType;

        explicit coroutine(coroutine_handle_type coroutine_handle) : m_handle(coroutine_handle) {}
        ~coroutine() {
            reset();
        }

        coroutine(const coroutine&) = delete;
        coroutine& operator=(const coroutine&) = delete;

        coroutine(coroutine &&other) AVALANCHE_NOEXCEPT {
            swap(other);
        }
        coroutine& operator=(coroutine &&other) AVALANCHE_NOEXCEPT {
            coroutine(std::move(other)).swap(*this);
            return *this;
        }

        void reset() {
            if (m_handle) {
                m_handle.destroy();
            }
        }

        void swap(coroutine &other) AVALANCHE_NOEXCEPT {
            using std::swap;
            swap(m_handle, other.m_handle);
        }

        void resume() const {
            AVALANCHE_CHECK(m_handle, "Trying to resume a non-existent coroutine handle");
            m_handle.resume();
        }

        coroutine_handle_type release() {
            AVALANCHE_CHECK(m_handle, "Trying to release a non-existent coroutine handle");
            coroutine_handle_type old = m_handle;
            m_handle = nullptr;
            return old;
        }

        awaiter_type operator co_await() {
            return awaiter_type(m_handle);
        }

    private:
        coroutine_handle_type m_handle;
    };

    template <typename T, typename AwaiterType>
    struct coroutine<T, AwaiterType>::promise_type {
        using Outer = coroutine<T, AwaiterType>;

        decltype(auto) get_return_object() {
            return coroutine(Outer::coroutine_handle_type::from_promise(*this));
        }

        static std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT { return {}; }
        static std::suspend_always final_suspend() AVALANCHE_NOEXCEPT { return {};}

        template <typename... Args>
        void return_value(Args&&...) AVALANCHE_NOEXCEPT {}

        static void unhandled_exception() { throw; }
    };

    template <typename AwaiterType>
    struct coroutine<void, AwaiterType>::promise_type {
        using Outer = coroutine<void, AwaiterType>;

        decltype(auto) get_return_object() {
            return coroutine(Outer::coroutine_handle_type::from_promise(*this));
        }

        static std::suspend_always initial_suspend() AVALANCHE_NOEXCEPT { return {}; }
        static std::suspend_always final_suspend() AVALANCHE_NOEXCEPT { return {};}

        void return_void() AVALANCHE_NOEXCEPT {}

        static void unhandled_exception() { throw; }
    };

}
