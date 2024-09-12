#include "class.h"
#include "metaspace.h"

namespace avalanche {

    Class *Class::for_name(const std::string_view name) { return MetaSpace::get().find_class(name); }

    bool Class::is_derived_from(const std::string_view name) const {
        int32_t num;
        const char * const* result;
        base_classes(num, result);

        for (int32_t i = 0; i < num; ++i) {
            if (name == result[i]) {
                return true;
            }
        }

        return false;
    }

    bool Class::is_derived_from_object() const {
        return is_derived_from("avalanche::Object");
    }

}

