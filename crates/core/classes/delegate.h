#pragma once

#include "container/functional.h"
#include "container/vector.hpp"
#include "container/optional.hpp"

namespace avalanche {
    template<typename... Args>
    class multicast_delegate {
    public:
        using DelegateType = function<void(Args...)>;

        void add(const DelegateType& delegate) {
            delegates.push_back(delegate);
        }

        void remove(const DelegateType& delegate) {
            delegates.erase(std::remove(delegates.begin(), delegates.end(), delegate), delegates.end());
        }

        void invoke(Args... args) {
            for (auto& delegate : delegates) {
                delegate(args...);
            }
        }

    private:
        vector<DelegateType> delegates;
    };

    template<typename R, typename... Args>
    class unicast_delegate {
    public:
        using DelegateType = function<R(Args...)>;

        void bind(const DelegateType& delegate_in) {
            this->delegate = delegate_in;
        }

        void unbind() {
            this->delegate.reset();
        }

        optional<R> invoke(Args... args) {
            if (delegate) {
                return (*delegate)(args...);
            }
            return std::nullopt;
        }

    private:
        optional<DelegateType> delegate;
    };
}
