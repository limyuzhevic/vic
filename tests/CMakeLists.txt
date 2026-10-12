cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_config_builder.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    
    # Unit tests
    unit/TestConfigBuilder.cpp
    unit/TestBrainCore.cpp
    unit/TestMemorySystems.cpp
    unit/TestNeuromodulation.cpp
    unit/TestCognition.cpp
    unit/TestPlasticity.cpp
    
    # Integration tests
    integration/TestPhase2Integration.cpp
    integration/TestPhase6Integration.cpp
    integration/TestEndToEnd.cpp
    
    # Performance tests
    performance/TestPerformance.cpp
    
    # Test framework
    Framework/TestFramework.cpp
    Helpers/TestHelpers.cpp
    
    # Test utilities
    utils/TestAssertions.cpp
    utils/TestComparators.cpp
    utils/TestDataProviders.cpp
    utils/TestBenchmarks.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Create integration test executable
add_executable(nlm_integration_test
    IntegrationRunner.cpp
)

target_link_libraries(nlm_integration_test PRIVATE nlm_core)
target_compile_features(nlm_integration_test PRIVATE cxx_std_20)

# Enable integration testing
add_test(NAME nlm_integration_test COMMAND nlm_integration_test)
