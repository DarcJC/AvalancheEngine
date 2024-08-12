#include "vulkan_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace avalanche::rendering::vulkan {

    VulkanWindowServer::VulkanWindowServer() : m_render_device(nullptr) {}

    window::IWindow *VulkanWindowServer::create_window(window::WindowSettings settings) { return nullptr; }

    void VulkanWindowServer::destroy_window(window::IWindow *window) {}

    void VulkanWindowServer::initialize() {
        AVALANCHE_LOGGER.info("Initialized Window Server with vulkan context");
    }

    VulkanWindowServer::VulkanWindowServer(RenderDevice& render_device)
        : m_render_device(&render_device)
    {}

} // namespace avalanche::rendering::vulkan
