#include "resource/vulkan_image.h"

#include "vulkan_format.h"
#include "vulkan_render_device_impl.h"


namespace avalanche::rendering::vulkan {


    Image::Image(IRenderDevice& render_device)
        : ResourceCRTPBase(render_device)
        , m_raw_handle(nullptr)
    {}

    Image::Image(IRenderDevice& render_device, const vk::Image image, const bool is_external/* = false*/)
        : ResourceCRTPBase(render_device)
        , m_is_committed(true)
        , m_is_external(is_external)
        , m_raw_handle(image)
    {}

    Image::~Image() {
        if (!m_is_external && m_raw_handle) {
            const auto& device = render_device<RenderDeviceImpl>();
            device.get_context().device().destroyImage(m_raw_handle);
        }
    }

    bool Image::is_committed() const { return m_is_committed; }

    bool Image::is_valid() const { return m_raw_handle; }

    bool Image::is_external() const { return m_is_external; }

    ImageView::ImageView(IRenderDevice& render_device) : ResourceCRTPBase(render_device), m_image_view(nullptr) {}

    ImageView::ImageView(IRenderDevice &render_device, const vk::ImageView image_view, bool is_external/* = false*/)
        : ResourceCRTPBase(render_device)
        , m_image_view(image_view)
        , m_is_external(is_external)
    {}

    bool ImageView::is_valid() const { return m_image_view; }

    bool ImageView::is_external() const { return m_is_external; }

    void ImageView::initialize(const ImageViewDesc& desc) {
        AVALANCHE_CHECK_RUNTIME(!m_image_view, "Double initialization isn't allowed.");
        AVALANCHE_CHECK(desc.image, "Invalid image handle");
        auto& device = render_device<RenderDeviceImpl>();
        const Image* image = device.get_resource_by_handle<Image>(desc.image);
        vk::ImageViewCreateInfo create_info{};
        create_info
            .setImage(image->m_raw_handle)
            .setViewType(get_image_view_type(desc.dimension))
            .setFormat(get_format(desc.format))
            .setComponents(vk::ComponentSwizzle::eIdentity)
            .setSubresourceRange(vk::ImageSubresourceRange {
                get_image_aspect_flags(desc.aspect),
                desc.base_mip_level,
                desc.num_levels,
                desc.base_array_layer,
                desc.num_layers
            })
        ;
        m_image_view = device.get_context().device().createImageView(create_info);
    }

    ImageView::~ImageView() {
        if (!m_is_external && m_image_view) {
            const auto& device = render_device<RenderDeviceImpl>();
            device.get_context().device().destroyImageView(m_image_view);
        }
    }
} // namespace avalanche::rendering::vulkan
