cmake_minimum_required(VERSION 3.28)

add_subdirectory(thirdparty)

project(AvalancheEngine LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Building library and executable in same directory to avoid "dll not found" problem in Windows
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/Binaries")

# Set current root directory
set(AVALANCHE_CMAKE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

# CMake modules
include(CMakeDependentOption)
include(GenerateExportHeader)
include(CMakeParseArguments)

add_subdirectory(buildsystem/generator)

# Project defined cmake modules
include(buildsystem/cmake/SetupPython.cmake)
include(buildsystem/cmake/TargetHelper.cmake)
include(buildsystem/cmake/Options.cmake)

include_directories(
        AFTER ${CMAKE_BINARY_DIR}/ExportHeaders
)

add_subdirectory(crates)

generate_enabled_modules_list()
