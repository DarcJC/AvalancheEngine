
#include "logger.h"
#include "vulkan_context.h"
#include "render_enums.h"
#include "render_device.h"
#include "GLFW/glfw3.h"
#include <algorithm>
#include <unordered_map>

#if defined(AVALANCHE_BUILD_SHARED)
// Expand vulkan dynamic dispatcher
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

namespace avalanche::rendering::vulkan {

    vk::Instance Context::create_instance() const {
        init_vulkan_dispatcher();

        vk::ApplicationInfo app_info("AvalancheEngine", AVALANCHE_PROJECT_VERSION_MAJOR, "AvalancheEngine", AVALANCHE_PROJECT_VERSION_MAJOR, VK_API_VERSION_1_3);
        vk::InstanceCreateInfo create_info({}, &app_info);

        create_info.enabledExtensionCount = static_cast<uint32_t>(m_extensions_and_layers.instance_extensions.size());
        create_info.ppEnabledExtensionNames = m_extensions_and_layers.instance_extensions.data();
        create_info.enabledLayerCount = static_cast<uint32_t>(m_extensions_and_layers.instance_layers.size());
        create_info.ppEnabledLayerNames = m_extensions_and_layers.instance_layers.data();

        vk::Instance instance = vk::createInstance(create_info);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
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

    vk::PhysicalDevice Context::pick_physical_device(EGPUPowerPreference preference) const {
        std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();

        AVALANCHE_CHECK(!devices.empty(), "Can't find render devices");

        std::sort(devices.begin(), devices.end(), DeviceComparator{
            .power_preference = preference,
            .required_display = m_device_settings.required_features.display,
        });

        return devices.front();
    }

    vk::Device Context::create_device() const {
        return nullptr;
    }

    Context::Context(const DeviceSettings& device_settings)
            : m_device_settings(device_settings) {
        if (m_device_settings.required_features.display) {
            AVALANCHE_CHECK(glfwVulkanSupported(), "GLFW said vulkan isn't supported to using display functionality");
        }
        m_extensions_and_layers = ExtensionAndLayer::create_from_features(m_device_settings.required_features);
        AVALANCHE_CHECK(m_extensions_and_layers.validate_instance(), "Found invalid layer(s) or extension(s)");

        m_instance = create_instance();
        // Create debug messenger
        if (m_device_settings.required_features.debug) {
            inject_debug_callback();
        }

        m_primary_physical_device = pick_physical_device(m_device_settings.power_preference);
        m_available_queue = make_unique<AvailableQueue>(m_primary_physical_device);
    }

    void Context::init_vulkan_dispatcher() {
        AVALANCHE_MAYBE_UNUSED static bool val = ([] () {
            VULKAN_HPP_DEFAULT_DISPATCHER.init();
            return true;
        })();
    }

}
