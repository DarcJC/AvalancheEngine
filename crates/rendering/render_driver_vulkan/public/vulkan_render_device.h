#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "container/unique_ptr.hpp"
#include "render_device.h"
#include "render_resource.h"

namespace avalanche::rendering::vulkan {

    class AVALANCHE_RENDER_DRIVER_VULKAN_API RenderDevice : public IRenderDevice {
    public:
        static constexpr auto graphics_api_type = EGraphicsAPIType::Vulkan;

        static unique_ptr<RenderDevice> create_instance(const DeviceSettings& settings);
    };

}


namespace avalanche::rendering::EResourceType {
    constexpr resource_type_t VulkanImage = 256;
    constexpr resource_type_t VulkanImageView = 257;
    constexpr resource_type_t VulkanSemaphore = 258;
    constexpr resource_type_t VulkanFence = 259;
    constexpr resource_type_t VulkanCommandBuffer = 260;
    constexpr resource_type_t VulkanCommandPool = 261;
    constexpr resource_type_t VulkanSurface = 262;
    constexpr resource_type_t VulkanSwapchain = 263;
    constexpr resource_type_t VulkanRenderPipeline = 300;
    constexpr resource_type_t VulkanPipelineLayout = 301;
    constexpr resource_type_t VulkanBindGroupLayout = 302;
    constexpr resource_type_t VulkanBindGroup = 303;
} // namespace EResourceType

