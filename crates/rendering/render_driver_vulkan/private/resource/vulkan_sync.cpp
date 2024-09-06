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

    void Semaphore::initialize() {
        AVALANCHE_CHECK(!m_semaphore, "Duplicated initialize of semaphore");

        vk::SemaphoreCreateInfo info{};

        m_semaphore = render_device<RenderDeviceImpl>().get_context().device().createSemaphore(info);
    }

    vk::Semaphore Semaphore::raw_handle() const {
        return m_semaphore;
    }

    bool Fence::is_external() const { return false; }

    void Fence::initialize(const FenceDesc &desc) {
        AVALANCHE_CHECK(!m_fence, "Duplicated initialize of fence");

        vk::FenceCreateInfo info{};
        if (desc.signaled) {
            info.flags |= vk::FenceCreateFlagBits::eSignaled;
        }

        m_fence = render_device<RenderDeviceImpl>().get_context().device().createFence(info);
    }

    vk::Fence Fence::raw_handle() const { return m_fence; }

    Fence::Fence(IRenderDevice &render_device, vk::Fence fence) : ResourceCRTPBase(render_device), m_fence(fence) {}

    Fence::~Fence() {
        if (m_fence) {
            render_device<RenderDeviceImpl>().get_context().device().destroy(m_fence);
        }
    }

} // namespace avalanche::rendering::vulkan
