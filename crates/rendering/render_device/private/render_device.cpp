//
// Created by DarcJC on 2024/8/1.
//
#include "render_device.h"

#include <manager/server_manager.h>
#include <ranges>
#include <unordered_map>

#include "../../window_server/public/window_server.h"
#include "render_resource.h"
#include "resource.h"

namespace avalanche::rendering {
    using core::handle_t;

    class RenderResourcePool : public IRenderResourcePool {
    public:
        explicit RenderResourcePool(IRenderDevice* render_device)
            : m_resource{}
            , m_render_device(render_device) {
            AVALANCHE_CHECK_RUNTIME(m_render_device != nullptr, "Invalid render device");
        }

        IResource* get_resource(const core::handle_t &handle) override {
            if (const auto it = m_resource.find(handle); it != m_resource.end()) {
                return it->second;
            }
            return nullptr;
        }

        handle_t register_resource(IResource *resource) override {
            AVALANCHE_CHECK(nullptr != resource, "Invalid resource");
            AVALANCHE_CHECK(is_resource_exist(resource), "Resource is already exist");
            handle_t handle = handle_t::new_handle();
            m_resource[handle] = resource;
            return handle;
        }

        ~RenderResourcePool() override {
            reset();
        }

        void reset() {
            for (const auto &res: m_resource | std::views::values) {
                m_render_device->add_pending_delete_resource(res);
            }
            m_resource.clear();
        }

        bool is_resource_exist(IResource* resource) {
            const auto view = m_resource | std::views::values;
            return std::ranges::find(view, resource) != view.end();
        }
    private:
        std::unordered_map<handle_t, IResource*> m_resource;

        IRenderDevice* m_render_device;
    };

    IRenderResourcePool *IRenderResourcePool::new_pool(IRenderDevice* render_device) {
        return new RenderResourcePool(render_device);
    }

    void IRenderResourcePool::delete_pool(const IRenderResourcePool * pool) {
        delete pool;
    }

    IRenderResourcePool::~IRenderResourcePool() = default;

    IRenderDevice::IRenderDevice()
        : m_render_resource_pool(IRenderResourcePool::new_pool(this))
    {}

    IRenderDevice::~IRenderDevice() { delete m_render_resource_pool; }

    void IRenderDevice::disable_display_support() {
        core::ServerManager::get().unregister_server_and_delete<window::IWindowServer>();
    }

    void IRenderDevice::add_pending_delete_resource(IResource *resource) { resource->flags().mark_for_delete(); }

    IRenderResourcePool* IRenderDevice::get_resource_pool() {
        return m_render_resource_pool;
    }

} // namespace avalanche::rendering

