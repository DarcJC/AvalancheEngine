#pragma once

#include "avalanche_core_export.h"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"
#include "execution/coroutine.h"
#include <cstdint>
#include <coroutine>
#include <thread>

namespace avalanche::core::execution {

    class AVALANCHE_CORE_API async_task_queue {
    public:
        using erased_coroutine_handle_type = std::coroutine_handle<void>;

        async_task_queue();

        void push(erased_coroutine_handle_type coro_handle);
        erased_coroutine_handle_type pop();

    private:
        class impl;
        unique_ptr<impl> m_impl;
    };

    class AVALANCHE_CORE_API async_coroutine_executor {
    public:
        struct async_awaiter;
        using size_type = size_t;
        using handle_type = std::coroutine_handle<>;
        static constexpr size_type default_threads_to_start = 4;

        explicit async_coroutine_executor(size_type num_threads = default_threads_to_start);
        virtual ~async_coroutine_executor();

        void enqueue_task(handle_type coro_handle);

        void terminate();

        bool is_terminated() const;

        static async_coroutine_executor& get_global_executor();

    protected:
        virtual void task_worker();

    private:
        async_task_queue m_queue;
        std::atomic<bool> m_running{true};
        vector<std::jthread> m_worker_threads;
    };

    struct AVALANCHE_CORE_API async_coroutine_executor::async_awaiter {
        using Outer = async_coroutine_executor;
        using handle_type = std::coroutine_handle<>;

        Outer& parent;
        handle_type coroutine_handle;

        explicit async_awaiter(handle_type handle);
        async_awaiter(Outer& outer, handle_type handle);

        bool await_ready() const AVALANCHE_NOEXCEPT;
        void await_suspend(handle_type coroutine_handle) AVALANCHE_NOEXCEPT;
        void await_resume() const AVALANCHE_NOEXCEPT;
    };

    using async = coroutine<async_coroutine_executor::async_awaiter>;

    template <typename Awaitable>
    void launch(Awaitable a) {
        async_coroutine_executor::get_global_executor().enqueue_task(a.release());
    }
}
