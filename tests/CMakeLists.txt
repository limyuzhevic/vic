# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Enable testing
enable_testing()

# Individual test executables for better test categorization

# Core system tests
add_executable(test_types test_types.cpp)
target_link_libraries(test_types nlm_core)
add_test(NAME test_types COMMAND test_types)

add_executable(test_config test_config.cpp)
target_link_libraries(test_config nlm_core)
add_test(NAME test_config COMMAND test_config)

add_executable(test_random test_random.cpp)
target_link_libraries(test_random nlm_core)
add_test(NAME test_random COMMAND test_random)

add_executable(test_clock test_clock.cpp)
target_link_libraries(test_clock nlm_core)
add_test(NAME test_clock COMMAND test_clock)

# Neural system tests
add_executable(test_neuron test_neuron.cpp)
target_link_libraries(test_neuron nlm_core)
add_test(NAME test_neuron COMMAND test_neuron)

add_executable(test_synapse test_synapse.cpp)
target_link_libraries(test_synapse nlm_core)
add_test(NAME test_synapse COMMAND test_synapse)

add_executable(test_stdp test_stdp.cpp)
target_link_libraries(test_stdp nlm_core)
add_test(NAME test_stdp COMMAND test_stdp)

add_executable(test_brain test_brain.cpp)
target_link_libraries(test_brain nlm_core)
add_test(NAME test_brain COMMAND test_brain)

add_executable(test_main test_main.cpp)
target_link_libraries(test_main nlm_core)
add_test(NAME test_main COMMAND test_main)

# Comprehensive test suite
add_executable(test_comprehensive test_comprehensive.cpp)
target_link_libraries(test_comprehensive nlm_core)
add_test(NAME test_comprehensive COMMAND test_comprehensive)

# Set test properties for timeout and working directory
set_tests_properties(test_types test_config test_random test_clock test_neuron test_synapse
    test_stdp test_brain test_main test_comprehensive
    PROPERTIES TIMEOUT 300 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Add test categories and custom targets
function(add_test_categories)
    # Target for running all tests
    add_custom_target(test_all
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_types test_config test_random test_clock
                test_neuron test_synapse test_stdp test_brain test_main test_comprehensive
        COMMENT "Running all NLM tests"
    )
    
    # Target for running comprehensive tests only
    add_custom_target(test_comprehensive_only
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R comprehensive
        DEPENDS test_comprehensive
        COMMENT "Running comprehensive NLM tests only"
    )
    
    # Target for running core system tests only
    add_custom_target(test_core_only
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "types|config|random|clock"
        DEPENDS test_types test_config test_random test_clock
        COMMENT "Running core system tests only"
    )
    
    # Target for running neural system tests only
    add_custom_target(test_neural_only
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R "neuron|synapse|stdp|brain|main"
        DEPENDS test_neuron test_synapse test_stdp test_brain test_main
        COMMENT "Running neural system tests only"
    )
endfunction()

add_test_categories()