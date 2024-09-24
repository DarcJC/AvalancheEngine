#pragma once
#include "container/unique_ptr.hpp"
#include "container/string.hpp"
#include <utility>
#include <string_view>
#include <mutex>

#include "slang-com-ptr.h"
#include "slang.h"
#include "shader_server.h"


namespace avalanche {

    struct ShaderCompileData {
    };

    /// @note Following slang design, this class isn't thread-safe. Use @code static thread_local@endcode to create instance per thread.
    class ShaderCompilerBase {
    public:
        ShaderCompilerBase();
        virtual ~ShaderCompilerBase() = default;

        virtual Slang::ComPtr<slang::ISession> create_compiler_session(const ShaderCompileDesc& desc);

        /// @brief Compile shader code
        virtual unique_ptr<ShaderCompileData> compile(const ShaderCompileDesc& desc);

    protected:
        virtual SlangProfileID retrieve_compile_profile(const ShaderCompileDesc& desc);

        Slang::ComPtr<slang::IGlobalSession> m_slang_global_session_;
        std::recursive_mutex m_mutex_{};
    };

} // namespace avalanche
