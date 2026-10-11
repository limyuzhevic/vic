cmake_minimum_required(VERSION 3.16)
project(NLM_EnhancedConfigTest CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Include pybind11
find_package(pybind11 CONFIG REQUIRED)

# Include the core library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Test configuration
add_executable(test_enhanced_config test_enhanced_config.cpp)

# Link with the core libraries
target_link_libraries(test_enhanced_config PRIVATE
    nlm_core
)

# Compiler options
target_compile_options(test_enhanced_config PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Target output
install(TARGETS test_enhanced_config DESTINATION bin)