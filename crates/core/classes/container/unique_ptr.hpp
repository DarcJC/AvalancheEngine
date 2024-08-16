#pragma once

#include "polyfill.h"
#include <type_traits>
#include <utility>
#include <concepts>

namespace avalanche
{
    template<typename T>
    class unique_ptr {
    public:
        unique_ptr() : ptr(nullptr) {}
        unique_ptr(std::nullptr_t) : ptr(nullptr) {}

        template <typename U = T>
        explicit unique_ptr(U* p) : ptr(p) {}

        template <typename U = T>
        requires std::copy_constructible<T> && std::convertible_to<U*, T*>
        unique_ptr(const unique_ptr<U>& other)
            : unique_ptr(std::move(other.template clone<T>()))
        {}

        template <typename U = T>
        requires std::copy_constructible<T> && std::convertible_to<U*, T*>
        unique_ptr& operator=(const unique_ptr<U>& other) {
            if (static_cast<void *>(this) != static_cast<void *>(&other)) {
                reset();
                unique_ptr<T> temp = other.template clone<T>();
                ptr = std::exchange(temp.ptr, nullptr);
            }
            return *this;
        }

        template <typename U = T>
        requires std::convertible_to<U*, T*>
        unique_ptr(unique_ptr<U>&& moving) noexcept : ptr(moving.release()) {}

        template <typename U = T>
        requires std::convertible_to<U*, T*>
        unique_ptr<T>& operator=(unique_ptr<U>&& moving) noexcept {
            if (static_cast<void *>(this) != static_cast<void *>(&moving)) {
                reset(moving.release());
            }
            return *this;
        }

        ~unique_ptr() {
            reset();
        }

        T& operator*() const { return *ptr; }
        T* operator->() const { return ptr; }

        T* get() const { return ptr; }

        T* release() {
            T* temp = ptr;
            ptr = nullptr;
            return temp;
        }

        void reset(T* p = nullptr) {
            T* old = ptr;
            ptr = p;
            delete old;
        }

        AVALANCHE_NO_DISCARD bool is_null() const { return ptr == nullptr; }

        operator bool() const {
            return !is_null();
        }

        template <typename U = T>
        requires std::convertible_to<T*, U*>
        operator unique_ptr<U>() {
            return unique_ptr<U>(release());
        }

        template <typename U = T>
        bool operator==(const unique_ptr<U>& other) {
            return static_cast<void*>(ptr) == static_cast<void*>(other.ptr);
        }

        template <typename U = T>
        requires std::copy_constructible<T> && std::convertible_to<T*, U*>
        AVALANCHE_NO_DISCARD unique_ptr<U> clone() const {
            U* u = new T(*ptr);
            return unique_ptr<U>{u};
        }

    private:
        T* ptr;

        template <typename>
        friend class unique_ptr;
    };

    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}
