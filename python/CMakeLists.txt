cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings VERSION 0.1.0 LANGUAGES CXX)

# Enable modern CMake features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find Python with proper version checking
find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter Development)
if(POLICY_CMP0148)  # CMake 3.24+ policy for better pybind11 integration
    # Use modern Python detection
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE} CACHE FILEPATH "Python interpreter")
    set(PYTHON_INCLUDE_DIR ${Python3_INCLUDE_DIRS} CACHE PATH "Python include directory")
    set(PYTHON_LIBRARIES ${Python3_LIBRARIES} CACHE STRING "Python libraries")
    set(PYTHON_SITEARCH ${Python3_SITEARCH} CACHE PATH "Python site-packages")
else()
    # Fallback for older CMake versions
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
endif()

# Validate Python version
if(PYTHON_VERSION_MAJOR LESS 3 OR 
   (PYTHON_VERSION_MAJOR EQUAL 3 AND PYTHON_VERSION_MINOR LESS 8))
    message(FATAL_ERROR "Python 3.8+ required for NLM bindings. Found ${Python3_VERSION}")
endif()

message(STATUS "Found Python ${Python3_VERSION}")
message(STATUS "Python include dir: ${PYTHON_INCLUDE_DIR}")
message(STATUS "Python libraries: ${PYTHON_LIBRARIES}")

# Find pybind11 with version checking
find_package(pybind11 CONFIG REQUIRED)
if(pybind11_VERSION LESS "2.11.0")
    message(WARNING "pybind11 version ${pybind11_VERSION} may not support all features. Consider upgrading to >= 2.11.0")
endif()

message(STATUS "Found pybind11 ${pybind11_VERSION}")

# Configure build options
option(BUILD_PYTHON_PACKAGE "Build Python package with setup.py" ON)
option(BUILD_TESTS "Build Python binding tests" OFF)
option(BUILD_EXAMPLES "Build Python examples" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(NLM_ENABLE_TESTS "Enable NLM core tests" OFF)

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Core library using modern CMake
set(NLM_CORE_SOURCES
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

# Agent system
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES}
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Core library with better organization
add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Modern CMake target-based configuration
target_compile_features(nlm_core PUBLIC cxx_std_20)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

# Better agent configuration
add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_agent PUBLIC cxx_std_20)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# Better world configuration
add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(nlm_world PUBLIC cxx_std_20)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Create Python bindings target with modern CMake
pybind11_add_module(pynlm bindings.cpp)

# Modern target-based configuration
target_compile_features(pynlm PRIVATE cxx_std_20)

# Better include directories using generator expressions
target_include_directories(pynlm PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Link libraries using target-based approach
target_link_libraries(pynlm PRIVATE
    nlm_core
    nlm_agent  
    nlm_world
    PUBLIC pybind11::module
)

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(pynlm PRIVATE _WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS)
    target_link_options(pynlm PRIVATE /SUBSYSTEM:CONSOLE)
    target_compile_options(pynlm PRIVATE /permissive-)
elseif(APPLE)
    # macOS-specific settings
    target_compile_definitions(pynlm PRIVATE __APPLE__)
    target_link_options(pynlm PRIVATE -framework CoreFoundation)
    target_compile_options(pynlm PRIVATE -stdlib=libc++)
elseif(UNIX)
    # Linux/Unix-specific settings
    target_compile_definitions(pynlm PRIVATE _POSIX_C_SOURCE=200809L _REENTRANT)
    target_link_options(pynlm PRIVATE -pthread)
endif()

# Architecture-specific optimizations
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    target_compile_definitions(pynlm PRIVATE NLM_X64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|AARCH64")
    target_compile_definitions(pynlm PRIVATE NLM_ARM64)
endif()

# Better compile options using generator expressions
target_compile_options(pynlm PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# Add Python-specific definitions
target_compile_definitions(pynlm PRIVATE
    PYBIND11_DETAILED_ERROR_MESSAGES
    PYBIND11_CHECK_PYTHON_VERSION
)

# Include GNUInstallDirs for proper installation paths
include(GNUInstallDirs)

# Installation configuration
install(TARGETS pynlm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/pynlm
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/pynlm
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/pynlm
)

# Install Python package files if requested
if(BUILD_PYTHON_PACKAGE)
    # Create Python package directory
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pynlm
        FILES_MATCHING PATTERN "*.py"
        PATTERN "*.py.in"
        PATTERN "*.md"
        PATTERN "*.txt"
    )
    
    # Install Python module init file
    install(FILES __init__.py
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pynlm
        COMPONENT python_package
    )
    
    # Install main Python module
    install(FILES pynlm.py
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pynlm
        COMPONENT python_package
    )
    
    # Install documentation
    install(FILES README.md
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pynlm
        COMPONENT python_package
    )
endif()

# Add Python package metadata generation if requested
if(BUILD_PYTHON_PACKAGE)
    # Configure pyproject.toml from template
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.template.in
        ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
        @ONLY
    )
    
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pynlm
    )
endif()

# Testing integration
if(BUILD_TESTS)
    enable_testing()
    
    # Add Python bindings tests
    add_test(NAME test_pynlm_basic
        COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/test_pynlm_basic.py -v
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add test discovery for all test files
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.py")
    foreach(test_source ${TEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        # Skip if it's a template or not a test file
        if(NOT test_name MATCHES "\.in$" AND test_name MATCHES "^test")
            add_test(NAME ${test_name}
                COMMAND ${Python3_EXECUTABLE} -m pytest ${test_source} -v
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
        endif()
    endforeach()
    
    # Add test dependency
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
endif()

# Add examples if requested
if(BUILD_EXAMPLES)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
        add_subdirectory(examples)
    else()
        message(STATUS "Examples directory not found, skipping examples build")
    endif()
endif()

# Generate export header for better integration
if(BUILD_PYTHON_PACKAGE)
    # Add custom command to generate export header
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pynlm_export.h
        COMMAND ${Python3_EXECUTABLE} -c "
import sys
sys.path.insert(0, '${CMAKE_CURRENT_SOURCE_DIR}')
from pynlm import __version__
with open('${CMAKE_CURRENT_BINARY_DIR}/pynlm_export.h', 'w') as f:
    f.write(f'// Auto-generated export header for pynlm version {__version__}\\n')
    f.write('// Generated on: ' + __import__('datetime').datetime.now().isoformat() + '\\n')
"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pynlm.py
        COMMENT "Generating pynlm export header"
    )
    
    add_custom_target(generate_export_header DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pynlm_export.h)
endif()

# Add target for testing the installation
add_custom_target(check_python_package
    COMMAND ${Python3_EXECUTABLE} -c "
import sys
sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}')
try:
    import pynlm
    print('Successfully imported pynlm version', pynlm.__version__)
except Exception as e:
    print('ERROR: Failed to import pynlm:', e)
    sys.exit(1)
"
    DEPENDS pynlm
    COMMENT "Check if Python package can be imported"
)
