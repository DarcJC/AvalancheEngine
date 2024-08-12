#pragma once

#include <cstdint>
#include "window_server.h"

namespace avalanche::rendering::vulkan {

    class RenderDevice;

    class VulkanWindowServer final : public window::IWindowServer {
    public:
        VulkanWindowServer();

        window::IWindow* create_window(window::WindowSettings settings) override;
        void destroy_window(window::IWindow* window) override;
        void initialize() override;

    private:
        explicit VulkanWindowServer(RenderDevice& render_device);

        RenderDevice* m_render_device = nullptr;
        friend class RenderDevice;
    };

}
