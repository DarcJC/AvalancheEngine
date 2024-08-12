#pragma once

#include "avalanche_render_device_export.h"
#include "render_enums.h"

namespace avalanche::rendering {

    class IRenderGraph;

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

    class AVALANCHE_RENDER_DEVICE_API IRenderDevice {
    public:
        IRenderDevice();
        virtual ~IRenderDevice();

        virtual void wait_for_device_idle() = 0;
    };

}
