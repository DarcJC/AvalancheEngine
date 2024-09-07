#pragma once
#include <container/string.hpp>
#include <container/functional.h>

namespace avalanche::core {

    class INamedEvent {
    public:
        using CallbackType = function<void(void*)>;
        using CancellerType = function<void()>;

        static INamedEvent& get();

        virtual ~INamedEvent();

        virtual void publish(const string& event_name, void* data) = 0;
        virtual CancellerType subscribe(const string& event_name, CallbackType callback) = 0;
    };

} // namespace avalanche::core
