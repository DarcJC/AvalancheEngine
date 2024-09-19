#include "vulkan_command_encoder.h"


namespace avalanche::rendering::vulkan {
    unique_ptr<IRenderPassEncoder> CommandEncoder::begin_render_pass(const RenderPassDesc &desc) { return nullptr; }

    unique_ptr<IComputePassEncoder> CommandEncoder::begin_compute_pass(const ComputePassDesc &desc) { return nullptr; }

    handle_t CommandEncoder::finish() {
        return handle_t::null_handle();
    }
} // namespace avalanche::rendering::vulkan
