#pragma once

#include "vulkan/vulkan.hpp"
#include "render_enums.h"
#include "render_device.h"
#include "container/hash_map.hpp"
#include "container/vector.hpp"

namespace avalanche::rendering::vulkan
{
    struct ExtensionAndLayer {
        vector<const char*> instance_extensions;
        vector<const char*> device_extensions;
        vector<const char*> instance_layers;
        vector<const char*> device_layers;

        static ExtensionAndLayer create_from_features(const DeviceFeatures& features);
    };

    class AvailableQueue {
    };

    class Context {
    public:
        explicit Context(const DeviceSettings& device_settings);

    protected:
        DeviceSettings m_device_settings;
        ExtensionAndLayer m_extensions_and_layers;

        vk::Instance m_instance;
        vk::PhysicalDevice m_primary_physical_device;
        vk::Device m_device;

    protected:
        AVALANCHE_NO_DISCARD vk::Instance create_instance() const;
        AVALANCHE_NO_DISCARD vk::PhysicalDevice pick_physical_device(EGPUPowerPreference preference) const;
        AVALANCHE_NO_DISCARD vk::Device create_device() const;
    };
}

