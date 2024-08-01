
#include "logger.h"
#include "vulkan_context.h"
#include "render_enums.h"
#include "render_device.h"
#include <unordered_map>

#if defined(AVALANCHE_BUILD_SHARED)
// Expand vulkan dynamic dispatcher
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

namespace avalanche::rendering::vulkan {

    vk::UniqueInstance Context::create_instance() {
        vk::ApplicationInfo app_info("AvalancheEngine", AVALANCHE_PROJECT_VERSION_MAJOR, "AvalancheEngine", AVALANCHE_PROJECT_VERSION_MAJOR, VK_API_VERSION_1_3);
        vk::InstanceCreateInfo create_info({}, &app_info);

        create_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions_and_layers.instance_extensions.size());
        create_info.ppEnabledExtensionNames = m_extensions_and_layers.instance_extensions.data();
        create_info.enabledLayerCount = static_cast<uint32_t>(m_extensions_and_layers.instance_layers.size());
        create_info.ppEnabledLayerNames = m_extensions_and_layers.instance_layers.data();

        vk::UniqueInstance instance = vk::createInstanceUnique(create_info);
        return instance;
    }

    struct DeviceComparator {
        EGPUPowerPreference power_preference;
        bool required_display;

        static inline std::unordered_map<vk::PhysicalDeviceType, uint8_t> type_scores{
                {vk::PhysicalDeviceType::eOther,         0},
                {vk::PhysicalDeviceType::eCpu,           1},
                {vk::PhysicalDeviceType::eIntegratedGpu, 2},
                {vk::PhysicalDeviceType::eDiscreteGpu,   3},
                {vk::PhysicalDeviceType::eVirtualGpu,    3},
        };

        bool operator()(const vk::PhysicalDevice &lhs, const vk::PhysicalDevice &rhs) const {
            const vk::PhysicalDeviceProperties p1 = lhs.getProperties();
            const vk::PhysicalDeviceProperties p2 = rhs.getProperties();

            // TODO: check does queue families support display

            uint8_t score1 = type_scores[p1.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);
            uint8_t score2 = type_scores[p2.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);

            return score1 > score2;
        }
    };

    vk::PhysicalDevice Context::pick_physical_device(EGPUPowerPreference preference) {
        std::vector<vk::PhysicalDevice> devices = m_instance->enumeratePhysicalDevices();

        AVALANCHE_CHECK(!devices.empty(), "Can't find render devices");

        std::sort(devices.begin(), devices.end(), DeviceComparator{
            .power_preference = preference,
            .required_display = m_features.display,
        });

        return devices.front();
    }

}
