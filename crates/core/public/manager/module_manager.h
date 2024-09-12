#pragma once

#include "string_view"


namespace avalanche {

    using module_handle_t = void*;

    class ModuleManager {
    public:
        static ModuleManager& get();

        virtual ~ModuleManager() = default;

        virtual void load_enabled_modules() = 0;

        virtual module_handle_t load_module(std::string_view name) = 0;
    };
}
