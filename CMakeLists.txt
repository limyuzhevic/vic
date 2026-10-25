// NLM Advanced Application - CMake Integration

# NLM Advanced Application Build Configuration

project(NLM-Advanced Application VERSION 0.1.0 LANGUAGES CXX)

# Advanced application build configuration
set(ADVANCED_APP_SOURCES
    src/AdvancedApplication/main.cpp
    src/AdvancedFeatures/AdvancedFeatures.cpp
    src/AdvancedFeatures/AdvancedAPI.cpp
    src/brain/BrainIntegrationManager.cpp
)

# Target executable
add_executable(nlm_advanced
    ${ADVANCED_APP_SOURCES}
)

# Link libraries
target_link_libraries(nlm_advanced
    nlm_core
    nlm_agent
    nlm_world
    nlm_phase3
    nlm_phase4
)

# Include directories
target_include_directories(nlm_advanced PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdvancedFeatures
    ${CMAKE_CURRENT_SOURCE_DIR}/src/brain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# Advanced application specific compiler options
target_compile_options(nlm_advanced PRIVATE
    -std=c++20
    -O2
    -Wall
    -Wextra
    -Wpedantic
    -fPIC
)

# Advanced application definitions
target_compile_definitions(nlm_advanced PRIVATE
    NLM_ADVANCED_APP
    NLM_PHASE6_ADVANCED
)

# Installation
install(TARGETS nlm_advanced
    RUNTIME DESTINATION bin
)

# Documentation
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/advanced_app_readme.md.in
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/advanced_app_readme.md
)

# Help and usage information
add_custom_target(advanced_help
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/show_help.sh
    COMMENT "Show NLM Advanced Application help"
    VERBATIM
)
