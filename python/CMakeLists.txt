cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Performance optimization options
option(PY_NLM_ENABLE_PERF_OPTIMIZATIONS "Enable performance optimizations for Python bindings" ON)
option(PY_NLM_ENABLE_ASSERTIONS "Enable debug assertions for Python bindings" OFF)
option(PY_NLM_ENABLE_PROFILING "Enable profiling support for Python bindings" OFF)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Platform-specific optimizations for Python bindings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang optimizations
    if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
        add_compile_options(-O3 -march=native -flto -fvisibility=hidden -fPIC)
    endif()
    
    # Link-time optimization
    if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto")
        set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -flto")
    endif()
elseif(MSVC)
    # MSVC optimizations
    if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
        add_compile_options(/O2 /GL /LTCG /DYNAMICBASE /NXCOMPAT)
    endif()
endif()

# Debug builds for Python bindings
if(PY_NLM_ENABLE_ASSERTIONS AND NOT PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    add_compile_options(-g -O0 -DDEBUG)
endif()

# Enhanced Python bindings configuration
set(PYTHON_MODULE_NAME "pynlm")
set(PYTHON_MODULE_ENHANCED_NAME "nlm_enhanced")
set(PYTHON_MODULE_VERSION "2.0.0")
set(PYTHON_MODULE_DESCRIPTION "Enhanced Python bindings for NLM with advanced features")

# Python module properties
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/pynlm_setup.py.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/pynlm_setup.py"
    @ONLY
)

# Core library
add_library(nlm_core STATIC
    ${NLM_CORE_SOURCES}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Performance optimizations for core library
if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(nlm_core PRIVATE -march=native -flto)
    target_link_libraries(nlm_core PUBLIC -flto)
endif()

# Agent system
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Performance optimizations for agent system
if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(nlm_agent PRIVATE -march=native -flto)
endif()

# World system
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Performance optimizations for world system
if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(nlm_world PRIVATE -march=native -flto)
endif()

# Basic Python bindings
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Performance optimizations for basic Python bindings
if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(pynlm PRIVATE -march=native -flto -fvisibility=hidden)
    target_link_libraries(pynlm PRIVATE -flto)
endif()

# Set Python module properties for basic bindings
set_target_properties(pynlm PROPERTIES
    OUTPUT_NAME "${PYTHON_MODULE_NAME}"
    VERSION "${PYTHON_MODULE_VERSION}"
    SOVERSION "1"
)

# Enhanced Python bindings
pybind11_add_module(pynlm_enhanced python/bindings.cpp)
target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Performance optimizations for enhanced Python bindings
if(PY_NLM_ENABLE_PERF_OPTIMIZATIONS)
    target_compile_options(pynlm_enhanced PRIVATE -march=native -flto -fvisibility=hidden)
    target_link_libraries(pynlm_enhanced PRIVATE -flto)
endif()

# Set Python module properties for enhanced bindings
set_target_properties(pynlm_enhanced PROPERTIES
    OUTPUT_NAME "${PYTHON_MODULE_ENHANCED_NAME}"
    VERSION "${PYTHON_MODULE_VERSION}"
    SOVERSION "2"
)

if(MSVC)
    target_compile_options(pynlm_enhanced PRIVATE /permissive-)
endif()

# Installation targets
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

install(TARGETS pynlm_enhanced
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Build configuration summary
message(STATUS "=== Python Bindings Build Configuration ===")
message(STATUS "Python executable: ${Python_EXECUTABLE}")
message(STATUS "Python include dir: ${Python_INCLUDE_DIRS}")
message(STATUS "Python libraries: ${Python_LIBRARIES}")
message(STATUS "Performance optimizations: ${PY_NLM_ENABLE_PERF_OPTIMIZATIONS}")
message(STATUS "Debug assertions: ${PY_NLM_ENABLE_ASSERTIONS}")
message(STATUS "Profiling support: ${PY_NLM_ENABLE_PROFILING}")
message(STATUS "Basic Python module name: ${PYTHON_MODULE_NAME}")
message(STATUS "Enhanced Python module name: ${PYTHON_MODULE_ENHANCED_NAME}")
message(STATUS "Module version: ${PYTHON_MODULE_VERSION}")
message(STATUS "==============================================")