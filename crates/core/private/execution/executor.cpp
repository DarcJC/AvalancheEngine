#include "execution/executor.h"
#include "execution/generator.h"
#include "container/vector.hpp"
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>


namespace avalanche::core::execution {
    struct threaded_coroutine_executor::impl {
        explicit impl(const size_type num_threads) : m_threads(num_threads) {
            for (AVALANCHE_MAYBE_UNUSED auto i : range<size_t>(0, num_threads)) {
                m_threads.emplace_back([this]() {
                    worker();
                });
            }
        }

        ~impl() {
            terminate();
        }

        AVALANCHE_NO_DISCARD bool is_empty() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_threads.is_empty();
        }

        void terminate() {
            m_is_running = false;
            m_threads.clear();
        }

        void push(coroutine_handle handle) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_queue.push(std::move(handle));
            }
            m_cv.notify_one();
        }

        AVALANCHE_NO_DISCARD coroutine_handle pop() {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this] {
                return !m_queue.empty();
            });
            const coroutine_handle top = m_queue.front();
            m_queue.pop();
            return top;
        }

        void wait_for_all_jobs(size_type how_long_to_wait_ms) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (how_long_to_wait_ms > 0) {
                const auto timeout = std::chrono::milliseconds(how_long_to_wait_ms);
                m_cv_empty_check.wait_for(lock, timeout, [this] {
                    return m_queue.empty();
                });
            } else {
                m_cv_empty_check.wait(lock, [this] {
                    return m_queue.empty();
                });
            }
        }

        void worker() {
            while (m_is_running) {
                if (coroutine_handle handle = pop(); handle) {
                    if (!handle->done()) {
                        handle->resume();
                    }
                    if (m_queue.empty()) {
                        m_cv_empty_check.notify_all();
                    }
                }
            }
        }

        std::atomic<bool> m_is_running{true};
        std::queue<coroutine_handle> m_queue{};
        std::mutex m_mutex{};
        std::condition_variable m_cv{};
        std::condition_variable m_cv_empty_check{};
        vector<std::jthread> m_threads;
    };

    coroutine_executor_base::~coroutine_executor_base() = default;

    void coroutine_executor_base::wait_for_all_jobs(size_type how_long_to_wait_ms) {}

    void coroutine_executor_base::notify_before_handle_destroy(coroutine_handle handle) {}

    void sync_coroutine_executor::push_coroutine(const coroutine_handle handle) {
        if (handle && !handle->done()) {
            handle->resume();
        }
    }

    sync_coroutine_executor & sync_coroutine_executor::get_global_executor() {
        static sync_coroutine_executor executor{};
        return executor;
    }

    threaded_coroutine_executor::threaded_coroutine_executor(const size_type num_threads)
        : m_impl_(new impl(num_threads)){}

    threaded_coroutine_executor::~threaded_coroutine_executor() {
        delete m_impl_;
    }

    bool threaded_coroutine_executor::is_empty() {
        return m_impl_->is_empty();
    }

    void threaded_coroutine_executor::terminate() {
        m_impl_->terminate();
    }

    void threaded_coroutine_executor::push_coroutine(coroutine_handle handle) {
        m_impl_->push(std::move(handle));
    }

    void threaded_coroutine_executor::wait_for_all_jobs(size_type how_long_to_wait_ms) {
        m_impl_->wait_for_all_jobs(how_long_to_wait_ms);
    }

    threaded_coroutine_executor& threaded_coroutine_executor::get_global_executor() {
        static threaded_coroutine_executor executor{};
        return executor;
    }
}
