#include "window_server.h"


namespace avalanche::window {
    void IWindowServer::tick(duration_type delta_time) {
        glfwPollEvents();
    }

    void IWindowServer::on_startup() {
        ServerCRTPTickable::on_startup();
        AVALANCHE_CHECK(glfwInit() == GLFW_TRUE, "GLFW initialization failed");
        this->initialize();
    }

    void IWindowServer::on_shutdown() {
        deinitialize();
        glfwTerminate();
        ServerCRTPTickable::on_shutdown();
    }

    IWindowServer* IWindowServer::get() {
        return core::ServerManager::get().get_server_checked<IWindowServer>();
    }
} // namespace avalanche::window
