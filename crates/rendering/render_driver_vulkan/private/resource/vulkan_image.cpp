#include "resource/vulkan_image.h"


namespace avalanche::rendering::vulkan {


    Image::Image()
        : m_raw_handle(nullptr)
    {}

    Image::Image(const vk::Image image) : m_is_committed(true), m_raw_handle(image) {}

    bool Image::is_committed() const { return m_is_committed; }

    ImageView::ImageView() : m_image_view(nullptr) {}

    ImageView::ImageView(const vk::ImageView image_view)
        : m_image_view(image_view)
    {}
} // namespace avalanche::rendering::vulkan
