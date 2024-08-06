#include "container/exception.hpp"
#include "logger.h"
#include <exception>

namespace avalanche {
    exception_base::~exception_base() {}

    simple_error::simple_error(const char *msg): m_what(msg) {}

    const char * simple_error::what() const noexcept {
        return m_what;
    }

    out_of_range::out_of_range(): simple_error("Index out of bounds") {}

    invalid_access::invalid_access() : simple_error("Accessing invalid value") {}
}


namespace {
    void uncaught_exception_handler() {
        try {
            throw; // Rethrow current exception
        } catch (const avalanche::exception_base& err) {
            AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Critical, "Uncaught avalanche exception:");
            AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Critical, "{}", err.what());
        } catch (const std::exception& err) {
            AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Critical, "Uncaught std exception:");
            AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Critical, "{}", err.what());
        } catch (...) {
            AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Critical, "Uncaught unknown exception");
        }
        std::abort();
    }

    AVALANCHE_MAYBE_UNUSED struct ExceptionHandlerRegistration {
        ExceptionHandlerRegistration() {
            std::set_terminate(uncaught_exception_handler);
        }
    } reg_;
}
