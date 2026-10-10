cmake_minimum_required(VERSION 3.16)

# Brain module library
add_library(nlm_neural
    Neuron.cpp
    Synapse.cpp
    NeuralPopulation.cpp
    NeuralRegion.cpp
    Brain.cpp
)

target_include_directories(nlm_neural PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/brain>
    $<INSTALL_INTERFACE:include/brain>
)

# Neural dynamics module
add_library(nlm_neural_dynamics
    ../dynamics/NeuralDynamics.cpp
    ../dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_neural_dynamics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/dynamics>
)

# Connect neural modules
target_link_libraries(nlm_neural PUBLIC nlm_core_utils nlm_core_types nlm_neural_dynamics)

# Brain test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_brain_test
        ../tests/brain/test_brain.cpp
        ../tests/brain/test_neuron.cpp
        ../tests/brain/test_synapse.cpp
        ../tests/brain/test_neural_population.cpp
    )
    
    target_link_libraries(nlm_brain_test PRIVATE nlm_neural)
    target_compile_features(nlm_brain_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_brain_test COMMAND nlm_brain_test)
endif()