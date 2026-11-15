cmake_minimum_required(VERSION 3.16)
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

# Advanced build options
option(NLM_BUILD_TESTS "Build NLM test suite" ON)
option(NLM_BUILD_EXAMPLES "Build example executables" ON)
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" OFF)
option(NLM_BUILD_DOCUMENTATION "Build documentation" OFF)
option(NLM_ENABLE_COVERAGE "Enable code coverage reporting" OFF)
option(NLM_ENABLE_ADDRESS_SANITIZER "Enable AddressSanitizer for memory debugging" OFF)
option(NLM_ENABLE_UNDEFINED_SANITIZER "Enable UndefinedSanitizer for runtime safety" OFF)
option(NLM_ENABLE_THREAD_SANITIZER "Enable ThreadSanitizer for thread safety analysis" OFF)
option(NLM_ENABLE_VALGRIND "Enable Valgrind instrumentation" OFF)
option(NLM_BUILD_SCALE_BENCHMARKS "Build scaling benchmark suite" ON)
option(NLM_BUILD_EXPERIMENTAL_FEATURES "Build experimental features" OFF)

# Advanced optimization options
option(NLM_OPTIMIZE_FOR_PERFORMANCE "Optimize for maximum performance (disable debugging features)" ON)
option(NLM_OPTIMIZE_FOR_SPEED "Optimize for compilation speed (aggressive precompilation)" OFF)
option(NLM_ENABLE_NUMERIC_OPTIMIZATIONS "Enable numeric optimization (SIMD, vectorization)" ON)
option(NLM_ENABLE_MEMORY_POOLS "Enable memory pooling for better performance" ON)
option(NLM_ENABLE_EVENT_DRIVEN "Use event-driven architecture for better performance" ON)

# Hardware-specific optimizations
option(NLM_TARGET_X86_64 "Optimize for x86_,每个架构 (enabled by default)" ON)
option(NLM_TARGET_ARM64 "Optimize for ARM64" OFF)
option(NLM_TARGET_GPU "Target GPU acceleration where possible" OFF)

# Performance monitoring options
set(NLM_PERF_MON_SAMPLE_INTERVAL "Performance monitoring sample interval (in seconds)" 0.1)
set(NLM_PERF_MON_MAX_STEPS "Maximum steps for real-time monitoring" 10000)
set(NLM_PERF_MON_ENABLE_REALTIME "Enable real-time performance monitoring" OFF)

# Experimental feature options
if(NLM_BUILD_EXPERIMENTAL_FEATURES)
    add_subdirectory(src/experimental)
endif()

# Source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/Memory.cpp
    src/performance/CheckpointSystem.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/motor/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

# Core library
add_library(nlm_core STATIC
    # Core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    # Brain
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    # Dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    # Plasticity
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    # Development
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    # Memory
    src/memory/Memory.cpp
    src/performance/CheckpointSystem.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    # Cognition
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    # Sensory
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    # Prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    # Motor
    src/motor/MotorSystem.cpp
    # Environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/motor/Action.cpp
    # Experiments
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    # Visualization
    src/visualization/VisualizationInterface.cpp
)

target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

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
target_link_libraries(nlm_world PUBLIC nlm_core nlm_agent)

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
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 5 experiments
add_library(nlm_phase5 STATIC
    src/experiments/Phase5Experiment.cpp
)
target_include_directories(nlm_phase5 PUBLIC src)
target_link_libraries(nlm_phase5 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 6 experiments
add_library(nlm_phase6 STATIC
    src/experiments/Phase6IntegratedExperiment.cpp
)
target_include_directories(nlm_phase6 PUBLIC src)
target_link_libraries(nlm_phase6 PUBLIC nlm_core nlm_agent nlm_world)

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

# Phase 5 demo executable (Phase 5, added here: pattern probe,
# sandbox run, and checkpoint round-trip for the brain-personality
# step — previously Phase 5 was library-only with no demo).
add_executable(nlm_phase5_demo
    src/experiments/Phase5Demo.cpp
)
target_link_libraries(nlm_phase5_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5)

# Phase 6 demo executable
add_executable(nlm_phase6_demo
    src/experiments/Phase6Demo.cpp
)
target_link_libraries(nlm_phase6_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase6)

# Phase 6 text/thought demo (new in Phase 6): text-as-a-sense
# association probe, concept/social probes, and a sandbox run with
# a teacher, planner overrides, and a checkpoint round-trip. The old
# nlm_phase6_demo integration demo is left unchanged.
add_executable(nlm_phase6_text_demo
    src/experiments/Phase6TextDemo.cpp
)
target_link_libraries(nlm_phase6_text_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase5 nlm_phase6)

# Benchmark executable
add_executable(nlm_benchmark
    src/benchmark.cpp
)
target_link_libraries(nlm_benchmark PUBLIC nlm_core)

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
    
    # Advanced Python binding options
    option(NLM_PYTHON_BINDINGS_OPTIMIZED "Enable optimized Python bindings (faster startup, more features)" ON)
    option(NLM_PYTHON_BINDINGS_DEBUG "Enable debug features in Python bindings" OFF)
    option(NLM_PYTHON_BINDINGS_EXPERIMENTAL "Enable experimental Python features" OFF)
    option(NLM_PYTHON_BINDINGS_DOCUMENTATION "Generate Python binding API documentation" OFF)
    
    # Sanitizer options for Python bindings
    if(NLM_ENABLE_ADDRESS_SANITIZER)
        target_compile_options(pynlm PRIVATE -fsanitize=address)
        target_link_options(pynlm PRIVATE -fsanitize=address)
    endif()
    
    if(NLM_ENABLE_UNDEFINED_SANITIZER)
        target_compile_options(pynlm PRIVATE -fsanitize=undefined)
        target_link_options(pynlm PRIVATE -fsanitize=undefined)
    endif()
    
    if(NLM_ENABLE_THREAD_SANITIZER)
        target_compile_options(pynlm PRIVATE -fsanitize=thread)
        target_link_options(pynlm PRIVATE -fsanitize=thread)
    endif()
    
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    
    # Python bindings optimization settings
    if(NLM_PYTHON_BINDINGS_OPTIMIZED)
        target_compile_options(pynlm PRIVATE -DNPY_ADVANCED_FEATURES)
    endif()
    
    if(NLM_PYTHON_BINDINGS_DEBUG)
        target_compile_options(pynlm PRIVATE -DNPY_DEBUG_MODE)
    endif()
    
    if(NLM_PYTHON_BINDINGS_EXPERIMENTAL)
        target_compile_options(pynlm PRIVATE -DNPY_EXPERIMENTAL_MODE)
    endif()
    
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    
    # Python bindings install
    install(TARGETS pynlm LIBRARY DESTINATION .)
    
    # Python bindings documentation
    if(NLM_BUILD_DOCUMENTATION)
        add_custom_target(generate_python_docs
            COMMAND python -m pip install -q sphinx sphinx-rtd-theme
            COMMAND sphinx-build -b html python/ docs/python-api
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generate Python API documentation"
            VERBATIM
        )
    endif()
endif()

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_benchmark
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Scale benchmarks (built separately for large-scale testing)
if(NLM_BUILD_SCALE_BENCHMARKS)
    add_subdirectory(src/experiments/scale_benchmark)
endif()

# Experimental features directory
if(NLM_BUILD_EXPERIMENTAL_FEATURES)
    add_subdirectory(src/experimental)
endif()

# Testing
enable_testing()
add_subdirectory(tests)
