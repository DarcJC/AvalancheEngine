
#include "vulkan_render_device_impl.h"
#include "vulkan_window.h"
#include "vulkan_context.h"
#include "resource/vulkan_resource.h"

#include <mutex>
#include <render_resource.h>


namespace avalanche::rendering::vulkan {

    RenderDeviceImpl::RenderDeviceImpl(const DeviceSettings &settings) :
        m_settings(settings), m_context(make_unique<Context>(settings)) {
        if (settings.required_features.display) {
            RenderDeviceImpl::enable_display_support();
        }
    }

    RenderDeviceImpl::~RenderDeviceImpl() {
        if (m_settings.required_features.display) {
            RenderDeviceImpl::disable_display_support();
        }
    }

    EGraphicsAPIType RenderDeviceImpl::get_graphics_api_type() { return EGraphicsAPIType::Vulkan; }

    void RenderDeviceImpl::wait_for_device_idle() { m_context->device().waitIdle(); }

    void RenderDeviceImpl::enable_display_support() {
        AVALANCHE_CHECK_RUNTIME(core::ServerManager::get().get_server<VulkanWindowServer>() == nullptr,
                                "Only one RenderDevice allow to enable display support.");
        auto *window_server = new VulkanWindowServer(*this);
        core::ServerManager::get().register_server(window_server);
    }

    void RenderDeviceImpl::disable_display_support() {
        core::ServerManager::get().unregister_server_and_delete<window::IWindowServer>();
    }

    void RenderDeviceImpl::clean_pending_delete_resource() {
        if (m_queued_delete_resource.empty()) {
            return;
        }
        std::lock_guard lock(m_queue_mutex);
        while (!m_queued_delete_resource.empty()) {
            IResource* resource = m_queued_delete_resource.front();
            m_queued_delete_resource.pop();
            AVALANCHE_CHECK(nullptr != resource, "Trying to delete a nullptr resource");
            if (resource->flags().set_deleting()) {
                delete_resource(resource);
            }
        }
    }

    void RenderDeviceImpl::add_pending_delete_resource(IResource *resource) {
        IRenderDevice::add_pending_delete_resource(resource);
        std::lock_guard lock(m_queue_mutex);
        m_queued_delete_resource.push(resource);
    }

    handle_t RenderDeviceImpl::create_image_view(const ImageViewDesc &desc) {
        return create_resource<ImageView>(desc);
    }

    Context &RenderDeviceImpl::get_context() const { return *m_context; }

    unique_ptr<RenderDevice> RenderDevice::create_instance(const DeviceSettings& settings) {
        return { make_unique<RenderDeviceImpl>(settings) };
    }

}
