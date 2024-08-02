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
    void raise_expection(ExpType exp) {
#if __cpp_exceptions >= 199711L
        throw exp;
#else
        AVALANCHE_CHECK(false, "Fatal exception occurred:\n\t{}", exp.what());
        std::terminate();
#endif
    }

    struct AVALANCHE_CORE_API exception_base {
        virtual ~exception_base();

        virtual const char* what() const AVALANCHE_NOEXCEPT = 0;
    };

    struct simple_error : exception_base {
        explicit simple_error(const char* msg);

        const char* what() const AVALANCHE_NOEXCEPT override;

    protected:
        const char* m_what = nullptr;
    };

    struct out_of_range final : simple_error {
        out_of_range();
    };
}
