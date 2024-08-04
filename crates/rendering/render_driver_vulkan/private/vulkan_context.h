#pragma once

#include "avalanche_render_driver_vulkan_export.h"
#include "vulkan/vulkan.hpp"
#include "render_enums.h"
#include "render_device.h"
#include "container/hash_map.hpp"
#include "container/unique_ptr.hpp"
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

    struct queue_not_available final : simple_error {
        queue_not_available();
    };

    class AvailableQueue {
    public:
        using index_type = uint32_t;

    private:
        hash_map<EQueueType, vector<index_type>> m_queue_family_indices{};

    public:
        explicit AvailableQueue(vk::PhysicalDevice physical_device);

        index_type acquire_queue_index(EQueueType queue_type);
    };

    class Context {
    public:
        explicit Context(const DeviceSettings& device_settings);

    protected:
        DeviceSettings m_device_settings;
        ExtensionAndLayer m_extensions_and_layers;

        unique_ptr<AvailableQueue> m_available_queue;

        vk::Instance m_instance;
        vk::PhysicalDevice m_primary_physical_device;
        vk::Device m_device;

    protected:
        static void init_vulkan_dispatcher();
        AVALANCHE_NO_DISCARD vk::Instance create_instance() const;
        AVALANCHE_NO_DISCARD vk::PhysicalDevice pick_physical_device(EGPUPowerPreference preference) const;
        AVALANCHE_NO_DISCARD vk::Device create_device() const;
    };
}

