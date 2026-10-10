cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Include directories for the new bindings structure
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Create the enhanced Python bindings module
pybind11_add_module(pynlm src/nlm_bindings.cpp)

# Link against pybind11 and NLM core libraries
target_link_libraries(pynlm PRIVATE pybind11::module)

# Set target properties
target_compile_features(pynlm PRIVATE cxx_std_20)

# Install the Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Installation of headers (for development)
install(DIRECTORY include/nlm/
    DESTINATION pynlm/include/nlm
    FILES_MATCHING PATTERN "*.hpp"
)
