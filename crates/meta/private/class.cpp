#include "class.h"
#include "metaspace.h"

namespace avalanche {

    Class* Class::for_name(const std::string_view name) {
        return MetaSpace::get().find_class(name);
    }

}

