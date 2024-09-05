#pragma once
#include "render_resource.h"
#include "vulkan_render_device.h"
#include "vulkan_context.h"
#include "vulkan/vulkan.hpp"
#include <queue>
#include <mutex>


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

