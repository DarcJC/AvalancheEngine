#pragma once

#include "avalanche_meta_export.h"
#include <cstddef>
#include <cstdint>


namespace avalanche {

    struct AVALANCHE_META_API CanGetClassMixin {
        virtual ~CanGetClassMixin() = default;

        [[nodiscard]] virtual class Class* get_class() const = 0;
    };

    struct AVALANCHE_META_API CanGetDeclaringClassMixin {
        virtual ~CanGetDeclaringClassMixin() = default;

        [[nodiscard]] virtual class Class* get_declaring_class() const = 0;
    };

    struct AVALANCHE_META_API HasNameMixin {
        virtual ~HasNameMixin() = default;

        [[nodiscard]] virtual const char* get_name() const = 0;
    };

    struct AVALANCHE_META_API HasMetadataMixin {
        virtual ~HasMetadataMixin() = default;
    };

    struct AVALANCHE_META_API ManageMemoryMixin {
        virtual ~ManageMemoryMixin() = default;

        virtual void* memory() = 0;
        [[nodiscard]] virtual void const* memory() const = 0;
    };

    struct TypeQualifiers {
        uint32_t default_initialized: 1 = true;
        uint32_t reference: 1 = false;
        uint32_t pointer: 1 = false;
        uint32_t lvalue: 1 = false;
        uint32_t rvalue : 1 = false;

        bool operator==(const TypeQualifiers &) const = default;
        bool operator!=(const TypeQualifiers &) const = default;
    };

    struct AVALANCHE_META_API HasQualifiersMixin {
        virtual ~HasQualifiersMixin() = default;

        /// @brief Get type qualifiers of contained type
        [[nodiscard]] virtual TypeQualifiers qualifiers() const = 0;
    };

} // namespace avalanche
