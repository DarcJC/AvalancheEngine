#pragma once
#include "metamixin.h"
#include "type_eraser.h"

namespace avalanche {

    /// @brief Data access interface for reflection usage
    class Field : public HasMetadataMixin, public CanGetDeclaringClassMixin, public HasNameMixin {
    public:
        [[nodiscard]] virtual Chimera get(Chimera object) const = 0;

        template <typename T>
        requires has_class_name<std::decay_t<T>>
        [[nodiscard]] Chimera get_value(T object) const {
            Class* clazz = Class::for_name(class_name_v<std::decay_t<T>>);
            FieldProxyStruct proxy_struct(&object, clazz);
            return get(Chimera(&proxy_struct));
        }
    };

} // namespace avalanche
