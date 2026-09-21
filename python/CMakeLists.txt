# NLM Python Bindings Module
# This CMakeLists.txt is included from the main CMakeLists.txt
# It creates the pynlm Python extension module by linking against pre-built NLM libraries

cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Set Python module properties
set(PYBIND11_MODULE true)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# The main CMakeLists.txt already builds nlm_core, nlm_agent, and nlm_world
# We just need to link against them and create the Python module

# Create Python module from bindings.cpp
pybind11_add_module(pynlm bindings.cpp)

# Link with NLM libraries (these are already built targets from the parent)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include NLM headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Configure for Python module
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Add this directory to the source path for the pynlm target
# This ensures the bindings.cpp is available
add_custom_target(copy_bindings ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/bindings.cpp ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS nlm_core nlm_agent nlm_world
)

# Set output name to match Python package
target_sources(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings.cpp
)

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)