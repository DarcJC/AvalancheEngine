#pragma once

#define GLFW_INCLUDE_VULKAN
#include <cstdint>
#include <mutex>
#include "container/vector.hpp"
#include "window_server.h"
#include "vulkan/vulkan.hpp"

namespace avalanche::rendering::vulkan {

    using core::handle_t;

    class RenderDevice;
    class VulkanWindow;

    class VulkanWindowServer final : public window::IWindowServer {
    public:
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
        friend class RenderDeviceImpl;
    };

    class VulkanWindow : public window::IWindow {
    public:
        explicit VulkanWindow(const window::WindowSettings& settings, RenderDevice* render_device);
        ~VulkanWindow() override;

    protected:
        /**
         * @brief Create / Recreate swapchain.
         */
        void create_swapchain();

        void clean_old_resource();

        handle_t create_image_view(vk::Image image);

        // Begin IWindow interface
        void on_framebuffer_size_changed(int width, int height) override;
        // End IWindow interface

        // Begin CanRenderOnMixin interface
        FrameInFlight prepare_frame_context() override;
        // End CanRenderOnMixin interface

    private:
        vk::SurfaceKHR m_surface = nullptr;
        vk::SwapchainKHR m_swapchain = nullptr;
        vector<handle_t> m_image_handles{};
        vector<handle_t> m_image_view_handles{};
        std::mutex m_window_lock{};

        vk::SurfaceFormatKHR m_surface_format;

        handle_t m_image_acquire_semaphore;
        handle_t m_final_fence;

        friend class VulkanWindowServer;
    };

}
