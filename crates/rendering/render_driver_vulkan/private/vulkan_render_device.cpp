
#include "vulkan_render_device_impl.h"
#include "vulkan_window.h"
#include "vulkan_context.h"

#include <mutex>


namespace avalanche::rendering::vulkan {

    RenderDeviceImpl::RenderDeviceImpl(const DeviceSettings &settings)
        : m_settings(settings)
        , m_context(make_unique<Context>(settings)) {
        if (settings.required_features.display) {
            RenderDeviceImpl::enable_display_support();
        }
    }

    EGraphicsAPIType RenderDeviceImpl::get_graphics_api_type() { return EGraphicsAPIType::Vulkan; }

    void RenderDeviceImpl::wait_for_device_idle() { m_context->device().waitIdle(); }

    void RenderDeviceImpl::enable_display_support() {
        AVALANCHE_CHECK_RUNTIME(core::ServerManager::get().get_server<VulkanWindowServer>() == nullptr,
                                "Only one RenderDevice allow to enable display support.");
        static std::once_flag once;
        std::call_once(once, [this]() {
            auto *window_server = new VulkanWindowServer(*this);
            core::ServerManager::get().register_server(window_server);
        });
    }

    void RenderDeviceImpl::add_pending_delete_resource(IResource *resource) { AVALANCHE_TODO(); }

    handle_t RenderDeviceImpl::register_external_image(const vk::Image &image) {
        return handle_t::null_handle();
    }

    Context &RenderDeviceImpl::get_context() const { return *m_context; }

    unique_ptr<RenderDevice> RenderDevice::create_instance(const DeviceSettings& settings) {
        return make_unique<RenderDeviceImpl>(settings);
    }

}
