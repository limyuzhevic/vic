cmake_minimum_required(VERSION 3.20)
project(pynlm_bindings LANGUAGES CXX)

# Modern C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimized build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Use file globbing for better maintainability
file(GLOB_RECURSE NLM_BINDINGS_SOURCES
    "${NLM_SOURCE_DIR}/**/*.cpp"
)

# Create the python module
pybind11_add_module(pynlm ${NLM_BINDINGS_SOURCES})

target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Modern compiler flags
target_compile_features(pynlm PRIVATE cxx_std_20)

target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CONFIG:Release>:-O3 -march=native -flto>
)

# Python-specific optimizations
target_compile_definitions(pynlm PRIVATE
    PYBIND11_MODULE
    NLM_PYTHON_BINDINGS
)

# Set output properties
target_properties(pynlm PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pynlm"
    SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# Install the Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
