cmake_minimum_required(VERSION 3.16)

# Neural system module (nlm_neural)
get_filename_component(NLM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(NLM_ROOT_DIR ${NLM_SRC_DIR} DIRECTORY)

# Create neural system libraries
add_library(nlm_neural
    brain/Neuron.cpp
    brain/Synapse.cpp
    brain/NeuralPopulation.cpp
    brain/NeuralRegion.cpp
    brain/Brain.cpp
    dynamics/NeuralDynamics.cpp
    dynamics/SpikeSystem.cpp
)

target_include_directories(nlm_neural PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

# Brain-specific test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_brain_test
        brain/tests/test_brain.cpp
        brain/tests/test_neuron.cpp
        brain/tests/test_synapse.cpp
        brain/tests/test_neural_population.cpp
    )
    
    target_link_libraries(nlm_brain_test PRIVATE nlm_neural)
    target_compile_features(nlm_brain_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_brain_test COMMAND nlm_brain_test)
endif()