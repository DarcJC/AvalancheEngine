#pragma once

#include "avalanche_core_export.h"
#include <cstdint>
#include <coroutine>

namespace avalanche::core::execution {

    class AVALANCHE_CORE_API coroutine_executor_base {
    public:
        using coroutine_handle = std::coroutine_handle<void>;

        virtual ~coroutine_executor_base();

        virtual void push_coroutine(coroutine_handle handle) = 0;
    };

    class AVALANCHE_CORE_API sync_coroutine_executor : public coroutine_executor_base {
    public:
        using coroutine_handle = std::coroutine_handle<void>;

        void push_coroutine(coroutine_handle handle) override;

        static sync_coroutine_executor& get_global_executor();
    };

    class AVALANCHE_CORE_API threaded_coroutine_executor : public coroutine_executor_base {
    public:
        using coroutine_handle = std::coroutine_handle<void>;
        using size_type = size_t;
        static constexpr size_type default_thread_group_size = 4;

        explicit threaded_coroutine_executor(size_type num_threads = default_thread_group_size);
        ~threaded_coroutine_executor() override;

        AVALANCHE_NO_DISCARD bool is_empty();
        void terminate();
        void push_coroutine(coroutine_handle handle) override;

        static threaded_coroutine_executor& get_global_executor();

    private:
        struct impl;
        impl* m_impl_;
    };

}
