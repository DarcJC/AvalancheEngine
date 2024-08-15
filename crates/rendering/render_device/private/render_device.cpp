//
// Created by DarcJC on 2024/8/1.
//
#include "render_device.h"
#include "render_resource.h"
#include "resource.h"
#include <unordered_map>
#include <deque>

namespace avalanche::rendering {
    using core::handle_t;

    class RenderResourcePool : public IRenderResourcePool {
    public:
        explicit RenderResourcePool(IRenderDevice* render_device)
            : m_render_device(render_device)
        {}

        IResource* get_resource(const core::handle_t &handle) override {
            AVALANCHE_TODO();
            return nullptr;
        }

        handle_t register_resource(IResource *resource) override {
            AVALANCHE_TODO();
            return handle_t::null_handle();
        }

        ~RenderResourcePool() override {
        }
    private:
        IRenderDevice* m_render_device;
    };

    IRenderResourcePool *IRenderResourcePool::new_pool(IRenderDevice* render_device) {
        return new RenderResourcePool(render_device);
    }

    void IRenderResourcePool::delete_pool(const IRenderResourcePool * pool) {
        delete pool;
    }

    IRenderResourcePool::~IRenderResourcePool() = default;

    IRenderDevice::IRenderDevice() = default;

    IRenderDevice::~IRenderDevice() = default;

    void IRenderDevice::add_pending_delete_resource(IResource* resource) {
        resource->flags().mark_for_delete();
    }

} // namespace avalanche::rendering

