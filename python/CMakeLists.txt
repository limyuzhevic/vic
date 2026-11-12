# Python bindings build system - standalone alternative to root CMakeLists.txt
# This file builds the pynlm Python module by relying on the core NLM libraries
# that should be built by the root CMakeLists.txt first.
# 
# To use this standalone build:
#   cmake -S python -B build
# 
# To build Python bindings as part of the main CMake project (recommended for
# development), use the root CMakeLists.txt with:
#   cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The module is shared, so the static NLM libs it links need PIC.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pynlm bindings.cpp)

# These targets (nlm_core, nlm_agent, nlm_world) must be available from the parent
# project or built elsewhere. For this standalone build to work, you would need
# to either:
# 1. Build the core libraries first, then pass them to this build, OR
# 2. Comment out the target_link_libraries line below and build core libraries here
#    (which brings back the duplication issue)
# 
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)