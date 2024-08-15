//
// Created by DarcJC on 2024/8/1.
//
#include "render_device.h"
#include "render_resource.h"
#include "resource.h"
#include <unordered_map>

namespace avalanche::rendering {
    using core::handle_t;

    class RenderResourcePool : public IRenderResourcePool {
        struct ResourceBlock {
            IResource* resource = nullptr;
            ResourceBlock* prev = nullptr;
            ResourceBlock* next = nullptr;
        };

    public:
        explicit RenderResourcePool(IRenderDevice* render_device)
            : m_render_device(render_device)
            , m_head_block {
                .resource = nullptr,
                .prev = nullptr,
                .next = nullptr,
            }
            , m_query_map()
        {}

        ~RenderResourcePool() override {
            reset();
        }

        IResource* get_resource(const handle_t& handle) override {
            AVALANCHE_TODO();
            return nullptr;
        }

        void reset() {
            ResourceBlock* current = m_head_block.next;

            while (nullptr != current) {
                if (nullptr != current->resource) {
                    m_render_device->add_pending_delete_resource(current->resource);
                }

                ResourceBlock* next_block = current->next;

                delete current;

                current = next_block;
            }

            m_head_block.next = nullptr;
        }

    private:
        IRenderDevice* m_render_device;
        ResourceBlock m_head_block;
        std::unordered_map<handle_t, ResourceBlock*> m_query_map;
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

