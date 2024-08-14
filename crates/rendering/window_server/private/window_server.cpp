#include "window_server.h"


namespace avalanche::window {
    void IWindowServer::tick(duration_type delta_time) {
        glfwPollEvents();
    }

    void IWindowServer::on_startup() {
        AVALANCHE_CHECK(glfwInit() == GLFW_TRUE, "GLFW initialization failed");
        core::ITickManager::get().register_tickable(this, core::TickGroup::PreRendering);
        this->initialize();
    }
    void IWindowServer::on_shutdown() {
        deinitialize();
        core::ITickManager::get().unregister_tickable(this);
        glfwTerminate();
    }
} // namespace avalanche::window
