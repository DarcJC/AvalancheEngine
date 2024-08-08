#pragma once

#include "avalanche_core_export.h"
#include "container/unique_ptr.hpp"
#include "container/vector.hpp"
#include <cstdint>
#include <coroutine>
#include <thread>
#include <functional>

namespace avalanche::core::execution {

    class coroutine_executor_base {
    };

    class sync_executor : public coroutine_executor_base {
    };

    class threaded_coroutine_executor : public coroutine_executor_base {
    public:

    private:
    };

}
