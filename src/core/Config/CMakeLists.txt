# High-level CMake configuration for NLM - includes external dependencies

# Try to find nlohmann_json - prefer package manager, then CMake FetchContent
cmake_minimum_required(VERSION 3.16)

# Find nlohmann_json if available
find_package(nlohmann_json QUIET)
if (NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, using simple config parser")
    set(NLM_USE_SIMPLE_CONFIG_PARSER TRUE)
else()
    message(STATUS "Using nlohmann_json for configuration parsing")
    set(NLM_USE_SIMPLE_CONFIG_PARSER FALSE)
endif()

# Configure NLM core with enhanced config support
add_library(nlm_core_enhanced STATIC
    src/core/Config/Config.cpp
)

# Add config-related source files for JSON/YAML support
if (NOT NLM_USE_SIMPLE_CONFIG_PARSER)
    target_sources(nlm_core_enhanced PRIVATE
        src/core/Config/ConfigJSON.cpp
        src/core/Config/ConfigYAML.cpp
    )
endif()

# Add target-specific include directories
target_include_directories(nlm_core_enhanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Link to required libraries
target_link_libraries(nlm_core_enhanced PUBLIC
    nlm_core
)