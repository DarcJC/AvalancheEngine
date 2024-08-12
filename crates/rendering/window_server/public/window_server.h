#pragma once

#include "avalanche_window_server_export.h"
#include <cstdint>
#include "server_manager.h"


namespace avalanche::window {

    struct WindowSettings {
        int32_t width = 800;
        int32_t height = 600;
        bool fullscreen = false;
        bool low_input_latency = false;
    };

    class AVALANCHE_WINDOW_SERVER_API IWindow {
    public:
        virtual ~IWindow();
    };

    /**
     * @brief The Window Manager
     * Using glfw3 as the window context provider.
     */
    class AVALANCHE_WINDOW_SERVER_API IWindowServer : public core::ServerCRTPBase<IWindowServer> {
    public:
        virtual IWindow* create_window(WindowSettings settings) = 0;
        virtual void destroy_window(IWindow* window) = 0;
        virtual void initialize() = 0;

        void on_startup() override;
    };

}
