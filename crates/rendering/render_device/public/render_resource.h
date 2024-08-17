#pragma once
#include <atomic>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "avalanche_render_device_export.h"
#include "logger.h"
#include "polyfill.h"


namespace avalanche::rendering {

    class IRenderDevice;

    using resource_type_t = uint16_t;

    namespace EResourceType {
        constexpr resource_type_t Unknown = 0;
        // 255 - 512 Is allocated to Vulkan Graphics API
        // 512 - 768 Is allocated to DX12 Graphics API
    }


    namespace detail {
        class ResourceFlags {
            struct Bits {
                static constexpr uint32_t MarkForDelete = 1 << 30;
                static constexpr uint32_t Deleting = 1 << 31;
                static constexpr uint32_t RefCounterMask = ~(MarkForDelete | Deleting);
            };

            std::atomic<uint32_t> m_value { 0 };

        public:
            AVALANCHE_RENDER_DEVICE_API int32_t increase_rc(std::memory_order memory_order = std::memory_order_seq_cst);

            AVALANCHE_RENDER_DEVICE_API int32_t decrease_rc(std::memory_order memory_order = std::memory_order_seq_cst);

            AVALANCHE_RENDER_DEVICE_API bool mark_for_delete(std::memory_order memory_order = std::memory_order_seq_cst);

            AVALANCHE_RENDER_DEVICE_API bool unmark_for_delete(std::memory_order memory_order = std::memory_order_seq_cst);

            AVALANCHE_RENDER_DEVICE_API bool set_deleting();

            AVALANCHE_RENDER_DEVICE_API bool is_valid(std::memory_order memory_order = std::memory_order_seq_cst) const;

            AVALANCHE_RENDER_DEVICE_API bool is_marked_for_delete(std::memory_order memory_order = std::memory_order_seq_cst) const;

            AVALANCHE_RENDER_DEVICE_API int32_t ref_count(std::memory_order memory_order) const;
        };
    }

    class IResource {
    public:
        explicit IResource(IRenderDevice& render_device);

        AVALANCHE_RENDER_DEVICE_API virtual ~IResource();

        AVALANCHE_RENDER_DEVICE_API virtual resource_type_t get_resource_type() const = 0;

        AVALANCHE_RENDER_DEVICE_API virtual bool is_committed() const;

        AVALANCHE_RENDER_DEVICE_API virtual bool is_valid() const;

        AVALANCHE_RENDER_DEVICE_API virtual bool is_external() const;

        AVALANCHE_RENDER_DEVICE_API virtual IRenderDevice& get_render_device();

        AVALANCHE_NO_DISCARD AVALANCHE_RENDER_DEVICE_API detail::ResourceFlags& flags();

    protected:
        mutable detail::ResourceFlags m_flag;
        IRenderDevice& m_render_device;
    };

    template <resource_type_t ResourceType, typename BaseType = IResource>
    class ResourceCRTPBase : public BaseType {
    public:
        using Super = BaseType;

        explicit ResourceCRTPBase(IRenderDevice& render_device) : BaseType(render_device) {}

        ResourceCRTPBase(const ResourceCRTPBase&) = delete;
        ResourceCRTPBase& operator=(const ResourceCRTPBase&) = delete;
        ResourceCRTPBase(ResourceCRTPBase&&) = delete;
        ResourceCRTPBase& operator=(ResourceCRTPBase&&) = delete;

        static constexpr resource_type_t resource_type = ResourceType;

        AVALANCHE_NO_DISCARD resource_type_t get_resource_type() const override {
            return resource_type;
        }

        template <typename DeviceType>
        DeviceType& render_device() {
            AVALANCHE_CHECK(this->get_render_device().get_graphics_api_type() == DeviceType::graphics_api_type, "Device doesn't corresponding to target type");
            return static_cast<DeviceType&>(this->get_render_device());
        }
    };

    template <typename T>
    concept is_render_resource = requires () {
        { T::resource_type } -> std::same_as<resource_type_t>;
    };

    template <typename CastTo>
    requires is_render_resource<CastTo>
    CastTo* cast(IResource* resource) {
        if (nullptr != resource) {
            if (CastTo::resource_type == resource->get_resource_type()) {
                return static_cast<CastTo *>(resource);
            }
        }
        return nullptr;
    }

    template <class T, typename... Args>
    requires std::derived_from<T, IResource> && std::constructible_from<T, Args...>
    T* construct_resource(Args&&... args) {
        return new T(std::forward<Args>(args)...);
    }

    template <class T>
    requires std::derived_from<T, IResource>
    void delete_resource(T* resource) {
        delete resource;
    }

} // namespace avalanche::rendering
