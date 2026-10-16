cmake_minimum_required(VERSION 3.16)

project(nlm_unified_cli VERSION 1.0.0 LANGUAGES CXX)

# Unified CLI sources

set(UNIFIED_CLI_SOURCES
    main.cpp
    benchmark.cpp
)

# Add unified CLI executable
add_executable(nlm_unified_cli ${UNIFIED_CLI_SOURCES})

target_include_directories(nlm_unified_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(nlm_unified_cli PUBLIC
    nlm_core
    nlm_brain
    nlm_memory
    nlm_prediction
    nlm_cognition
    nlm_neuromodulation
    nlm_development
    nlm_agent
    nlm_world
    nlm_phase
)

# Install the unified CLI
install(TARGETS nlm_unified_cli
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
