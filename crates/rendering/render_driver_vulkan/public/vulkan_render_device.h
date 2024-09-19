#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "container/unique_ptr.hpp"
#include "render_device.h"
#include "render_resource.h"

namespace avalanche::rendering::vulkan {

    class AVALANCHE_RENDER_DRIVER_VULKAN_API RenderDevice : public IRenderDevice {
    public:
        static constexpr auto graphics_api_type = EGraphicsAPIType::Vulkan;

        static unique_ptr<RenderDevice> create_instance(const DeviceSettings& settings);
    };

}
