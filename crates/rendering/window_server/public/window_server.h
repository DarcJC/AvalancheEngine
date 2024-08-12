#pragma once

#include <cstdint>
#include "server_manager.h"


namespace avalanche::window {

    struct WindowSettings {
        int32_t width = 800;
        int32_t height = 600;
        bool fullscreen = false;
        bool low_input_latency = false;
    };

    class IWindow {
    public:
        virtual ~IWindow();
    };

    /**
     * @brief The Window Manager
     * Using glfw3 as the window context provider.
     */
    class IWindowServer : public core::ServerCRTPBase<IWindowServer> {
    public:
        virtual IWindow* create_window(WindowSettings settings) = 0;
    };

}
