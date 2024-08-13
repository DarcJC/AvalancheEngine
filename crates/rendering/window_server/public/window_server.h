#pragma once

#include <cstdint>
#include "GLFW/glfw3.h"
#include "avalanche_window_server_export.h"
#include "container/string.hpp"
#include "manager/server_manager.h"


namespace avalanche::window {

    struct WindowSettings {
        int32_t width = 800;
        int32_t height = 600;
        int32_t refresh_rate = 60;
        bool fullscreen = false;
        bool low_input_latency = false;
        string title = "Avalanche Engine";
    };

    class AVALANCHE_WINDOW_SERVER_API IWindow {
    public:
        explicit IWindow(GLFWwindow* window);
        virtual ~IWindow();

        virtual void maximize();
        virtual void minimize();
        /**
         * @brief Recovery from the maximize or minimize operation
         */
        virtual void restore();

        virtual void hide();

        virtual void show();

    protected:
        GLFWwindow* m_window;
    };

    /**
     * @brief The Window Manager
     * Using glfw3 as the window context provider.
     */
    class AVALANCHE_WINDOW_SERVER_API IWindowServer : public core::ServerCRTPBase<IWindowServer> {
    public:
        virtual IWindow* create_window(const WindowSettings& settings) = 0;
        virtual void destroy_window(IWindow* window) = 0;
        virtual void initialize() = 0;
        virtual void deinitialize() = 0;

        void on_startup() override;
        void on_shutdown() override;
    };

}
