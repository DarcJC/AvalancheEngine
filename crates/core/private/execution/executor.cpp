#include "execution/executor.h"
#include "execution/generator.h"
#include <queue>
#include <mutex>
#include <condition_variable>


namespace avalanche::core::execution {
    class async_task_queue::impl {
    public:
        using Outer = async_task_queue;
        using item_type = typename Outer::erased_coroutine_handle_type;
        impl() = default;

        void push(const item_type task) {
            {
                std::lock_guard lock(m_mutex);
                m_tasks.push(task);
            }
            m_cv.notify_one();
        }

        item_type pop() {
            std::unique_lock lock(m_mutex);
            m_cv.wait(lock, [this] {
                return !m_tasks.empty();
            });
            const item_type task = m_tasks.top();
            m_tasks.pop();
            return task;
        }

    private:
        std::priority_queue<item_type> m_tasks;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };

    async_task_queue::async_task_queue() {
        m_impl = make_unique<impl>();
    }

    void async_task_queue::push(const erased_coroutine_handle_type coro_handle) {
        m_impl->push(coro_handle);
    }

    async_task_queue::erased_coroutine_handle_type async_task_queue::pop() {
        return m_impl->pop();
    }

    async_coroutine_executor::async_coroutine_executor(const size_type num_threads) : m_running(true), m_worker_threads(num_threads) {
        AVALANCHE_CHECK(num_threads < 128, "Trying to start too many threads");
        for (size_type i : range<size_type>(0, num_threads)) {
            m_worker_threads.emplace_back([this] {
                this->task_worker();
            });
        }
    }

    async_coroutine_executor::~async_coroutine_executor() = default;

    void async_coroutine_executor::enqueue_task(const handle_type coro_handle) {
        AVALANCHE_CHECK(!is_terminated(), "Executor has terminated. Enqueuing new tasks is a invalid access.");
        m_queue.push(coro_handle);
    }

    void async_coroutine_executor::terminate() {
        m_running = false;
    }

    bool async_coroutine_executor::is_terminated() const {
        return !m_running;
    }

    async_coroutine_executor& async_coroutine_executor::get_global_executor() {
        static async_coroutine_executor executor{};
        return executor;
    }

    void async_coroutine_executor::task_worker() {
        while (m_running) {
            handle_type handle =  m_queue.pop();
            handle.resume();
        }
    }

    async_coroutine_executor::async_awaiter::async_awaiter(const handle_type handle)
        : parent(get_global_executor()), coroutine_handle(handle)
    {}

    async_coroutine_executor::async_awaiter::async_awaiter(Outer& outer, const handle_type handle)
        : parent(outer)
        , coroutine_handle(handle)
    {}

    bool async_coroutine_executor::async_awaiter::await_ready() const AVALANCHE_NOEXCEPT {
        return false;
    }

    void async_coroutine_executor::async_awaiter::await_suspend(const handle_type coroutine_handle) AVALANCHE_NOEXCEPT {
        parent.enqueue_task(coroutine_handle);
    }

    void async_coroutine_executor::async_awaiter::await_resume() const noexcept {
    }
}
