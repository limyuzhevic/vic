# Test executable
add_executable(nlm_test EXCLUDE_FROM_ALL
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)

add_test(NAME nlm_test COMMAND nlm_test)
