#pragma once

#include "vulkan/vulkan.hpp"
#include "resource/vulkan_resource.h"


namespace avalanche::rendering::EResourceType {
    constexpr resource_type_t VulkanSemaphore = 258;
} // namespace EResourceType

namespace avalanche::rendering::vulkan {

    class Semaphore : public ResourceCRTPBase<EResourceType::VulkanSemaphore> {
    public:
        explicit Semaphore(IRenderDevice& render_device);
        Semaphore(IRenderDevice& render_device, vk::Semaphore semaphore, bool is_external = false);

        // Begin IResource API
        bool is_external() const override;
        // End IResource API

    private:
        vk::Semaphore m_semaphore;
        bool m_is_external = false;
    };

} // namespace avalanche::rendering::vulkan
