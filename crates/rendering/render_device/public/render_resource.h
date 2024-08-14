#pragma once
#include "avalanche_render_device_export.h"
#include <cstddef>
#include <atomic>
#include <cstdint>
#include <type_traits>
#include <concepts>
#include "polyfill.h"
#include "logger.h"


namespace avalanche::rendering {

    using resource_type_t = uint32_t;

    namespace EResourceType {
        constexpr resource_type_t Unknown = 0;
    }

    class AVALANCHE_RENDER_DEVICE_API IResource {
    public:
        virtual ~IResource();

        AVALANCHE_NO_DISCARD virtual resource_type_t get_resource_type() const = 0;

    protected:
        class Flags {
            struct Bits {
                static constexpr uint32_t MarkForDelete = 1 << 30;
                static constexpr uint32_t Deleting = 1 << 31;
                static constexpr uint32_t RefCounterMask = ~(MarkForDelete | Deleting);
            };

            std::atomic<uint32_t> m_value { 0 };

        public:
            int32_t increase_rc(std::memory_order memory_order = std::memory_order_seq_cst);

            int32_t decrease_rc(std::memory_order memory_order = std::memory_order_seq_cst);

            bool mark_for_delete(std::memory_order memory_order = std::memory_order_seq_cst);

            bool unmark_for_delete(std::memory_order memory_order = std::memory_order_seq_cst);

            bool set_deleting();

            bool is_valid(std::memory_order memory_order = std::memory_order_seq_cst) const;

            bool is_marked_for_delete(std::memory_order memory_order = std::memory_order_seq_cst) const;

            int32_t ref_count(std::memory_order memory_order) const;
        };

        mutable Flags m_flag;
    };

    template <resource_type_t ResourceType, typename BaseType = IResource>
    class ResourceCTRPBase : public BaseType {
    public:
        using Super = BaseType;

        static constexpr resource_type_t resource_type = ResourceType;

        AVALANCHE_NO_DISCARD resource_type_t get_resource_type() const override {
            return resource_type;
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

} // namespace avalanche::rendering
