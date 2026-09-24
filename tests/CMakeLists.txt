# Legacy test configuration (backward compatibility)
# Original test infrastructure before Phase 6 GoogleTest integration

cmake_minimum_required(VERSION 3.16)

# Test configuration for legacy tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create legacy test executable
add_executable(nlm_test_legacy
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

target_link_libraries(nlm_test_legacy PRIVATE nlm_core)
target_compile_features(nlm_test_legacy PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test_legacy COMMAND nlm_test_legacy)

# Note: Phase 6 tests are integrated into the main CMakeLists.txt
# using GoogleTest framework with separate test executable:
# nlm_test_phase6 (using GoogleTest)
# nlm_test_legacy (backward compatibility)