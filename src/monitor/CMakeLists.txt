# CMakeLists.txt for NLM Performance Monitoring

# This CMake file configures the NLM Performance Monitoring module

cmake_minimum_required(VERSION 3.16)
project(NLM_Monitor VERSION 0.1.0 LANGUAGES CXX)

# Modern CMake policies
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG -march=native -flto)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g -DDEBUG)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_options(-O2 -g -DNDEBUG)
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_compile_options(-Os -DNDEBUG)
endif()

# Platform-specific configurations
if(WIN32)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Feature options
option(NLM_ENABLE_MONITORING "Enable performance monitoring" ON)
option(NLM_MONITORING_TYPE "Monitoring implementation type" "comprehensive")
option(NLM_ENABLE_PYTHON_BINDINGS "Build Python bindings" ON)
option(NLM_ENABLE_MONITORS_DOC "Generate monitoring documentation" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../brain)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../performance)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cognition)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../memory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../neuromodulation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../visualization)

# Monitor core sources
file(GLOB MONITOR_CORE_SOURCES
    "monitor/PerformanceMonitor.cpp"
    "monitor/MemoryMonitor.cpp"
    "monitor/NeuralMonitor.cpp"
    "monitor/NeuromodulationMonitor.cpp"
    "monitor/monitor.cpp"
)

# Monitor tools sources
file(GLOB TOOLS_SOURCES
    "monitor/tools/MemoryLeakDetector.cpp"
    "monitor/tools/MemoryPoolManager.cpp"
    "monitor/tools/PerformanceProfiler.cpp"
    "monitor/tools/BottleneckAnalyzer.cpp"
)

# Python bindings sources
file(GLOB PYTHON_BINDINGS_SOURCES
    "python/monitor_bindings.cpp"
)

# Create monitor library
if(NLM_ENABLE_MONITORING)
    add_library(nlm_monitor STATIC ${MONITOR_CORE_SOURCES} ${TOOLS_SOURCES})
    
    target_include_directories(nlm_monitor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/nlm_monitor>
    )
    
    target_link_libraries(nlm_monitor PUBLIC nlm_headers)
    
    # Set target properties
    set_target_properties(nlm_monitor PROPERTIES
        OUTPUT_NAME "nlm_monitor"
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
    )
    
    # Python bindings
    if(NLM_ENABLE_PYTHON_BINDINGS)
        add_library(nlm_monitor_python MODULE ${PYTHON_BINDINGS_SOURCES})
        
        target_include_directories(nlm_monitor_python PRIVATE
            ${PYTHON_INCLUDE_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../python
        )
        
        target_link_libraries(nlm_monitor_python PRIVATE
            nlm_monitor
            nlm
            pybind11::module
        )
        
        set_target_properties(nlm_monitor_python PROPERTIES
            PREFIX ""
            SUFFIX "${PYTHON_MODULE_EXTENSION}"
            OUTPUT_NAME "monitor_bindings"
        )
        
        # Install Python module
        install(TARGETS nlm_monitor_python
            DESTINATION ${PYTHON_SITE_PACKAGES}
        )
    endif()
    
    # Install monitor library
    install(TARGETS nlm_monitor
        EXPORT nlmMonitorTargets
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    )
    
    # Install headers
    install(DIRECTORY monitor/ DESTINATION include/nlm_monitor
        FILES_MATCHING PATTERN "*.hpp"
    )
    
    install(DIRECTORY monitor/tools/ DESTINATION include/nlm_monitor/tools
        FILES_MATCHING PATTERN "*.hpp"
    )
endif()

# Documentation generation
if(NLM_ENABLE_MONITORS_DOC)
    find_program(SPHINX_BUILD sphinx-build)
    if(SPHINX_BUILD)
        add_custom_target(docs
            COMMAND ${SPHINX_BUILD} -b html ${CMAKE_CURRENT_SOURCE_DIR}/docs ${CMAKE_CURRENT_BINARY_DIR}/docs/html
            COMMENT "Building monitoring documentation"
            VERBATIM
        )
    endif()
endif()

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_monitorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nlm_monitorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_monitorConfig.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm_monitor
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_monitorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nlm_monitorConfigVersion.cmake
    DESTINATION ${LIB_INSTALL_DIR}/cmake/nlm_monitor
)

# Examples
if(NLM_ENABLE_MONITORING)
    # Copy examples to installation
    file(COPY examples/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/examples
        PATTERN "*.py"
    )
    
    install(DIRECTORY examples/ DESTINATION examples/monitor
        FILES_MATCHING PATTERN "*.py"
    )
endif()

# Testing
if(NLM_ENABLE_TESTS)
    enable_testing()
    
    # Add test executables
    add_test(NAME monitor_basic_test
        COMMAND monitor_test
    )
    
    # Add integration tests
    add_test(NAME monitor_integration_test
        COMMAND monitor_integration_test
    )
endif()

# Summary message
message(STATUS "NLM Performance Monitoring Configuration:")
message(STATUS "  Monitoring enabled: ${NLM_ENABLE_MONITORING}")
message(STATUS "  Python bindings: ${NLM_ENABLE_PYTHON_BINDINGS}")
message(STATUS "  Documentation: ${NLM_ENABLE_MONITORS_DOC}")
message(STATUS "  Monitoring type: ${NLM_MONITORING_TYPE}")
message(STATUS "  Library output: libnlm_monitor.${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Create a test build target
add_custom_target(test_monitor
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running monitoring tests"
    VERBATIM
)