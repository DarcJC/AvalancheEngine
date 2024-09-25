#pragma once

#include "metamixin.h"


namespace avalanche {
    class Method : public HasNameMixin, public CanGetDeclaringClassMixin, public HasArgsInfo, public HasMetadataMixin {
    };
} // namespace avalanche
