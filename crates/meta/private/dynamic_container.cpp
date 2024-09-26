#include "dynamic_container.h"

namespace avalanche {
    void IMetadataKeyValueStorage::keys(std::vector<std::string_view>& o_vec) const {
        size_t num;
        const std::string_view* ptr;
        keys(num, ptr);
        o_vec.clear();
        o_vec.reserve(num);
        for (size_t i = 0; i < num; ++i) {
            o_vec.emplace_back(*(ptr + i));
        }
    }

    auto DynamicContainerBase::get_class_name() const -> std::string_view {
        constexpr static std::string_view clazz_name = "<unregistered>";
        return clazz_name;
    }
} // namespace avalanche
