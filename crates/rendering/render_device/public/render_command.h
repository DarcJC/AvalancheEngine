#pragma once

#include "render_resource.h"


namespace avalanche::rendering {
    struct RenderPassDesc;
    struct ComputePassDesc;

    class IRenderPassEncoder {
    public:
        virtual ~IRenderPassEncoder() = default;
    };

    class IComputePassEncoder {
    public:
        virtual ~IComputePassEncoder() = default;
    };

    class IRayTracingPassEncoder {
    public:
        virtual ~IRayTracingPassEncoder() = default;
    };

    class ICommandEncoder {
    public:
        virtual ~ICommandEncoder() = default;

        virtual unique_ptr<IRenderPassEncoder> begin_render_pass(const RenderPassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_compute_pass(const ComputePassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_ray_tracing_pass(const ComputePassDesc&);
    };

} // namespace avalanche::rendering
