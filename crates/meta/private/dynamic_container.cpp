#include "dynamic_container.h"

namespace avalanche {
    auto DynamicContainerBase::get_class_name() const -> std::string_view {
        constexpr static std::string_view clazz_name = "<unregistered>";
        return clazz_name;
    }
} // namespace avalanche
