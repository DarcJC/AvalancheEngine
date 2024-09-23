#pragma once

#include "avalanche_shader_server_export.h"

#include <container/string.hpp>
#include <render_enums.h>


namespace avalanche {

    struct ShaderCompileDesc {
        string code;
        rendering::EGraphicsAPIType target_api;
    };


    /// @reflect
    /// @brief A singleton class for managing shader complication.
    class AVALANCHE_SHADER_SERVER_API IShaderServer {
    public:
        static IShaderServer& get();

        virtual ~IShaderServer() = default;

    };
} // namespace avalanche
