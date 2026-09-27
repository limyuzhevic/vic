# ==============================================================================
# Debug CLI Build System
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(NLM_Debug_CLI LANGUAGES CXX)

# Find required packages
find_package(Threads REQUIRED)
find_package(CryptoPP REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Debug CLI source files
set(DEBUG_CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugCLI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugCommands.cpp
)

# Debug CLI header files
set(DEBUG_CLI_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugCLI.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugCommands.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugHelpers.hpp
)

# Create debug CLI library
add_library(nlm_debug_cli ${DEBUG_CLI_SOURCES} ${DEBUG_CLI_HEADERS})

# Target properties
target_include_directories(nlm_debug_cli PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_debug_cli PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(nlm_debug_cli PUBLIC
    Threads::Threads
    CryptoPP::cryptopp
)

# Install target
install(TARGETS nlm_debug_cli
    EXPORT NLM_Debug_CLI_Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${DEBUG_CLI_HEADERS}
    DESTINATION include/nlm/debug
)

# Export targets
install(EXPORT NLM_Debug_CLI_Targets
    FILE NLM_Debug_CLI_Targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm_debug_cli
)

# Create configuration wizard target
add_executable(configuration_wizard ${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationWizard.cpp)
target_link_libraries(configuration_wizard PRIVATE nlm_debug_cli)
install(TARGETS configuration_wizard DESTINATION bin)

# Create experiment manager target
add_executable(experiment_manager ${CMAKE_CURRENT_SOURCE_DIR}/ExperimentManager.cpp)
target_link_libraries(experiment_manager PRIVATE nlm_debug_cli)
install(TARGETS experiment_manager DESTINATION bin)

# Create performance dashboard target
add_executable(performance_dashboard ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceDashboard.cpp)
target_link_libraries(performance_dashboard PRIVATE nlm_debug_cli)
install(TARGETS performance_dashboard DESTINATION bin)

# Package generation
include(GNUInstallDirs)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_Debug_CLIConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_Debug_CLIConfig.cmake
    @ONLY
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_Debug_CLIPkgConfig.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_Debug_CLIPkgConfig.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_Debug_CLIConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_Debug_CLIPkgConfig.pc
    DESTINATION lib/cmake/nlm_debug_cli
)

# Add dependencies
add_dependencies(nlm_debug_cli nlm_config nlm_performance)

# Build options
option(BUILD_DEBUG_CLI "Build NLM Debug CLI" ON)
option(INSTALL_DEBUG_CLI "Install NLM Debug CLI" ON)
option(INSTALL_DEBUG_CLI_COMPONENTS "Install Debug CLI components" ON)

if(NOT BUILD_DEBUG_CLI)
    add_custom_target(debug_cli_off)
    add_dependencies(nlm_debug_cli_off debug_cli_off)
endif()
