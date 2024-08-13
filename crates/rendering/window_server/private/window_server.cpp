#include "window_server.h"


namespace avalanche::window {
    IWindow::~IWindow() = default;

    void IWindowServer::on_startup() {
        AVALANCHE_CHECK(glfwInit() == GLFW_TRUE, "GLFW initialization failed");
        this->initialize();
    }
    void IWindowServer::on_shutdown() {
        glfwTerminate();
    }

    IWindow::IWindow(GLFWwindow *window)
        : m_window(window)
    {}
} // namespace avalanche::window
