
add_custom_target(avalanche_generator_clean_up
        WORKING_DIRECTORY "${AVALANCHE_CMAKE_ROOT_DIR}/buildsystem"
        COMMAND "${Python3_EXECUTABLE}" -m generator clean --binary-dir "${CMAKE_BINARY_DIR}"
        COMMENT "Cleaning"
)

macro(avalanche_generate_binding)
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "TARGET"
            "SRCS"
            ${ARGN}
    )

    set(target_generation_path "${CMAKE_BINARY_DIR}/Generator/Generated/${PARSED_ARGS_TARGET}")
    file(MAKE_DIRECTORY "${target_generation_path}/Headers" "${target_generation_path}/Sources")

    target_include_directories(${PARSED_ARGS_TARGET} PUBLIC "${target_generation_path}/Headers")

    set(target_include_dirs $<LIST:REMOVE_DUPLICATES,$<LIST:APPEND,$<TARGET_PROPERTY:${PARSED_ARGS_TARGET},INCLUDE_DIRECTORIES>,${SYSTEM_IMPLICIT_INCLUDE_DIRS}>>)
    set(target_include_dirs_cli $<JOIN:target_include_dirs, --include_dir >)

    foreach (src ${PARSED_ARGS_SRCS})
        get_filename_component(src_filename ${src} NAME_WE)
        cmake_path(ABSOLUTE_PATH src OUTPUT_VARIABLE abs_src)
        set(header_path "${target_generation_path}/Headers/${src_filename}.generated.h")
        cmake_path(ABSOLUTE_PATH header_path OUTPUT_VARIABLE abs_header_path)
        set(source_path "${target_generation_path}/Sources/${src_filename}.generated.cpp")
        cmake_path(ABSOLUTE_PATH source_path OUTPUT_VARIABLE abs_source_path)
        file(TOUCH "${header_path}" "${source_path}")
        add_custom_command(
                OUTPUT "${header_path}" "${source_path}"
                COMMAND "${Python3_EXECUTABLE}" -m generator binding --binary-dir "${CMAKE_BINARY_DIR}" --input-header "${abs_src}" --out-header "${abs_header_path}" --out-source "${abs_source_path}" --include-path "${target_include_dirs}"
                WORKING_DIRECTORY "${AVALANCHE_CMAKE_ROOT_DIR}/buildsystem"
                DEPENDS ${src}
                COMMENT "Generating binding for ${src} ..."
        )
        target_sources(${PARSED_ARGS_TARGET} PRIVATE "${source_path}")
    endforeach ()
endmacro()

function(generate_enabled_modules_list)
    get_property(enabled_modules GLOBAL PROPERTY AVALANCHE_ENABLED_MODULES)
    set(output_path "${CMAKE_BINARY_DIR}/ExportHeaders/enabled_modules.h")
    execute_process(
            COMMAND "${Python3_EXECUTABLE}" -m generator module-list --output-file "${output_path}" --modules "${enabled_modules}"
            WORKING_DIRECTORY "${AVALANCHE_CMAKE_ROOT_DIR}/buildsystem"
            COMMAND_ERROR_IS_FATAL ANY
    )
endfunction()
