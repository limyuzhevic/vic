# CMake configuration for NLM Command-Line Tools
# Advanced command-line interface for NLM (熙然) neural brain simulation

cmake_minimum_required(VERSION 3.16)
project(nlm_cmd VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files for command-line tool
add_executable(nlm_cmd
    tools/nlm_cmd.cpp
)

# Include directories
target_include_directories(nlm_cmd PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
)

# Link against NLM core libraries
target_link_libraries(nlm_cmd
    nlm_core
    nlm_agent
    nlm_world
)

# Add dependencies
target_include_directories(nlm_cmd PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Installation
install(TARGETS nlm_cmd
    RUNTIME DESTINATION bin
)

# Add tests for command-line tool
enable_testing()

# Add simple test for command-line help
add_test(NAME nlm_cmd_help
    COMMAND nlm_cmd --help
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add test for version
add_test(NAME nlm_cmd_version
    COMMAND nlm_cmd --version
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Create a test for valid arguments
add_test(NAME nlm_cmd_valid_args
    COMMAND nlm_cmd --experiment basic --steps 10 --quiet
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)