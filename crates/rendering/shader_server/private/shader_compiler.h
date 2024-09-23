#pragma once
#include "container/unique_ptr.hpp"
#include "container/string.hpp"
#include <utility>
#include <string_view>
#include <mutex>

#include "include/slang-com-ptr.h"
#include "include/slang.h"
#include "shader_server.h"
#include "source/slang/slang-profile.h"


namespace avalanche {

    struct ShaderCompileData {
    };

    /// @note Following slang design, this class isn't thread-safe. Use @code static thread_local@endcode to create instance per thread.
    class ShaderCompilerBase {
    public:
        virtual ~ShaderCompilerBase() = default;

        explicit ShaderCompilerBase(Slang::ComPtr<slang::ISession> session);

        /// @brief Compile shader code
        unique_ptr<ShaderCompileData> compile(const ShaderCompileDesc& desc);

    protected:
        static Slang::Profile retrieve_compile_profile(const ShaderCompileDesc& desc);

        Slang::ComPtr<slang::ISession> m_slang_session_;
        std::recursive_mutex m_mutex_{};
    };

} // namespace avalanche
