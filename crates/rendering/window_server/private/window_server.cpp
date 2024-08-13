#include "window_server.h"


namespace avalanche::window {
    IWindow::~IWindow() = default;

    void IWindowServer::on_startup() {
        AVALANCHE_CHECK(glfwInit() == GLFW_TRUE, "GLFW initialization failed");
        this->initialize();
    }
    void IWindowServer::on_shutdown() {
        deinitialize();
        glfwTerminate();
    }

    IWindow::IWindow(GLFWwindow *window)
        : m_window(window)
    {}

    void IWindow::maximize() {
        glfwMaximizeWindow(m_window);
    }

    void IWindow::minimize() {
        glfwIconifyWindow(m_window);
    }

    void IWindow::restore() {
        glfwRestoreWindow(m_window);
    }

    void IWindow::hide() {
        glfwHideWindow(m_window);
    }

    void IWindow::show() {
        glfwShowWindow(m_window);
    }
} // namespace avalanche::window
