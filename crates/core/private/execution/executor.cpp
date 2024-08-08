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
            const item_type task = m_tasks.front();
            m_tasks.pop();
            return task;
        }

        AVALANCHE_NO_DISCARD bool is_empty() AVALANCHE_NOEXCEPT {
            std::lock_guard lock(m_mutex);
            return m_tasks.empty();
        }

    private:
        std::queue<item_type> m_tasks;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };

    class async_coroutine_executor::impl {
    public:
        explicit impl(size_type num_threads = default_threads_to_start);
        virtual ~impl();

        void enqueue_task(handle_type coro_handle);
        void terminate();
        bool is_terminated() const;
        bool is_queue_empty();

    protected:
        void task_worker();

    private:
        async_task_queue m_queue;
        std::atomic<bool> m_running{true};
        vector<std::jthread> m_worker_threads;
    };


    async_task_queue::async_task_queue() {
        m_impl = new impl();
    }

    async_task_queue::~async_task_queue() {
        delete m_impl;
    }

    void async_task_queue::push(const erased_coroutine_handle_type coro_handle) {
        m_impl->push(coro_handle);
    }

    async_task_queue::erased_coroutine_handle_type async_task_queue::pop() {
        return m_impl->pop();
    }

    bool async_task_queue::is_empty() {
        return m_impl->is_empty();
    }

    async_coroutine_executor::impl::impl(const size_type num_threads) : m_running(true), m_worker_threads(num_threads) {
        AVALANCHE_CHECK(num_threads < 128, "Trying to start too many threads");
        for (size_type i : range<size_type>(0, num_threads)) {
            m_worker_threads.emplace_back([this] {
                this->task_worker();
            });
        }
    }

    async_coroutine_executor::impl::~impl() {
        terminate();
    }

    void async_coroutine_executor::impl::enqueue_task(const handle_type coro_handle) {
        AVALANCHE_CHECK(!is_terminated(), "Executor has terminated. Enqueuing new tasks is a invalid access.");
        m_queue.push(coro_handle);
    }

    void async_coroutine_executor::impl::terminate() {
        m_running = false;
    }

    bool async_coroutine_executor::impl::is_terminated() const {
        return !m_running;
    }

    bool async_coroutine_executor::impl::is_queue_empty() {
        return m_queue.is_empty();
    }

    async_coroutine_executor::async_coroutine_executor(size_type num_threads) : m_impl(new impl(num_threads)) {}

    async_coroutine_executor::~async_coroutine_executor() {
        delete m_impl;
    }

    async_coroutine_executor::async_coroutine_executor(async_coroutine_executor &&other) AVALANCHE_NOEXCEPT : m_impl(other.m_impl) {
        other.m_impl = nullptr;
    }

    async_coroutine_executor& async_coroutine_executor::operator=(async_coroutine_executor &&other) noexcept {
        async_coroutine_executor(std::move(other)).swap(*this);
        return *this;
    }

    void async_coroutine_executor::swap(async_coroutine_executor &other) noexcept {
        using std::swap;
        swap(m_impl, other.m_impl);
    }

    void async_coroutine_executor::enqueue_task(handle_type coro_handle) {
        AVALANCHE_CHECK(m_impl, "Invalid dispatcher in async_coroutine");
        m_impl->enqueue_task(coro_handle);
    }

    void async_coroutine_executor::terminate() {
        AVALANCHE_CHECK(m_impl, "Invalid dispatcher in async_coroutine");
        m_impl->terminate();
    }

    bool async_coroutine_executor::is_terminated() const AVALANCHE_NOEXCEPT {
        return m_impl && m_impl->is_terminated();
    }

    bool async_coroutine_executor::is_queue_empty() const AVALANCHE_NOEXCEPT {
        AVALANCHE_CHECK(m_impl, "Invalid dispatcher in async_coroutine");
        return m_impl->is_queue_empty();
    }

    async_coroutine_executor& async_coroutine_executor::get_global_executor() {
        static async_coroutine_executor executor{};
        return executor;
    }

    void async_coroutine_executor::impl::task_worker() {
        while (m_running) {
            if (handle_type handle =  m_queue.pop(); handle) {
                if (!handle.done()) {
                    handle.resume();
                }
            }
        }
    }
}
