#pragma once


namespace avalanche {

    template <typename T, AllocatorType Allocator = default_allocator<T>>
    class vector_queue_base : public vector_base<T, Allocator> {
    public:
        // Using constructor from vector_base
        using Super = vector_base<T, Allocator>;
        using vector_base<T, Allocator>::vector_base;
        using size_type = Super::size_type;
        using value_type = Super::value_type;
        using reference_type = Super::reference_type;
        using const_reference_type = Super::const_reference_type;

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

    private:
        size_type m_queue_cursor = 0;
    };
}
