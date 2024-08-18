#pragma once


#include "resource.h"


namespace avalanche::rendering {
    using core::handle_t;

    struct FrameInFlight {
        handle_t final_render_target = handle_t::null_handle();
        handle_t render_finish_semaphore = handle_t::null_handle();
        handle_t swapchain_finish_semaphore = handle_t::null_handle();
    };
} // namespace avalanche::rendering
