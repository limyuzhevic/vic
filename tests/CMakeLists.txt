# CMakeLists.txt for NLM tests
# Add all test executables

# Existing tests
add_executable(test_brain tests/test_brain.cpp)
target_link_libraries(test_brain PUBLIC nlm_core)

add_executable(test_neuron tests/test_neuron.cpp)
target_link_libraries(test_neuron PUBLIC nlm_core)

add_executable(test_synapse tests/test_synapse.cpp)
target_link_libraries(test_synapse PUBLIC nlm_core)

add_executable(test_stdp tests/test_stdp.cpp)
target_link_libraries(test_stdp PUBLIC nlm_core)

add_executable(test_clock tests/test_clock.cpp)
target_link_libraries(test_clock PUBLIC nlm_core)

add_executable(test_random tests/test_random.cpp)
target_link_libraries(test_random PUBLIC nlm_core)

add_executable(test_config tests/test_config.cpp)
target_link_libraries(test_config PUBLIC nlm_core)

add_executable(test_types tests/test_types.cpp)
target_link_libraries(test_types PUBLIC nlm_core)

add_executable(test_main tests/test_main.cpp)
target_link_libraries(test_main PUBLIC nlm_core)

# New development component tests
add_executable(test_development tests/test_development.cpp)
target_link_libraries(test_development PUBLIC nlm_core)

# New sensory processing tests
add_executable(test_vision tests/test_vision.cpp)
target_link_libraries(test_vision PUBLIC nlm_core)

add_executable(test_audio tests/test_audio.cpp)
target_link_libraries(test_audio PUBLIC nlm_core)

# New neural dynamics and plasticity tests
add_executable(test_neural_dynamics tests/test_neural_dynamics.cpp)
target_link_libraries(test_neural_dynamics PUBLIC nlm_core)

# New visualization system tests
add_executable(test_visualization tests/test_visualization.cpp)
target_link_libraries(test_visualization PUBLIC nlm_core)

# New error handling system tests
add_executable(test_error_handling tests/test_error_handling.cpp)
target_link_libraries(test_error_handling PUBLIC nlm_core)

# New performance benchmark tests
add_executable(test_benchmark tests/test_benchmark.cpp)
target_link_libraries(test_benchmark PUBLIC nlm_core)

# Add all tests to ctest
add_test(NAME test_brain COMMAND test_brain)
add_test(NAME test_neuron COMMAND test_neuron)
add_test(NAME test_synapse COMMAND test_synapse)
add_test(NAME test_stdp COMMAND test_stdp)
add_test(NAME test_clock COMMAND test_clock)
add_test(NAME test_random COMMAND test_random)
add_test(NAME test_config COMMAND test_config)
add_test(NAME test_types COMMAND test_types)
add_test(NAME test_main COMMAND test_main)
add_test(NAME test_development COMMAND test_development)
add_test(NAME test_vision COMMAND test_vision)
add_test(NAME test_audio COMMAND test_audio)
add_test(NAME test_neural_dynamics COMMAND test_neural_dynamics)
add_test(NAME test_visualization COMMAND test_visualization)
add_test(NAME test_error_handling COMMAND test_error_handling)
add_test(NAME test_benchmark COMMAND test_benchmark)

# Set test properties

# Run test_brain first (it's the most fundamental)
set_tests_properties(test_brain PROPERTIES RUN_SERIAL TRUE)

# Run development and neural dynamics tests early
foreach(test dev neural_dynamics vision audio)
    set_tests_properties(test_${test} PROPERTIES RUN_SERIAL TRUE)
endforeach()
