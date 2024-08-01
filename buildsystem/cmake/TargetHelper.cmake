
function(avalanche_target)
    set(options SHARED_LIBRARY STATIC_LIBRARY EXECUTABLE)
    set(one_value_args NAME)
    set(multi_value_args SRCS)
    cmake_parse_arguments(
            PARSED_ARGS
            "${options}"
            "${one_value_args}"
            "${multi_value_args}"
            ${ARGN}
    )

    set(target_name "avalanche_${PARSED_ARGS_NAME}")
    if (PARSED_ARGS_EXECUTABLE)
        add_executable("${target_name}" "${PARSED_ARGS_SRCS}")
        add_executable("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
        target_compile_definitions("${target_name}" PRIVATE AVALANCHE_BUILD_EXECUTABLE=1)
    elseif (PARSED_ARGS_SHARED_LIBRARY)
        add_library("${target_name}" SHARED "${PARSED_ARGS_SRCS}")
        add_library("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
        target_compile_definitions("${target_name}" PRIVATE AVALANCHE_BUILD_SHARED=1)
    elseif (PARSED_ARGS_STATIC_LIBRARY)
        add_library("${target_name}" STATIC "${PARSED_ARGS_SRCS}")
        add_library("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
        target_compile_definitions("${target_name}" PRIVATE AVALANCHE_BUILD_STATIC=1)
    else ()
        message(FATAL_ERROR "Target type must be specified")
    endif ()

    target_include_directories(${target_name} PUBLIC "public")
    target_include_directories(${target_name} PRIVATE "private")
    target_include_directories(${target_name} PUBLIC "classes")

    string(TOUPPER ${target_name} TARGET_NAME_UPPER)
    string(TOLOWER ${target_name} TARGET_NAME_LOWER)
    string(TOUPPER ${PARSED_ARGS_NAME} NAME_UPPER)
    generate_export_header(${target_name}
            EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_API"
            NO_EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_INTERNAL"
            EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/ExportHeaders/${TARGET_NAME_LOWER}_export.h"
    )
    target_compile_definitions(${target_name} PRIVATE
            "AVALANCHE_${PARSED_ARGS_NAME}_EXPORTS"
            "${TARGET_NAME_UPPER}_EXPORTS"
    )

    set(AVALANCHE_TARGET_${NAME_UPPER} PARENT_SCOPE ${target_name})
endfunction()
