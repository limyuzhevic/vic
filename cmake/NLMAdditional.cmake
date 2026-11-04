cmake_minimum_required(VERSION 3.16)

project(NLM_Additional VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Additional core libraries

# Factory system
add_library(nlm_factory STATIC
    core/Factory/Factory.cpp
)
target_include_directories(nlm_factory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core/Factory>
    $<INSTALL_INTERFACE:include>
)

# Performance profiling
add_library(nlm_performance STATIC
    performance/Profiler.cpp
)
target_include_directories(nlm_performance PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/performance>
    $<INSTALL_INTERFACE:include>
)

# Python bindings (enhanced)
option(NLM_BUILD_PYTHON_BINDINGS_ENHANCED "Build the enhanced pynlm Python module" OFF)
if(NLM_BUILD_PYTHON_BINDINGS_ENHANCED)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_factory nlm_performance nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    pybind11_add_module(pynlm_enhanced python/bindings_enhanced.cpp)
    target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world nlm_factory nlm_performance)
    target_include_directories(pynlm_enhanced PRIVATE src)
    install(TARGETS pynlm_enhanced LIBRARY DESTINATION .)
endif()

# Installation
install(TARGETS nlm_factory nlm_performance
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Package configuration
install(EXPORT NLMPackageConfig
    FILE NLMAdditionalConfig.cmake
    NAMESPACE NLM::
    DESTINATION lib/cmake/NLM
)