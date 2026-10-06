cmake_minimum_required(VERSION 3.16)

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

# Create specialized test executables

# Connectivity test
add_executable(nlm_connectivity_test
    test_connectivity.cpp
)
target_link_libraries(nlm_connectivity_test PRIVATE nlm_core)
target_compile_features(nlm_connectivity_test PRIVATE cxx_std_20)

# Plasticity test
add_executable(nlm_plasticity_test
    test_plasticity.cpp
)
target_link_libraries(nlm_plasticity_test PRIVATE nlm_core)
target_compile_features(nlm_plasticity_test PRIVATE cxx_std_20)

# STDP test
add_executable(nlm_stdp_test
    test_stdp.cpp
)
target_link_libraries(nlm_stdp_test PRIVATE nlm_core)
target_compile_features(nlm_stdp_test PRIVATE cxx_std_20)

# Integration test (comprehensive system testing)
add_executable(nlm_integration_test
    test_integration.cpp
)
target_link_libraries(nlm_integration_test PRIVATE nlm_core)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_connectivity_test COMMAND nlm_connectivity_test)
add_test(NAME nlm_plasticity_test COMMAND nlm_plasticity_test)
add_test(NAME nlm_stdp_test COMMAND nlm_stdp_test)
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)
