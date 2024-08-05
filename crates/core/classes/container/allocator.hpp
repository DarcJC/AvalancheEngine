#pragma once

#include <limits>
#include <type_traits>
#include "container/exception.hpp"

namespace avalanche {

    struct bad_alloc : simple_error {
        bad_alloc();
    };

    AVALANCHE_CORE_API void* allocate_memory(size_t bytes, const void* hint = nullptr);
    AVALANCHE_CORE_API void deallocate_memory(void* pointer, size_t bytes);

    template <typename Allocator>
    concept AllocatorType = requires(Allocator allocator, typename Allocator::value_type* ptr, typename Allocator::size_type n, const void* hint)
    {
        typename Allocator::value_type;

        { allocator.allocate(n, hint) } -> std::same_as<typename Allocator::pointer_type>;
        { allocator.deallocate(ptr, n) } AVALANCHE_NOEXCEPT -> std::same_as<void>;

        { allocator.construct(ptr) } -> std::same_as<void>;
        { allocator.destroy(ptr) } -> std::same_as<void>;

        { allocator.max_size() } AVALANCHE_NOEXCEPT -> std::same_as<typename Allocator::size_type>;
    };

    template <typename T>
    class simple_allocator {
    public:
        using value_type = T;
        using pointer_type = T*;
        using const_pointer_type = const T*;
        using reference_type = T&;
        using const_reference_type = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        simple_allocator() noexcept = default;

        template <typename U>
        simple_allocator(const simple_allocator<U>&) AVALANCHE_NOEXCEPT {}

        pointer_type allocate(size_type n, const void* hint = nullptr) {
            pointer_type memory = static_cast<pointer_type>(allocate_memory(n * sizeof(T)));
            return memory;
        }

        void deallocate(pointer_type pointer, size_type n) AVALANCHE_NOEXCEPT {
            deallocate_memory(pointer, n * sizeof(T));
        }

        template <typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
        }

        template <typename U>
        void destroy(U* p) {
            p->~U();
        }

        size_type max_size() const noexcept {
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }
    };

    template <typename T1, typename T2>
    bool operator==(const simple_allocator<T1>&, const simple_allocator<T2>&) AVALANCHE_NOEXCEPT {
        return true;
    }

    template <typename T1, typename T2>
    bool operator!=(const simple_allocator<T1>& a, const simple_allocator<T2>& b) AVALANCHE_NOEXCEPT {
        return !(a == b);
    }

    template <typename T>
    using default_allocator = simple_allocator<T>;
}
