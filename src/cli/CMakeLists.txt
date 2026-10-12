# CLI CMakeLists.txt - Build configuration for NLM CLI

# Find CLI11 library (simplified - assumes it's available)
find_package(CLI11 REQUIRED)

# Include nlohmann/json
find_package(nlohmann_json REQUIRED)

# Add CLI executable
add_executable(nlm_cli
    nlm_cli.cpp
    cli_utils.cpp
)

target_include_directories(nlm_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Find dependencies and link
find_package(yaml-cpp REQUIRED)
find_package(pybind11 REQUIRED)

target_link_libraries(nlm_cli PRIVATE
    nlm_core
    nlohmann_json::nlohmann_json
    yaml-cpp
    pybind11::module
)

# Install CLI executable
install(TARGETS nlm_cli
    RUNTIME DESTINATION bin
)

# CLI entry point configuration
set_target_properties(nlm_cli PROPERTIES
    OUTPUT_NAME "nlm"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)