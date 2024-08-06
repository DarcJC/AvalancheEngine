#pragma once

#include "polyfill.h"
#include <concepts>
#include <type_traits>
#include <coroutine>


namespace avalanche::core::execution {

    template <typename T>
    class coroutine {
    public:
        struct promise_type;
        using coroutine_handle_type = std::coroutine_handle<promise_type>;
    };

    template <typename T>
    using task = coroutine<T>;

    template <typename T>
    class awaitable_base {
    };

    template <typename T>
    struct coroutine<T>::promise_type {
        using value_type = T;

        value_type await_transform(value_type&& value) {
            return std::forward<value_type>(value);
        }
    };

}
