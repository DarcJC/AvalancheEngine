
add_custom_target(avalanche_generator_clean_up
        WORKING_DIRECTORY "${AVALANCHE_CMAKE_ROOT_DIR}/buildsystem"
        COMMAND "${Python3_EXECUTABLE}" -m generator clean --base-dir "${CMAKE_BINARY_DIR}"
        COMMENT "Cleaning up generated files"
)

function(avalanche_generate_binding)
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "TARGET"
            "SRCS"
            ${ARGN}
    )

    set(target_generation_path "${CMAKE_BINARY_DIR}/Generator/Generated/${PARSED_ARGS_TARGET}")
    file(MAKE_DIRECTORY "${target_generation_path}/Headers" "${target_generation_path}/Sources")

    foreach (src ${PARSED_ARGS_SRCS})
        get_filename_component(src_filename ${src} NAME_WE)
        set(header_path "${target_generation_path}/Headers/${src_filename}.generated.h")
        set(source_path "${target_generation_path}/Sources/${src_filename}.generated.cpp")
        file(TOUCH "${header_path}" "${source_path}")
        add_custom_command(
                TARGET ${PARSED_ARGS_TARGET}
                OUTPUT "${header_path}" "${source_path}"
                COMMAND "${Python3_EXECUTABLE}" -m generator binding --base-dir "${CMAKE_BINARY_DIR}" --input "${src}" --out-header "${header_path}" --out-source "${source_path}"
                DEPENDS "avalanche_generator_clean_up"
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Generator"
        )
        target_sources(INTERFACE "${header_path}" "${source_path}")
    endforeach ()
endfunction()
