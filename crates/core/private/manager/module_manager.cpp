#include <unordered_map>

#include "logger.h"
#include "enabled_modules.h"
#include "dylib.hpp"

#include "manager/module_manager.h"


namespace avalanche {
    class ModuleManagerImpl : public ModuleManager {
        std::unordered_map<std::string, dylib*> m_loaded_modules;
    public:
        module_handle_t load_module(std::string_view name) override;
        void load_enabled_modules() override;
    };

    module_handle_t ModuleManagerImpl::load_module(std::string_view name) {
        const std::string temp_name(name);
        AVALANCHE_CHECK(!m_loaded_modules.contains(temp_name), "Loading module twice doesn't supported yet");
        auto* new_lib = new dylib(name);
        m_loaded_modules.insert_or_assign(temp_name, new_lib);
        return new_lib;
    }

    void ModuleManagerImpl::load_enabled_modules() {
        for (const char* s : generated::enabled_modules::value) {
            load_module(s);
            AVALANCHE_LOGGER.info("Loaded module '{}'", s);
        }
    }

    ModuleManager &ModuleManager::get() {
        static ModuleManagerImpl impl{};
        return impl;
    }

} // namespace avalanche
