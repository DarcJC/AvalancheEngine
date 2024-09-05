#pragma once


#include "resource.h"


namespace avalanche::rendering {
    using core::handle_t;

    /**
     * @brief All required state to share in a frame
     */
    struct FrameInFlight {
        // The render target to draw result on
        handle_t final_render_target = handle_t::null_handle();
        // Semaphore to waiting for current frame been rendered
        handle_t render_finish_semaphore = handle_t::null_handle();
        // Semaphore to acquire/release of the final_render_target
        handle_t target_usage_semaphore = handle_t::null_handle();
        // Fence to wait all operations are done in current frame
        handle_t final_fence = handle_t::null_handle();
    };

} // namespace avalanche::rendering
