#pragma once


#include <cstdint>
#include "render_enums.h"
#include "render_resource.h"


namespace avalanche::rendering {

    struct descriptor_tag {};

    struct ImageViewDesc : descriptor_tag {
        handle_t image;
        EFormat format = EFormat::RGBA8_Unorm_SRGB;
        EImageDimension dimension = EImageDimension::Image1D;
        EImageAspect aspect = EImageAspect::Color;
        uint32_t base_mip_level = 0;
        // Leaves zero to using the remaining mip levels
        uint32_t num_levels = 1;
        uint32_t base_array_layer = 0;
        // Leaves zero to using the remaining array layers
        uint32_t num_layers = 1;
    };

    struct CommandBufferDesc : descriptor_tag {
        handle_t pool;
        bool is_primary = false;
    };

    struct FenceDesc : descriptor_tag {
        bool signaled = false;
    };

    struct RenderPassDesc {
        vector<handle_t> color_attachments;
        handle_t depth_stencil_attachment;
        size_t max_draw_call_count = 50000000ULL;
    };

    struct ComputePassDesc {
    };

    struct RenderBundleDesc {
    };

}
