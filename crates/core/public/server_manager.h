#pragma once


#include "avalanche_core_export.h"
#include "logger.h"
#include <cstdint>
#include <type_traits>
#include <concepts>


namespace avalanche::core {

    template <typename T>
    concept sub_server = std::is_default_constructible_v<T> && requires(const T t) {
        { t.get_server_id() } -> std::convertible_to<size_t>;
    };

    class AVALANCHE_CORE_API ServerManager {
    public:
        class IServer;
        using size_type = size_t;

        virtual ~ServerManager();
        static ServerManager& get();

        virtual void register_server(IServer* server) = 0;
        virtual void unregister_server_and_delete(IServer* server) = 0;
        virtual IServer* get_server(size_type id) = 0;

        template <class SubServer>
        requires sub_server<SubServer>
        SubServer* get_server() {
            constexpr size_type id = SubServer::server_id;
            return static_cast<SubServer*>(get_server(id));
        }

        template <class SubServer>
        requires sub_server<SubServer>
        SubServer* create_and_register_server() {
            AVALANCHE_CHECK(get_server(SubServer::server_id) == nullptr, "Server already exists");
            register_server(new SubServer());
            return static_cast<SubServer*>(get_server(SubServer::server_id));
        }

        template <class SubServer>
        requires sub_server<SubServer>
        void unregister_server_and_delete() {
            AVALANCHE_CHECK(get_server(SubServer::server_id) != nullptr, "Server doesn't exists");
            unregister_server_and_delete(get_server(SubServer::server_id));
        }
    };

    class AVALANCHE_CORE_API ServerManager::IServer {
    public:
        using size_type = typename ServerManager::size_type;

        virtual ~IServer();

        virtual size_type get_server_id() const = 0;

        virtual void on_startup();
        virtual void on_shutdown();
    };

    template <typename T>
    class ServerCRTPBase : public ServerManager::IServer {
    public:
        using Super = ServerManager::IServer;
        using Super::size_type;
        using Self = T;

        static constexpr auto server_id = __COUNTER__;

        size_type get_server_id() const override {
            return server_id;
        }

        ServerCRTPBase() = default;

        ServerCRTPBase(const ServerCRTPBase&) = delete;
        ServerCRTPBase(ServerCRTPBase&&) = delete;

        ServerCRTPBase& operator=(const ServerCRTPBase&) = delete;
        ServerCRTPBase& operator=(ServerCRTPBase&&) = delete;
    };

}
