#pragma once
#include "metamixin.h"
#include "type_eraser.h"

namespace avalanche {

    /// @brief Data access interface for reflection usage
    class Field : public HasMetadataMixin, public CanGetDeclaringClassMixin {
    public:
        virtual Chimera get(Chimera object) const = 0;
    };

} // namespace avalanche
