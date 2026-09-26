# Performance Profiling CMake Configuration
# Build system for NLM performance profiling tools

cmake_minimum_required(VERSION 3.10)
project(NLM-PerformanceProfiler LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Performance profiling options
option(NLM_PERFORMANCE_PROFILER_ENABLED "Enable performance profiler" ON)
option(NLM_PERFORMANCE_WEB_INTERFACE "Enable web interface for performance reports" ON)
option(NLM_PERFORMANCE_CLI_INTEGRATION "Integrate performance CLI with Advanced CLI" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../brain)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cli)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dynamics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../plasticity)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../memory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../performance)

# Performance profiler source files
set(PERFORMANCE_PROFILER_SRC
    PerformanceProfiler.cpp
)

# Performance CLI source files
set(PERFORMANCE_CLI_SRC
    PerformanceProfilerCLI.cpp
)

# Advanced CLI with performance integration
set(ADVANCED_CLI_PERF_SRC
    AdvancedCLIWithPerformance.cpp
)

# Performance reporting tools
set(PERFORMANCE_REPORTING_SRC
    # Performance reporting tools can be added here
)

# Create performance profiler library
if(NLM_PERFORMANCE_PROFILER_ENABLED)
    add_library(nlm_performance_profiler ${PERFORMANCE_PROFILER_SRC})
    target_include_directories(nlm_performance_profiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(nlm_performance_profiler nlm_brain nlm_core)
    set_target_properties(nlm_performance_profiler PROPERTIES
        OUTPUT_NAME "nlm_performance_profiler"
        VERSION "1.0.0"
        SOVERSION "1"
    )
endif()

# Create performance CLI tool
if(NLM_PERFORMANCE_PROFILER_ENABLED AND NLM_PERFORMANCE_CLI_INTEGRATION)
    add_executable(nlm_performance_cli ${PERFORMANCE_CLI_SRC})
    target_link_libraries(nlm_performance_cli nlm_performance_profiler nlm_cli)
    set_target_properties(nlm_performance_cli PROPERTIES
        OUTPUT_NAME "nlm_performance_cli"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Create advanced CLI with performance integration
if(NLM_PERFORMANCE_PROFILER_ENABLED AND NLM_PERFORMANCE_CLI_INTEGRATION)
    add_executable(nlm_advanced_cli_with_perf ${ADVANCED_CLI_PERF_SRC})
    target_link_libraries(nlm_advanced_cli_with_perf nlm_performance_profiler nlm_advanced_cli)
    set_target_properties(nlm_advanced_cli_with_perf PROPERTIES
        OUTPUT_NAME "nlm_advanced_cli_with_perf"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Performance tools executables
if(NLM_PERFORMANCE_PROFILER_ENABLED)
    # Performance monitoring tool
    add_executable(nlm_perf_monitor PerformanceProfiler.cpp)
    target_link_libraries(nlm_perf_monitor nlm_performance_profiler nlm_brain)
    set_target_properties(nlm_perf_monitor PROPERTIES
        OUTPUT_NAME "nlm_perf_monitor"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Performance benchmarking tool
    add_executable(nlm_perf_benchmark PerformanceProfiler.cpp)
    target_link_libraries(nlm_perf_benchmark nlm_performance_profiler nlm_brain)
    set_target_properties(nlm_perf_benchmark PROPERTIES
        OUTPUT_NAME "nlm_perf_benchmark"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Installation
install(TARGETS nlm_performance_profiler
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(NLM_PERFORMANCE_PROFILER_ENABLED AND NLM_PERFORMANCE_CLI_INTEGRATION)
    install(TARGETS nlm_performance_cli nlm_advanced_cli_with_perf
        RUNTIME DESTINATION bin
    )
endif()

# Performance data directory
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/performance_data
    DESTINATION share/nlm/performance_data
    FILES_MATCHING PATTERN "*.json" PATTERN "*.csv" PATTERN "*.html"
)

# Find packages
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Dependencies
target_link_libraries(nlm_performance_profiler
    PRIVATE
    nlohmann_json::nlohmann_json
    ${CURL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    Threads::Threads
)

# Performance configurations
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/performance_config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/performance_config.h
    @ONLY
)

# Summary
message(STATUS "NLM Performance Profiler Configuration:")
message(STATUS "  Performance Profiler: ${NLM_PERFORMANCE_PROFILER_ENABLED}")
message(STATUS "  CLI Integration: ${NLM_PERFORMANCE_CLI_INTEGRATION}")
message(STATUS "  Web Interface: ${NLM_PERFORMANCE_WEB_INTERFACE}")
message(STATUS "  Output Directory: ${CMAKE_CURRENT_SOURCE_DIR}/performance_data")
message(STATUS "  Build Directory: ${CMAKE_BINARY_DIR}/bin")