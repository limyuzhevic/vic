# CMake Modernisation Plan for NLM Project

## Current State Summary

The NLM project has a basic CMake build system with the following characteristics:
- Uses CMake 3.16 with target-based libraries but lacks modern practices
- Multiple CMakeLists.txt files with duplicated configuration
- Limited dependency management and build customization
- Missing documentation generation, package exports, and advanced build features

## Prioritized CMake Improvements

### 1. **Root CMakeLists.txt Modernisation** (HIGH PRIORITY)

**Problem**: Monolithic root CMakeLists.txt with duplicated source listings
**Solution**: Implement target-based modular structure with external CMake modules

**Files to Modify**: 
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/CMakeLists.txt`

**Implementation**:
```cmake
# Add modern CMake features
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard with target-specific properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure build types with compiler flags
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)
if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    add_compile_options(-O0 -g -DDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELEASE")
    add_compile_options(-O3 -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "RELWITHDEBINFO")
    add_compile_options(-O2 -g -DNDEBUG)
elseif(BUILD_TYPE_UPPER STREQUAL "MINSIZEREL")
    add_compile_options(-Os -DNDEBUG)
endif()

# Create CMake modules directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

# Include modern CMake patterns
include(cmake/modern_cmake.cmake)
include(cmake/dependency_management.cmake)
include(cmake/testing.cmake)
include(cmake/installation.cmake)

# Add source subdirectories using target-based approach
add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(tests)
```

### 2. **CMake Modules Structure** (HIGH PRIORITY)

**Problem**: No modular CMake organization
**Solution**: Create organized CMake modules directory with specialized functionality

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/modern_cmake.cmake`
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/dependency_management.cmake`
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/testing.cmake`
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/installation.cmake`

**Implementation**:
```cmake
# cmake/modern_cmake.cmake
function(nlm_add_library target_name)
    add_library(${target_name} STATIC)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_compile_options(${target_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
    target_link_libraries(${target_name} PUBLIC nlm_core)
    set_target_properties(${target_name} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endfunction()

function(nlm_add_executable target_name)
    add_executable(${target_name} ${ARGN})
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    target_link_libraries(${target_name} PRIVATE nlm_core)
endfunction()

# Export functionality
export(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
       FILE "${CMAKE_CURRENT_BINARY_DIR}/NLMTargets.cmake")
export(PACKAGE NLM)
```

### 3. **Target-Based Library Organization** (HIGH PRIORITY)

**Problem**: Direct source listing in root CMakeLists.txt
**Solution**: Create separate CMakeLists.txt for each subsystem using target-based approach

**Files to Modify**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/src/CMakeLists.txt`
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/src/core/CMakeLists.txt`

**Implementation**:
```cmake
# src/CMakeLists.txt
# Core libraries using target-based approach
add_library(nlm_core STATIC)
target_sources(nlm_core PRIVATE
    core/Types/Types.cpp
    core/Config/Config.cpp
    core/Random/Random.cpp
    core/Logger/Logger.cpp
    core/SimulationClock/SimulationClock.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Other subsystem libraries using the same pattern
add_library(nlm_agent STATIC src/agent/AgentBrain.cpp src/agent/SensoryPercept.cpp)
add_library(nlm_world STATIC src/world/SimpleWorld.cpp)
```

### 4. **Dependency Management with FetchContent** (HIGH PRIORITY)

**Problem**: No external dependency management
**Solution**: Implement modern dependency management using FetchContent and find_package

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/dependency_management.cmake`

**Implementation**:
```cmake
# cmake/dependency_management.cmake
include(FetchContent)

# Python bindings dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Optional: Add external libraries like LAPACK, BLAS for performance
# find_package(LAPACK REQUIRED)
# find_package(BLAS REQUIRED)

# FetchContent for complex dependencies
FetchContent_Declare(
    nlm_test_framework
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(nlm_test_framework)
```

### 5. **Generator Expressions for Conditional Compilation** (MEDIUM PRIORITY)

**Problem**: Basic conditional compilation without generator expressions
**Solution**: Use generator expressions for platform-specific and compiler-specific code

**Files to Modify**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/modern_cmake.cmake`

**Implementation**:
```cmake
# Platform-specific configurations
target_compile_definitions(nlm_core PRIVATE
    $<$<PLATFORM_ID:Linux>:_LINUX>
    $<$<PLATFORM_ID:Windows>:_WINDOWS>
    $<$<PLATFORM_ID:Darwin>:_MACOS>
)

# Compiler-specific optimizations
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-march=native>
    $<$<CXX_COMPILER_ID:Clang>:-march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
)

# Feature detection
target_compile_definitions(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:_HAS_CXX17=1>
    $<$<CXX_COMPILER_ID:MSVC>:_HAS_CXX17=1>
)
```

### 6. **Advanced Build Configurations** (MEDIUM PRIORITY)

**Problem**: Basic build type handling without comprehensive configuration
**Solution**: Implement advanced build configurations with preset configurations

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/config.cmake`

**Implementation**:
```cmake
# cmake/config.cmake
# Build type presets
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Configuration-specific compile options
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -DNDEBUG")

# Position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# RPATH configuration
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
```

### 7. **Package Export System** (MEDIUM PRIORITY)

**Problem**: Basic install targets without proper package exports
**Solution**: Implement comprehensive package export system for downstream usage

**Files to Modify**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/installation.cmake`

**Implementation**:
```cmake
# cmake/installation.cmake
include(GNUInstallDirs)

# Package configuration
install(EXPORT NLMTargets
    FILE NLMConfig.cmake
    NAMESPACE NLM::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Package version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NLM
)

# Install headers
install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm
    FILES_MATCHING PATTERN "*.hpp"
)

# Install libraries
install(TARGETS nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4
    EXPORT NLMTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
```

### 8. **Documentation Generation** (MEDIUM PRIORITY)

**Problem**: No documentation generation support
**Solution**: Add Doxygen and other documentation generation capabilities

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/docs/CMakeLists.txt`

**Implementation**:
```cmake
# docs/CMakeLists.txt
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxygen.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/html)
    
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/../docs/
        COMMENT "Generate documentation"
    )
endif()
```

### 9. **Comprehensive Testing Framework** (MEDIUM PRIORITY)

**Problem**: Basic testing without advanced test framework
**Solution**: Implement comprehensive testing with Google Test integration

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/testing.cmake`

**Implementation**:
```cmake
# cmake/testing.cmake
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add test targets with proper dependencies
add_executable(nlm_test_test)
target_link_libraries(nlm_test_test PRIVATE nlm_core gtest_main)
add_test(NAME nlm_test COMMAND nlm_test_test)

# Coverage support (if available)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(nlm_test_test PRIVATE --coverage)
    target_link_libraries(nlm_test_test PRIVATE --coverage)
endif()
```

### 10. **Configuration Header Generation** (MEDIUM PRIORITY)

**Problem**: No runtime configuration header generation
**Solution**: Add CMake-based configuration header generation

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/config_header.cmake`

**Implementation**:
```cmake
# cmake/config_header.cmake
# Generate configuration header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/nlm_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_config.h
    @ONLY
)

# Add generated header to targets
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)
```

### 11. **Platform-Specific Optimizations** (LOW PRIORITY)

**Problem**: No platform-specific build optimizations
**Solution**: Add platform-specific compilation flags and optimizations

**Files to Modify**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/modern_cmake.cmake`

**Implementation**:
```cmake
# Linux-specific optimizations
target_link_libraries(nlm_core PRIVATE
    $<$<PLATFORM_ID:Linux>:-lm -ldl -pthread>
)

# Windows-specific configurations
target_compile_definitions(nlm_core PRIVATE
    $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
)

# macOS-specific optimizations
target_link_libraries(nlm_core PRIVATE
    $<$<PLATFORM_ID:Darwin>:-framework Accelerate>
)
```

### 12. **Build Time Analysis and Optimization** (LOW PRIORITY)

**Problem**: No build analysis capabilities
**Solution**: Add build time analysis and optimization targets

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/build_analysis.cmake`

**Implementation**:
```cmake
# cmake/build_analysis.cmake
# Add build time analysis target
add_custom_target(build_analysis
    COMMAND ${CMAKE_COMMAND} -E time cmake --build . --target all
    COMMENT "Analyze build time and performance"
    VERBATIM
)

# Add dependency graph generation
add_custom_target(dependency_graph
    COMMAND ${CMAKE_COMMAND} -E echo "Generating dependency graph..."
    COMMAND cat ${CMAKE_BINARY_DIR}/CMakeFiles/*.cmake | grep "add_library\|add_executable" > ${CMAKE_BINARY_DIR}/dependency_graph.txt
    COMMENT "Generate dependency graph"
    VERBATIM
)
```

### 13. **Code Generation Support** (LOW PRIORITY)

**Problem**: No code generation capabilities
**Solution**: Add CMake-based code generation for configuration or other artifacts

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/code_generation.cmake`

**Implementation**:
```cmake
# cmake/code_generation.cmake
# Add code generation capability
add_custom_target(generate_code
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_config.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_config.py
    COMMENT "Generate configuration files"
    VERBATIM
)

# Example: Generate configuration from template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/templates/config.template
    ${CMAKE_CURRENT_BINARY_DIR}/generated_config.cpp
    @ONLY
)
```

### 14. **Version Management System** (LOW PRIORITY)

**Problem**: Basic version without sophisticated version management
**Solution**: Implement comprehensive version management system

**Files to Modify**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/CMakeLists.txt`

**Implementation**:
```cmake
# Version management
project(NLM
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "NLM — 熙然 (Neural Learning Machine)"
    HOMEPAGE_URL "https://github.com/yourorg/nlm"
)

# Generate version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_version.h
    @ONLY
)

# Add version to targets
target_sources(nlm_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/nlm_version.h)
```

### 15. **Cross-Platform Build Support** (LOW PRIORITY)

**Problem**: Limited cross-platform support
**Solution**: Add comprehensive cross-platform build configurations

**Files to Create**:
- `/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_67558df8-db09-4a54-a97d-7bcaa3a2f952/cmake/cross_platform.cmake`

**Implementation**:
```cmake
# cmake/cross_platform.cmake
# Universal build configuration
if(WIN32)
    # Windows-specific settings
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    # macOS-specific settings
    set(CMAKE_MACOSX_RPATH ON)
elseif(UNIX)
    # Linux/Unix-specific settings
    add_definitions(-D_POSIX_C_SOURCE=200809L)
endif()

# Compiler detection and configuration
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")
elseif(MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif()
```

## Implementation Priority and Order

1. **Phase 1 (High Priority - First)**:
   - Modernize root CMakeLists.txt
   - Create CMake modules directory structure
   - Implement target-based library organization
   - Add dependency management

2. **Phase 2 (Medium Priority - Second)**:
   - Add generator expressions for conditional compilation
   - Implement advanced build configurations
   - Add package export system
   - Add documentation generation

3. **Phase 3 (Medium Priority - Third)**:
   - Implement comprehensive testing framework
   - Add configuration header generation
   - Add platform-specific optimizations

4. **Phase 4 (Low Priority - Last)**:
   - Add build time analysis
   - Add code generation support
   - Implement version management
   - Add cross-platform support

## Backward Compatibility Notes

This modernization maintains backward compatibility by:
- Preserving existing install targets
- Maintaining library interfaces
- Keeping existing build configurations
- Ensuring target names remain consistent
- Supporting both existing and new build patterns

## Next Steps

1. Backup existing CMakeLists.txt files
2. Create CMake modules directory structure
3. Implement Phase 1 improvements
4. Test build system after each major change
5. Gradually add additional features
6. Validate cross-platform compatibility

This plan provides a comprehensive roadmap for modernizing the NLM CMake build system while maintaining backward compatibility and adding new capabilities for improved developer experience and build performance.
