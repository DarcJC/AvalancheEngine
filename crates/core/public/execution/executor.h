#pragma once

#include "avalanche_core_export.h"
#include "container/shared_ptr.hpp"
#include "polyfill.h"
#include <cstdint>
#include <coroutine>

namespace avalanche::core::execution {

    class promise_state_base {
    public:
        AVALANCHE_CORE_API virtual ~promise_state_base() = default;

        AVALANCHE_CORE_INTERNAL virtual std::coroutine_handle<> get_erased_handle() = 0;

        AVALANCHE_CORE_INTERNAL virtual bool set_ready() = 0;

        AVALANCHE_CORE_INTERNAL virtual bool done() = 0;

        AVALANCHE_CORE_INTERNAL virtual void resume() = 0;

        AVALANCHE_CORE_INTERNAL AVALANCHE_NO_DISCARD virtual bool is_ready() = 0;
    };

    class AVALANCHE_CORE_API coroutine_executor_base {
    public:
        using coroutine_handle = shared_ptr<promise_state_base, true>;
        using size_type = size_t;

        virtual ~coroutine_executor_base();

        virtual void push_coroutine(coroutine_handle handle) = 0;
        virtual void wait_for_all_jobs(size_type how_long_to_wait_ms);
        virtual void notify_before_handle_destroy(coroutine_handle handle);
    };

    class AVALANCHE_CORE_API sync_coroutine_executor : public coroutine_executor_base {
    public:
        using coroutine_handle = shared_ptr<promise_state_base, true>;

        void push_coroutine(coroutine_handle handle) override;

        static sync_coroutine_executor& get_global_executor();
    };

    class AVALANCHE_CORE_API threaded_coroutine_executor : public coroutine_executor_base {
    public:
        using coroutine_handle = shared_ptr<promise_state_base, true>;
        using coroutine_executor_base::size_type;
        static constexpr size_type default_thread_group_size = 4;

        explicit threaded_coroutine_executor(size_type num_threads = default_thread_group_size);
        ~threaded_coroutine_executor() override;

        AVALANCHE_NO_DISCARD bool is_empty();
        void terminate();
        void push_coroutine(coroutine_handle handle) override;
        void wait_for_all_jobs(size_type how_long_to_wait_ms) override;

        static threaded_coroutine_executor& get_global_executor();

    private:
        struct impl;
        impl* m_impl_;
    };

}
