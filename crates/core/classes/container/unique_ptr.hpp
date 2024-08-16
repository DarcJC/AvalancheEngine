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

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        unique_ptr(unique_ptr&& moving) noexcept : ptr(moving.ptr) {
            moving.ptr = nullptr;
        }

        template <typename U = T>
        explicit unique_ptr(unique_ptr<U>&& other) noexcept {
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        template <typename U = T>
        unique_ptr<T>& operator=(unique_ptr<U>&& moving) noexcept {
            if ((void*)this != (void*)&moving) {
                delete ptr;
                ptr = moving.ptr;
                moving.ptr = nullptr;
            }
            return *this;
        }

        ~unique_ptr() {
            delete ptr;
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

        T* ptr;
    };

    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

}
