#pragma once

#include "vulkan/vulkan.hpp"
#include "render_resource.h"


namespace avalanche::rendering::EResourceType {
    constexpr resource_type_t VulkanImage = 256;
    constexpr resource_type_t VulkanImageView = 257;
} // namespace EResourceType

namespace avalanche::rendering::vulkan {

    class Image : public ResourceCRTPBase<EResourceType::VulkanImage> {
    public:
        /**
         * @brief Create a nullptr handle and can create from descriptor later
         */
        explicit Image(IRenderDevice& render_device);
        /**
         * @brief Create from an image handle which committed memory
         * @param image an image that has committed memory
         * @param render_device
         */
        Image(IRenderDevice& render_device, vk::Image image);

        ~Image() override;

        // Begin IResource API
        bool is_committed() const override;
        // End IResource API

    private:
        bool m_is_committed = false;
        vk::Image m_raw_handle;

        friend class RenderDeviceImpl;
    };

    class ImageView : public ResourceCRTPBase<EResourceType::VulkanImageView> {
    public:
        explicit ImageView(IRenderDevice& render_device);
        ImageView(IRenderDevice& render_device, vk::ImageView image_view);

        ~ImageView() override;

    private:
        vk::ImageView m_image_view;
    };

}
