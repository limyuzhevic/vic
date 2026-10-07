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

set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

# Create a list of all binding source files
file(GLOB NLM_BINDING_SOURCES
    "id_types.cpp"
    "enums.cpp"
    "config_bindings.cpp"
    "sensory_bindings.cpp"
    "state_bindings.cpp"
    "world_bindings.cpp"
    "brain_bindings.cpp"
    "agent_bindings.cpp"
    "main_bindings.cpp"
)

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

set(NLM_AGENT_SOURCES
    ${NLM_SOURCE_DIR}/agent/SensoryPercept.cpp
    ${NLM_SOURCE_DIR}/agent/AgentBrain.cpp
)

set(NLM_WORLD_SOURCES
    ${NLM_SOURCE_DIR}/world/SimpleWorld.cpp
)

add_library(nlm_core STATIC ${NLM_CORE_SOURCES})
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)
target_compile_options(nlm_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_library(nlm_agent STATIC ${NLM_AGENT_SOURCES})
target_include_directories(nlm_agent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_agent PUBLIC nlm_core)

add_library(nlm_world STATIC ${NLM_WORLD_SOURCES})
target_include_directories(nlm_world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(nlm_world PUBLIC nlm_core)

# Main Python module
pybind11_add_module(pynlm ${NLM_BINDING_SOURCES})
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

enable_testing()
add_test(NAME test_python_bindings
    COMMAND ${PYTHON_EXECUTABLE} -c "import pynlm; print('Python bindings imported successfully')"
)

# Documentation target for generating API docs
add_custom_target(generate_python_docs
    COMMAND ${PYTHON_EXECUTABLE} -c "
import pynlm
import inspect

# Get all public classes and functions from the module
classes = []
functions = []

for name in dir(pynlm):
    if not name.startswith('_'):
        obj = getattr(pynlm, name)
        if inspect.isclass(obj):
            classes.append(name)
        elif callable(obj) and not inspect.ismethod(obj):
            functions.append(name)

# Create documentation structure
doc = {
    'module': 'pynlm',
    'classes': classes,
    'functions': functions,
    'version': '0.1.0'
}

# Write to documentation file
with open('python_bindings_api.md', 'w') as f:
    f.write('# NLM Python Bindings API Documentation\\n\\n')
    f.write(f"**Version**: {doc['version']}\\n\\n")
    f.write('## Classes\\n')
    for cls in sorted(classes):
        f.write(f'- `{cls}`\\n')
    f.write('\\n## Functions\\n')
    for func in sorted(functions):
        f.write(f'- `{func}`\\n')
"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generate Python bindings documentation"
    VERBATIM
)

# Help target for developers
add_custom_target(help
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets for Python bindings:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all - Build all targets (default)"
    COMMAND ${CMAKE_COMMAND} -E echo "  pynlm - Build Python module only"
    COMMAND ${CMAKE_COMMAND} -E echo "  nlm_core - Build core C++ library only"
    COMMAND ${CMAKE_COMMAND} -E echo "  tests - Run Python binding tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  generate_python_docs - Generate API documentation"
    COMMAND ${CMAKE_COMMAND} -E echo "  help - Show this help message"
    COMMENT "Show available build targets for Python bindings"
    VERBATIM
)
