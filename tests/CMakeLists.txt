cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/advanced)

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
    test_brain.cpp
    test_advanced_features.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_advanced)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Create advanced features demo executable
add_executable(demo_advanced_features
    demo_advanced_features.cpp
)

target_link_libraries(demo_advanced_features PUBLIC nlm_core nlm_advanced)
target_compile_features(demo_advanced_features PRIVATE cxx_std_20)

add_test(NAME demo_advanced_features COMMAND demo_advanced_features)
