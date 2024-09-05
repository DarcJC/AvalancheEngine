//
// Created by i on 9/5/2024.
//

#include "resource/vulkan_command_buffer.h"

#include <vulkan_render_device_impl.h>

#include <utility>

namespace avalanche::rendering::vulkan {

    CommandPool::CommandPool(IRenderDevice &render_device, vk::CommandPool command_pool) :
        ResourceCRTPBase(render_device), m_command_pool(command_pool) {}

    CommandPool::~CommandPool() {
        if (m_command_pool) {
            render_device<RenderDeviceImpl>().get_context().device().destroy(m_command_pool);
        }
    }

    CommandBuffer::CommandBuffer(IRenderDevice &render_device, handle_t  parent_pool, vk::CommandBuffer command_buffer) :
        ResourceCRTPBase(render_device), m_command_buffer(command_buffer), m_command_pool_handle(std::move(parent_pool)) {}

    CommandBuffer::~CommandBuffer() {
        if (m_command_buffer) {
            auto *parent = render_device<RenderDeviceImpl>().get_resource_by_handle<CommandPool>(m_command_pool_handle);
            render_device<RenderDeviceImpl>().get_context().device().freeCommandBuffers(parent->m_command_pool,
                                                                                        {m_command_buffer});
        }
    }

    void CommandBuffer::initialize(const CommandBufferDesc& desc) {
        AVALANCHE_CHECK(!m_command_buffer, "Duplicated initialization");

        vk::CommandBufferAllocateInfo allocate_info{};
        allocate_info
            .setCommandPool(get_command_pool_checked()->m_command_pool)
            .setCommandBufferCount(1)
            .setLevel(desc.is_primary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary);

        m_command_buffer = render_device<RenderDeviceImpl>().get_context().device().allocateCommandBuffers(allocate_info).front();
    }

    void CommandBuffer::begin_record() {
        AVALANCHE_CHECK(m_command_buffer, "Invalid command buffer");

        m_command_buffer.begin(vk::CommandBufferBeginInfo());
    }

    void CommandBuffer::end_record() {
        AVALANCHE_CHECK(m_command_buffer, "Invalid command buffer");
        m_command_buffer.end();
    }

    void CommandBuffer::reset() {
        AVALANCHE_CHECK(m_command_buffer, "Invalid command buffer");
        m_command_buffer.reset();
    }

    CommandPool* CommandBuffer::get_command_pool_checked() {
        AVALANCHE_CHECK(m_command_pool_handle, "Invalid command buffer");
        auto* result = render_device<RenderDeviceImpl>().get_resource_by_handle<CommandPool>(m_command_pool_handle);
        AVALANCHE_CHECK(result != nullptr, "Invalid command buffer");

        return result;
    }

} // namespace avalanche::rendering::vulkan
