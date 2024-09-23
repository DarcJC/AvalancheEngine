#include "shader_compiler.h"

#include <logger.h>

#include "source/slang/slang-profile.h"


namespace avalanche {

    ShaderCompilerBase::ShaderCompilerBase(Slang::ComPtr<slang::ISession> session) :
        m_slang_session_(std::move(session)) {}

    unique_ptr<ShaderCompileData> ShaderCompilerBase::compile(const ShaderCompileDesc &desc) {
        std::lock_guard lock(m_mutex_);

        Slang::ComPtr<slang::ICompileRequest> compile_request;
        m_slang_session_->createCompileRequest(compile_request.writeRef());

        AVALANCHE_CHECK_RUNTIME(nullptr != compile_request, "Failed to create compile request");

        Slang::Profile target_profile = retrieve_compile_profile(desc);
        // compile_request->setTargetProfile(0, SLANG_PROFILE_);
    }

    Slang::Profile ShaderCompilerBase::retrieve_compile_profile(const ShaderCompileDesc &desc) {}

} // namespace avalanche
