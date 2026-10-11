cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Enhanced Python bindings source files
set(ENHANCED_PYTHON_SOURCES
    bindings.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedMain.cpp
)

# Enhanced core classes
file(GLOB ENHANCED_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/ConfigObject.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedBrain.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedWorld.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedTrainingAPI.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedSimulationAPI.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedAnalysisAPI.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedVisualizationAPI.cpp
    ${NLM_SOURCE_DIR}/core/EnhancedExportImportAPI.cpp
)

# Original Python bindings source files
set(ORIGINAL_PYTHON_SOURCES
    bindings.cpp
)

# Create enhanced Python module
pybind11_add_module(nlm_enhanced_enhanced_bindings ${ENHANCED_PYTHON_SOURCES} ${ENHANCED_CORE_SOURCES})

set_target_properties(nlm_enhanced_enhanced_bindings PROPERTIES
    OUTPUT_NAME "nlm_enhanced"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(nlm_enhanced_enhanced_bindings PRIVATE
    ${NLM_SOURCE_DIR}
    ${NLM_SOURCE_DIR}/core
    pybind11/include
)

target_link_libraries(nlm_enhanced_enhanced_bindings PRIVATE
    pybind11::module
)

# Create original Python module
pybind11_add_module(pynlm_original ${ORIGINAL_PYTHON_SOURCES})

set_target_properties(pynlm_original PROPERTIES
    OUTPUT_NAME "pynlm"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(pynlm_original PRIVATE
    ${NLM_SOURCE_DIR}
    pybind11/include
)

target_link_libraries(pynlm_original PRIVATE
    pybind11::module
)

# Python package installation
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
    DESTINATION pynlm_enhanced
)

# Install enhanced Python module
install(TARGETS nlm_enhanced_enhanced_bindings
    LIBRARY DESTINATION pynlm_enhanced
    ARCHIVE DESTINATION pynlm_enhanced
    RUNTIME DESTINATION pynlm_enhanced
)

# Install original Python module
install(TARGETS pynlm_original
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Copy examples directory
file(GLOB PYTHON_EXAMPLE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.py)
install(FILES ${PYTHON_EXAMPLE_FILES}
    DESTINATION pynlm_enhanced/examples
)

# Configure Python module version
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/__init__.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
    @ONLY
)

# Add examples target
add_custom_target(run_examples
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../examples/example1_basic.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../examples/example2_events.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../examples/example3_training.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../examples/example4_simulation.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../examples/example5_visualization.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
    COMMENT "Running enhanced Python examples"
    VERBATIM
)
