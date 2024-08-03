#include "container/exception.hpp"

avalanche::exception_base::~exception_base() {}

avalanche::simple_error::simple_error(const char *msg): m_what(msg) {}

const char * avalanche::simple_error::what() const noexcept {
    return m_what;
}

avalanche::out_of_range::out_of_range(): simple_error("Index out of bounds") {}

avalanche::invalid_access::invalid_access() : simple_error("Accessing invalid value") {}
