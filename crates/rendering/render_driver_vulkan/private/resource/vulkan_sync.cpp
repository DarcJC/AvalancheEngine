#include "resource/vulkan_sync.h"

#include <vulkan_render_device_impl.h>


namespace avalanche::rendering::vulkan {


    Semaphore::Semaphore(IRenderDevice &render_device, const vk::Semaphore semaphore) :
        ResourceCRTPBase(render_device), m_semaphore(semaphore) {}

    Semaphore::~Semaphore() {
        if (m_semaphore) {
            render_device<RenderDeviceImpl>().get_context().device().destroy(m_semaphore);
        }
    }

    bool Semaphore::is_external() const { return false; }

    bool Fence::is_external() const { return false; }

    Fence::Fence(IRenderDevice &render_device, vk::Fence fence) : ResourceCRTPBase(render_device), m_fence(fence) {}

    Fence::~Fence() {
        if (m_fence) {
            render_device<RenderDeviceImpl>().get_context().device().destroy(m_fence);
        }
    }

} // namespace avalanche::rendering::vulkan
