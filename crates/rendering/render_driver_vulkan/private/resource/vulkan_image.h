#pragma once

#include "render_resource.h"
#include "render_descriptor.h"
#include "vulkan/vulkan.hpp"
#include "vulkan_render_device.h"


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
         * @param is_external External resource will be not destroyed while Image deconstruct
         */
        Image(IRenderDevice& render_device, vk::Image image, bool is_external = false);

        ~Image() override;

        // Begin IResource API
        bool is_committed() const override;
        bool is_valid() const override;
        bool is_external() const override;
        // End IResource API

    private:
        bool m_is_committed = false;
        bool m_is_external = false;
        vk::Image m_raw_handle;

        friend class RenderDeviceImpl;
        friend class ImageView;
    };

    class ImageView : public ResourceCRTPBase<EResourceType::VulkanImageView> {
    public:
        explicit ImageView(IRenderDevice& render_device);
        ImageView(IRenderDevice& render_device, vk::ImageView image_view, bool is_external = false);
        ~ImageView() override;

        // Begin IResource API
        bool is_valid() const override;
        bool is_external() const override;
        // End IResource API

        void initialize(const ImageViewDesc& desc);

    private:
        vk::ImageView m_image_view;
        bool m_is_external = false;

        friend class VulkanWindow;
    };

}
