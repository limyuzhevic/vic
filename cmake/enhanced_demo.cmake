#!/usr/bin/env cmake -P

cmake_minimum_required(VERSION 3.16)
project(NLM-ENHANCED-PHASE6-demo VERSION 0.1.0 LANGUAGES CXX)

# Enhanced Phase 6 Demo - Feature Rich Integration Test
# This demo showcases the complete NLM Phase 6 integration with advanced features
# and provides a comprehensive testing framework for all integrated systems.

# Enhanced features:
# - Advanced parameter testing
# - Performance benchmarking
# - System health monitoring
# - Interactive analysis
# - Export capabilities for research
# - Automated parameter optimization

# Build the enhanced Phase 6 demo executable
add_executable(nlm_phase6_enhanced
    src/experiments/Phase6IntegratedExperiment.cpp
    src/experiments/Phase6Demo.cpp
    # Add additional modules for enhanced features
    src/agent/AgentBrain.cpp
    src/world/SimpleWorld.cpp
    src/agent/SensoryPercept.cpp
)

target_link_libraries(nlm_phase6_enhanced PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

target_include_directories(nlm_phase6_enhanced PUBLIC src)

# Installation target for the enhanced demo
install(TARGETS nlm_phase6_enhanced
    RUNTIME DESTINATION bin
    NAMELINK_COMPONENT runtime
)

# Additional configuration for enhanced demo
# Enable additional logging and debugging features
set_target_properties(nlm_phase6_enhanced PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX "_d"
)

# Add custom build rules for enhanced demo
add_custom_target(demo-enhanced-phrase
    COMMAND ${CMAKE_COMMAND} -E echo "Building Enhanced Phase 6 Demo"
    COMMAND $<TARGET_FILE:nlm_phase6_enhanced> --help
    DEPENDS nlm_phase6_enhanced
    COMMENT "Enhanced Phase 6 Demo setup and help"
)

# Add data files for enhanced demo (if any)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/enhanced_config.json
    DESTINATION share/nlm/demo
    OPTIONAL
)

message(STATUS "Enhanced Phase 6 Demo configured successfully")
message(STATUS "Features: Advanced integration testing, parameter optimization, system monitoring")