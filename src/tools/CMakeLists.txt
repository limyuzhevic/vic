# CMake configuration for NLM tools
cmake_minimum_required(VERSION 3.16)
project(nlm_tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Source files for the config tool
set(TOOLS_SOURCES
    src/tools/config_tool.cpp
)

# Include directories
target_include_directories(nlm_tools PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Compiler options
target_compile_options(nlm_tools PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Link dependencies
find_package(nlm REQUIRED)

# Create the config tool executable
add_executable(config_tool ${TOOLS_SOURCES})
target_link_libraries(config_tool PRIVATE nlm_core)

# Install the config tool
install(TARGETS config_tool
    RUNTIME DESTINATION bin
)

# Create a convenience alias
target_link_libraries(config_tool nlm_core)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)