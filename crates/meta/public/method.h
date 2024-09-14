#pragma once

#include "metamixin.h"


namespace avalanche {
    class Method : public HasNameMixin, public CanGetDeclaringClassMixin {
    };
} // namespace avalanche
