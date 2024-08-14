#pragma once

#include "avalanche_render_device_export.h"
#include "render_enums.h"

namespace avalanche::rendering {

    class IRenderGraph;
    class IResource;
    class ResourceHandle;

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

    class AVALANCHE_RENDER_DEVICE_API IRenderResourcePool {
    public:
        static IRenderResourcePool* new_pool();
        static void delete_pool(const IRenderResourcePool * pool);

        virtual ~IRenderResourcePool();

        virtual IResource* get_resource(const ResourceHandle& handle) = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API IRenderDevice {
    public:
        IRenderDevice();
        virtual ~IRenderDevice();

        virtual EGraphicsAPIType get_graphics_api_type() = 0;

        virtual void wait_for_device_idle() = 0;
        virtual void enable_display_support() = 0;
    };

}
