# NLM CMake Build System Configuration

# Policy settings for modern CMake
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # Allow command argument quoting to escape quotes
endif()

if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW) # Make variable references optional
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW) # Option reference not quoted
endif()

if(POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW) # Do not define variable to expand to empty string
endif()

# Project configuration
project(NLM VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "An Experimental Artificial Developmental Brain" HOMEPAGE_URL "https://github.com/" CONTACT "Research project")

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
endif()

# Set available build types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NLM_PLATFORM_LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(NLM_PLATFORM_MACOS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(NLM_PLATFORM_WINDOWS TRUE)
endif()

# Package manager detection
find_program(CONAN_EXECUTABLE conan)
if(CONAN_EXECUTABLE)
    set(NLM_HAS_CONAN TRUE)
endif()

find_program(VCPKG_EXECUTABLE vcpkg)
if(VCPKG_EXECUTABLE)
    set(NLM_HAS_VCPKG TRUE)
endif()

# Compiler feature detection
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(NLM_HAS_CXX_STD_20 TRUE)
endif()

# Build options
option(NLM_ENABLE_TESTS "Enable building tests" ON)
option(NLM_ENABLE_PYBIND11 "Enable building Python bindings" OFF)
option(NLM_ENABLE_VISUALIZATION "Enable visualization components" ON)
option(NLM_ENABLE_DOCUMENTATION "Enable building documentation" OFF)
option(NLM_SHARED_LIBS "Build shared libraries instead of static" OFF)
option(NLM_EXPORT_SYMBOLS "Export symbols for dynamic loading" OFF)

# Warning configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Security features
if(NLM_PLATFORM_LINUX OR NLM_PLATFORM_MACOS)
    add_compile_options(-fstack-protector-strong -fPIC)
endif()

# Dependency management
if(NLM_HAS_CONAN)
    # Use Conan to manage dependencies
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Conan.cmake OPTIONAL)
elseif(NLM_HAS_VCPKG)
    # Use vcpkg to manage dependencies
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Vcpkg.cmake OPTIONAL)
else()
    # Fallback to find_package for dependencies
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindDependencies.cmake OPTIONAL)
endif()

# Function to add source files to target
function(nlm_add_library target_name)
    add_library(${target_name} ${ARGN})
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    if(NLM_SHARED_LIBS)
        target_compile_definitions(${target_name} PUBLIC NLM_SHARED_LIB)
    endif()
    if(NLM_EXPORT_SYMBOLS)
        target_compile_definitions(${target_name} PUBLIC NLM_EXPORT_SYMBOLS)
        generate_export_header(${target_name}
            BASE_NAME "${target_name}"
            EXPORT_MACRO_NAME "NLM_${target_name}_EXPORT"
            STATIC_DEFINE "NLM_${target_name}_STATIC"
        )
    endif()
endfunction()

# Function to configure include directories
function(nlm_target_includes target_name)
    foreach(dir ${ARGN})
        target_include_directories(${target_name} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>
            $<INSTALL_INTERFACE:include/${dir}>
        )
    endforeach()
endfunction()

# Function to configure tests
function(nlm_add_test test_name)
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
endfunction()
