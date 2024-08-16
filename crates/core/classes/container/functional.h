#pragma once

#include <functional>
#include <utility>
#include "container/unique_ptr.hpp"

#include "logger.h"


namespace avalanche {

    enum class callable_type {
        Unknown,
        Function,
        MemberFunction,
        MemberFunctionWithObject,
    };

    // 基类，用于类型擦除和统一调用接口
    template<typename Ret, typename... Args>
    class callable_base {
    public:
        virtual ~callable_base() = default;
        virtual Ret operator()(Args... args) = 0;
        AVALANCHE_NO_DISCARD virtual callable_type type() const {
            return callable_type::Unknown;
        }
        AVALANCHE_NO_DISCARD virtual bool equal_to(const callable_base* other) const = 0;
        AVALANCHE_NO_DISCARD virtual unique_ptr<callable_base> clone() const = 0;
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

        AVALANCHE_NO_DISCARD callable_type type() const override {
            return callable_type::Function;
        }

        AVALANCHE_NO_DISCARD bool equal_to(const callable_base<Ret, Args...>* other) const override {
            if (other->type() != callable_type::Function) {
                return false;
            }
            const auto* casted = static_cast<const callable_holder<Func, Ret, Args...>*>(other);
            return func == casted->func;
        }

        unique_ptr<callable_base<Ret, Args...>> clone() const override {
            return { avalanche::make_unique<callable_holder<Func, Ret, Args...>>(func) };
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

        AVALANCHE_NO_DISCARD callable_type type() const override {
            return callable_type::MemberFunction;
        }

        AVALANCHE_NO_DISCARD bool equal_to(const callable_base<Ret, ClassType&, Args...>* other) const override {
            if (other->type() != callable_type::MemberFunction) {
                return false;
            }
            const auto* casted = static_cast<const callable_holder<Ret(ClassType::*)(Args...), Ret, ClassType&, Args...>*>(other);
            return method == casted->method;
        }

        unique_ptr<callable_base<Ret, ClassType&, Args...>> clone() const override {
            return { avalanche::make_unique<callable_holder<Ret(ClassType::*)(Args...), Ret, ClassType&, Args...>>(method) };
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

        AVALANCHE_NO_DISCARD callable_type type() const override {
            return callable_type::MemberFunctionWithObject;
        }

        AVALANCHE_NO_DISCARD bool equal_to(const callable_base<Ret, Args...>* other) const override {
            if (other->type() != callable_type::MemberFunctionWithObject) {
                return false;
            }
            const auto* casted = static_cast<const callable_holder<std::pair<Func, Obj>, Ret, Args...>*>(other);
            return func_and_obj.first == casted->func_and_obj.first && &func_and_obj.second == &casted->func_and_obj.second;
        }

        unique_ptr<callable_base<Ret, Args...>> clone() const override {
            return { avalanche::make_unique<callable_holder<std::pair<Func, Obj>, Ret, Args...>>(func_and_obj) };
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

        function(const function& other)
            : callable(other.callable ? other.callable->clone() : nullptr)
        {}

        function& operator=(const function& other) {
            if (this != &other) {
                callable = other.callable ? other.callable->clone() : nullptr;
            }
            return *this;
        }

        function(function&& other) AVALANCHE_NOEXCEPT
            : callable(std::exchange(other.callable, nullptr))
        {}

        function& operator=(function&& other) AVALANCHE_NOEXCEPT {
            if (this != &other) {
                callable = std::exchange(other.callable, nullptr);
            }
            return *this;
        }

        Ret operator()(Args... args) {
            AVALANCHE_CHECK(callable, "Trying to invoke nullptr function");
            return (*callable)(std::forward<Args>(args)...);
        }

        bool operator==(const function& other) const {
            if (callable && other.callable) {
                return callable->equal_to(other.callable.get());
            }
            return callable == other.callable;
        }
    };
} // namespace avalanche
