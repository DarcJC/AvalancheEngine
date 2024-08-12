#include "vulkan_render_device.h"
#include "vulkan_window.h"
#include "vulkan_context.h"

#include <mutex>


namespace avalanche::rendering::vulkan {

    class RenderDevice::Impl {
    public:
        explicit Impl(RenderDevice& parent, const DeviceSettings& settings)
            : m_settings(settings)
            , m_context(make_unique<Context>(settings))
            , m_parent(parent) {
            if (settings.required_features.display) {
                enable_display_support();
            }
        }

        void wait_for_device_idle() const {
            m_context->device().waitIdle();
        }

        void enable_display_support() {
            static std::once_flag once;
            std::call_once(once, [this]() {
                VulkanWindowServer* window_server = new VulkanWindowServer();
                core::ServerManager::get().register_server(window_server);
            });
        }

    private:
        DeviceSettings m_settings{};
        unique_ptr<Context> m_context;
        RenderDevice& m_parent;
    };

    unique_ptr<RenderDevice> RenderDevice::create_instance(const DeviceSettings& settings) {
        return make_unique<RenderDevice>(settings);
    }

    RenderDevice::~RenderDevice() {
        delete m_impl;
    }

    void RenderDevice::wait_for_device_idle() { m_impl->wait_for_device_idle(); }

    void RenderDevice::enable_display_support() {
        m_impl->enable_display_support();
    }

    RenderDevice::RenderDevice(const DeviceSettings& settings) {
        m_impl = new Impl(*this, settings);
    }

}
