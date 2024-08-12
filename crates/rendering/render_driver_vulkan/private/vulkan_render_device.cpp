#include "vulkan_render_device.h"
#include "vulkan_context.h"


namespace avalanche::rendering::vulkan {

    class RenderDevice::Impl {
    public:
        explicit Impl(const DeviceSettings& settings)
            : m_settings(settings)
            , m_context(make_unique<Context>(settings))
        {}

        void wait_for_device_idle() const {
            m_context->device().waitIdle();
        }

    private:
        DeviceSettings m_settings{};
        unique_ptr<Context> m_context;
    };

    unique_ptr<RenderDevice> RenderDevice::create_instance(const DeviceSettings& settings) {
        return make_unique<RenderDevice>(settings);
    }

    RenderDevice::~RenderDevice() {
        delete m_impl;
    }

    void RenderDevice::wait_for_device_idle() {
        m_impl->wait_for_device_idle();
    }

    RenderDevice::RenderDevice(const DeviceSettings& settings) {
        m_impl = new Impl(settings);
    }

}
