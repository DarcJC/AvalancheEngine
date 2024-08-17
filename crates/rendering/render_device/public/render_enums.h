#pragma once

#include <cstdint>

namespace avalanche::rendering {

    enum class EGPUPowerPreference : uint8_t {
        LowPower,
        HighPerformance,
    };

    enum class EQueueType : uint8_t {
        Graphics = 0,
        Compute,
        Transfer,
        SparseBinding,
        VideoDecode,
        VideoEncode,
        Max,
    };

    enum class EGraphicsAPIType : uint8_t {
        None,
        Vulkan,
        D3D12,
        Max,
    };

    /**
     * @brief Texture Format
     * Using same naming rule as W3C WebGPU.
     *
     * r, g, b, a = red, green, blue, alpha
     * unorm = unsigned normalized (0 to 1)
     * snorm = signed normalized (-1 to 1)
     * uint = unsigned int
     * sint = signed int
     * float = floating point
     */
    enum class EFormat : uint32_t {
        Unknown = 0,
        // 8-bit formats
        R8_Unorm,
        R8_Snorm,
        R8_Uint,
        R8_Sint,

        // 16-bit formats
        R16_Uint,
        R16_Sint,
        R16_Float,
        RG8_Unorm,
        RG8_Snorm,
        RG8_Uint,
        RG8_Sint,

        // 32-bit formats
        R32_Uint,
        R32_Sint,
        R32_Float,
        RG16_Uint,
        RG16_Sint,
        RG16_Float,
        RGBA8_Unorm,
        RGBA8_Unorm_SRGB,
        RGBA8_Snorm,
        RGBA8_Uint,
        RGBA8_Sint,
        BGRA8_Unorm,
        BGRA8_Unorm_SRGB,
        RGB9E5_UFloat,
        RGB10A2_Uint,
        RGB10A2_Unorm,
        RG11B10_UFloat,

        // 64-bit formats
        RG32_Uint,
        RG32_Sint,
        RG32_Float,
        RGBA16_Uint,
        RGBA16_Sint,
        RGBA16_Float,

        // 128-bit formats
        RGBA32_Uint,
        RGBA32_Sint,
        RGBA32_Float,

        // Depth/stencil formats
        Stencil8,
        Depth16_Unorm,
        Depth24_Plus,
        Depth24_Plus_Stencil8,
        Depth32_Float,
        Depth32_Float_Stencil8,

        // BC compressed formats
        BC1_RGBA_Unorm,
        BC1_RGBA_Unorm_SRGB,
        BC2_RGBA_Unorm,
        BC2_RGBA_Unorm_SRGB,
        BC3_RGBA_Unorm,
        BC3_RGBA_Unorm_SRGB,
        BC4_R_Unorm,
        BC4_R_Snorm,
        BC5_RG_Unorm,
        BC5_RG_Snorm,
        BC6H_RGB_UFloat,
        BC6H_RGB_Float,
        BC7_RGBA_Unorm,
        BC7_RGBA_Unorm_SRGB,

        // ETC2 compressed formats
        ETC2_RGB8_Unorm,
        ETC2_RGB8_Unorm_SRGB,
        ETC2_RGB8A1_Unorm,
        ETC2_RGB8A1_Unorm_SRGB,
        ETC2_RGBA8_Unorm,
        ETC2_RGBA8_Unorm_SRGB,
        EAC_R11_Unorm,
        EAC_R11_Snorm,
        EAC_RG11_Unorm,
        EAC_RG11_Snorm,

        // ASTC compressed formats
        ASTC_4x4_Unorm,
        ASTC_4x4_Unorm_SRGB,
        ASTC_5x4_Unorm,
        ASTC_5x4_Unorm_SRGB,
        ASTC_5x5_Unorm,
        ASTC_5x5_Unorm_SRGB,
        ASTC_6x5_Unorm,
        ASTC_6x5_Unorm_SRGB,
        ASTC_6x6_Unorm,
        ASTC_6x6_Unorm_SRGB,
        ASTC_8x5_Unorm,
        ASTC_8x5_Unorm_SRGB,
        ASTC_8x6_Unorm,
        ASTC_8x6_Unorm_SRGB,
        ASTC_8x8_Unorm,
        ASTC_8x8_Unorm_SRGB,
        ASTC_10x5_Unorm,
        ASTC_10x5_Unorm_SRGB,
        ASTC_10x6_Unorm,
        ASTC_10x6_Unorm_SRGB,
        ASTC_10x8_Unorm,
        ASTC_10x8_Unorm_SRGB,
        ASTC_10x10_Unorm,
        ASTC_10x10_Unorm_SRGB,
        ASTC_12x10_Unorm,
        ASTC_12x10_Unorm_SRGB,
        ASTC_12x12_Unorm,
        ASTC_12x12_Unorm_SRGB,

        Max,
    };

    enum class EImageAspect {
        None = 0,
        Color,
        Depth,
        Stencil,
        DepthStencil,
        SparseMetadata,
        Max,
    };

    enum class EImageDimension {
        Image1D,
        Image2D,
        Image3D,
        ImageCube,
        Image1DArray,
        Image2DArray,
        ImageCubeArray,
        Max,
    };

}
