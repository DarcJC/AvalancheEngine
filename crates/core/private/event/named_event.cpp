#include "event/named_event.h"

#include <container/vector.hpp>
#include <unordered_map>


namespace avalanche::core {

    struct CallbackNode {
        CallbackNode* next;
        INamedEvent::CallbackType callback;
    };

    class NamedEvent : public INamedEvent {
        std::unordered_map<string, CallbackNode*> m_subscribers;

        // Begin INamedEvent interface
        void publish(const string& event_name, void *data) override {
            auto it = m_subscribers.find(event_name);
            if (it != m_subscribers.end()) {
                auto* current = it->second;
                while (current != nullptr) {
                    current->callback(data);
                    current = current->next;
                }
            }
        }

        CancellerType subscribe(const string& event_name, CallbackType callback) override {
            auto* new_node = new CallbackNode {
                .next = nullptr,
                .callback = std::move(callback),
            };
            if (!m_subscribers.contains(event_name)) {
                m_subscribers[event_name] = new_node;
            } else {
                CallbackNode* current = m_subscribers[event_name];
                while (current != nullptr && current->next != nullptr) {
                    current = current->next;
                }
                AVALANCHE_CHECK(current != nullptr, "Internal Error");
                current->next = new_node;
            }

            // Return a lambda to provide cancel ability
            return [this, event_name, new_node] {
                auto it = m_subscribers.find(event_name);
                if (it != m_subscribers.end()) {
                    CallbackNode* current = it->second;
                    CallbackNode* prev = nullptr;
                    while (current != nullptr && current != new_node) {
                        prev = current;
                        current = current->next;
                    }
                    if (current == new_node) {
                        if (prev != nullptr) {
                            prev->next = current->next;
                        } else {
                            it->second = current->next; // update head
                        }
                        delete current;
                    }
                }
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
