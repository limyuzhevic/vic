"""CMake configuration for pynlm Python module."""

cmake_minimum_required(VERSION 3.16)
project(pynlm VERSION 0.1.0 LANGUAGES CXX)

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add pybind11 via find_package (expects pybind11 to be installed)
find_package(pybind11 REQUIRED)

# Define module
pybind11_add_module(pynlm_module
    "src/core.cpp"
    "src/brain.cpp"
    "src/world.cpp"
    "src/agent.cpp"
    "src/sensory.cpp"
    "src/motor.cpp"
    "src/neuromodulation.cpp"
)

# Set target properties
set_target_properties(pynlm_module PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# Include directories
target_include_directories(pynlm_module PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(pynlm_module PRIVATE
    pybind11::module
)

# Install target
install(TARGETS pynlm_module
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install Python package
install(DIRECTORY pynlm/ DESTINATION lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages/pynlm
    FILES_MATCHING PATTERN "*.py"
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
)