#include "resource/vulkan_sync.h"


namespace avalanche::rendering::vulkan {


    Semaphore::Semaphore(IRenderDevice &render_device, const vk::Semaphore semaphore) :
        ResourceCRTPBase(render_device), m_semaphore(semaphore) {}

    bool Semaphore::is_external() const { return false; }

    bool Fence::is_external() const { return false; }

    Fence::Fence(IRenderDevice &render_device, vk::Fence fence) :
        ResourceCRTPBase(render_device), m_fence(fence) {}

} // namespace avalanche::rendering::vulkan
