cmake_minimum_required(VERSION 3.20)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Modern C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimized build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Modern compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Use file globbing for better maintainability
file(GLOB_RECURSE NLM_CORE_SOURCES
    "${NLM_SOURCE_DIR}/core/**/*.cpp"
    "${NLM_SOURCE_DIR}/brain/**/*.cpp"
    "${NLM_SOURCE_DIR}/dynamics/**/*.cpp"
    "${NLM_SOURCE_DIR}/plasticity/**/*.cpp"
    "${NLM_SOURCE_DIR}/development/**/*.cpp"
    "${NLM_SOURCE_DIR}/neuromodulation/**/*.cpp"
    "${NLM_SOURCE_DIR}/memory/**/*.cpp"
    "${NLM_SOURCE_DIR}/cognition/**/*.cpp"
    "${NLM_SOURCE_DIR}/sensory/**/*.cpp"
    "${NLM_SOURCE_DIR}/prediction/**/*.cpp"
    "${NLM_SOURCE_DIR}/motor/**/*.cpp"
    "${NLM_SOURCE_DIR}/environment/**/*.cpp"
    "${NLM_SOURCE_DIR}/experiments/**/*.cpp"
    "${NLM_SOURCE_DIR}/visualization/**/*.cpp"
    "${NLM_SOURCE_DIR}/agent/**/*.cpp"
    "${NLM_SOURCE_DIR}/world/**/*.cpp"
)

# Create core library with optimized settings
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Modern compiler flags with optimization levels
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CONFIG:Release>:-O3 -march=native -flto>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
    -fconcepts -fconstexpr-ops-limit=1000000
)

# Preprocessor definitions
target_compile_definitions(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:_GNU_SOURCE>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    NLM_EXPORTS
    NLM_CORE_IMPLEMENTATION
)

# Position independent code for shared libraries
target_compile_options(nlm_core PRIVATE -fPIC)

# Linker optimization
target_link_libraries(nlm_core PUBLIC pthread)

# Installation configuration
set(INSTALL_INCDIR include/nlm CACHE PATH "Installation directory for headers")

set_target_properties(nlm_core PROPERTIES
    PUBLIC_HEADER "nlm_core.h"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION ${INSTALL_INCDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${INSTALL_INCDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Modern compiler flags with optimization levels
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CONFIG:Release>:-O3 -march=native -flto>
    $<$<CONFIG:Debug>:-O0 -g -DDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
    -fconcepts -fconstexpr-ops-limit=1000000
)

# Preprocessor definitions
target_compile_definitions(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:_GNU_SOURCE>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    NLM_EXPORTS
    NLM_CORE_IMPLEMENTATION
)

# Position independent code for shared libraries
target_compile_options(nlm_core PRIVATE -fPIC)

# Linker optimization
target_link_libraries(nlm_core PUBLIC pthread)

# Installation configuration
set(INSTALL_INCDIR include/nlm CACHE PATH "Installation directory for headers")

set_target_properties(nlm_core PROPERTIES
    PUBLIC_HEADER "nlm_core.h"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS nlm_core
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION ${INSTALL_INCDIR}
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${INSTALL_INCDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
