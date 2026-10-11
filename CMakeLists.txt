# CMake Build System for NLM - Unified Modern Build System
# Supports multiple build types, package managers, and installation options

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# ----------------------------------------------------------------------------
# Configuration Options
# ----------------------------------------------------------------------------

# Build type validation
option(NLM_BUILD_CORE "Build core NLM libraries" ON)
option(NLM_BUILD_TESTS "Build NLM tests" ON)
option(NLM_BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_BUILD_CLI "Build advanced CLI and plugin system" ON)
option(NLM_BUILD_DEMOS "Build demo executables" ON)
option(NLM_ENABLE_CONAN "Enable Conan package manager" OFF)
option(NLM_ENABLE_VCPKG "Enable vcpkg package manager" OFF)
option(NLM_ENABLE_CPACK "Enable CPack for packaging" OFF)
option(NLM_BUILD_COMPONENTS "Build all components (excluding demos)" ON)
option(NLM_BUILD_FULL "Build everything including demos and tests" OFF)
option(NLM_BUILD_MINIMAL "Build minimal core only" OFF)
option(NLM_BUILD_DEV "Build development version with headers and docs" OFF)

# Build type with validation
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Validate build type
set(VALID_BUILD_TYPES Debug Release RelWithDebInfo MinSizeRel)
list(FIND VALID_BUILD_TYPES "${CMAKE_BUILD_TYPE}" _build_type_index)
if(_build_type_index EQUAL -1)
    message(FATAL_ERROR "Invalid build type '${CMAKE_BUILD_TYPE}'. Valid options: ${VALID_BUILD_TYPES}")
endif()

# Build component selection based on build types
if(NLM_BUILD_FULL)
    set(NLM_BUILD_TESTS ON)
    set(NLM_BUILD_PYTHON_BINDINGS ON)
    set(NLM_BUILD_CLI ON)
    set(NLM_BUILD_DEMOS ON)
elseif(NLM_BUILD_MINIMAL)
    set(NLM_BUILD_TESTS OFF)
    set(NLM_BUILD_PYTHON_BINDINGS OFF)
    set(NLM_BUILD_CLI OFF)
    set(NLM_BUILD_DEMOS OFF)
elseif(NLM_BUILD_COMPONENTS)
    set(NLM_BUILD_TESTS ON)
    set(NLM_BUILD_PYTHON_BINDINGS ON)
endif()

# Build time reporting
function(report_build_time)
    if(BUILD_START_TIME)
        math(EXPR BUILD_DURATION "${CMAKE_CURRENT_TIME} - ${BUILD_START_TIME}")
        message(STATUS "Build completed in ${BUILD_DURATION} seconds")
    endif()
endfunction()

# ----------------------------------------------------------------------------
# Package Manager Support
# ----------------------------------------------------------------------------

# Try to find Conan
if(NLM_ENABLE_CONAN)
    find_program(CONAN_PATH conan)
    if(CONAN_PATH)
        message(STATUS "Found Conan at ${CONAN_PATH}")
        execute_process(COMMAND ${CONAN_PATH} info --requires
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_VARIABLE CONAN_REQUIRES_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        # Parse conanfile.py for dependencies
        if(EXISTS "${CMAKE_SOURCE_DIR}/conanfile.py")
            message(STATUS "Conan configuration found")
        endif()
    else()
        message(WARNING "Conan not found, disabling Conan support")
        set(NLM_ENABLE_CONAN OFF CACHE BOOL "" FORCE)
    endif()
endif()

# Try to find vcpkg
if(NLM_ENABLE_VCPKG)
    if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Found vcpkg at ${VCPKG_ROOT}")
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    else()
        message(WARNING "vcpkg not found, disabling vcpkg support")
        set(NLM_ENABLE_VCPKG OFF CACHE BOOL "" FORCE)
    endif()
endif()

# ----------------------------------------------------------------------------
# Dependency Management
# ----------------------------------------------------------------------------

# Core dependencies (with fallback to FetchContent)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found, fetching from GitHub")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

find_package(yaml-cpp QUIET)
if(NOT yaml-cpp_FOUND)
    message(STATUS "yaml-cpp not found, fetching from GitHub")
    include(FetchContent)
    FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/yaml-cpp/yaml-cpp.git
        GIT_TAG yaml-cpp-0.8.0
    )
    FetchContent_MakeAvailable(yaml-cpp)
endif()

# pybind11 for Python bindings
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(pybind11 QUIET)
    if(NOT pybind11_FOUND)
        message(STATUS "pybind11 not found, fetching from GitHub")
        include(FetchContent)
        FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG v2.11.1
        )
        FetchContent_MakeAvailable(pybind11)
    endif()
endif()

# ----------------------------------------------------------------------------
# Core Library
# ----------------------------------------------------------------------------

if(NLM_BUILD_CORE)
    message(STATUS "Building core NLM libraries")
    
    add_library(nlm_core STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Types/Types.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Config/Config.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Random/Random.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/SimulationClock/SimulationClock.cpp
    )
    
    add_library(nlm_agent STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/agent/SensoryPercept.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/agent/AgentBrain.cpp
    )
    
    add_library(nlm_world STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/world/SimpleWorld.cpp
    )
    
    add_library(nlm_phase3 STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase3Experiment.cpp
    )
    
    add_library(nlm_phase4 STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase4Experiment.cpp
    )
    
    # Link dependencies
    target_link_libraries(nlm_agent PUBLIC nlm_core)
    target_link_libraries(nlm_world PUBLIC nlm_core)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
    
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    )
    
    target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    
    target_compile_features(nlm_core PRIVATE cxx_std_20)
endif()

# ----------------------------------------------------------------------------
# Executables
# ----------------------------------------------------------------------------

if(NLM_BUILD_CORE)
    # Main executable
    add_executable(nlm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    )
    target_link_libraries(nlm PUBLIC nlm_core)
    
    # Phase 3 demo executable
    if(NLM_BUILD_DEMOS)
        add_executable(nlm_phase3_demo
            ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase3Demo.cpp
        )
        target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
    endif()
    
    # Phase 4 demo executable
    if(NLM_BUILD_DEMOS)
        add_executable(nlm_phase4_demo
            ${CMAKE_CURRENT_SOURCE_DIR}/src/experiments/Phase4Demo.cpp
        )
        target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    endif()
    
    # Advanced CLI (Phase 3)
    if(NLM_BUILD_CLI)
        add_executable(nlm_advanced_cli
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/NLMSession.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/PluginManager.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/cli/CommandLineParser.cpp
        )
        target_link_libraries(nlm_advanced_cli PUBLIC nlm_core)
        target_include_directories(nlm_advanced_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    endif()
endif()

# ----------------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------------

if(NLM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

# ----------------------------------------------------------------------------
# Python Bindings
# ----------------------------------------------------------------------------

if(NLM_BUILD_PYTHON_BINDINGS AND pybind11_FOUND)
    message(STATUS "Building Python bindings")
    enable_language(Python)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/python)
endif()

# ----------------------------------------------------------------------------
# Installation
# ----------------------------------------------------------------------------

include(GNUInstallDirs)

# Default installation configuration
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN ".*" EXCLUDE
)

if(NLM_BUILD_CORE)
    install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
        EXPORT nlm_targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    
    if(NLM_BUILD_DEMOS)
        install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
    
    if(NLM_BUILD_CLI)
        install(TARGETS nlm_advanced_cli
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()
endif()

# Python bindings installation
if(NLM_BUILD_PYTHON_BINDINGS AND pybind11_FOUND)
    install(TARGETS pynlm
        LIBRARY DESTINATION ${PYTHON_SITEARCH}
        ARCHIVE DESTINATION ${PYTHON_SITEARCH}
        RUNTIME DESTINATION ${PYTHON_SITEARCH}
    )
endif()

# ----------------------------------------------------------------------------
# CPack Configuration
# ----------------------------------------------------------------------------

if(NLM_ENABLE_CPACK)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CPackConfig.cmake)
endif()

# ----------------------------------------------------------------------------
# Final Configuration Report
# ----------------------------------------------------------------------------

message(STATUS "\n=== NLM Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Core libraries: ${NLM_BUILD_CORE}")
message(STATUS "Tests: ${NLM_BUILD_TESTS}")
message(STATUS "Python bindings: ${NLM_BUILD_PYTHON_BINDINGS}")
message(STATUS "Advanced CLI: ${NLM_BUILD_CLI}")
message(STATUS "Demos: ${NLM_BUILD_DEMOS}")
message(STATUS "Package manager (Conan): ${NLM_ENABLE_CONAN}")
message(STATUS "Package manager (vcpkg): ${NLM_ENABLE_VCPKG}")
message(STATUS "CPack: ${NLM_ENABLE_CPACK}")
message(STATUS "========================================\n")

# Report build time
report_build_time()
