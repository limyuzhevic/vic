# CMakeLists.txt for NLM CLI tool

cmake_minimum_required(VERSION 3.16)

# CLI target
add_executable(nlm-cli
    nlm_cli.cpp
    CLIContext.cpp
    ConfigCommand.cpp
    BrainCommand.cpp
    SimulationCommand.cpp
)

target_include_directories(nlm-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Find required libraries
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(nlohmann_json REQUIRED)
target_link_libraries(nlm-cli PRIVATE nlohmann_json::nlohmann_json)

# Target nlm_core (core library)
target_link_libraries(nlm-cli PRIVATE nlm_core)

# Install target
install(TARGETS nlm-cli DESTINATION bin)
