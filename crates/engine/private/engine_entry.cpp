#include <manager/server_manager.h>
#include <window_server.h>
#include "engine.h"

namespace avalanche {

    class Engine : public IEngine {
    public:
        explicit Engine(const EngineDesc &desc);

        // Begin IEngine interface
        void poll() override;
        window::IWindow *create_window(const window::WindowSettings &settings) override;
        // End IEngine interface

    private:
        EngineDesc m_create_desc;
        unique_ptr<ServerManager> m_server_manager;
    };

    unique_ptr<IEngine> IEngine::create_instance(const EngineDesc &desc) { return {make_unique<Engine>(desc)}; }

    Engine::Engine(const EngineDesc &desc)
        : m_create_desc(desc)
        , m_server_manager(ServerManager::create_non_static_manager()) {
    }

    void Engine::poll() {}

    window::IWindow *Engine::create_window(const window::WindowSettings &settings) {
        auto* window_server = ServerManager::get().get_server_checked<window::IWindowServer>();
        return window_server->create_window(settings);
    }

    IEngine::~IEngine() = default;
} // namespace avalanche
