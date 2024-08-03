#pragma once

#include <cstdint>

namespace avalanche::rendering {

    enum class EGPUPowerPreference : uint8_t {
        LowPower,
        HighPerformance,
    };

    enum class EQueueType : uint8_t {
        Graphics,
        Compute,
        Transfer,
        SparseBinding,
        VideoDecode,
        VideoEncode,
        Max,
    };
}
