#pragma once
#include "render_resource.h"
#include "vulkan_render_device.h"
#include "vulkan/vulkan.hpp"



namespace avalanche::rendering::vulkan {

    class BindGroupLayout : public ResourceCRTPBase<EResourceType::VulkanBindGroupLayout> {
    public:
        explicit BindGroupLayout(IRenderDevice& render_device, vk::DescriptorSetLayout raw_handle = nullptr);
        ~BindGroupLayout() override;

    private:
        vk::DescriptorSetLayout m_handle;
    };

} // namespace avalanche::rendering::vulkan
