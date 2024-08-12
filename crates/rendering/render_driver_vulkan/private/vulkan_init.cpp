
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

        create_info
            .setEnabledExtensionCount(static_cast<uint32_t>(m_extensions_and_layers.instance_extensions.size()))
            .setPEnabledExtensionNames(m_extensions_and_layers.instance_extensions)
            .setEnabledLayerCount(static_cast<uint32_t>(m_extensions_and_layers.instance_layers.size()))
            .setPEnabledLayerNames(m_extensions_and_layers.instance_layers)
        ;

        vk::Instance instance = vk::createInstance(create_info);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
        return instance;
    }

    struct DeviceComparator {
        EGPUPowerPreference power_preference;
        vk::Instance instance;
        bool required_display;

        static inline std::unordered_map<vk::PhysicalDeviceType, uint8_t> type_scores{
                {vk::PhysicalDeviceType::eOther,         0},
                {vk::PhysicalDeviceType::eCpu,           1},
                {vk::PhysicalDeviceType::eIntegratedGpu, 2},
                {vk::PhysicalDeviceType::eDiscreteGpu,   3},
                {vk::PhysicalDeviceType::eVirtualGpu,    3},
        };

        bool support_display(vk::PhysicalDevice pd) const {
            const auto props =  pd.getQueueFamilyProperties2();
            for (int32_t i = 0; i < props.size(); ++i) {
                if (glfwGetPhysicalDevicePresentationSupport(instance, pd, i)) {
                    return true;
                }
            }
            return false;
        }

        bool operator()(const vk::PhysicalDevice &lhs, const vk::PhysicalDevice &rhs) const {
            const vk::PhysicalDeviceProperties p1 = lhs.getProperties();
            const vk::PhysicalDeviceProperties p2 = rhs.getProperties();

            uint8_t score1 = type_scores[p1.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);
            uint8_t score2 = type_scores[p2.deviceType] * (power_preference == EGPUPowerPreference::LowPower ? -1 : 1);

            if (required_display) {
                if (!support_display(lhs)) {
                    score1 -= 100;
                }
                if (!support_display(rhs)) {
                    score2 -= 100;
                }
            }

            return score1 > score2;
        }
    };

    vk::PhysicalDevice Context::pick_physical_device(EGPUPowerPreference preference) const {
        std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();

        AVALANCHE_CHECK_RUNTIME(!devices.empty(), "Can't find render devices");

        std::sort(devices.begin(), devices.end(), DeviceComparator{
            .power_preference = preference,
            .instance = m_instance,
            .required_display = m_device_settings.required_features.display,
        });

        return devices.front();
    }

    vk::Device Context::create_device() const {
        vk::DeviceCreateInfo device_create_info{};
        const vk::PhysicalDeviceFeatures2 features = m_primary_physical_device.getFeatures2();
        device_create_info
            .setEnabledExtensionCount(static_cast<uint32_t>(m_extensions_and_layers.device_extensions.size()))
            .setPEnabledExtensionNames(m_extensions_and_layers.device_extensions)
            .setEnabledLayerCount(static_cast<uint32_t>(m_extensions_and_layers.device_layers.size()))
            .setPEnabledLayerNames(m_extensions_and_layers.device_layers)
            .setPEnabledFeatures(nullptr)
            .setPNext(&features)
        ;

        return m_primary_physical_device.createDevice(device_create_info);
    }

    Context::Context(const DeviceSettings& device_settings)
            : m_device_settings(device_settings) {
        m_extensions_and_layers = ExtensionAndLayer::create_from_features(m_device_settings.required_features);
        AVALANCHE_CHECK_RUNTIME(m_extensions_and_layers.validate_instance(), "Found invalid instance layer(s) or extension(s)");

        m_instance = create_instance();

        // Create debug messenger
        if (m_device_settings.required_features.debug) {
            inject_debug_callback();
        }

        m_primary_physical_device = pick_physical_device(m_device_settings.power_preference);
        AVALANCHE_CHECK_RUNTIME(m_extensions_and_layers.validate_device(m_primary_physical_device), "Found invalid device layer(s) or extension(s)");

        m_available_queue = make_unique<AvailableQueue>(m_primary_physical_device);

        m_device = create_device();
    }

    Context::~Context() {
        m_device.destroy();
        if (m_debug_messenger) {
            m_instance.destroyDebugUtilsMessengerEXT(*m_debug_messenger);
        }
        m_debug_messenger.reset();
        m_instance.destroy();
        m_available_queue.reset();
    }

    vk::Instance Context::instance() const {
        return m_instance;
    }

    vk::PhysicalDevice Context::physical_device() const {
        return m_primary_physical_device;
    }

    vk::Device Context::device() const {
        return m_device;
    }

    void Context::init_vulkan_dispatcher() {
        AVALANCHE_MAYBE_UNUSED static bool val = ([] () {
            VULKAN_HPP_DEFAULT_DISPATCHER.init();
            return true;
        })();
    }

}
