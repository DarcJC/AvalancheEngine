#include "server_manager.h"


#include <shared_mutex>
#include <mutex>
#include <unordered_map>


namespace avalanche::core {
    class RealServerManager : public ServerManager {
    public:
        using Super = ServerManager;
        using Super::size_type;

        RealServerManager() = default;
        ~RealServerManager() override {
            unregister_all_servers();
        }

        void register_server(IServer* server) override {
            AVALANCHE_CHECK(nullptr != server, "Trying to register invalid server");
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_servers.insert_or_assign(server->get_server_id(), server);
            // TODO: move startup event out of here
            server->on_startup();
        }

        void unregister_server_and_delete(IServer* server) override {
            AVALANCHE_CHECK(nullptr != server, "Trying to unregister invalid server");
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            server->on_shutdown();
            const size_type id = server->get_server_id();
            m_servers.erase(id);
            delete server;
        }

        IServer* get_server(const size_type id) override {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            if (m_servers.contains(id)) {
                return m_servers[id];
            }
            return nullptr;
        }

        void unregister_all_servers() {
            for (auto& [id, srv] : m_servers) {
                AVALANCHE_LOGGER.info("Shutting down server {}", id);
                srv->on_shutdown();
                delete srv;
            }
            m_servers.clear();
        }

    private:
        std::shared_mutex m_mutex{};
        std::unordered_map<size_type, IServer*> m_servers{};
    };

    ServerManager::IServer::~IServer() = default;

    void ServerManager::IServer::on_startup() {}

    void ServerManager::IServer::on_shutdown() {}

    ServerManager::~ServerManager() = default;

    ServerManager& ServerManager::get() {
        static RealServerManager manager{};
        return manager;
    }
}
