#pragma once

#include <cstdint>
#include "GLFW/glfw3.h"
#include "avalanche_window_server_export.h"
#include "container/string.hpp"
#include "manager/server_manager.h"

#include <manager/tick_manager.h>


namespace avalanche::window {

    struct WindowSettings {
        int32_t width = 800;
        int32_t height = 600;
        int32_t refresh_rate = 60;
        bool fullscreen = false;
        bool low_input_latency = false;
        string title = "Avalanche Engine";
    };

    class AVALANCHE_WINDOW_SERVER_API IWindow : public core::ITickable {
    public:
        explicit IWindow(GLFWwindow* window);
        ~IWindow() override;

        virtual void maximize();
        virtual void minimize();
        /**
         * @brief Recovery from the maximize or minimize operation
         */
        virtual void restore();

        virtual void hide();

        virtual void show();

        virtual bool should_window_close(int flag) const;

        /**
         * @brief Window's tick isn't managed by TickManager directly in normal.
         */
        void tick(float delta_time) override;

    protected:
        AVALANCHE_NO_DISCARD GLFWwindow* get_wrapper_handle() const;

        GLFWwindow* m_window;
    };

    /**
     * @brief The Window Manager
     * Using glfw3 as the window context provider.
     */
    class AVALANCHE_WINDOW_SERVER_API IWindowServer : public core::ServerCRTPBase<IWindowServer>, public core::ITickable {
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
