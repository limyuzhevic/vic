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
    test_brain.cpp
    
    # Phase 6 comprehensive tests
    run_all_tests.cpp
    run_regression_tests.cpp
    run_behavioral_tests.cpp
    
    # Unit test files
    MemoryTests.cpp
    NeuromodulationTests.cpp
    CognitionTests.cpp
    DevelopmentTests.cpp
    BehavioralTests.cpp
    MultiAgentTests.cpp
    StatisticalTests.cpp
    
    # Test infrastructure
    TestFramework.hpp
    TestUtils.hpp
    MetricsCollector.hpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)
add_test(NAME nlm_all_tests COMMAND nlm_test)
add_test(NAME nlm_regression_tests COMMAND nlm_test --regression)
add_test(NAME nlm_behavioral_tests COMMAND nlm_test --behavioral)
