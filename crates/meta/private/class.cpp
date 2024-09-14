#include "class.h"

#include <field.h>

#include "metaspace.h"

namespace avalanche {

    Class *Class::for_name(const std::string_view name) { return MetaSpace::get().find_class(name); }

    const char* Class::get_name() const {
        return full_name_str().c_str();
    }

    void Class::base_classes(int32_t& num_result, const char *const *& out_data) const {
        num_result = 0;
        out_data = nullptr;
    }

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

    bool Class::is_primitive_type() const { return false; }

    void Class::fields(int32_t &num_result, const Field *const *&out_data) const {
        num_result = 0;
        out_data = nullptr;
    }

    const Field* Class::get_field(std::string_view name) const {
        int32_t num_fields;
        const Field *const* out_fields;
        fields(num_fields, out_fields);
        for (int32_t i = 0; i < num_fields; ++i) {
            if (name == out_fields[i]->get_name()) {
                return out_fields[i];
            }
        }
        return nullptr;
    }

    bool Class::equals_to(const Class &other) const {
        if (&other == this)
            return true;

        return other.full_name() == full_name();
    }

    bool Class::operator==(const Class& other) const { return equals_to(other); }

    bool Class::operator!=(const Class& other) const { return !(*this == other); }

    PrimitiveClass::PrimitiveClass(const char* name) : m_name(name), m_name_str(name) {}

    std::string_view PrimitiveClass::full_name() const { return full_name_str(); }

    const std::string& PrimitiveClass::full_name_str() const {
        return m_name_str;
    }

    size_t PrimitiveClass::hash() const {
        return FNV1aHash::hash_64_fnv1a(full_name());
    }

    bool PrimitiveClass::is_primitive_type() const { return true; }

}

