
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

    message(STATUS ${ARGV})

    set(target_name "avalanche_${PARSED_ARGS_NAME}")
    if (PARSED_ARGS_EXECUTABLE)
        add_executable("${target_name}" "${PARSED_ARGS_SRCS}")
        add_executable("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
    elseif (PARSED_ARGS_SHARED_LIBRARY)
        add_library("${target_name}" SHARED "${PARSED_ARGS_SRCS}")
        add_library("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
    elseif (PARSED_ARGS_STATIC_LIBRARY)
        add_library("${target_name}" STATIC "${PARSED_ARGS_SRCS}")
        add_library("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
    else ()
        message(FATAL_ERROR "Target type must be specified")
    endif ()

    target_include_directories(${target_name} PUBLIC "public")
    target_include_directories(${target_name} PRIVATE "private")
    target_include_directories(${target_name} INTERFACE "classes")

    string(TOUPPER ${target_name} TARGET_NAME_UPPER)
    string(TOLOWER ${target_name} TARGET_NAME_LOWER)
    generate_export_header(${target_name}
            EXPORT_MACRO_NAME "AVALANCHE_${TARGET_NAME_UPPER}_API"
            NO_EXPORT_MACRO_NAME "AVALANCHE_${TARGET_NAME_UPPER}_INTERNAL"
            EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/ExportHeaders/${TARGET_NAME_LOWER}_export.h"
    )
    target_compile_definitions(${target_name} PRIVATE
            "AVALANCHE_${target_name}_EXPORTS"
            "AVALANCHE_${TARGET_NAME_UPPER}_EXPORTS"
    )
endfunction()
