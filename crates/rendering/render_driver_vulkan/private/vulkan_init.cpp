#include <Logger.h>

#include "vulkan_context.h"

#include "render_enums.h"
#include "EASTL/algorithm.h"
#include "EASTL/sort.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"

using namespace avalanche::rendering;

struct DeviceComparator {
    EGPUPowerPreference power_preference;

    static inline eastl::unordered_map<vk::PhysicalDeviceType, uint8_t> type_scores {
        { vk::PhysicalDeviceType::eOther, 0 },
        { vk::PhysicalDeviceType::eCpu, 1 },
        { vk::PhysicalDeviceType::eIntegratedGpu, 2 },
        { vk::PhysicalDeviceType::eDiscreteGpu, 3 },
        { vk::PhysicalDeviceType::eVirtualGpu, 3 },
    };

    bool operator()(const vk::PhysicalDevice& lhs, const vk::PhysicalDevice& rhs) const {
        const vk::PhysicalDeviceProperties p1 = lhs.getProperties();
        const vk::PhysicalDeviceProperties p2 = rhs.getProperties();

        uint8_t score1 = type_scores[p1.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);
        uint8_t score2 = type_scores[p2.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);

        return score1 > score2;
    }
};

vk::PhysicalDevice pick_physical_device(vk::Instance instance, EGPUPowerPreference preference) {
    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    AVALANCHE_CHECK(!devices.empty(), "Can't find render devices");

    eastl::quick_sort(devices.begin(), devices.end(), DeviceComparator{preference});
    return devices.front();
}

