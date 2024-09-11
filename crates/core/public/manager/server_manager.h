#pragma once


#include <concepts>
#include <cstdint>
#include <type_traits>
#include "avalanche_core_export.h"
#include "container/unique_ptr.hpp"
#include "logger.h"
#include "tick_manager.h"
#include "type_helper.h"

#include <class.h>


#define EXTERN_REGISTER_SERVER(NAME) namespace __static_init { extern size_t NAME; }
#define DEFINE_REGISTER_SERVER(NAME, CLASS_NAME) namespace __static_init {\
    size_t NAME = ([] () {\
        avalanche::core::ServerManager::get().register_server(new CLASS_NAME());\
        return 0; \
    }) ();\
}


namespace avalanche::core {

    template <typename T>
    concept sub_server = requires(const T t) {
        { t.get_server_id() } -> std::convertible_to<size_t>;
    };

    class AVALANCHE_CORE_API ServerManager {
    public:
        class IServer;
        using size_type = size_t;

        virtual ~ServerManager();
        static ServerManager& get();
        static unique_ptr<ServerManager> create_non_static_manager();

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
        SubServer* get_server_checked() {
            constexpr size_type id = SubServer::server_id;
            auto* result = static_cast<SubServer*>(get_server(id));
            AVALANCHE_CHECK_RUNTIME(result != nullptr, "Could not found server with id {}", id);
            return result;
        }

        template <class SubServer>
        requires sub_server<SubServer> && std::is_default_constructible_v<SubServer>
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

        AVALANCHE_NO_DISCARD virtual size_type get_server_id() const = 0;

        virtual void on_startup();
        virtual void on_shutdown();
    };

    template <typename T>
    class ServerCRTPBase : public ServerManager::IServer, public ObjectCRTP<T> {
    public:
        using Super = ServerManager::IServer;
        using Super::size_type;
        using Self = T;

        static constexpr size_type server_id = id_of_type<T>();

        AVALANCHE_NO_DISCARD size_type get_server_id() const override {
            return server_id;
        }

        ServerCRTPBase() = default;

        ServerCRTPBase(const ServerCRTPBase&) = delete;
        ServerCRTPBase(ServerCRTPBase&&) = delete;

        ServerCRTPBase& operator=(const ServerCRTPBase&) = delete;
        ServerCRTPBase& operator=(ServerCRTPBase&&) = delete;
    };

    template <typename T, tick_group_t TG = TickGroup::PostUpdate>
    class ServerCRTPTickable : public ServerCRTPBase<T>, public ITickable {
    public:
        static constexpr tick_group_t server_tick_group = TG;

        void on_startup() override {
            ITickManager::get().register_tickable(this, TG);
        }

        void on_shutdown() override {
            ITickManager::get().unregister_tickable(this);
        }
    };

}
