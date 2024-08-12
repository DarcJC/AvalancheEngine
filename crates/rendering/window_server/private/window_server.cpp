#include "window_server.h"


namespace avalanche::window {
    IWindow::~IWindow() = default;

    void IWindowServer::on_startup() {
        static std::once_flag once{};
        std::call_once(once, [this]() {
            AVALANCHE_CHECK(glfwInit() == GLFW_TRUE, "GLFW initialization failed");
            this->initialize();
        });
    }

    IWindow::IWindow(GLFWwindow *window)
        : m_window(window)
    {}
} // namespace avalanche::window
