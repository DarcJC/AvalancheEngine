#pragma once

#include <cstddef>
#include <cstdint>
#include "container/vector.hpp"
#include "render_resource.h"
#include "render_enums.h"


namespace avalanche::rendering {

    struct AVALANCHE_RENDER_DEVICE_API BindGroupLayoutItem {
        uint32_t binding_slot = 0;
        shader_stage_t stage_visibility_flag = ShaderStageFlags::All;
        BindingResourceType type = BindingResourceType::None;
        size_t push_constant_size = 0;

        bool operator==(const BindGroupLayoutItem &other) const;
        bool operator!=(const BindGroupLayoutItem &other) const;

#define AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(TYPE) \
        static constexpr BindGroupLayoutItem TYPE(const uint32_t slot, shader_stage_t visibility_flags = ShaderStageFlags::All) { \
            BindGroupLayoutItem result{}; \
            result.binding_slot = slot; \
            result.stage_visibility_flag = visibility_flags; \
            return result; \
        }

        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(SampledImage);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(StorageImage);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(UniformTexelBuffer);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(StorageTexelBuffer);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(UniformBuffer);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(StorageBuffer);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(Sampler);
        AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY(AccelerateStructure);

        static constexpr BindGroupLayoutItem PushConstants(const uint32_t slot, const size_t size, shader_stage_t visibility_flags = ShaderStageFlags::All) {
            BindGroupLayoutItem result{};
            result.binding_slot = slot;
            result.push_constant_size = size;
            result.stage_visibility_flag = visibility_flags;
            return result;
        }

#undef AVALANCHE_BIND_GROUP_LAYOUT_ITEM_UTILITY
    };

    static constexpr uint32_t GMaxBindingsPerLayout = 128;

    struct BindGroupLayoutDesc {
        size_t group_index = 0;
        vector<BindGroupLayoutItem> bindings {GMaxBindingsPerLayout};

        AVALANCHE_RENDER_DEVICE_API BindGroupLayoutDesc& add_item(const BindGroupLayoutItem& new_item);
        AVALANCHE_RENDER_DEVICE_API BindGroupLayoutDesc& set_group_index(size_t new_index);
    };

    struct BindlessLayoutDesc {
        uint32_t first_slot = 0;
        uint32_t max_capacity = 0;
        vector<BindGroupLayoutItem> register_spaces { 16 };
    };

    struct RenderPipelineDesc {
        // TODO

        handle_t vertex_shader;
        handle_t hull_shader;
        handle_t domain_shader;
        handle_t geometry_shader;
        handle_t pixel_shader;

        vector<handle_t> binding_layouts;
    };

} // namespace avalanche::rendering
