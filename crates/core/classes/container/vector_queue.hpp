#pragma once

#include "container/vector.hpp"

namespace avalanche {

    template <typename T, AllocatorType Allocator = default_allocator<T>>
    class vector_queue_base : public vector_base<T, Allocator> {
    public:
        // Using constructor from vector_base
        using vector_base<T, Allocator>::vector_base;

        using Super = vector_base<T, Allocator>;
        using size_type = typename Super::size_type;
        using value_type = typename Super::value_type;
        using reference_type = typename Super::reference_type;
        using const_reference_type = typename Super::const_reference_type;

        reference_type queue_front() {
            return this->at(m_queue_cursor);
        }

        const_reference_type queue_front() const {
            return this->at(m_queue_cursor);
        }

        value_type queue_pop_front() {
            return this->at(m_queue_cursor++);
        }

        value_type queue_is_empty() const {
            return m_queue_cursor >= this->size();
        }

        void enqueue(const_reference_type value) {
            this->push_back(value);
        }

    private:
        size_type m_queue_cursor = 0;
    };

    template <typename T, AllocatorType Allocator = default_allocator<T>>
    using vector_queue = vector_queue_base<T, Allocator>;
}
