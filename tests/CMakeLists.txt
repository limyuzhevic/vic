cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create legacy test executable (existing functionality tests)
add_executable(nlm_legacy_test
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

target_link_libraries(nlm_legacy_test PRIVATE nlm_core)
target_compile_features(nlm_legacy_test PRIVATE cxx_std_20)

# Add production unit tests to CTest (separate executable)
add_executable(nlm_production_unit_tests
    run_production_tests.cpp
    NLMTestFramework.cpp
    test_config_production.cpp
)

target_link_libraries(nlm_production_unit_tests PRIVATE nlm_core)
target_compile_features(nlm_production_unit_tests PRIVATE cxx_std_20)

# Add production unit tests to CTest
add_test(NAME nlm_production_unit_tests COMMAND nlm_production_unit_tests)

# Create a convenience target for all tests
add_custom_target(all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS nlm_legacy_test nlm_production_unit_tests
    COMMENT "Run all NLM tests"
)