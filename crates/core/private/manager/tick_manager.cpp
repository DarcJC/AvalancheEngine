#include "manager/tick_manager.h"
#include "container/vector.hpp"
#include "execution/generator.h"
#include <queue>
#include <shared_mutex>
#include <chrono>
#include <utility>


namespace avalanche::core {
    class TickManager : public ITickManager {
    public:

        using clock_type = std::chrono::steady_clock;
        using time_point_type = std::chrono::time_point<clock_type>;
        using duration_type = std::chrono::duration<typename ITickable::duration_type, std::milli>;

        TickManager()
            : m_is_shutdown(false)
            , m_previous_frame_time(clock_type::now())
        {}

        struct TickPair {
            ITickable* tickable;
            tick_group_t group;
        };

        struct PriorityCompartor {
            bool operator()(const TickPair& lhs, const TickPair& rhs) {
                // The smaller number has higher priority
                return lhs.group > rhs.group;
            }
        };

        void register_tickable(avalanche::core::ITickable *tickable, tick_group_t group) override {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_registered_tickable.push_back({ tickable, group });
        }

        void unregister_tickable(avalanche::core::ITickable* tickable) override {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            for (auto i : execution::range<size_t>(0, m_registered_tickable.size())) {
                const auto& [t, g] = m_registered_tickable[i];
                if (t == tickable) {
                    m_registered_tickable.remove_at(i);
                    return;
                }
            }
        }

        bool tick_frame() override {
            if (m_is_shutdown.load(std::memory_order_acquire)) {
                return false;
            }
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            const time_point_type time_point = std::exchange(m_previous_frame_time, clock_type::now());
            const duration_type duration = m_previous_frame_time - time_point;
            std::priority_queue<TickPair, std::vector<TickPair>, PriorityCompartor> queue { m_registered_tickable.begin(), m_registered_tickable.end() };
            for (; !queue.empty(); queue.pop()) {
                queue.top().tickable->tick(duration.count());
            }
            return true;
        }

        void shutdown() override {
            m_is_shutdown.store(true, std::memory_order_release);
        }

    private:
        std::atomic<bool> m_is_shutdown;
        std::shared_mutex m_mutex;
        vector<TickPair> m_registered_tickable{};
        time_point_type m_previous_frame_time;
    };

    ITickManager &ITickManager::get() {
        static TickManager manager{};
        return manager;
    }
} // namespace avalanche::core
