#pragma once

#include "render_enums.h"

namespace avalanche::rendering {

    struct DeviceFeatures {
        bool ray_tracing = true;
        bool display = false;
        bool line_rasterization = true;
        bool mesh_shader = true;
        bool performance_query = false;
        bool debug = false;
        bool tile_based = false; // Not impl yet
    };

    struct DeviceSettings {
        DeviceFeatures required_features{};
        EGPUPowerPreference power_preference = EGPUPowerPreference::HighPerformance;
    };

    class RenderDevice {
    public:
        RenderDevice();
    };

}
