#include "type_eraser.h"

#include <cassert>

#include <limits>
#include "class.h"


namespace avalanche {

    bool ScopedStruct::convertible_from(const ScopedStruct &other) const {
        // Return false if not the same type
        if (*get_class() != *other.get_class())
            return false;

        const TypeQualifiers self_qualifiers = qualifiers();
        const TypeQualifiers other_qualifiers = other.qualifiers();

        // Return true if qualifiers is same
        if (self_qualifiers == other_qualifiers)
            return true;

        // T& -> T*
        if (other_qualifiers.reference && self_qualifiers.pointer)
            return true;

        return false;
    }

    FieldProxyStruct::FieldProxyStruct(void *proxied_memory_, Class *clazz_) :
        m_proxied_memory(proxied_memory_), m_class(clazz_) {}

    Class* FieldProxyStruct::get_class() const {
        return m_class;
    }

    Chimera::Chimera() :
        m_value({.object = nullptr}), m_flags{.m_is_object = true, .m_is_struct = false, .m_managed_memory = false} {}

    Chimera::Chimera(ScopedStruct *scoped_struct_, const bool managed_/* = false*/) :
        m_value({.scoped_struct = scoped_struct_}), m_flags{.m_is_object = false, .m_is_struct = true, .m_managed_memory = managed_} {}

    Chimera::Chimera(Object *object_, const bool managed_/* = false*/) :
        m_value({.object = object_}), m_flags{ .m_is_object = true, .m_is_struct = false, .m_managed_memory = managed_ } {}

    Chimera::Chimera(Chimera &&other) noexcept : Chimera() {
        swap(other);
        other.release();
    }

    Chimera& Chimera::operator=(Chimera &&other) noexcept {
        if (&other != this) {
            release();
            swap(other);
        }
        return *this;
    }

    Chimera::~Chimera() {
        release();
    }

    Class *Chimera::get_class() const {
        if (!is_valid()) {
            return nullptr;
        }

        if (m_flags.m_is_object) {
            return m_value.object->get_class();
        }

        if (m_flags.m_is_struct) {
            return m_value.scoped_struct->get_class();
        }

        return nullptr;
    }

    void* Chimera::memory() {
        if (m_flags.m_is_object) {
            return m_value.object;
        }

        if (m_flags.m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->memory();
        }

        return nullptr;
    }

    void const *Chimera::memory() const {
        if (m_flags.m_is_object) {
            return m_value.object;
        }

        if (m_flags.m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->memory();
        }

        return nullptr;
    }

    TypeQualifiers Chimera::qualifiers() const {
        if (m_flags.m_is_object && m_value.object) {
            return m_value.object->qualifiers();
        }
        if (m_flags.m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->qualifiers();
        }
        return {};
    }

    bool Chimera::is_valid() const { return reinterpret_cast<const void *>(&m_value) != nullptr; }

    bool Chimera::is_managed() const {
        return m_flags.m_managed_memory;
    }

    void Chimera::release() {
        if (!m_flags.m_managed_memory) {
            return;
        }

        if (m_flags.m_is_struct) {
            delete m_value.scoped_struct;
        } else if (m_flags.m_is_object) {
            delete m_value.object;
        }
        m_value.object = nullptr;
        m_flags.m_is_struct = false;
        m_flags.m_is_object = true;
    }

    void Chimera::swap(Chimera &other) noexcept {
        std::swap(m_value, other.m_value);
        std::swap(m_flags, other.m_flags);
    }

    bool Chimera::is_signed_integer() const {
        static std::string_view allowed_name[] {
            class_name_sv<int8_t>,
            class_name_sv<int16_t>,
            class_name_sv<int32_t>,
            class_name_sv<int64_t>,
        };
        if (const Class *clazz = get_class()) {
            const std::string_view name = clazz->full_name();
            for (const auto& i : allowed_name) {
                if (i == name) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Chimera::is_unsigned_integer() const {
        static std::string_view allowed_name[] {
            class_name_sv<uint8_t>,
            class_name_sv<uint16_t>,
            class_name_sv<uint32_t>,
            class_name_sv<uint64_t>,
        };
        if (const Class *clazz = get_class()) {
            const std::string_view name = clazz->full_name();
            for (const auto& i : allowed_name) {
                if (i == name) {
                    return true;
                }
            }
        }
        return false;
    }

    int64_t Chimera::as_integer() const {
        const Class *clazz = get_class();
        assert(clazz != nullptr);
        if (clazz) {
            const std::string_view name = clazz->full_name();
            if (class_name_sv<uint8_t> == name) {
                return *static_cast<const uint8_t *>(memory());
            }
            if (class_name_sv<uint16_t> == name) {
                return *static_cast<const uint16_t *>(memory());
            }
            if (class_name_sv<uint32_t> == name) {
                return *static_cast<const uint32_t *>(memory());
            }
            if (class_name_sv<uint64_t> == name) {
                return *static_cast<const uint64_t *>(memory());
            }
            if (class_name_sv<int8_t> == name) {
                return *static_cast<const int8_t *>(memory());
            }
            if (class_name_sv<int16_t> == name) {
                return *static_cast<const int16_t *>(memory());
            }
            if (class_name_sv<int32_t> == name) {
                return *static_cast<const int32_t *>(memory());
            }
            if (class_name_sv<int64_t> == name) {
                return *static_cast<const int64_t *>(memory());
            }
            if (class_name_sv<float> == name) {
                return static_cast<int64_t>(*static_cast<const float *>(memory()));
            }
            if (class_name_sv<double> == name) {
                return static_cast<int64_t>(*static_cast<const double *>(memory()));
            }
            if (class_name_sv<long double> == name) {
                return static_cast<int64_t>(*static_cast<const long double *>(memory()));
            }
        }

        return std::numeric_limits<uint64_t>::min(); // Return 0 if storage isn't a numeric
    }

    double Chimera::as_float_point() const {
        const Class *clazz = get_class();
        assert(clazz != nullptr);
        if (clazz) {
            const std::string_view name = clazz->full_name();
            if (class_name_sv<uint8_t> == name) {
                return *static_cast<const uint8_t *>(memory());
            }
            if (class_name_sv<uint16_t> == name) {
                return *static_cast<const uint16_t *>(memory());
            }
            if (class_name_sv<uint32_t> == name) {
                return *static_cast<const uint32_t *>(memory());
            }
            if (class_name_sv<uint64_t> == name) {
                return static_cast<double>(*static_cast<const uint64_t *>(memory()));
            }
            if (class_name_sv<int8_t> == name) {
                return *static_cast<const int8_t *>(memory());
            }
            if (class_name_sv<int16_t> == name) {
                return *static_cast<const int16_t *>(memory());
            }
            if (class_name_sv<int32_t> == name) {
                return *static_cast<const int32_t *>(memory());
            }
            if (class_name_sv<int64_t> == name) {
                return static_cast<double>(*static_cast<const int64_t *>(memory()));
            }
            if (class_name_sv<float> == name) {
                return *static_cast<const float *>(memory());
            }
            if (class_name_sv<double> == name) {
                return *static_cast<const double *>(memory());
            }
            if (class_name_sv<long double> == name) {
                return static_cast<double>(*static_cast<const long double *>(memory()));
            }
        }
        return std::numeric_limits<double>::quiet_NaN();
    }

} // namespace avalanche
