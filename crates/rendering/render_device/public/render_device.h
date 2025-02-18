#pragma once

#include "avalanche_render_device_export.h"
#include "render_enums.h"
#include "render_resource.h"
#include "render_descriptor.h"

namespace avalanche::rendering {

    class IRenderGraph;
    class IResource;
    class IRenderDevice;
    struct ImageViewDesc;

    struct DeviceFeatures {
        bool ray_tracing = true;
        bool display = false;
        bool line_rasterization = true;
        bool mesh_shader = true;
        bool performance_query = false;
        bool debug = false;
        bool tile_based = false; // Not impl yet
    };

    struct DeviceSettings {
        DeviceFeatures required_features{};
        EGPUPowerPreference power_preference = EGPUPowerPreference::HighPerformance;
    };

    class AVALANCHE_RENDER_DEVICE_API IRenderResourcePool {
    public:
        static IRenderResourcePool* new_pool(IRenderDevice* render_device);
        static void delete_pool(const IRenderResourcePool * pool);

        virtual ~IRenderResourcePool();

        virtual IResource* get_resource(const handle_t& handle) = 0;
        virtual handle_t register_resource(IResource* resource) = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API IRenderDevice {
    public:
        IRenderDevice();
        virtual ~IRenderDevice();

        virtual EGraphicsAPIType get_graphics_api_type() = 0;

        virtual void wait_for_device_idle() = 0;
        virtual void enable_display_support() = 0;
        virtual void disable_display_support() = 0;

        virtual void on_handle_created(const handle_t& handle);
        virtual void on_handle_free(const handle_t& handle);

        virtual void clean_pending_delete_resource() = 0;

        virtual handle_t create_image_view(const ImageViewDesc &desc) = 0;

        virtual handle_t create_command_buffer(const CommandBufferDesc &desc) = 0;
        virtual void start_encoding_command(handle_t command_buffer) = 0;
        virtual void finish_encoding_command(handle_t command_buffer) = 0;

        virtual handle_t create_semaphore() = 0;
        virtual handle_t create_fence(const FenceDesc &desc) = 0;
        virtual uint32_t get_fence_status(handle_t fence) = 0;
        /**
         * @brief Blocking on thread with a fence.
         *
         * @note In Vulkan GAPI, the fence isn't a timeline but binary.
         *
         * @param fence Target fence
         * @param excepted_value Wait util fence value == the excepted value
         * @param timeout is the timeout period in units of nanoseconds.
         */
        virtual bool block_on_fence(handle_t fence, uint32_t excepted_value, uint64_t timeout) = 0;

        template <typename ResourceType>
        requires std::derived_from<ResourceType, IResource>
        handle_t create_uninitialized_resource() {
            return get_resource_pool()->register_resource(construct_resource<ResourceType>(*this));
        }

        template <typename ResourceType, typename DescType, typename... Args>
        requires std::derived_from<ResourceType, IResource>
        handle_t create_resource(const DescType& desc, Args&&... args) {
            ResourceType* resource = construct_resource<ResourceType>(*this);
            resource->initialize(desc, std::forward<Args>(args)...);
            return get_resource_pool()->register_resource(resource);
        }

        template <typename ResourceType, typename... Args>
        requires std::derived_from<ResourceType, IResource>
        handle_t create_resource_no_desc(Args&&... args) {
            ResourceType* resource = construct_resource<ResourceType>(*this);
            resource->initialize(std::forward<Args>(args)...);
            return get_resource_pool()->register_resource(resource);
        }

        template <typename ResourceType>
        requires std::derived_from<ResourceType, IResource>
        ResourceType* get_resource_by_handle(const handle_t& handle) {
            IResource* resource = get_resource_pool()->get_resource(handle);
            AVALANCHE_CHECK(resource != nullptr && resource->get_resource_type() == ResourceType::resource_type, "");
            return static_cast<ResourceType*>(resource);
        }

    protected:
        virtual void add_pending_delete_resource(IResource* resource);
        AVALANCHE_NO_DISCARD IRenderResourcePool* get_resource_pool();

        friend class RenderResourcePool;

    private:
        IRenderResourcePool* m_render_resource_pool;
    };

}
