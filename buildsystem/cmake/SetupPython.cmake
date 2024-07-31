# We only wanted a Python3 interpreter, so we can use the Python3 module
find_package(Python3 COMPONENTS Interpreter)

if (Python3_VERSION_MAJOR LESS 3 OR Python3_VERSION_MINOR LESS 12)
    message(FATAL_ERROR "Python 3.12 or higher is required")
endif()
