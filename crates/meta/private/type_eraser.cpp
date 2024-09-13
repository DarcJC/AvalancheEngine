#include "type_eraser.h"
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

    void Chimera::swap(Chimera& other) noexcept {
        std::swap(m_value, other.m_value);
        std::swap(m_flags, other.m_flags);
    }

} // namespace avalanche
