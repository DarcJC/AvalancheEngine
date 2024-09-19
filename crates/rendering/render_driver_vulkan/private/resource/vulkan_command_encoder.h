#pragma once
#include <render_command.h>


namespace avalanche::rendering::vulkan {
    class CommandEncoder : public ICommandEncoder {
    public:
        unique_ptr<IRenderPassEncoder> begin_render_pass(const RenderPassDesc& desc) override;
        unique_ptr<IComputePassEncoder> begin_compute_pass(const ComputePassDesc& desc) override;
        handle_t finish() override;
    };
} // namespace avalanche::rendering::vulkan
