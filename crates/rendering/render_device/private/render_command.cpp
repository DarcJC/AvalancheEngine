#include "render_command.h"


namespace avalanche::rendering {

    unique_ptr<IComputePassEncoder> ICommandEncoder::begin_ray_tracing_pass(const ComputePassDesc &) {
        AVALANCHE_TODO("Ray tracing doesn't implemented");
        return nullptr;
    }
} // namespace avalanche::rendering
