#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "container/unique_ptr.hpp"
#include "render_device.h"


namespace avalanche::rendering::vulkan {

    class AVALANCHE_RENDER_DRIVER_VULKAN_API RenderDevice final : public IRenderDevice {
    public:
        explicit RenderDevice(const DeviceSettings& settings);
        ~RenderDevice() override;

        // Begin IRenderDevice interface
        EGraphicsAPIType get_graphics_api_type() override;
        void wait_for_device_idle() override;
        void enable_display_support() override;
        void add_pending_delete_resource(IResource *resource) override;
        // End IRenderDevice interface

        AVALANCHE_NO_DISCARD class Context& get_context() const;

        static unique_ptr<RenderDevice> create_instance(const DeviceSettings& settings);
    private:
        class Impl;
        Impl* m_impl;
    };

}
