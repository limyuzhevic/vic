cmake_minimum_required(VERSION 3.16)

# Systems module (nlm_systems)
get_filename_component(NLM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(NLM_ROOT_DIR ${NLM_SRC_DIR} DIRECTORY)

# Create systems libraries
add_library(nlm_systems
    agent/AgentBrain.cpp
    agent/SensoryPercept.cpp
    world/SimpleWorld.cpp
    neuromodulation/Neuromodulator.cpp
    neuromodulation/Reward.cpp
    neuromodulation/PredictionError.cpp
    neuromodulation/Novelty.cpp
    neuromodulation/Curiosity.cpp
    sensory/Vision.cpp
    sensory/Audio.cpp
    sensory/SensoryInput.cpp
    sensory/InternalSignals.cpp
    motor/MotorSystem.cpp
    motor/Action.cpp
    prediction/PredictionSystem.cpp
    prediction/NeuralPrediction.cpp
    environment/Environment.cpp
    environment/Observation.cpp
    environment/Action.cpp
    development/DevelopmentSystem.cpp
    development/Synaptogenesis.cpp
    development/Pruning.cpp
    development/Maturation.cpp
)

target_include_directories(nlm_systems PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Systems-specific test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_systems_test
        systems/tests/test_agent.cpp
        systems/tests/test_world.cpp
        systems/tests/test_neuromodulation.cpp
    )
    
    target_link_libraries(nlm_systems_test PRIVATE nlm_systems)
    target_compile_features(nlm_systems_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_systems_test COMMAND nlm_systems_test)
endif()