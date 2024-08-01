//
// Created by DarcJC on 2024/3/5.
//
#pragma once

#include <functional>
#include <optional>
#include <vector>

namespace avalanche::core {
    template<typename... Args>
    class MulticastDelegate {
    public:
        using DelegateType = std::function<void(Args...)>;

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
        std::vector<DelegateType> delegates;
    };

    template<typename R, typename... Args>
    class UnicastDelegate {
    public:
        using DelegateType = std::function<R(Args...)>;

        void bind(const DelegateType& delegate) {
            this->delegate = delegate;
        }

        void unbind() {
            this->delegate.reset();
        }

        std::optional<R> invoke(Args... args) {
            if (delegate) {
                return (*delegate)(args...);
            }
            return std::nullopt;
        }

    private:
        std::optional<DelegateType> delegate;
    };
}
