#include "vulkan_context.h"
#include "vulkan_macro.h"
#include "GLFW/glfw3.h"

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
            res.instance_extensions.push_back(VK_EXT_LAYER_SETTINGS_EXTENSION_NAME);
            res.instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            if (features.ray_tracing) {
                // Should we enable manufacture specified extension?
                // res.instance_extensions.push_back(VK_NV_RAY_TRACING_VALIDATION_EXTENSION_NAME);
            }
            res.instance_layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        if (features.display) {
            res.instance_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            uint32_t count_glfw_extensions;
            const char **extensions = glfwGetRequiredInstanceExtensions(&count_glfw_extensions);
            for (uint32_t i = 0; i < count_glfw_extensions; ++i) {
                res.instance_extensions.push_back(extensions[i]);
            }
        }

        return res;
    }

    AvailableQueue::AvailableQueue(vk::PhysicalDevice physical_device) {
        auto props = physical_device.getQueueFamilyProperties2();

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

    queue_not_available::queue_not_available() : simple_error("Specified queue is not available") {}
}
