#include "type_eraser.h"
#include "class.h"


namespace avalanche {

    Chimera::Chimera() : m_value({.object = nullptr}), m_is_object(true), m_is_struct(false) {}

    Chimera::Chimera(ScopedStruct *scoped_struct_) :
        m_value({.scoped_struct = scoped_struct_}), m_is_struct(true), m_is_object(false) {}

    Chimera::Chimera(Object *object_) : m_value({.object = object_}), m_is_object(true), m_is_struct(false) {}

    Class *Chimera::get_class() const {
        if (reinterpret_cast<const void *>(&m_value) == nullptr) {
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

    void const* Chimera::memory() const {
        if (m_is_object) {
            return m_value.object;
        }

        if (m_is_struct && m_value.scoped_struct) {
            return m_value.scoped_struct->memory();
        }

        return nullptr;
    }

} // namespace avalanche
