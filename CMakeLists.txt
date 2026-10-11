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

# Advanced neural network models and algorithms
add_library(nlm_advanced STATIC
    # Advanced neuron models
    src/brain/advanced/AdvancedNeuronModels.hpp
    src/brain/advanced/HodgkinHuxleyNeuron.hpp
    src/brain/advanced/HodgkinHuxleyNeuron.cpp
    src/brain/advanced/IzhikevichNeuron.hpp
    src/brain/advanced/IzhikevichNeuron.cpp
    src/brain/advanced/AdaptiveExponentialNeuron.hpp
    src/brain/advanced/AdaptiveExponentialNeuron.cpp
    src/brain/advanced/ThetaNeuron.hpp
    src/brain/advanced/ThetaNeuron.cpp
    src/brain/advanced/MorrisLecarNeuron.hpp
    src/brain/advanced/MorrisLecarNeuron.cpp
    src/brain/advanced/AdvancedNeuronFactory.cpp
    # Advanced network architectures (to be implemented)
    src/brain/advanced/convolutional/ConvolutionalNetwork.hpp
    src/brain/advanced/convolutional/ConvolutionalNetwork.cpp
    src/brain/advanced/recurrent/RNN.hpp
    src/brain/advanced/recurrent/RNN.cpp
    src/brain/advanced/recurrent/LSTM.hpp
    src/brain/advanced/recurrent/LSTM.cpp
    src/brain/advanced/recurrent/GRU.hpp
    src/brain/advanced/recurrent/GRU.cpp
    src/brain/advanced/graph/GraphNeuralNetwork.hpp
    src/brain/advanced/graph/GraphNeuralNetwork.cpp
    src/brain/advanced/transformer/Transformer.hpp
    src/brain/advanced/transformer/Transformer.cpp
    src/brain/advanced/attention/Attention.hpp
    src/brain/advanced/attention/Attention.cpp
    src/brain/advanced/residual/ResidualNetwork.hpp
    src/brain/advanced/residual/ResidualNetwork.cpp
    # Advanced learning algorithms (to be implemented)
    src/brain/advanced/learning/BPTT.hpp
    src/brain/advanced/learning/BPTT.cpp
    src/brain/advanced/learning/PolicyGradient.hpp
    src/brain/advanced/learning/PolicyGradient.cpp
    src/brain/advanced/learning/ActorCritic.hpp
    src/brain/advanced/learning/ActorCritic.cpp
    src/brain/advanced/learning/DQN.hpp
    src/brain/advanced/learning/DQN.cpp
    src/brain/advanced/learning/MetaLearning.hpp
    src/brain/advanced/learning/MetaLearning.cpp
    # Optimization and compression (to be implemented)
    src/brain/advanced/optimization/Pruning.hpp
    src/brain/advanced/optimization/Pruning.cpp
    src/brain/advanced/optimization/Quantization.hpp
    src/brain/advanced/optimization/Quantization.cpp
    src/brain/advanced/optimization/KnowledgeDistillation.hpp
    src/brain/advanced/optimization/KnowledgeDistillation.cpp
    src/brain/advanced/optimization/NeuralArchitectureSearch.hpp
    src/brain/advanced/optimization/NeuralArchitectureSearch.cpp
    # Integration layer
    src/brain/advanced/integration/CompatibilityLayer.hpp
    src/brain/advanced/integration/CompatibilityLayer.cpp
    src/brain/advanced/integration/ModelConverter.hpp
    src/brain/advanced/integration/ModelConverter.cpp
    # Configuration system
    src/brain/advanced/config/AdvancedNeuronConfig.hpp
    src/brain/advanced/config/AdvancedNeuronConfig.cpp
    src/brain/advanced/config/NetworkConfig.hpp
    src/brain/advanced/config/NetworkConfig.cpp
    src/brain/advanced/config/TrainingConfig.hpp
    src/brain/advanced/config/TrainingConfig.cpp
    # Testing and validation
    src/brain/advanced/testing/AdvancedNeuronTests.hpp
    src/brain/advanced/testing/AdvancedNeuronTests.cpp
    src/brain/advanced/testing/BenchmarkSuite.hpp
    src/brain/advanced/testing/BenchmarkSuite.cpp
    src/brain/advanced/testing/ValidationTests.hpp
    src/brain/advanced/testing/ValidationTests.cpp
)

target_include_directories(nlm_advanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(nlm_advanced PUBLIC nlm_core)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC src)
target_link_libraries(nlm_agent PUBLIC nlm_core nlm_advanced)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC src)
target_link_libraries(nlm_world PUBLIC nlm_core nlm_advanced)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC src)
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world nlm_advanced)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC src)
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world nlm_advanced)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core nlm_advanced)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_advanced)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_advanced)

# Advanced test runner for advanced neuron models
add_executable(advanced_neuron_tests
    src/brain/advanced/testing/AdvancedNeuronTestRunner.cpp
)
target_link_libraries(advanced_neuron_tests PUBLIC nlm_advanced)

# Add the advanced test target to the tests
add_test(NAME advanced_neuron_tests
    COMMAND advanced_neuron_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo nlm_advanced
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
