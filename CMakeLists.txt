cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(WIN32)
    # Windows-specific settings
    add_definitions(-DNLM_WINDOWS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)  # Export symbols for DLL
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    
    # Windows compiler warnings
    add_compile_options(/W4 /WX)
    
    # Disable secure warnings for development
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    
elseif(APPLE)
    # macOS-specific settings
    add_definitions(-DNLM_MACOS)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    
    # macOS compiler warnings
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=unused-function)
    
elseif(UNIX)
    # Linux and other Unix-like systems
    add_definitions(-DNLM_LINUX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    
    # Linux compiler warnings
    add_compile_options(-Wall -Wextra -Wpedantic -Werror=unused-variable)
endif()

# Build type selection
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNLM_DEBUG)
    add_compile_options(-g -O0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNLM_RELEASE)
    add_compile_options(-O3 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions(-DNLM_RELWITHDEBINFO)
    add_compile_options(-O2 -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    add_definitions(-DNLM_MINSIZEREL)
    add_compile_options(-Os)
endif()

# Enable warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library with comprehensive source files
add_library(nlm_core STATIC
    # Core utilities
    ${NLM_SOURCE_DIR}/core/Types/Types.cpp
    ${NLM_SOURCE_DIR}/core/Config/Config.cpp
    ${NLM_SOURCE_DIR}/core/Random/Random.cpp
    ${NLM_SOURCE_DIR}/core/Logger/Logger.cpp
    ${NLM_SOURCE_DIR}/core/SimulationClock/SimulationClock.cpp
    
    # Brain components
    ${NLM_SOURCE_DIR}/brain/Neuron.cpp
    ${NLM_SOURCE_DIR}/brain/Synapse.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralPopulation.cpp
    ${NLM_SOURCE_DIR}/brain/NeuralRegion.cpp
    ${NLM_SOURCE_DIR}/brain/Brain.cpp
    
    # Neural dynamics
    ${NLM_SOURCE_DIR}/dynamics/NeuralDynamics.cpp
    ${NLM_SOURCE_DIR}/dynamics/SpikeSystem.cpp
    
    # Plasticity rules
    ${NLM_SOURCE_DIR}/plasticity/PlasticityRule.cpp
    ${NLM_SOURCE_DIR}/plasticity/Hebbian.cpp
    ${NLM_SOURCE_DIR}/plasticity/STDP.cpp
    ${NLM_SOURCE_DIR}/plasticity/StructuralPlasticity.cpp
    
    # Development system
    ${NLM_SOURCE_DIR}/development/DevelopmentSystem.cpp
    ${NLM_SOURCE_DIR}/development/Synaptogenesis.cpp
    ${NLM_SOURCE_DIR}/development/Pruning.cpp
    ${NLM_SOURCE_DIR}/development/Maturation.cpp
    
    # Neuromodulation
    ${NLM_SOURCE_DIR}/neuromodulation/Neuromodulator.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Reward.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/PredictionError.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Novelty.cpp
    ${NLM_SOURCE_DIR}/neuromodulation/Curiosity.cpp
    
    # Memory systems
    ${NLM_SOURCE_DIR}/memory/Memory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralWorkingMemory.cpp
    ${NLM_SOURCE_DIR}/memory/NeuralEpisodicMemory.cpp
    ${NLM_SOURCE_DIR}/performance/CheckpointSystem.cpp
    
    # Cognition
    ${NLM_SOURCE_DIR}/cognition/ConceptFormation.cpp
    ${NLM_SOURCE_DIR}/cognition/NeuralPlanner.cpp
    
    # Sensory processing
    ${NLM_SOURCE_DIR}/sensory/SensoryInput.cpp
    ${NLM_SOURCE_DIR}/sensory/Vision.cpp
    ${NLM_SOURCE_DIR}/sensory/Audio.cpp
    ${NLM_SOURCE_DIR}/sensory/InternalSignals.cpp
    
    # Prediction system
    ${NLM_SOURCE_DIR}/prediction/PredictionSystem.cpp
    ${NLM_SOURCE_DIR}/prediction/NeuralPrediction.cpp
    
    # Motor system
    ${NLM_SOURCE_DIR}/motor/MotorSystem.cpp
    ${NLM_SOURCE_DIR}/motor/Action.cpp
    
    # Environment interface
    ${NLM_SOURCE_DIR}/environment/Environment.cpp
    ${NLM_SOURCE_DIR}/environment/Observation.cpp
    
    # Experiments
    ${NLM_SOURCE_DIR}/experiments/Experiment.cpp
    ${NLM_SOURCE_DIR}/experiments/ExperimentRunner.cpp
    ${NLM_SOURCE_DIR}/experiments/Metrics.cpp
    
    # Visualization
    ${NLM_SOURCE_DIR}/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase3Experiment.cpp
)

target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase4Experiment.cpp
)

target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase5Experiment.cpp
)

target_include_directories(nlm_phase5 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    ${NLM_SOURCE_DIR}/experiments/Phase6IntegratedExperiment.cpp
)

target_include_directories(nlm_phase6 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)

# Main executable
add_executable(nlm
    ${NLM_SOURCE_DIR}/main.cpp
)

target_link_libraries(nlm PUBLIC nlm_core)
set_target_properties(nlm PROPERTIES
    OUTPUT_NAME "nlm"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    ${NLM_SOURCE_DIR}/experiments/Phase3Demo.cpp
)

target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
set_target_properties(nlm_phase3_demo PROPERTIES
    OUTPUT_NAME "nlm_phase3_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    ${NLM_SOURCE_DIR}/experiments/Phase4Demo.cpp
)

target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
set_target_properties(nlm_phase4_demo PROPERTIES
    OUTPUT_NAME "nlm_phase4_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 5 demo executable
add_executable(nlm_phase5_demo
    ${NLM_SOURCE_DIR}/experiments/Phase5Demo.cpp
)

target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5)
set_target_properties(nlm_phase5_demo PROPERTIES
    OUTPUT_NAME "nlm_phase5_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    ${NLM_SOURCE_DIR}/experiments/Phase6Demo.cpp
)

target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)
set_target_properties(nlm_phase6_demo PROPERTIES
    OUTPUT_NAME "nlm_phase6_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Phase 6 text demo (new in Phase 6)
add_executable(nlm_phase6_text_demo
    ${NLM_SOURCE_DIR}/experiments/Phase6TextDemo.cpp
)

target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)
set_target_properties(nlm_phase6_text_demo PROPERTIES
    OUTPUT_NAME "nlm_phase6_text_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Benchmark executable
add_executable(nlm_benchmark
    ${NLM_SOURCE_DIR}/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core)
set_target_properties(nlm_benchmark PROPERTIES
    OUTPUT_NAME "nlm_benchmark"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Python bindings (Phase 5: this is what `pip install .` builds).
# Previously pyproject.toml claimed scikit-build-core would build pynlm,
# but the root CMakeLists never built the module — pip install
# succeeded yet `import pynlm` failed. The option is OFF by default so
# a plain C++ build needs no Python/pybind11; pyproject.toml turns it
# ON. The standalone python/CMakeLists.txt build still works too, but
# it must not be added here as a subdirectory: it defines its own
# nlm_core/nlm_agent/nlm_world targets, which would collide with the
# ones above. This target reuses the root libraries instead.
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" OFF)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    
    # Set output directories for Python module
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
    
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})
    
    # Set Python module output directory
    set_target_properties(pynlm PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
        SUFFIX "${PYTHON_MODULE_EXTENSION}"
    )
    
    # Install Python module
    install(TARGETS pynlm DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
endif()

# Testing
enable_testing()
add_subdirectory(tests)

# Installation
include(GNUInstallDirs)

# Install core library
install(TARGETS nlm_core
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install agent library
install(TARGETS nlm_agent
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install world library
install(TARGETS nlm_world
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install phase libraries
install(TARGETS nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install executables
install(TARGETS nlm nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_phase6_text_demo nlm_benchmark
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers
install(DIRECTORY ${NLM_SOURCE_DIR}/core DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/brain DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/dynamics DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/plasticity DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/development DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/neuromodulation DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/memory DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/cognition DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/sensory DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/prediction DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/motor DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/environment DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/agent DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/world DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/experiments DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)
install(DIRECTORY ${NLM_SOURCE_DIR}/visualization DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nlm)

# Package configuration for find_package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/nlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nlmConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nlm
)