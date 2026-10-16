# Modern CMake Configuration for NLM Project
# This file provides centralized configuration and common utilities for all CMakeLists.txt files

cmake_minimum_required(VERSION 3.16)

# Project configuration validation
function(validate_nlm_config)
    # Check for required directories
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src")
        message(FATAL_ERROR "Source directory 'src' not found")
    endif()

    # Check for required files
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
        message(WARNING "main.cpp not found - executable will not be built")
    endif()

    # Set default build type if not specified
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
        message(STATUS "No build type specified, using Release")
    endif()

    # Validate C++ standard
    if(CMAKE_CXX_STANDARD LESS 20)
        message(FATAL_ERROR "C++ standard must be at least 20")
    endif()
endfunction()

# Performance monitoring integration
function(add_performance_monitoring_targets)
    if(BUILD_PERFORMANCE_TESTS)
        add_executable(nlm_performance_benchmark src/benchmark.cpp)
        target_link_libraries(nlm_performance_benchmark PRIVATE nlm_core)
        add_test(NAME performance_benchmark COMMAND nlm_performance_benchmark)
        
        # Add profiling support
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(nlm_performance_benchmark PRIVATE -g -O0)
        else()
            target_compile_options(nlm_performance_benchmark PRIVATE -O3 -DNDEBUG)
        endif()
    endif()
endfunction()

# Component-based source file management
function discover_sources COMPONENT_NAME PATTERN_PREFIX)
    file(GLOB ${COMPONENT_NAME}_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/${PATTERN_PREFIX}*/*.cpp")
    if(NOT ${COMPONENT_NAME}_SOURCES)
        message(WARNING "No sources found for component ${COMPONENT_NAME} with pattern ${PATTERN_PREFIX}")
    endif()
    set(${COMPONENT_NAME}_SOURCES ${${COMPONENT_NAME}_SOURCES}} PARENT_SCOPE)
endfunction()

# Create component library with modern CMake practices
macro(add_nlm_component LIBRARY_NAME SOURCES)
    add_library(${LIBRARY_NAME} STATIC ${SOURCES})
    
    # Set library properties
    target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)
    target_compile_options(${LIBRARY_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
    
    # Include directories
    target_include_directories(${LIBRARY_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    )
    
    # Export for modern IDE support
    set_target_properties(${LIBRARY_NAME} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endmacro()

# Install configuration helper
function(configure_install_targets)
    # Install headers
    file(GLOB_RECURSE HEADER_FILES "src/*/*.hpp" "src/*/*.h")
    install(FILES ${HEADER_FILES} DESTINATION include/nlm)
    
    # Create package config for find_package support
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nlm-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake"
        @ONLY
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nlm-config.cmake"
            DESTINATION lib/cmake/nlm)
endfunction()

# Error handling and recovery
macro(add_conditional_target TARGET_NAME SOURCES CONDITION)
    if(${CONDITION})
        add_library(${TARGET_NAME} STATIC ${SOURCES})
        message(STATUS "Added target ${TARGET_NAME} (condition: ${CONDITION})")
    else()
        message(STATUS "Skipping target ${TARGET_NAME} (condition: ${CONDITION})")
    endif()
endmacro()
