#pragma once
#include <render_resource.h>
#include "vulkan/vulkan.hpp"

#include <render_descriptor.h>
#include <resource.h>

namespace avalanche::rendering::EResourceType {
    constexpr resource_type_t VulkanCommandBuffer = 260;
    constexpr resource_type_t VulkanCommandPool = 261;
} // namespace EResourceType


namespace avalanche::rendering::vulkan {

    class CommandPool : public ResourceCRTPBase<EResourceType::VulkanCommandPool> {
    public:
        explicit CommandPool(IRenderDevice& render_device, vk::CommandPool command_pool = nullptr);
        ~CommandPool() override;

    private:
        vk::CommandPool m_command_pool;

        friend class CommandBuffer;
    };

    class CommandBuffer : public ResourceCRTPBase<EResourceType::VulkanCommandBuffer> {
    public:
        explicit CommandBuffer(IRenderDevice& render_device, core::handle_t parent_pool, vk::CommandBuffer command_buffer = nullptr);
        ~CommandBuffer() override;

        void initialize(const CommandBufferDesc& desc);

        void begin_record();
        void end_record();
        void reset();

    protected:
        CommandPool* get_command_pool_checked();

    private:
        vk::CommandBuffer m_command_buffer;
        core::handle_t m_command_pool_handle;
    };

} // namespace avalanche::rendering::vulkan
