#pragma once

#include "avalanche_meta_export.h"


namespace avalanche {

    struct AVALANCHE_META_API CanGetClassMixin {
        virtual ~CanGetClassMixin() = default;

        [[nodiscard]] virtual class Class* get_class() const = 0;
    };

    struct AVALANCHE_META_API CanGetDeclaringClassMixin {
        virtual ~CanGetDeclaringClassMixin() = default;

        [[nodiscard]] virtual class Class* get_declaring_class() const = 0;
    };

    struct AVALANCHE_META_API HasMetadataMixin {
        virtual ~HasMetadataMixin() = default;
    };

    struct AVALANCHE_META_API ManageMemoryMixin {
        virtual ~ManageMemoryMixin() = default;

        virtual void* memory() = 0;
        [[nodiscard]] virtual void const* memory() const = 0;
    };

} // namespace avalanche
