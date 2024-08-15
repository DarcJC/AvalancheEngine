#include "vulkan_window.h"

#include <GLFW/glfw3.h>
#include <vector>
#include "vulkan_macro.h"
#include "vulkan_context.h"
#include "vulkan_render_device.h"

namespace avalanche::rendering::vulkan {

    RenderDevice* to_vulkan_render_device(IRenderDevice* device) {
        if (device && device->get_graphics_api_type() == EGraphicsAPIType::Vulkan) {
            return static_cast<RenderDevice*>(device);
        }
        return nullptr;
    }

    window::IWindow* VulkanWindowServer::create_window(const window::WindowSettings& settings) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_REFRESH_RATE, settings.refresh_rate);
        auto* window = new VulkanWindow(settings, m_render_device);
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

    VulkanWindow::VulkanWindow(const window::WindowSettings &settings, RenderDevice* render_device)
        : window::IWindow(settings, render_device)
    {
        AVALANCHE_CHECK(nullptr != m_render_device, "Invalid render_device");
        VkSurfaceKHR surface;
        AVALANCHE_CHECK_VK_SUCCESS(glfwCreateWindowSurface(to_vulkan_render_device(m_render_device)->get_context().instance(), m_window, nullptr, &surface));
        m_surface = surface;
        create_swapchain();
    }

    VulkanWindow::~VulkanWindow() { glfwDestroyWindow(m_window); }

    void VulkanWindow::create_swapchain() {
        RenderDevice *render_device = to_vulkan_render_device(m_render_device);
        vk::PhysicalDevice physical_device = render_device->get_context().physical_device();

        auto surface_capabilities = physical_device.getSurfaceCapabilitiesKHR(m_surface);
        std::vector<vk::SurfaceFormatKHR> surface_formats = physical_device.getSurfaceFormatsKHR(m_surface);
        std::vector<vk::PresentModeKHR> surface_present_modes = physical_device.getSurfacePresentModesKHR(m_surface);

        vk::SurfaceFormatKHR surface_format = surface_formats.front();
        {
            if (surface_format.format == vk::Format::eUndefined) {
                surface_format.format = vk::Format::eR8G8B8A8Srgb;
                surface_format.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
            }
        }

        vk::PresentModeKHR present_mode = surface_present_modes.front();
        {
            for (const vk::PresentModeKHR &mode: surface_present_modes) {
                if (m_settings.low_input_latency) {
                    if (m_settings.vsync) {
                        if (mode == vk::PresentModeKHR::eMailbox) {
                            present_mode = mode;
                            break;
                        }
                    } else {
                        if (mode == vk::PresentModeKHR::eImmediate) {
                            present_mode = mode;
                            break;
                        }
                    }
                }
            }
        }

        uint32_t image_count = surface_capabilities.minImageCount + 1;
        if (surface_capabilities.maxImageCount > 0 && image_count > surface_capabilities.maxImageCount) {
            image_count = surface_capabilities.maxImageCount;
        }

        vk::Extent2D extent;
        if (surface_capabilities.currentExtent.width != UINT32_MAX) {
            extent = surface_capabilities.currentExtent;
        } else {
            extent.setWidth(std::clamp(m_settings.width, surface_capabilities.minImageExtent.width,
                                       surface_capabilities.maxImageExtent.width));
            extent.setHeight(std::clamp(m_settings.height, surface_capabilities.minImageExtent.height,
                                        surface_capabilities.maxImageExtent.height));
        }

        vk::SwapchainCreateInfoKHR swapchain_create_info{};
        swapchain_create_info.setSurface(m_surface)
                .setMinImageCount(image_count)
                .setImageFormat(surface_format.format)
                .setImageColorSpace(surface_format.colorSpace)
                .setImageExtent(extent)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                .setImageSharingMode(vk::SharingMode::eExclusive)
                .setQueueFamilyIndices({})
                .setPreTransform(surface_capabilities.currentTransform)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setPresentMode(present_mode)
                .setClipped(true)
                .setOldSwapchain(nullptr);

        if (m_swapchain) {
            swapchain_create_info.setOldSwapchain(m_swapchain);
        }

        m_swapchain = render_device->get_context().device().createSwapchainKHR(swapchain_create_info);
    }

    void VulkanWindow::clean_swapchain_images() {
    }
} // namespace avalanche::rendering::vulkan
