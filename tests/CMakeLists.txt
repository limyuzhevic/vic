cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Find required dependencies
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(toml REQUIRED)

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_management.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlohmann_json::nlohmann_json yaml-cpp toml++)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
