# Test CMakeLists.txt - for building and running tests

# Test executables
set(TEST_SOURCES
    test_main.cpp
)

add_executable(test_main ${TEST_SOURCES})
target_link_libraries(test_main nlm_core)

# Main test runner executable (includes all test categories)
add_executable(comprehensive_test
    main_test.cpp
)
target_link_libraries(comprehensive_test nlm_core)

# Test targets
add_test(NAME basic_tests COMMAND test_main)
add_test(NAME comprehensive_tests COMMAND comprehensive_test)

# Add all test files to IDE
file(GLOB TEST_FILES
    *.cpp
)

# Add executable for each test category for easier debugging
# Config tests
add_executable(test_config
    tests/test_config.cpp
)
target_link_libraries(test_config nlm_core)

# Brain tests
add_executable(test_brain
    tests/test_brain.cpp
)
target_link_libraries(test_brain nlm_core)

# Neuron tests
add_executable(test_neuron
    tests/test_neuron.cpp
)
target_link_libraries(test_neuron nlm_core)

# Synapse tests
add_executable(test_synapse
    tests/test_synapse.cpp
)
target_link_libraries(test_synapse nlm_core)

# Additional basic tests
add_executable(test_stdp
    tests/test_stdp.cpp
)
target_link_libraries(test_stdp nlm_core)

add_executable(test_clock
    tests/test_clock.cpp
)
target_link_libraries(test_clock nlm_core)

add_executable(test_random
    tests/test_random.cpp
)
target_link_libraries(test_random nlm_core)

add_executable(test_types
    tests/test_types.cpp
)
target_link_libraries(test_types nlm_core)

# New test categories
add_executable(test_config_validation
    tests/test_config_validation.cpp
)
target_link_libraries(test_config_validation nlm_core)

add_executable(test_error_handling
    tests/test_error_handling.cpp
)
target_link_libraries(test_error_handling nlm_core)

add_executable(test_integration
    tests/test_integration.cpp
)
target_link_libraries(test_integration nlm_core)

add_executable(test_performance
    tests/test_performance.cpp
)
target_link_libraries(test_performance nlm_core)