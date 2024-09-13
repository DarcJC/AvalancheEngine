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

    bool Class::is_derived_from_object() const { return is_derived_from("avalanche::Object"); }

    bool Class::is_primitive_type() const {
        return false;
    }

    PrimitiveClass::PrimitiveClass(const char* name) : m_name(name), m_name_str(name) {}

    std::string_view PrimitiveClass::full_name() const { return full_name_str(); }

    const std::string& PrimitiveClass::full_name_str() const {
        return m_name_str;
    }

    size_t PrimitiveClass::hash() const {
        return FNV1aHash::hash_64_fnv1a(full_name());
    }

    void PrimitiveClass::base_classes(int32_t &num_result, const char *const *&out_data) const {
        num_result = 0;
        out_data = nullptr;
    }

    bool PrimitiveClass::is_primitive_type() const { return true; }

}

