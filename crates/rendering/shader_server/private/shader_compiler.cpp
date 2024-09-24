#include "shader_compiler.h"

#include <logger.h>

namespace avalanche {

    ShaderCompilerBase::ShaderCompilerBase() {
        AVALANCHE_CHECK_RUNTIME(slang::createGlobalSession(m_slang_global_session_.writeRef()) == SLANG_OK, "Failed to create slang session");
    }

    Slang::ComPtr<slang::ISession> ShaderCompilerBase::create_compiler_session(const ShaderCompileDesc &desc) {
        std::lock_guard lock(m_mutex_);

        slang::SessionDesc session_desc{};
        slang::TargetDesc target_desc{};
        if (desc.target_api == rendering::EGraphicsAPIType::Vulkan) {
            target_desc.format = SLANG_SPIRV;
        } else {
            target_desc.format = SLANG_HLSL;
        }
        target_desc.profile = retrieve_compile_profile(desc);

        session_desc.targets = &target_desc;
        session_desc.targetCount = 1;

        Slang::ComPtr<slang::ISession> session;
        AVALANCHE_CHECK_RUNTIME(m_slang_global_session_->createSession(session_desc, session.writeRef()) == SLANG_OK, "Failed to create slang session");
        return session;
    }

    unique_ptr<ShaderCompileData> ShaderCompilerBase::compile(const ShaderCompileDesc &desc) {
        std::lock_guard lock(m_mutex_);

        Slang::ComPtr<slang::ISession> compile_session = create_compiler_session(desc);
        Slang::ComPtr<slang::ICompileRequest> compile_request;

        AVALANCHE_CHECK_RUNTIME(compile_session->createCompileRequest(compile_request.writeRef()) == SLANG_OK, "Failed to create compile request");

        AVALANCHE_CHECK_RUNTIME(!desc.modules.is_empty(), "Modules is an empty list");

        for (const auto& module : desc.modules) {
            int translation_unit_index = compile_request->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, module.name.data());
            compile_request->addTranslationUnitSourceString(translation_unit_index, module.path.data(), module.code.data());
        }

        compile_request->compile();

        AVALANCHE_LOGGER.debug("Shader compilation result: \n\t{}", compile_request->getDiagnosticOutput());

        return nullptr;
    }

    SlangProfileID ShaderCompilerBase::retrieve_compile_profile(const ShaderCompileDesc &desc) {
        if (desc.target_api == rendering::EGraphicsAPIType::Vulkan) {
            return m_slang_global_session_->findProfile("spirv_1_6");
        }
        return m_slang_global_session_->findProfile("lib_6_7");
    }

} // namespace avalanche
