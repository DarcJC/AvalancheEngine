#include "event/named_event.h"

#include <container/vector.hpp>
#include <unordered_map>


namespace avalanche::core {

    class NamedEvent : public INamedEvent {
        // std::unordered_map<string, vector<CallbackType>> m_subscribers;

        // Begin INamedEvent interface
        void publish(const string& event_name, void *data) override {
            // auto it = m_subscribers.find(event_name);
            // if (it != m_subscribers.end()) {
            //     for (auto& callback : it->second) {
            //         callback(data);
            //     }
            // }
        }

        CancellerType subscribe(const string& event_name, CallbackType callback) override {
            // m_subscribers[event_name].push_back(callback);

            // Return a lambda to provide cancel ability
            // return [this, event_name, callback] {
            //     auto it = m_subscribers.find(event_name); if (it != m_subscribers.end()) {
            //         auto& deque = it->second;
            //         deque.remove(callback);
            //     }
            // };
            return [] {
                AVALANCHE_TODO("Impl me");
            };
        }
        // End INamedEvent interface

    };

    INamedEvent &INamedEvent::get() {
        static NamedEvent bus{};
        return bus;
    }

    INamedEvent::~INamedEvent() = default;

} // namespace avalanche::core
