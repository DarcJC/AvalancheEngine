#pragma once

#include "vulkan/vulkan.hpp"
#include "render_enums.h"

namespace avalanche::rendering::vulkan {

    vk::Format get_format(EFormat format);

    vk::ImageAspectFlags get_image_aspect_flags(EImageAspect image_aspect);

    vk::ImageViewType get_image_view_type(EImageDimension image_dimension);

} // namespace avalanche::rendering::vulkan
