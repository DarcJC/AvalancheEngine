#include "container/string.hpp"
#include "vulkan_context.h"
#include "vulkan_macro.h"
#include "GLFW/glfw3.h"
#include <set>

namespace avalanche::rendering::vulkan {

    ExtensionAndLayer ExtensionAndLayer::create_from_features(
            const DeviceFeatures &features) {
        ExtensionAndLayer res{};

        res.device_extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        res.device_extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

        if (features.ray_tracing) {
            res.device_extensions.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
            res.device_extensions.push_back(VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME);
            res.device_extensions.push_back(VK_KHR_RAY_QUERY_EXTENSION_NAME);
            res.device_extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
            res.device_extensions.push_back(VK_KHR_RAY_TRACING_POSITION_FETCH_EXTENSION_NAME);
            res.device_extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
        }

        if (features.line_rasterization) {
            res.device_extensions.push_back(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME);
        }

        if (features.mesh_shader) {
            res.device_extensions.push_back(VK_NV_MESH_SHADER_EXTENSION_NAME);
        }

        if (features.performance_query) {
            res.device_extensions.push_back(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME);
        }

        if (features.debug) {
            res.instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            // VK_EXT_validation_features was deprecated and replaced with VK_EXT_layer_settings enabling all settings to be controlled programmatically
            // https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/main/docs/khronos_validation_layer.md
            res.instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            if (features.ray_tracing) {
                // Should we enable manufacture specified extension?
                // res.instance_extensions.push_back(VK_NV_RAY_TRACING_VALIDATION_EXTENSION_NAME);
            }
        }

        if (features.display) {
            glfwInit();
            AVALANCHE_CHECK_RUNTIME(glfwVulkanSupported(), "GLFW doesn't compatible with current vulkan runtime");
            res.device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            uint32_t count_glfw_extensions;
            const char **extensions = glfwGetRequiredInstanceExtensions(&count_glfw_extensions);
            for (uint32_t i = 0; i < count_glfw_extensions; ++i) {
                res.instance_extensions.push_back(extensions[i]);
            }
        }

        return res;
    }

    bool ExtensionAndLayer::validate_instance() const {
        const vk::DynamicLoader dynamic_loader;
        const auto vkGetInstanceProcAddr = dynamic_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        AVALANCHE_CHECK_RUNTIME(vkGetInstanceProcAddr, "");
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        const auto available_layers = vk::enumerateInstanceLayerProperties();
        vector<string> available_layer_names{};
        for (const auto& layer : available_layers) {
            available_layer_names.push_back(std::string_view(layer.layerName));
        }

        const auto available_extensions = vk::enumerateInstanceExtensionProperties();
        vector<string> available_extension_names;
        for (const auto& extension : available_extensions) {
            available_extension_names.push_back(std::string_view(extension.extensionName));
        }

        for (const auto& layer : instance_layers) {
            if (!available_extension_names.contains(layer)) {
                AVALANCHE_ENSURE(false, "Invalid instance layer: {}", layer);
                return false;
            }
        }

        for (const auto& extension : instance_extensions) {
            if (!available_extension_names.contains(extension)) {
                AVALANCHE_ENSURE(false, "Invalid instance extension: {}", extension);
                return false;
            }
        }

        return true;
    }

    bool ExtensionAndLayer::validate_device(const vk::PhysicalDevice physical_device) const {
        const auto available_layers = physical_device.enumerateDeviceLayerProperties();
        vector<string> available_layer_names{};
        for (const auto& layer : available_layers) {
            available_layer_names.push_back(std::string_view(layer.layerName));
        }

        const auto available_extensions = physical_device.enumerateDeviceExtensionProperties();
        vector<string> available_extension_names;
        for (const auto& extension : available_extensions) {
            available_extension_names.push_back(std::string_view(extension.extensionName));
        }

        for (const auto& layer : device_layers) {
            if (!available_extension_names.contains(layer)) {
                AVALANCHE_ENSURE(false, "Invalid device layer: {}", layer);
                return false;
            }
        }

        for (const auto& extension : device_extensions) {
            if (!available_extension_names.contains(extension)) {
                AVALANCHE_ENSURE(false, "Invalid device extension: {}", extension);
                return false;
            }
        }

        return true;
    }

    AvailableQueue::AvailableQueue(vk::PhysicalDevice physical_device) {
        const auto props = physical_device.getQueueFamilyProperties2();

        for (auto i = static_cast<uint8_t>(EQueueType::Graphics); i < static_cast<uint8_t>(EQueueType::Max); ++i) {
            m_queue_family_indices.insert_defaulted_if_absent(static_cast<EQueueType>(i));
        }

        for (uint32_t i = 0; i < props.size(); ++i) {
            const vk::QueueFamilyProperties& queue_family = props[i].queueFamilyProperties;

            if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics) {
                m_queue_family_indices[EQueueType::Graphics].push_back(i);
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eCompute) {
                m_queue_family_indices[EQueueType::Compute].push_back(i);
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eTransfer) {
                m_queue_family_indices[EQueueType::Transfer].push_back(i);
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eSparseBinding) {
                m_queue_family_indices[EQueueType::SparseBinding].push_back(i);
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eVideoDecodeKHR) {
                m_queue_family_indices[EQueueType::VideoDecode].push_back(i);
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eVideoEncodeKHR) {
                m_queue_family_indices[EQueueType::VideoEncode].push_back(i);
            }
        }
    }

    AvailableQueue::index_type AvailableQueue::acquire_queue_index(EQueueType queue_type) {
        if (!m_queue_family_indices.contains(queue_type)) {
            raise_exception(queue_not_available());
        }
        auto indices = m_queue_family_indices[queue_type];
        if (indices.is_empty()) {
            raise_exception(queue_not_available());
        }
        return indices.front_item();
    }
    vector<uint32_t> AvailableQueue::acquire_all_queue_family_indices() {
        vector<uint32_t> res;
        std::set<uint32_t> s;
        for (int i = static_cast<int>(EQueueType::Graphics); i < static_cast<uint8_t>(EQueueType::Max); ++i) {
            s.insert(acquire_queue_index(static_cast<EQueueType>(i)));
        }
        for (uint32_t i = 0; i < s.size(); ++i) {
            res.push_back(i);
        }
        return res;
    }

    queue_not_available::queue_not_available() : simple_error("Specified queue is not available") {}
}
