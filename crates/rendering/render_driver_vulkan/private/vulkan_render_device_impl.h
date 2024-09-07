#pragma once
#include <mutex>
#include <queue>
#include <render_descriptor.h>
#include "render_resource.h"
#include "vulkan_context.h"
#include "vulkan_render_device.h"


namespace avalanche::rendering::vulkan {

    class RenderDeviceImpl : public RenderDevice {
    public:
        using RenderDevice::graphics_api_type;

        explicit RenderDeviceImpl(const DeviceSettings &settings);
        ~RenderDeviceImpl() override;

        // Begin IRenderDevice interface
        EGraphicsAPIType get_graphics_api_type() override;
        void wait_for_device_idle() override;
        void enable_display_support() override;
        void disable_display_support() override;
        void clean_pending_delete_resource() override;
        void add_pending_delete_resource(IResource *resource) override;

        handle_t create_image_view(const ImageViewDesc &desc) override;

        handle_t create_command_buffer(const CommandBufferDesc& desc) override;
        void start_encoding_command(handle_t command_buffer) override;
        void finish_encoding_command(handle_t command_buffer) override;

        handle_t create_semaphore() override;
        handle_t create_fence(const FenceDesc &desc) override;
        uint32_t get_fence_status(handle_t fence) override;
        bool block_on_fence(handle_t fence, uint32_t excepted_value, uint64_t timeout) override;
        // End IRenderDevice interface

        template <typename VulkanHandleType, typename ResourceType, typename... Args>
        requires std::derived_from<ResourceType, IResource>
        handle_t register_external_resource(const VulkanHandleType& vulkan_handle, Args&&... args) {
            AVALANCHE_CHECK(vulkan_handle, "Invalid vulkan handle");
            return get_resource_pool()->register_resource(construct_resource<ResourceType>(*this, vulkan_handle, std::forward<Args>(args)...));
        }

        AVALANCHE_NO_DISCARD Context& get_context() const;

    private:
        DeviceSettings m_settings{};
        unique_ptr<Context> m_context;

        std::mutex m_queue_mutex{};
        std::queue<IResource*> m_queued_delete_resource{};
    };

} // namespace avalanche::rendering::vulkan

