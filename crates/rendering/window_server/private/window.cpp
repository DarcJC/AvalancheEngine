#include "window_server.h"


namespace avalanche::window {

    void framebuffer_callback(GLFWwindow* window, const int width, const int height) {
        static_cast<IWindow*>(glfwGetWindowUserPointer(window))->on_framebuffer_size_changed(width, height);
    }

    IWindow::~IWindow() = default;

    IWindow::IWindow(const WindowSettings& settings, rendering::IRenderDevice* device)
        : m_window(glfwCreateWindow(settings.width, settings.height, settings.title.data(), settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr))
        , m_render_device(device)
        , m_settings(settings) {
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_callback);
    }

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

    bool IWindow::should_window_close(int flag) const { return true; }

    void IWindow::on_framebuffer_size_changed(int width, int height) {}

    void IWindow::tick(float delta_time) {
        if (int flag = glfwWindowShouldClose(m_window)) {
            if (should_window_close(flag)) {
                IWindowServer::get()->destroy_window(this);
                return;
            }
            glfwSetWindowShouldClose(m_window, 0);
        }
        // Queue frame to render
        // AVALANCHE_TODO();
    }

    GLFWwindow *IWindow::get_wrapper_handle() const { return m_window; }

} // namespace avalanche::window
