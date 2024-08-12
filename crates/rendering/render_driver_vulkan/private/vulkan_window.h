#pragma once

#include <cstdint>
#include "container/vector.hpp"
#include "window_server.h"

namespace avalanche::rendering::vulkan {

    class RenderDevice;
    class VulkanWindow;

    class VulkanWindowServer final : public window::IWindowServer {
    public:
        VulkanWindowServer();

        window::IWindow* create_window(const window::WindowSettings& settings) override;
        void destroy_window(window::IWindow* window) override;
        void initialize() override;

    private:
        explicit VulkanWindowServer(RenderDevice& render_device);

        vector<VulkanWindow*> m_windows;

        RenderDevice* m_render_device = nullptr;
        friend class RenderDevice;
    };

    class VulkanWindow : public window::IWindow {
    public:
        explicit VulkanWindow(const window::WindowSettings& settings);

    private:
        friend class VulkanWindowServer;
    };

}
