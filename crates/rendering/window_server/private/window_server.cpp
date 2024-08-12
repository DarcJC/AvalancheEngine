#include "window_server.h"


namespace avalanche::window {
    IWindow::~IWindow() = default;

    void IWindowServer::on_startup() {
        static std::once_flag once{};
        std::call_once(once, [this]() {
            this->initialize();
        });
    }

} // namespace avalanche::window
