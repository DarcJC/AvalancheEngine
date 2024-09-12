#pragma once
#include "metamixin.h"

namespace avalanche {

    /// @brief Data access interface for reflection usage
    class Field : public HasMetadataMixin, CanGetDeclaringClassMixin {
    public:
    };

} // namespace avalanche
