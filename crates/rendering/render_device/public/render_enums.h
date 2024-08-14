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

}
