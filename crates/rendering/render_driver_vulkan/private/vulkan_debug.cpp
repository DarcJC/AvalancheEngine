//
// Created by DarcJC on 8/5/2024.
//

#include "vulkan_context.h"

namespace avalanche::rendering::vulkan {

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data) {

        const auto print_message_if_valid = [callback_data] (core::LogLevel level) {
            if (callback_data) {
                AVALANCHE_LOGGER.log(level, "Vulkan validation message:\n\t{}", callback_data->pMessage);
            } else {
                AVALANCHE_LOGGER.log(level, "Vulkan validation event without message");
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
