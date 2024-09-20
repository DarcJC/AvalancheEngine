#include "resource/vulkan_resource_binding.h"

#include <vulkan_render_device_impl.h>

namespace avalanche::rendering::vulkan {

    BindGroupLayout::BindGroupLayout(IRenderDevice &render_device, vk::DescriptorSetLayout raw_handle) :
        ResourceCRTPBase(render_device) {}

    BindGroupLayout::~BindGroupLayout() {
        if (m_handle) {
            render_device<RenderDeviceImpl>().get_context().device().destroy(m_handle);
        }
    }

} // namespace avalanche::rendering::vulkan
