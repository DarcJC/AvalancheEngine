//
// Created by DarcJC on 2024/8/1.
//
#include "render_device.h"
#include "render_handle.h"

namespace avalanche::rendering {

    class RenderResourcePool : public IRenderResourcePool {
    public:
        IResource* get_resource(const handle_t& handle) override {
            return nullptr;
        }
    };

    IRenderResourcePool *IRenderResourcePool::new_pool() { return new RenderResourcePool(); }

    void IRenderResourcePool::delete_pool(const IRenderResourcePool * pool) {
        delete pool;
    }

    IRenderResourcePool::~IRenderResourcePool() = default;

    IRenderDevice::IRenderDevice() = default;

    IRenderDevice::~IRenderDevice() = default;


} // namespace avalanche::rendering

