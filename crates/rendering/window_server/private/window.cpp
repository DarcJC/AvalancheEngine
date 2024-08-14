#include "window_server.h"


namespace avalanche::window {

    IWindow::~IWindow() = default;

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

    void IWindow::show() { glfwShowWindow(m_window); }

    bool IWindow::should_window_close(int flag) const {
        return true;
    }

    void IWindow::tick(float delta_time) {
        if (int flag = glfwWindowShouldClose(m_window)) {
            if (should_window_close(flag)) {
                IWindowServer::get()->destroy_window(this);
                return;
            }
            glfwSetWindowShouldClose(m_window, 0);
        }
    }

    GLFWwindow *IWindow::get_wrapper_handle() const { return m_window; }

} // namespace avalanche::window
