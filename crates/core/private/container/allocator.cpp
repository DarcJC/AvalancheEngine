#include "container/allocator.hpp"

avalanche::bad_alloc::bad_alloc(): simple_error("Bad alloc") {}

void * avalanche::allocate_memory(const size_t bytes, const void *hint) {
    if (bytes > std::numeric_limits<size_t>::max())
        raise_expection(bad_alloc());

        if (auto p = std::malloc(bytes))
            return p;

        raise_expection(bad_alloc());
        return nullptr; // mute control flow analyser
}

void avalanche::deallocate_memory(void *pointer, size_t bytes) {
    std::free(pointer);
}
