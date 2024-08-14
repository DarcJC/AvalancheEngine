#include "vulkan_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace avalanche::rendering::vulkan {

    VulkanWindowServer::VulkanWindowServer() : m_render_device(nullptr), m_windows(1) {}

    window::IWindow* VulkanWindowServer::create_window(const window::WindowSettings& settings) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_REFRESH_RATE, settings.refresh_rate);
        auto* window = new VulkanWindow(settings);
        m_windows.emplace_back(window);

        return window;
    }

    void VulkanWindowServer::destroy_window(window::IWindow *window) {
        if (m_windows.contains(static_cast<VulkanWindow *>(window))) {
            m_windows.remove(static_cast<VulkanWindow *>(window));
        }
        delete window;
    }

    void VulkanWindowServer::register_external_window(window::IWindow *window) {
        AVALANCHE_CHECK(nullptr != window, "Invalid window");
        m_windows.emplace_back(static_cast<VulkanWindow*>(window));
    }

    void VulkanWindowServer::initialize() {
        // Other step for vulkan API has finished in context initialization
        AVALANCHE_LOGGER.info("Initialized Window Server with vulkan context");
    }

    VulkanWindowServer::VulkanWindowServer(RenderDevice& render_device)
        : m_render_device(&render_device)
    {}

    void VulkanWindowServer::deinitialize() {
        for (auto *window: m_windows) {
            delete window;
        }
        m_windows.clear();
    }

    void VulkanWindowServer::tick(duration_type delta_time) {
        IWindowServer::tick(delta_time);
        m_ticking_windows = m_windows;
        for (VulkanWindow* window: m_ticking_windows) {
            window->tick(delta_time);
        }
    }

    VulkanWindow::VulkanWindow(const window::WindowSettings &settings)
        : window::IWindow(glfwCreateWindow(settings.width, settings.height, settings.title.data(), settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr))
    {}

    VulkanWindow::~VulkanWindow() {
        glfwDestroyWindow(m_window);
    }
} // namespace avalanche::rendering::vulkan
