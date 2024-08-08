#pragma once

#include "avalanche_core_export.h"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"
#include "execution/coroutine.h"
#include <cstdint>
#include <coroutine>
#include <thread>

namespace avalanche::core::execution {

    class AVALANCHE_CORE_API async_task_queue final {
    public:
        using erased_coroutine_handle_type = std::coroutine_handle<void>;

        async_task_queue();
        ~async_task_queue();

        void push(erased_coroutine_handle_type coro_handle);
        erased_coroutine_handle_type pop();
        bool is_empty();

    private:
        class impl;
        impl* m_impl;
    };

    class AVALANCHE_CORE_API async_coroutine_executor {
    public:
        struct async_awaiter;
        using size_type = size_t;
        using handle_type = std::coroutine_handle<>;
        static constexpr size_type default_threads_to_start = 4;

        explicit async_coroutine_executor(size_type num_threads = default_threads_to_start);
        ~async_coroutine_executor();

        async_coroutine_executor(const async_coroutine_executor&) = delete;
        async_coroutine_executor& operator=(const async_coroutine_executor&) = delete;

        async_coroutine_executor(async_coroutine_executor&& other) AVALANCHE_NOEXCEPT;
        async_coroutine_executor& operator=(async_coroutine_executor&& other) AVALANCHE_NOEXCEPT;
        void swap(async_coroutine_executor& other) AVALANCHE_NOEXCEPT;

        void enqueue_task(handle_type coro_handle);
        void terminate();
        AVALANCHE_NO_DISCARD bool is_terminated() const AVALANCHE_NOEXCEPT;
        AVALANCHE_NO_DISCARD bool is_queue_empty() const AVALANCHE_NOEXCEPT;

        static async_coroutine_executor& get_global_executor();

    private:
        class impl;
        impl* m_impl;
    };

    struct async_coroutine_executor::async_awaiter {
        using Outer = async_coroutine_executor;
        using handle_type = std::coroutine_handle<>;

        Outer& parent;
        handle_type* coroutine_handle = nullptr;

        explicit async_awaiter(const handle_type& handle)
            : parent(get_global_executor()), coroutine_handle(new handle_type(handle)) {
            // parent.enqueue_task(*coroutine_handle);
        }

        async_awaiter(Outer& outer, const handle_type& handle)
            : parent(outer)
            , coroutine_handle(new handle_type(handle)) {
            // parent.enqueue_task(*coroutine_handle);
        }

        ~async_awaiter() {
            delete coroutine_handle;
        }

        bool await_ready() const AVALANCHE_NOEXCEPT {
            return false;
        }

        void await_suspend(const handle_type& coroutine_handle) AVALANCHE_NOEXCEPT {
            parent.enqueue_task(coroutine_handle);
        }

        void await_resume() const AVALANCHE_NOEXCEPT {}
    };

    template <typename T = void>
    using async = coroutine<T, async_coroutine_executor::async_awaiter>;

    using async_void = async<void>;

    template <typename Awaitable>
    void launch(Awaitable a) {
        async_coroutine_executor::get_global_executor().enqueue_task(a.release());
    }
}
