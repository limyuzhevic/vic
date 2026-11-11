# CMakeLists.txt for building pynlm standalone (alternative to main CMakeLists.txt)
# This builds the Python module independently of the main C++ targets.
cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Phase 5 fix: the module is a shared library, so the static NLM libs
# it links must be position-independent, or the link fails with
# "recompile with -fPIC".
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Use the pre-built libraries from the main build
# This assumes that nlm_core, nlm_agent, and nlm_world have been built
# by the main CMakeLists.txt and are available in the build directory.
add_library(nlm_core STATIC IMPORTED)
add_library(nlm_agent STATIC IMPORTED)
add_library(nlm_world STATIC IMPORTED)

# Find the built libraries in the parent build directory
# Look for the libraries in the build directory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../build/src")
    # Search for the built libraries
    file(GLOB CORE_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../build/src/*.a")
    if(CORE_LIBS)
        list(FIRST 1 CORE_LIBS ${CORE_LIBS})
        set_target_properties(nlm_core PROPERTIES IMPORTED_LOCATION ${CORE_LIBS})
        target_include_directories(nlm_core INTERFACE "${NLM_SOURCE_DIR}")
    endif()
    
    file(GLOB AGENT_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../build/src/*.a")
    if(AGENT_LIBS)
        list(FIRST 1 AGENT_LIBS ${AGENT_LIBS})
        set_target_properties(nlm_agent PROPERTIES IMPORTED_LOCATION ${AGENT_LIBS})
        target_include_directories(nlm_agent INTERFACE "${NLM_SOURCE_DIR}")
    endif()
    
    file(GLOB WORLD_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../build/src/*.a")
    if(WORLD_LIBS)
        list(FIRST 1 WORLD_LIBS ${WORLD_LIBS})
        set_target_properties(nlm_world PROPERTIES IMPORTED_LOCATION ${WORLD_LIBS})
        target_include_directories(nlm_world INTERFACE "${NLM_SOURCE_DIR}")
    endif()
endif()

# Build the Python module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)