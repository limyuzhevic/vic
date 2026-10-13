cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Find required packages
find_package(nlohmann_json 3.11.2 REQUIRED)
find_package(yaml-cpp REQUIRED)

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_config_enhanced.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlohmann_json::nlohmann_json yaml-cpp)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add benchmark test
add_executable(nlm_benchmark
    benchmark.cpp
)
target_link_libraries(nlm_benchmark PRIVATE nlm_core)
target_compile_features(nlm_benchmark PRIVATE cxx_std_20)
add_test(NAME nlm_benchmark COMMAND nlm_benchmark)

# Add integration test
add_executable(nlm_integration_test
    integration_test.cpp
)
target_link_libraries(nlm_integration_test PRIVATE nlm_core)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)
