#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "render_device.h"
#include "container/unique_ptr.hpp"


namespace avalanche::rendering::vulkan {

    class AVALANCHE_RENDER_DRIVER_VULKAN_API RenderDevice final : public IRenderDevice {
    public:
        explicit RenderDevice(const DeviceSettings& settings);
        ~RenderDevice() override;

        void wait_for_device_idle() override;

        static unique_ptr<RenderDevice> create_instance(const DeviceSettings& settings);
    private:

        class Impl;
        Impl* m_impl;
    };

}
