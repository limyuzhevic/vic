# ==============================================================================
# Integration Build System
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(NLM_Integration LANGUAGES CXX)

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

# Integration source files
set(INTEGRATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPythonInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPluginInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPerformanceInterface.cpp
)

# Integration header files
set(INTEGRATION_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPythonInterface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPluginInterface.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugPerformanceInterface.hpp
)

# Create integration library
add_library(nlm_integration ${INTEGRATION_SOURCES} ${INTEGRATION_HEADERS})

# Target properties
target_include_directories(nlm_integration PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_integration PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(nlm_integration PUBLIC
    Threads::Threads
    CryptoPP::cryptopp
)

# Install target
install(TARGETS nlm_integration
    EXPORT NLM_Integration_Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${INTEGRATION_HEADERS}
    DESTINATION include/nlm/debug
)

# Export targets
install(EXPORT NLM_Integration_Targets
    FILE NLM_Integration_Targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm_integration
)

# Create Python interface target
add_executable(python_interface ${CMAKE_CURRENT_SOURCE_DIR}/DebugPythonInterfaceCLI.cpp)
target_link_libraries(python_interface PRIVATE nlm_integration nlm_python)
install(TARGETS python_interface DESTINATION bin)

# Create plugin interface target
add_executable(plugin_interface ${CMAKE_CURRENT_SOURCE_DIR}/DebugPluginInterfaceCLI.cpp)
target_link_libraries(plugin_interface PRIVATE nlm_integration nlm_plugin)
install(TARGETS plugin_interface DESTINATION bin)

# Create performance interface target
add_executable(performance_interface ${CMAKE_CURRENT_SOURCE_DIR}/DebugPerformanceInterfaceCLI.cpp)
target_link_libraries(performance_interface PRIVATE nlm_integration nlm_performance)
install(TARGETS performance_interface DESTINATION bin)

# Package generation
include(GNUInstallDirs)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_IntegrationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_IntegrationConfig.cmake
    @ONLY
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_IntegrationPkgConfig.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_IntegrationPkgConfig.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_IntegrationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_IntegrationPkgConfig.pc
    DESTINATION lib/cmake/nlm_integration
)

# Add dependencies
add_dependencies(nlm_integration nlm_python nlm_plugin nlm_performance)

# Build options
option(BUILD_INTEGRATION "Build NLM Integration" ON)
option(INSTALL_INTEGRATION "Install NLM Integration" ON)
option(INSTALL_INTEGRATION_COMPONENTS "Install Integration components" ON)

if(NOT BUILD_INTEGRATION)
    add_custom_target(integration_off)
    add_dependencies(nlm_integration_off integration_off)
endif()
