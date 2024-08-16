#pragma once
#include "vulkan_render_device.h"
#include "vulkan_context.h"
#include "vulkan/vulkan.hpp"


namespace avalanche::rendering::vulkan {

    class RenderDeviceImpl : public RenderDevice {
    public:
        explicit RenderDeviceImpl(const DeviceSettings &settings);

        // Begin IRenderDevice interface
        EGraphicsAPIType get_graphics_api_type() override;
        void wait_for_device_idle() override;
        void enable_display_support() override;
        void add_pending_delete_resource(IResource *resource) override;
        // End IRenderDevice interface

        handle_t register_external_image(const vk::Image& image);

        AVALANCHE_NO_DISCARD Context& get_context() const;

    private:
        DeviceSettings m_settings{};
        unique_ptr<Context> m_context;
    };

} // namespace avalanche::rendering::vulkan

