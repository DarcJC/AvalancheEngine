#pragma once

#include <exception>
#include <type_traits>
#include "logger.h"

namespace avalanche {
    template <typename T>
    concept HasWhatFunction = requires(T a) {
        { a.what() } -> std::same_as<const char*>;
    };

    template <HasWhatFunction ExpType>
    inline void raise_exception(ExpType exp) {
#if __cpp_exceptions >= 199711L
        throw exp;
#else
        AVALANCHE_CHECK(false, "Fatal exception occurred:\n\t{}", exp.what());
        std::terminate();
#endif
    }

    struct AVALANCHE_CORE_API exception_base {
        virtual ~exception_base();

        AVALANCHE_NO_DISCARD virtual const char* what() const AVALANCHE_NOEXCEPT = 0;
    };

    struct AVALANCHE_CORE_API simple_error : exception_base {
        explicit simple_error(const char* msg);

        AVALANCHE_NO_DISCARD const char* what() const AVALANCHE_NOEXCEPT override;

    protected:
        const char* m_what = nullptr;
    };

    struct out_of_range final : simple_error {
        AVALANCHE_CORE_API out_of_range();
    };

    struct invalid_access final : simple_error {
        AVALANCHE_CORE_API invalid_access();
    };

}
