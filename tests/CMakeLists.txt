cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Find nlohmann/json package for enhanced tests
find_package(nlohmann_json REQUIRED)

# Create enhanced config test executable
add_executable(test_enhanced_config
    test_enhanced_config.cpp
)

target_link_libraries(test_enhanced_config PRIVATE nlm_core nlohmann_json::nlohmann_json)
target_compile_features(test_enhanced_config PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME test_enhanced_config COMMAND test_enhanced_config)

# Create performance test executable
add_executable(test_performance
    test_performance.cpp
)

target_link_libraries(test_performance PRIVATE nlm_core nlm_performance)
target_compile_features(test_performance PRIVATE cxx_std_20)

# Enable performance testing
add_test(NAME test_performance COMMAND test_performance)

# Create visualization test executable
add_executable(test_visualization
    test_visualization.cpp
)

target_link_libraries(test_visualization PRIVATE nlm_core nlm_visualization)
target_compile_features(test_visualization PRIVATE cxx_std_20)

# Enable visualization testing
add_test(NAME test_visualization COMMAND test_visualization)

# Create main test executable (legacy tests)
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

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
