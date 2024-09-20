#pragma once
#include <render_resource.h>
#include "vulkan/vulkan.hpp"

#include <render_descriptor.h>
#include <render_resource.h>
#include "vulkan_render_device.h"


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
        explicit CommandBuffer(IRenderDevice& render_device, handle_t parent_pool, vk::CommandBuffer command_buffer = nullptr);
        ~CommandBuffer() override;

        void initialize(const CommandBufferDesc& desc);

        void begin_record();
        void end_record();
        void reset();

    protected:
        CommandPool* get_command_pool_checked();

    private:
        vk::CommandBuffer m_command_buffer;
        handle_t m_command_pool_handle;
    };

} // namespace avalanche::rendering::vulkan
