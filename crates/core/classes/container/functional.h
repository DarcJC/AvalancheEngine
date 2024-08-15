#pragma once

#include <functional>
#include <utility>
#include "container/unique_ptr.hpp"


namespace avalanche {
    // 基类，用于类型擦除和统一调用接口
    template<typename Ret, typename... Args>
    class callable_base {
    public:
        virtual ~callable_base() = default;
        virtual Ret operator()(Args... args) = 0;
    };

    // default callable
    template<typename Func, typename Ret, typename... Args>
    class callable_holder : public callable_base<Ret, Args...> {
        Func func;

    public:
        callable_holder(Func f) : func(std::move(f)) {}

        Ret operator()(Args... args) override {
            return func(std::forward<Args>(args)...);
        }
    };

    // Member function ptr
    template<typename ClassType, typename Ret, typename... Args>
    class callable_holder<Ret(ClassType::*)(Args...), Ret, ClassType&, Args...> : public callable_base<Ret, ClassType&, Args...> {
        using Method = Ret(ClassType::*)(Args...);
        Method method;

    public:
        callable_holder(Method m) : method(m) {}

        Ret operator()(ClassType& obj, Args... args) override {
            return (obj.*method)(std::forward<Args>(args)...);
        }
    };

    // Allow binding member with the object
    template<typename Func, typename Obj, typename Ret, typename... Args>
    class callable_holder<std::pair<Func, Obj>, Ret, Args...> : public callable_base<Ret, Args...> {
        std::pair<Func, Obj> func_and_obj;

    public:
        callable_holder(std::pair<Func, Obj> p) : func_and_obj(std::move(p)) {}

        Ret operator()(Args... args) override {
            return (func_and_obj.second.*func_and_obj.first)(std::forward<Args>(args)...);
        }
    };

    template<typename>
    class function;

    template<typename Ret, typename... Args>
    class function<Ret(Args...)> {
        avalanche::unique_ptr<callable_base<Ret, Args...>> callable;

    public:
        function() = default;

        // construct from callable
        template<typename Func>
        function(Func f)
            : callable(avalanche::make_unique<callable_holder<Func, Ret, Args...>>(std::move(f))) {}

        // construct from function ptr
        template<typename ClassType>
        function(Ret(ClassType::*method)(Args...), ClassType& obj)
            : callable(avalanche::make_unique<callable_holder<std::pair<Ret(ClassType::*)(Args...), ClassType&>, Ret, Args...>>(std::make_pair(method, std::ref(obj)))) {}

        function(const function& other) = delete;
        function& operator=(const function& other) = delete;

        function(function&& other) noexcept = default;
        function& operator=(function&& other) noexcept = default;

        Ret operator()(Args... args) {
            return (*callable)(std::forward<Args>(args)...);
        }
    };
} // namespace avalanche
