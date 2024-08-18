#include "resource/vulkan_semaphore.h"


namespace avalanche::rendering::vulkan {


    Semaphore::Semaphore(IRenderDevice &render_device) : ResourceCRTPBase(render_device) {}

    Semaphore::Semaphore(IRenderDevice &render_device, const vk::Semaphore semaphore, const bool is_external) :
        ResourceCRTPBase(render_device), m_semaphore(semaphore), m_is_external(is_external) {}

    bool Semaphore::is_external() const { return m_is_external; }

} // namespace avalanche::rendering::vulkan
