cmake_minimum_required(VERSION 3.20)

project(NLM
    VERSION 0.1.0
    DESCRIPTION "Experimental Artificial Developmental Brain"
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/nlm-project/nlm"
)

# C++20 standard with modern features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wshadow -Wnon-virtual-dtor
                        -Wold-style-cast -Wcast-align -Wsign-conversion -Wnull-dereference
                        -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
                        -Wlogical-op -Wuseless-cast -Wignored-qualifiers -Wmissing-include-dirs
                        -Wunsafe-loop-opt -Wstrict-overflow=5)
endif()

# Position independent code (important for shared libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Default output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source file collection using globbing with pattern matching
function(collect_sources var)
    file(GLOB_RECURSE sources ${ARGN})
    set(${var} ${sources} PARENT_SCOPE)
endfunction()

# Performance optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -flto)
    add_link_options(-flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os)
else()
    add_compile_options(-O0 -g)
endif()

# Core library source files
collect_sources(CORE_SOURCES
    "src/core/Types/*.cpp"
    "src/core/Config/*.cpp"
    "src/core/Random/*.cpp"
    "src/core/Logger/*.cpp"
    "src/core/SimulationClock/*.cpp"
    "src/brain/*.cpp"
    "src/dynamics/*.cpp"
    "src/plasticity/*.cpp"
    "src/development/*.cpp"
    "src/neuromodulation/*.cpp"
    "src/memory/*.cpp"
    "src/cognition/*.cpp"
    "src/sensory/*.cpp"
    "src/prediction/*.cpp"
    "src/motor/*.cpp"
    "src/environment/*.cpp"
    "src/experiments/*.cpp"
    "src/visualization/*.cpp"
)

# Agent system sources
collect_sources(AGENT_SOURCES
    "src/agent/*.cpp"
)

# World system sources
collect_sources(WORLD_SOURCES
    "src/world/*.cpp"
)

# Phase experiment sources
collect_sources(PHASE3_SOURCES
    "src/experiments/Phase3Experiment.cpp"
)

collect_sources(PHASE4_SOURCES
    "src/experiments/Phase4Experiment.cpp"
)

collect_sources(PHASE5_SOURCES
    "src/experiments/Phase5Experiment.cpp"
)

# Main executable
add_executable(nlm src/main.cpp)
target_link_libraries(nlm PRIVATE nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Performance optimizations for main executable
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nlm PRIVATE -march=native -flto -DNDEBUG)
    target_link_options(nlm PRIVATE -flto)
endif()

# Core static library (the brain engine)
add_library(nlm_core STATIC ${CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system library
add_library(nlm_agent STATIC ${AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system library
add_library(nlm_world STATIC ${WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiment library
add_library(nlm_phase3 STATIC ${PHASE3_SOURCES})
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiment library
add_library(nlm_phase4 STATIC ${PHASE4_SOURCES})
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiment library
add_library(nlm_phase5 STATIC ${PHASE5_SOURCES})
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Documentation targets
add_custom_target(docs
    COMMENT "Generate documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/docs && doxygen Doxyfile
    VERBATIM
)

# Benchmarking target
add_custom_target(benchmark
    COMMENT "Run performance benchmarks"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ./nlm --benchmark
    DEPENDS nlm
    VERBATIM
)

# Testing configuration
enable_testing()

# Add tests
add_subdirectory(tests)

# Installation
include(GNUInstallDirs)

install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "*.h"
)

# CPack configuration for package generation
set(CPACK_PACKAGE_NAME "nlm")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain")
set(CPACK_PACKAGE_CONTACT "nlm-project@protonmail.com")
set(CPACK_PACKAGE_MAINTAINER "NLM Authors")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/nlm-project/nlm")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Computational brain simulation framework")

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

# Source package
include(CPackSource)
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    ".git/"
    ".gitignore"
    "build/"
    "*.o"
    "*.obj"
    "*.exe"
    "*.so"
    "*.dylib"
    "*.dll"
    "*.a"
    "*.lib"
    "*.pyc"
    "__pycache__/"
)

# Binary package
include(CPack)
set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_PACKAGE_FILE_NAME "nlm-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

# Install configuration files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION ${CMAKE_INSTALL_LICENSEDIR}
)

# Summary
message(STATUS "===========================================")
message(STATUS "NLM Build Configuration")
message(STATUS "===========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Core sources: ${CORE_SOURCES}")
message(STATUS "Total libraries: 5")
message(STATUS "===========================================")
