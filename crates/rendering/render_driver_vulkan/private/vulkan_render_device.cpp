
#include "vulkan_render_device_impl.h"
#include "vulkan_window.h"
#include "vulkan_context.h"
#include "resource/vulkan_resource.h"
#include "resource/vulkan_sync.h"
#include "resource/vulkan_command_buffer.h"

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

    handle_t RenderDeviceImpl::create_image_view(const ImageViewDesc &desc) { return create_resource<ImageView>(desc); }

    handle_t RenderDeviceImpl::create_command_buffer(const CommandBufferDesc &desc) {
        auto *buffer = construct_resource<CommandBuffer>(*this, desc.pool);
        buffer->initialize(desc);
        return get_resource_pool()->register_resource(buffer);
    }

    void RenderDeviceImpl::start_encoding_command(handle_t command_buffer) {
        get_resource_by_handle<CommandBuffer>(command_buffer)->begin_record();
    }

    void RenderDeviceImpl::finish_encoding_command(handle_t command_buffer) {
        get_resource_by_handle<CommandBuffer>(command_buffer)->end_record();
    }

    handle_t RenderDeviceImpl::create_semaphore() {
        return create_resource_no_desc<Semaphore>();
    }

    handle_t RenderDeviceImpl::create_fence(const FenceDesc &desc) { return create_resource<Fence>(desc); }

    uint32_t RenderDeviceImpl::get_fence_status(const handle_t fence) {
        const auto *f = get_resource_by_handle<Fence>(fence);
        const vk::Result result = get_context().device().getFenceStatus(f->raw_handle());
        AVALANCHE_CHECK(result != vk::Result::eErrorDeviceLost, "Device lost");
        return result == vk::Result::eSuccess ? 1 : 0;
    }

    bool RenderDeviceImpl::block_on_fence(handle_t fence, uint32_t excepted_value, uint64_t timeout) {
        const auto *f = get_resource_by_handle<Fence>(fence);
        vk::Result result = get_context().device().waitForFences({f->raw_handle()}, VK_TRUE, timeout);
        AVALANCHE_CHECK(result != vk::Result::eErrorDeviceLost, "Device lost")
        return result == vk::Result::eSuccess;
    }

    Context &RenderDeviceImpl::get_context() const { return *m_context; }

    unique_ptr<RenderDevice> RenderDevice::create_instance(const DeviceSettings& settings) {
        return { make_unique<RenderDeviceImpl>(settings) };
    }

} // namespace avalanche::rendering::vulkan
