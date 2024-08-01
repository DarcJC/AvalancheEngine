#include "vulkan_context.h"
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

    Context::Context(const DeviceFeatures &required_features)
            : m_features(required_features) {
        if (required_features.display) {
            AVALANCHE_CHECK(glfwVulkanSupported(), "GLFW said vulkan isn't supported to using display functionality");
        }
        m_extensions_and_layers = ExtensionAndLayer::create_from_features(m_features);
        m_instance = create_instance();
    }
}
