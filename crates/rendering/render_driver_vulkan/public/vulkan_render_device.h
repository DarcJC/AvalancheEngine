#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "container/unique_ptr.hpp"
#include "render_device.h"

namespace avalanche::rendering::vulkan {

    using core::handle_t;

    class AVALANCHE_RENDER_DRIVER_VULKAN_API RenderDevice : public IRenderDevice {
    public:
        static unique_ptr<RenderDevice> create_instance(const DeviceSettings& settings);
    };

}
