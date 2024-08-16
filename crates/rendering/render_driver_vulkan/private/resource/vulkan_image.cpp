#include "vulkan_render_device_impl.h"
#include "resource/vulkan_image.h"


namespace avalanche::rendering::vulkan {


    Image::Image(IRenderDevice& render_device)
        : ResourceCRTPBase(render_device)
        , m_raw_handle(nullptr)
    {}

    Image::Image(IRenderDevice& render_device, const vk::Image image) : ResourceCRTPBase(render_device), m_is_committed(true), m_raw_handle(image) {}

    Image::~Image() {
        if (m_raw_handle) {
            RenderDeviceImpl& device =  cast_to_vulkan_device(m_render_device);
            device.get_context().device().destroyImage(m_raw_handle);
        }
    }

    bool Image::is_committed() const { return m_is_committed; }

    ImageView::ImageView(IRenderDevice& render_device) : ResourceCRTPBase(render_device), m_image_view(nullptr) {}

    ImageView::ImageView(IRenderDevice &render_device, const vk::ImageView image_view) :
        ResourceCRTPBase(render_device), m_image_view(image_view) {}

    ImageView::~ImageView() {
        if (m_image_view) {
            RenderDeviceImpl& device =  cast_to_vulkan_device(m_render_device);
            device.get_context().device().destroyImageView(m_image_view);
        }
    }
} // namespace avalanche::rendering::vulkan
