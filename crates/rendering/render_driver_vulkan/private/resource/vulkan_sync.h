#pragma once

#include "resource/vulkan_resource.h"
#include "vulkan/vulkan.hpp"

#include <vulkan_render_device_impl.h>


namespace avalanche::rendering::EResourceType {
    constexpr resource_type_t VulkanSemaphore = 258;
    constexpr resource_type_t VulkanFence = 259;
} // namespace EResourceType

namespace avalanche::rendering::vulkan {

    class Semaphore : public ResourceCRTPBase<EResourceType::VulkanSemaphore> {
    public:
        explicit Semaphore(IRenderDevice& render_device, vk::Semaphore semaphore = nullptr);
        ~Semaphore() override;

        // Begin IResource API
        bool is_external() const override;
        // End IResource API

        void initialize() {
            AVALANCHE_CHECK(!m_semaphore, "Duplicated initialize of semaphore");

            vk::SemaphoreCreateInfo info{};

            m_semaphore = render_device<RenderDeviceImpl>().get_context().device().createSemaphore(info);
        }

    private:
        vk::Semaphore m_semaphore;
    };

    class Fence : public ResourceCRTPBase<EResourceType::VulkanFence> {
    public:
        explicit Fence(IRenderDevice& render_device, vk::Fence fence = nullptr);
        ~Fence() override;

        // Begin IResource API
        bool is_external() const override;
        // End IResource API

        void initialize(const FenceDesc& desc) {
            AVALANCHE_CHECK(!m_fence, "Duplicated initialize of fence");

            vk::FenceCreateInfo info{};
            if (desc.signaled) {
                info.flags |= vk::FenceCreateFlagBits::eSignaled;
            }

            m_fence = render_device<RenderDeviceImpl>().get_context().device().createFence(info);
        }

    private:
        vk::Fence m_fence;
    };

} // namespace avalanche::rendering::vulkan
