cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Set Python version requirement
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set the source directory for NLM headers
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Create the Python module
pybind11_add_module(pynlm bindings.cpp)

# Link against NLM libraries (these should be built by the main CMakeLists.txt)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

# Include NLM headers
target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Install the Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
