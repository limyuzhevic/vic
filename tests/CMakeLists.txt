cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_hebbian.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_logging)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Standalone test executables for individual components
add_executable(test_stdp
    test_stdp.cpp
)
target_link_libraries(test_stdp PRIVATE nlm_core nlm_logging)
target_compile_features(test_stdp PRIVATE cxx_std_20)

add_executable(test_hebbian
    test_hebbian.cpp
)
target_link_libraries(test_hebbian PRIVATE nlm_core nlm_logging)
target_compile_features(test_hebbian PRIVATE cxx_std_20)
