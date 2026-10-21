# CMake configuration for NLM Command-line Tools
# This file defines the standalone executables for NLM utilities

cmake_minimum_required(VERSION 3.16)
project(nlm_tools VERSION 0.1.0 LANGUAGES CXX)

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

# Include directories for NLM tools
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/brain)

# Basic Tools

# nlm-config executable
add_executable(nlm-config
    basic/nlm-config.cpp
)

target_link_libraries(nlm-config
    PUBLIC
    nlm_core
)

# nlm-validate executable  
add_executable(nlm-validate
    basic/nlm-validate.cpp
)

target_link_libraries(nlm-validate
    PUBLIC
    nlm_core
)

# nlm-info executable
add_executable(nlm-info
    basic/nlm-info.cpp
)

target_link_libraries(nlm-info
    PUBLIC
    nlm_core
)

# nlm-benchmark executable
add_executable(nlm-benchmark
    basic/nlm-benchmark.cpp
)

target_link_libraries(nlm-benchmark
    PUBLIC
    nlm_core
)

# nlm-save executable
add_executable(nlm-save
    basic/nlm-save.cpp
)

target_link_libraries(nlm-save
    PUBLIC
    nlm_core
)

# nlm-load executable
add_executable(nlm-load
    basic/nlm-load.cpp
)

target_link_libraries(nlm-load
    PUBLIC
    nlm_core
)

# Advanced Tools

# nlm-explore executable
add_executable(nlm-explore
    advanced/nlm-explore.cpp
)

target_link_libraries(nlm-explore
    PUBLIC
    nlm_core
)

# nlm-plot executable
add_executable(nlm-plot
    advanced/nlm-plot.cpp
)

target_link_libraries(nlm-plot
    PUBLIC
    nlm_core
)

# nlm-compare executable
add_executable(nlm-compare
    advanced/nlm-compare.cpp
)

target_link_libraries(nlm-compare
    PUBLIC
    nlm_core
)

# nlm-analyze executable
add_executable(nlm-analyze
    advanced/nlm-analyze.cpp
)

target_link_libraries(nlm-analyze
    PUBLIC
    nlm_core
)

# nlm-export executable
add_executable(nlm-export
    advanced/nlm-export.cpp
)

target_link_libraries(nlm-export
    PUBLIC
    nlm_core
)

# nlm-import executable
add_executable(nlm-import
    advanced/nlm-import.cpp
)

target_link_libraries(nlm-import
    PUBLIC
    nlm_core
)

# Development Tools

# nlm-profiler executable
add_executable(nlm-profiler
    development/nlm-profiler.cpp
)

target_link_libraries(nlm-profiler
    PUBLIC
    nlm_core
)

# nlm-monitor executable
add_executable(nlm-monitor
    development/nlm-monitor.cpp
)

target_link_libraries(nlm-monitor
    PUBLIC
    nlm_core
)

# nlm-test executable
add_executable(nlm-test
    development/nlm-test.cpp
)

target_link_libraries(nlm-test
    PUBLIC
    nlm_core
)

# nlm-setup executable
add_executable(nlm-setup
    development/nlm-setup.cpp
)

target_link_libraries(nlm-setup
    PUBLIC
    nlm_core
)

# nlm-help executable
add_executable(nlm-help
    development/nlm-help.cpp
)

target_link_libraries(nlm-help
    PUBLIC
    nlm_core
)

# Installation targets

install(TARGETS nlm-config nlm-validate nlm-info nlm-benchmark nlm-save nlm-load
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(TARGETS nlm-explore nlm-plot nlm-compare nlm-analyze nlm-export nlm-import
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(TARGETS nlm-profiler nlm-monitor nlm-test nlm-setup nlm-help
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Documentation

install(FILES README.md DESTINATION share/doc/nlm_tools)

# Generate configuration templates

# Create templates directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templates)

# Template for nlm-config
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/templates/nlm-config.cfg
"# NLM Configuration Template\n"
"# \n"
"# This template includes all available configuration options.\n"
"# Modify values as needed for your specific use case.\n"
"\n"
"# Basic settings\n"
"random_seed = 42\n"
"simulation_timestep = 0.001\n"
"neuron_count = 1000\n"
"region_count = 2\n"
"connection_probability = 0.1\n"
"\n"
"# Plasticity settings\n"
"plasticity_learning_rate = 0.01\n"
"stdp_ltp_weight = 0.01\n"
"stdp_ltd_weight = 0.012\n"
"stdp_time_constant = 20.0\n"
"\n"
"# Development settings\n"
"development_synaptogenesis_rate = 0.001\n"
"development_pruning_rate = 0.0001\n"
"\n"
"# Neuromodulation settings\n"
"dopamine_baseline = 0.1\n"
"\n"
"# Reward settings\n"
"reward_discount_factor = 0.99\n"
"\n"
"# Environment settings\n"
"environment_name = GridWorld\n"
"environment_width = 10\n"
"environment_height = 10\n"
"\n"
"# Logging settings\n"
"log_level = INFO\n"
"log_to_file = false\n"
"log_filename = nlm.log\n"
"\n"
"# Simulation settings\n"
"max_simulation_steps = 10000\n"
"simulation_time_limit = 0.0\n"
"\n"
"# Visualization settings\n"
"visualization_enabled = false\n"
"visualization_update_rate = 30.0\n"
)

# Installation of templates
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/templates/nlm-config.cfg
    DESTINATION share/nlm/templates
)

# Help documentation
add_custom_target(help_docs
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_help.cmake
    COMMENT "Generating help documentation"
)

# Add dependencies for build
add_dependencies(help_docs nlm-config nlm-validate nlm-info nlm-benchmark nlm-save nlm-load)

# Summary
message(STATUS "NLM Tools configured successfully")
message(STATUS "Tools available:")
message(STATUS "  Basic: nlm-config, nlm-validate, nlm-info, nlm-benchmark, nlm-save, nlm-load")
message(STATUS "  Advanced: nlm-explore, nlm-plot, nlm-compare, nlm-analyze, nlm-export, nlm-import")
message(STATUS "  Development: nlm-profiler, nlm-monitor, nlm-test, nlm-setup, nlm-help")
message(STATUS "  Installation: executables will be installed to bin/ directory")
