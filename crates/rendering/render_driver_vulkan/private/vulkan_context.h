#pragma once

#include "vulkan/vulkan.hpp"
#include "render_enums.h"
#include "render_device.h"

namespace avalanche::rendering::vulkan
{
    struct ExtensionAndLayer {
        std::vector<const char*> instance_extensions;
        std::vector<const char*> device_extensions;
        std::vector<const char*> instance_layers;
        std::vector<const char*> device_layers;

        static ExtensionAndLayer create_from_features(const DeviceFeatures& features);
    };

    class Context {
    public:
        explicit Context(const DeviceFeatures& required_features);

    protected:
        DeviceFeatures m_features;
        ExtensionAndLayer m_extensions_and_layers;
        vk::UniqueInstance m_instance;

    protected:
        vk::UniqueInstance create_instance();
        vk::PhysicalDevice pick_physical_device(EGPUPowerPreference preference);
    };
}

