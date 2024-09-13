#include "type_eraser.h"
#include "class.h"


namespace avalanche {

    Chimera::Chimera() : m_value({.object = nullptr}), m_is_object(true), m_is_struct(false) {}

    Chimera::Chimera(ScopedStruct *scoped_struct_) :
        m_value({.scoped_struct = scoped_struct_}), m_is_object(false), m_is_struct(true) {}

    Chimera::Chimera(Object *object_) : m_value({.object = object_}), m_is_object(true), m_is_struct(false) {}

    Chimera::Chimera(Chimera&& other) noexcept {
        reset();
        m_value = other.m_value;
        m_is_struct = other.m_is_struct;
        m_is_object = other.m_is_object;
        other.reset();
    }

    Chimera::~Chimera() {
        reset();
    }

    Class *Chimera::get_class() const {
        if (!is_valid()) {
            return nullptr;
        }

        if (m_is_object) {
            return m_value.object->get_class();
        }

        if (m_is_struct) {
            return m_value.scoped_struct->get_class();
        }

        return nullptr;
    }

    void* Chimera::memory() {
        if (m_is_object) {
            return m_value.object;
        }

        if (m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->memory();
        }

        return nullptr;
    }

    void const *Chimera::memory() const {
        if (m_is_object) {
            return m_value.object;
        }

        if (m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->memory();
        }

        return nullptr;
    }

    bool Chimera::is_valid() const { return reinterpret_cast<const void *>(&m_value) != nullptr; }

    void Chimera::reset() {
        if (m_is_struct) {
            delete m_value.scoped_struct;
        }
        m_value.object = nullptr;
        m_is_struct = false;
        m_is_object = true;
    }

} // namespace avalanche
