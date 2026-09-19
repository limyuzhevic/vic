# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create main test executable
add_executable(nlm_test
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
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Create config vector test executable
add_executable(test_config_vector
    test_config_vector.cpp
)

target_link_libraries(test_config_vector PRIVATE nlm_core)
target_compile_features(test_config_vector PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME test_config_vector COMMAND test_config_vector)
