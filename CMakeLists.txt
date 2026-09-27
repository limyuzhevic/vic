# cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Find Python for bindings
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Core library with simplified source enumeration
file(GLOB_RECURSE CORE_SOURCES "${NLM_SOURCE_DIR}/core/*.cpp")
file(GLOB_RECURSE BRAIN_SOURCES "${NLM_SOURCE_DIR}/brain/*.cpp")
file(GLOB_RECURSE DYNAMICS_SOURCES "${NLM_SOURCE_DIR}/dynamics/*.cpp")
file(GLOB_RECURSE PLASTICITY_SOURCES "${NLM_SOURCE_DIR}/plasticity/*.cpp")
file(GLOB_RECURSE DEVELOPMENT_SOURCES "${NLM_SOURCE_DIR}/development/*.cpp")
file(GLOB_RECURSE NEUROMODULATION_SOURCES "${NLM_SOURCE_DIR}/neuromodulation/*.cpp")
file(GLOB_RECURSE MEMORY_SOURCES "${NLM_SOURCE_DIR}/memory/*.cpp")
file(GLOB_RECURSE COGNITION_SOURCES "${NLM_SOURCE_DIR}/cognition/*.cpp")
file(GLOB_RECURSE SENSORY_SOURCES "${NLM_SOURCE_DIR}/sensory/*.cpp")
file(GLOB_RECURSE PREDICTION_SOURCES "${NLM_SOURCE_DIR}/prediction/*.cpp")
file(GLOB_RECURSE MOTOR_SOURCES "${NLM_SOURCE_DIR}/motor/*.cpp")
file(GLOB_RECURSE ENVIRONMENT_SOURCES "${NLM_SOURCE_DIR}/environment/*.cpp")
file(GLOB_RECURSE EXPERIMENTS_SOURCES "${NLM_SOURCE_DIR}/experiments/*.cpp")
file(GLOB_RECURSE VISUALIZATION_SOURCES "${NLM_SOURCE_DIR}/visualization/*.cpp")

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_core STATIC
    ${CORE_SOURCES}
    ${BRAIN_SOURCES}
    ${DYNAMICS_SOURCES}
    ${PLASTICITY_SOURCES}
    ${DEVELOPMENT_SOURCES}
    ${NEUROMODULATION_SOURCES}
    ${MEMORY_SOURCES}
    ${COGNITION_SOURCES}
    ${SENSORY_SOURCES}
    ${PREDICTION_SOURCES}
    ${MOTOR_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${EXPERIMENTS_SOURCES}
    ${VISUALIZATION_SOURCES}
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Add compiler definitions for better C++20 support
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(nlm_core PRIVATE -std=c++20 -Wall -Wextra -Wpedantic -Werror=vla)
elseif(MSVC)
    target_compile_options(nlm_core PRIVATE /W4 /WX)
endif()

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing
enable_testing()
add_subdirectory(tests)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Set up Python bindings with modern CMake
set(PYBIND11_PYTHON_VERSION 3.8)
set(PYBIND11_DEPS pybind11 scikit-build-core)

# Configure pybind11 bindings
set(PYBIND11_ARGS
    "-DCMAKE_CXX_STANDARD=20"
    "-DPYTHON_EXECUTABLE=${Python3_EXECUTABLE}"
)

# Create Python module
add_custom_target(pynlm ALL
    COMMENT "Building Python module pynlm"
    VERBATIM
)

# Add a target for building documentation
set(DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
if(EXISTS "${DOCS_DIR}/index.rst")
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Building documentation..."
        COMMENT "Building documentation"
    )
endif()