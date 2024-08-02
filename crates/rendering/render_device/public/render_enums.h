#pragma once


namespace avalanche::rendering {
    enum class EGPUPowerPreference {
        LowPower,
        HighPerformance,
    };

    enum class EQueueType {
        Graphics,
        Compute,
        Transfer,
        SparseBinding,
        VideoDecode,
        VideoEncode,
    };
}
