#pragma once

#include "avalanche_shader_server_export.h"

#include <container/string.hpp>
#include <container/vector.hpp>
#include <render_enums.h>


namespace avalanche {

    struct ShaderModuleDesc {
        string name = "<built-in>";
        string path = "<built-in>";
        string code;
    };

    struct ShaderMacroDesc {
        string name;
        string value = "1";
    };

    struct ShaderCompileDesc {
        vector<ShaderModuleDesc> modules;
        vector<ShaderMacroDesc> macro_defines;
        rendering::EGraphicsAPIType target_api;
    };

    struct ShaderCompileData {
        vector<std::byte> target_code;
    };

    /// @reflect
    /// @brief A singleton class for managing shader complication.
    class AVALANCHE_SHADER_SERVER_API IShaderServer {
    public:
        static IShaderServer& get();

        virtual ~IShaderServer() = default;

    };
} // namespace avalanche
