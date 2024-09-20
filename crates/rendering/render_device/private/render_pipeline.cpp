#include "render_pipeline.h"


namespace avalanche::rendering {
    bool BindGroupLayoutItem::operator==(const BindGroupLayoutItem &other) const {
        return other.binding_slot == binding_slot && other.type == type &&
               other.stage_visibility_flag == stage_visibility_flag && other.push_constant_size == push_constant_size;
    }

    bool BindGroupLayoutItem::operator!=(const BindGroupLayoutItem &other) const { return !(*this == other); }

    BindGroupLayoutDesc &BindGroupLayoutDesc::add_item(const BindGroupLayoutItem &new_item) {
        bindings.add_item(new_item);
        return *this;
    }

    BindGroupLayoutDesc& BindGroupLayoutDesc::set_group_index(const size_t new_index) {
        group_index = new_index;
        return *this;
    }

} // namespace avalanche::rendering
