# ==============================================================================
# Debugging Build System
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(NLM_Debugging LANGUAGES CXX)

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

# Debugging source files
set(DEBUGGING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryDebugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceDebugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginDebugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationDebugger.cpp
)

# Debugging header files
set(DEBUGGING_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryDebugger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceDebugger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginDebugger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationDebugger.hpp
)

# Create debugging library
add_library(nlm_debugging ${DEBUGGING_SOURCES} ${DEBUGGING_HEADERS})

# Target properties
target_include_directories(nlm_debugging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_debugging PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(nlm_debugging PUBLIC
    Threads::Threads
    CryptoPP::cryptopp
)

# Install target
install(TARGETS nlm_debugging
    EXPORT NLM_Debugging_Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${DEBUGGING_HEADERS}
    DESTINATION include/nlm/debug
)

# Export targets
install(EXPORT NLM_Debugging_Targets
    FILE NLM_Debugging_Targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm_debugging
)

# Create memory debugger target
add_executable(memory_debugger ${CMAKE_CURRENT_SOURCE_DIR}/MemoryDebuggerCLI.cpp)
target_link_libraries(memory_debugger PRIVATE nlm_debugging nlm_memory)
install(TARGETS memory_debugger DESTINATION bin)

# Create performance debugger target
add_executable(performance_debugger ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceDebuggerCLI.cpp)
target_link_libraries(performance_debugger PRIVATE nlm_debugging nlm_performance)
install(TARGETS performance_debugger DESTINATION bin)

# Create plugin debugger target
add_executable(plugin_debugger ${CMAKE_CURRENT_SOURCE_DIR}/PluginDebuggerCLI.cpp)
target_link_libraries(plugin_debugger PRIVATE nlm_debugging nlm_plugin)
install(TARGETS plugin_debugger DESTINATION bin)

# Create configuration debugger target
add_executable(configuration_debugger ${CMAKE_CURRENT_SOURCE_DIR}/ConfigurationDebuggerCLI.cpp)
target_link_libraries(configuration_debugger PRIVATE nlm_debugging nlm_config)
install(TARGETS configuration_debugger DESTINATION bin)

# Package generation
include(GNUInstallDirs)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_DebuggingConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_DebuggingConfig.cmake
    @ONLY
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_DebuggingPkgConfig.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_DebuggingPkgConfig.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_DebuggingConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_DebuggingPkgConfig.pc
    DESTINATION lib/cmake/nlm_debugging
)

# Add dependencies
add_dependencies(nlm_debugging nlm_memory nlm_performance nlm_plugin nlm_config)

# Build options
option(BUILD_DEBUGGING "Build NLM Debugging" ON)
option(INSTALL_DEBUGGING "Install NLM Debugging" ON)
option(INSTALL_DEBUGGING_COMPONENTS "Install Debugging components" ON)

if(NOT BUILD_DEBUGGING)
    add_custom_target(debugging_off)
    add_dependencies(nlm_debugging_off debugging_off)
endif()
