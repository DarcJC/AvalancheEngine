
set_property(GLOBAL PROPERTY AVALANCHE_ENABLED_MODULES "")
add_custom_target(avalanche_internal_all_modules
        ALL
        COMMENT "Building all enabled module even they are not linked to ..."
)

function(add_enabled_modules NAME)
    get_property(prev GLOBAL PROPERTY AVALANCHE_ENABLED_MODULES)
    list(APPEND prev "${NAME}")
    set_property(GLOBAL PROPERTY AVALANCHE_ENABLED_MODULES "${prev}")
    add_dependencies(avalanche_internal_all_modules "${NAME}")
endfunction()

function(avalanche_target)
    set(options SHARED_LIBRARY STATIC_LIBRARY EXECUTABLE REFLECTION)
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
        add_dependencies("${target_name}" avalanche_internal_all_modules)
    elseif (PARSED_ARGS_SHARED_LIBRARY)
        add_library("${target_name}" SHARED "${PARSED_ARGS_SRCS}")
        add_library("avalanche::${PARSED_ARGS_NAME}" ALIAS "${target_name}")
        target_compile_definitions("${target_name}" PRIVATE AVALANCHE_BUILD_SHARED=1)
        if (WIN32)
            set_target_properties("${target_name}" PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
        endif ()
        set_target_properties("${target_name}" PROPERTIES POSITION_INDEPENDENT_CODE ON)
        add_enabled_modules("${target_name}")
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
    if (NOT PARSED_ARGS_EXECUTABLE)
        generate_export_header(${target_name}
                EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_API"
                NO_EXPORT_MACRO_NAME "${TARGET_NAME_UPPER}_INTERNAL"
                EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/ExportHeaders/${TARGET_NAME_LOWER}_export.h"
        )
    endif ()
    target_compile_definitions(${target_name} PRIVATE
            "AVALANCHE_${PARSED_ARGS_NAME}_EXPORTS"
            "${TARGET_NAME_UPPER}_EXPORTS"
            "AVALANCHE_PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
            "AVALANCHE_PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
            "AVALANCHE_PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
    )

    set(AVALANCHE_TARGET_${NAME_UPPER} PARENT_SCOPE ${target_name})

    if (PARSED_ARGS_REFLECTION)
        file(GLOB_RECURSE public_header_files
                "public/*.h"
                "public/*.hpp"
        )

        avalanche_generate_binding(
                TARGET ${target_name}
                SRCS ${public_header_files}
        )
    endif (PARSED_ARGS_REFLECTION)
endfunction()
