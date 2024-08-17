//
// Created by DarcJC on 8/5/2024.
//

#include "vulkan_context.h"
#include "container/string.hpp"

namespace avalanche::rendering::vulkan {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data) {

        const auto get_type_prefix_name = [message_type] () -> string {
            switch (message_type) {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    return "General";
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    return "Validation";
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    return "Performance";
                case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
                    return "Device_Address_Binding";
                default:
                    return "Unknown";
            }
        };

        const auto print_message_if_valid = [callback_data, get_type_prefix_name] (const core::LogLevel level) {
            if (callback_data) {
                AVALANCHE_LOGGER.log(level, "[Vulkan] [{}] {}", get_type_prefix_name(), callback_data->pMessage);
            } else {
                AVALANCHE_LOGGER.log(level, "[Vulkan] [{}] <Empty Message>", get_type_prefix_name());
            }
        };

        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                print_message_if_valid(core::LogLevel::Debug);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                print_message_if_valid(core::LogLevel::Info);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                print_message_if_valid(core::LogLevel::Warn);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            default:
                print_message_if_valid(core::LogLevel::Critical);
                return VK_TRUE;
        }

        return VK_FALSE; // Tell vulkan validation layer we had handled this message.
    }

    void Context::inject_debug_callback() {
        vk::DebugUtilsMessengerCreateInfoEXT create_info{};
        create_info
        .setMessageSeverity(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        )
        .setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        )
        .setPfnUserCallback(debug_callback)
        .setPUserData(this);
        m_debug_messenger = make_unique<vk::DebugUtilsMessengerEXT>(m_instance.createDebugUtilsMessengerEXT(create_info));
    }

}
