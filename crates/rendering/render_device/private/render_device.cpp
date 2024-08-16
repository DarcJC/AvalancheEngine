//
// Created by DarcJC on 2024/8/1.
//
#include "render_device.h"

#include <manager/server_manager.h>
#include <ranges>
#include <unordered_map>
#include <shared_mutex>


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
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            if (const auto it = m_resource.find(handle); it != m_resource.end()) {
                return it->second;
            }
            return nullptr;
        }

        handle_t register_resource(IResource *resource) override {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            AVALANCHE_CHECK(nullptr != resource, "Invalid resource");
            AVALANCHE_CHECK(is_resource_exist(resource), "Resource is already exist");
            handle_t handle = handle_t::new_handle();
            m_resource[handle] = resource;
            return handle;
        }

        void delete_resource(const core::handle_t& handle) override {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            if (IResource* resource = get_resource(handle)) {
                m_resource.erase(handle);
                m_render_device->add_pending_delete_resource(resource);
            }
        }

        ~RenderResourcePool() override {
            reset();
        }

        void reset() {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
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
        std::shared_mutex m_mutex{};
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
        : m_render_resource_pool(IRenderResourcePool::new_pool(this)) {
        core::HandleCreateDelegate.add({&IRenderDevice::on_handle_created, *this});
        core::HandleFreeDelegate.add({&IRenderDevice::on_handle_free, *this});
    }

    IRenderDevice::~IRenderDevice() {
        core::HandleFreeDelegate.remove({&IRenderDevice::on_handle_free, *this});
        core::HandleCreateDelegate.remove({&IRenderDevice::on_handle_created, *this});
        RenderResourcePool::delete_pool(m_render_resource_pool);
    }

    void IRenderDevice::on_handle_created(const core::handle_t &handle) {
        if (IResource* resource = get_resource_pool()->get_resource(handle)) {
            resource->flags().increase_rc();
        }
    }

    void IRenderDevice::on_handle_free(const core::handle_t &handle) {
        auto* pool = get_resource_pool();
        if (IResource* resource = pool->get_resource(handle)) {
            if (resource->flags().decrease_rc() == 0) {
                pool->delete_resource(handle);
            }
        }
    }

    void IRenderDevice::add_pending_delete_resource(IResource *resource) { resource->flags().mark_for_delete(); }

    IRenderResourcePool* IRenderDevice::get_resource_pool() {
        return m_render_resource_pool;
    }

} // namespace avalanche::rendering

