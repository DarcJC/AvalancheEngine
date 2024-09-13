#pragma once

#include <cstdint>
#include "avalanche_window_server_export.h"
#include "container/string.hpp"
#include "manager/server_manager.h"
#include <manager/tick_manager.h>
#include <render_frame.h>

extern "C" {
    struct GLFWwindow;
}

namespace avalanche::rendering {
    class IRenderDevice;
}

namespace avalanche::window {

    struct WindowSettings {
        uint32_t width = 800;
        uint32_t height = 600;
        int32_t refresh_rate = 60;
        bool fullscreen = false;
        bool low_input_latency = false;
        string title = "Avalanche Engine";
        bool vsync = true;
    };

    class AVALANCHE_WINDOW_SERVER_API IWindow : public core::ITickable, rendering::CanRenderOnMixin {
    public:
        explicit IWindow(const WindowSettings& settings, rendering::IRenderDevice* device);
        ~IWindow() override;

        virtual void maximize();
        virtual void minimize();
        /**
         * @brief Recovery from the maximize or minimize operation
         */
        virtual void restore();

        virtual void hide();

        virtual void show();

        AVALANCHE_NO_DISCARD virtual bool should_window_close(int flag) const;

        virtual void on_framebuffer_size_changed(int width, int height);

        /**
         * @brief Window's tick isn't managed by TickManager directly in normal.
         */
        void tick(float delta_time) override;

    protected:
        AVALANCHE_NO_DISCARD GLFWwindow* get_wrapper_handle() const;

        GLFWwindow* m_window;
        rendering::IRenderDevice* m_render_device;
        WindowSettings m_settings;
    };

    /**
     * @brief The Window Manager
     * Using glfw3 as the window context provider.
     */
    class AVALANCHE_WINDOW_SERVER_API IWindowServer : public core::ServerCRTPTickable<IWindowServer, core::TickGroup::PreRendering> {
    public:
        AVALANCHE_NO_DISCARD static IWindowServer* get();

        virtual IWindow* create_window(const WindowSettings& settings) = 0;
        virtual void destroy_window(IWindow* window) = 0;
        virtual void initialize() = 0;
        virtual void deinitialize() = 0;
        virtual void register_external_window(IWindow* window) = 0;

        void tick(duration_type delta_time) override;

        void on_startup() override;
        void on_shutdown() override;
    };

}

#include "window_server.generated.h"
