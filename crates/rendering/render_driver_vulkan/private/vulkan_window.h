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

        void register_external_window(window::IWindow *window) override;

        void initialize() override;
        void deinitialize() override;

        void tick(duration_type delta_time) override;

    private:
        explicit VulkanWindowServer(RenderDevice& render_device);

        vector<VulkanWindow*> m_windows;
        vector<VulkanWindow*> m_ticking_windows;

        RenderDevice* m_render_device = nullptr;
        friend class RenderDevice;
    };

    class VulkanWindow : public window::IWindow {
    public:
        explicit VulkanWindow(const window::WindowSettings& settings);
        ~VulkanWindow() override;

    private:
        friend class VulkanWindowServer;
    };

}
