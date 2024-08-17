#include "resource/vulkan_format.h"


namespace avalanche::rendering::vulkan {

    vk::Format get_format(const EFormat format) {
        switch (format) {
            case EFormat::R8_Unorm:
                return vk::Format::eR8Unorm;
            case EFormat::R8_Snorm:
                return vk::Format::eR8Snorm;
            case EFormat::R8_Uint:
                return vk::Format::eR8Uint;
            case EFormat::R8_Sint:
                return vk::Format::eR8Sint;

            case EFormat::R16_Uint:
                return vk::Format::eR16Uint;
            case EFormat::R16_Sint:
                return vk::Format::eR16Sint;
            case EFormat::R16_Float:
                return vk::Format::eR16Sfloat;
            case EFormat::RG8_Unorm:
                return vk::Format::eR8G8Unorm;
            case EFormat::RG8_Snorm:
                return vk::Format::eR8G8Snorm;
            case EFormat::RG8_Uint:
                return vk::Format::eR8G8Uint;
            case EFormat::RG8_Sint:
                return vk::Format::eR8G8Sint;

            case EFormat::R32_Uint:
                return vk::Format::eR32Uint;
            case EFormat::R32_Sint:
                return vk::Format::eR32Sint;
            case EFormat::R32_Float:
                return vk::Format::eR32Sfloat;
            case EFormat::RG16_Uint:
                return vk::Format::eR16G16Uint;
            case EFormat::RG16_Sint:
                return vk::Format::eR16G16Sint;
            case EFormat::RG16_Float:
                return vk::Format::eR16G16Sfloat;
            case EFormat::RGBA8_Unorm:
                return vk::Format::eR8G8B8A8Unorm;
            case EFormat::RGBA8_Unorm_SRGB:
                return vk::Format::eR8G8B8A8Srgb;
            case EFormat::RGBA8_Snorm:
                return vk::Format::eR8G8B8A8Snorm;
            case EFormat::RGBA8_Uint:
                return vk::Format::eR8G8B8A8Uint;
            case EFormat::RGBA8_Sint:
                return vk::Format::eR8G8B8A8Sint;
            case EFormat::BGRA8_Unorm:
                return vk::Format::eB8G8R8A8Unorm;
            case EFormat::BGRA8_Unorm_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case EFormat::RGB9E5_UFloat:
                return vk::Format::eE5B9G9R9UfloatPack32;
            case EFormat::RGB10A2_Uint:
                return vk::Format::eA2B10G10R10UintPack32;
            case EFormat::RGB10A2_Unorm:
                return vk::Format::eA2B10G10R10UnormPack32;
            case EFormat::RG11B10_UFloat:
                return vk::Format::eB10G11R11UfloatPack32;

            case EFormat::RG32_Uint:
                return vk::Format::eR32G32Uint;
            case EFormat::RG32_Sint:
                return vk::Format::eR32G32Sint;
            case EFormat::RG32_Float:
                return vk::Format::eR32G32Sfloat;
            case EFormat::RGBA16_Uint:
                return vk::Format::eR16G16B16A16Uint;
            case EFormat::RGBA16_Sint:
                return vk::Format::eR16G16B16A16Sint;
            case EFormat::RGBA16_Float:
                return vk::Format::eR16G16B16A16Sfloat;

            case EFormat::RGBA32_Uint:
                return vk::Format::eR32G32B32A32Uint;
            case EFormat::RGBA32_Sint:
                return vk::Format::eR32G32B32A32Sint;
            case EFormat::RGBA32_Float:
                return vk::Format::eR32G32B32A32Sfloat;

            case EFormat::Stencil8:
                return vk::Format::eS8Uint;
            case EFormat::Depth16_Unorm:
                return vk::Format::eD16Unorm;
            case EFormat::Depth24_Plus:
                return vk::Format::eUndefined; // No direct match in Vulkan
            case EFormat::Depth24_Plus_Stencil8:
                return vk::Format::eD24UnormS8Uint; // Closest match
            case EFormat::Depth32_Float:
                return vk::Format::eD32Sfloat;
            case EFormat::Depth32_Float_Stencil8:
                return vk::Format::eD32SfloatS8Uint;

            // BC compressed formats
            case EFormat::BC1_RGBA_Unorm:
                return vk::Format::eBc1RgbaUnormBlock;
            case EFormat::BC1_RGBA_Unorm_SRGB:
                return vk::Format::eBc1RgbaSrgbBlock;
            case EFormat::BC2_RGBA_Unorm:
                return vk::Format::eBc2UnormBlock;
            case EFormat::BC2_RGBA_Unorm_SRGB:
                return vk::Format::eBc2SrgbBlock;
            case EFormat::BC3_RGBA_Unorm:
                return vk::Format::eBc3UnormBlock;
            case EFormat::BC3_RGBA_Unorm_SRGB:
                return vk::Format::eBc3SrgbBlock;
            case EFormat::BC4_R_Unorm:
                return vk::Format::eBc4UnormBlock;
            case EFormat::BC4_R_Snorm:
                return vk::Format::eBc4SnormBlock;
            case EFormat::BC5_RG_Unorm:
                return vk::Format::eBc5UnormBlock;
            case EFormat::BC5_RG_Snorm:
                return vk::Format::eBc5SnormBlock;
            case EFormat::BC6H_RGB_UFloat:
                return vk::Format::eBc6HUfloatBlock;
            case EFormat::BC6H_RGB_Float:
                return vk::Format::eBc6HSfloatBlock;
            case EFormat::BC7_RGBA_Unorm:
                return vk::Format::eBc7UnormBlock;
            case EFormat::BC7_RGBA_Unorm_SRGB:
                return vk::Format::eBc7SrgbBlock;

            // ETC2 compressed formats
            case EFormat::ETC2_RGB8_Unorm:
                return vk::Format::eEtc2R8G8B8UnormBlock;
            case EFormat::ETC2_RGB8_Unorm_SRGB:
                return vk::Format::eEtc2R8G8B8SrgbBlock;
            case EFormat::ETC2_RGB8A1_Unorm:
                return vk::Format::eEtc2R8G8B8A1UnormBlock;
            case EFormat::ETC2_RGB8A1_Unorm_SRGB:
                return vk::Format::eEtc2R8G8B8A1SrgbBlock;
            case EFormat::ETC2_RGBA8_Unorm:
                return vk::Format::eEtc2R8G8B8A8UnormBlock;
            case EFormat::ETC2_RGBA8_Unorm_SRGB:
                return vk::Format::eEtc2R8G8B8A8SrgbBlock;
            case EFormat::EAC_R11_Unorm:
                return vk::Format::eEacR11UnormBlock;
            case EFormat::EAC_R11_Snorm:
                return vk::Format::eEacR11SnormBlock;
            case EFormat::EAC_RG11_Unorm:
                return vk::Format::eEacR11G11UnormBlock;
            case EFormat::EAC_RG11_Snorm:
                return vk::Format::eEacR11G11SnormBlock;

            // ASTC compressed formats
            case EFormat::ASTC_4x4_Unorm:
                return vk::Format::eAstc4x4UnormBlock;
            case EFormat::ASTC_4x4_Unorm_SRGB:
                return vk::Format::eAstc4x4SrgbBlock;
            case EFormat::ASTC_5x4_Unorm:
                return vk::Format::eAstc5x4UnormBlock;
            case EFormat::ASTC_5x4_Unorm_SRGB:
                return vk::Format::eAstc5x4SrgbBlock;
            case EFormat::ASTC_5x5_Unorm:
                return vk::Format::eAstc5x5UnormBlock;
            case EFormat::ASTC_5x5_Unorm_SRGB:
                return vk::Format::eAstc5x5SrgbBlock;
            case EFormat::ASTC_6x5_Unorm:
                return vk::Format::eAstc6x5UnormBlock;
            case EFormat::ASTC_6x5_Unorm_SRGB:
                return vk::Format::eAstc6x5SrgbBlock;
            case EFormat::ASTC_6x6_Unorm:
                return vk::Format::eAstc6x6UnormBlock;
            case EFormat::ASTC_6x6_Unorm_SRGB:
                return vk::Format::eAstc6x6SrgbBlock;
            case EFormat::ASTC_8x5_Unorm:
                return vk::Format::eAstc8x5UnormBlock;
            case EFormat::ASTC_8x5_Unorm_SRGB:
                return vk::Format::eAstc8x5SrgbBlock;
            case EFormat::ASTC_8x6_Unorm:
                return vk::Format::eAstc8x6UnormBlock;
            case EFormat::ASTC_8x6_Unorm_SRGB:
                return vk::Format::eAstc8x6SrgbBlock;
            case EFormat::ASTC_8x8_Unorm:
                return vk::Format::eAstc8x8UnormBlock;
            case EFormat::ASTC_8x8_Unorm_SRGB:
                return vk::Format::eAstc8x8SrgbBlock;
            case EFormat::ASTC_10x5_Unorm:
                return vk::Format::eAstc10x5UnormBlock;
            case EFormat::ASTC_10x5_Unorm_SRGB:
                return vk::Format::eAstc10x5SrgbBlock;
            case EFormat::ASTC_10x6_Unorm:
                return vk::Format::eAstc10x6UnormBlock;
            case EFormat::ASTC_10x6_Unorm_SRGB:
                return vk::Format::eAstc10x6SrgbBlock;
            case EFormat::ASTC_10x8_Unorm:
                return vk::Format::eAstc10x8UnormBlock;
            case EFormat::ASTC_10x8_Unorm_SRGB:
                return vk::Format::eAstc10x8SrgbBlock;
            case EFormat::ASTC_10x10_Unorm:
                return vk::Format::eAstc10x10UnormBlock;
            case EFormat::ASTC_10x10_Unorm_SRGB:
                return vk::Format::eAstc10x10SrgbBlock;
            case EFormat::ASTC_12x10_Unorm:
                return vk::Format::eAstc12x10UnormBlock;
            case EFormat::ASTC_12x10_Unorm_SRGB:
                return vk::Format::eAstc12x10SrgbBlock;
            case EFormat::ASTC_12x12_Unorm:
                return vk::Format::eAstc12x12UnormBlock;
            case EFormat::ASTC_12x12_Unorm_SRGB:
                return vk::Format::eAstc12x12SrgbBlock;

            // Handle all other cases
            default:
                return vk::Format::eUndefined;
        }
    }

    vk::ImageAspectFlags get_image_aspect_flags(const EImageAspect image_aspect) {
        switch (image_aspect) {
            case EImageAspect::Color:
                return vk::ImageAspectFlagBits::eColor;
            case EImageAspect::Depth:
                return vk::ImageAspectFlagBits::eDepth;
            case EImageAspect::Stencil:
                return vk::ImageAspectFlagBits::eStencil;
            case EImageAspect::DepthStencil:
                return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            case EImageAspect::SparseMetadata:
                return vk::ImageAspectFlagBits::eMetadata;
            default:
                return vk::ImageAspectFlagBits::eNone;
        }
    }

    vk::ImageViewType get_image_view_type(const EImageDimension image_dimension) {
        switch (image_dimension) {
            case EImageDimension::Image2D:
                return vk::ImageViewType::e2D;
            case EImageDimension::Image3D:
                return vk::ImageViewType::e3D;
            case EImageDimension::ImageCube:
                return vk::ImageViewType::eCube;
            case EImageDimension::Image1DArray:
                return vk::ImageViewType::e1DArray;
            case EImageDimension::Image2DArray:
                return vk::ImageViewType::e2DArray;
            case EImageDimension::ImageCubeArray:
                return vk::ImageViewType::eCubeArray;
            case EImageDimension::Image1D:
            default:
                return vk::ImageViewType::e1D;
        }
    }

} // namespace avalanche::rendering::vulkan
