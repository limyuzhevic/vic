cmake_minimum_required(VERSION 3.16)

# Legacy tests directory structure
# This maintains backward compatibility with the existing test suite

# Legacy test executables
add_executable(test_types tests/test_types.cpp)
add_executable(test_config tests/test_config.cpp)
add_executable(test_random tests/test_random.cpp)
add_executable(test_clock tests/test_clock.cpp)
add_executable(test_neuron tests/test_neuron.cpp)
add_executable(test_synapse tests/test_synapse.cpp)
add_executable(test_stdp tests/test_stdp.cpp)
add_executable(test_brain tests/test_brain.cpp)

# Link libraries for legacy tests
target_link_libraries(test_types PUBLIC nlm_core)
target_link_libraries(test_config PUBLIC nlm_core)
target_link_libraries(test_random PUBLIC nlm_core)
target_link_libraries(test_clock PUBLIC nlm_core)
target_link_libraries(test_neuron PUBLIC nlm_core)
target_link_libraries(test_synapse PUBLIC nlm_core)
target_link_libraries(test_stdp PUBLIC nlm_core)
target_link_libraries(test_brain PUBLIC nlm_core)

# Legacy test runner (original test_main.cpp)
add_executable(nlm_test_legacy tests/test_main.cpp)
target_link_libraries(nlm_test_legacy PUBLIC nlm_core)

# Add legacy tests to CTest
foreach(test_exe test_types test_config test_random test_clock test_neuron test_synapse test_stdp test_brain nlm_test_legacy)
    add_test(NAME ${test_exe}
        COMMAND ${test_exe}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()

# Legacy test framework function
function(run_legacy_tests)
    message(STATUS "Running legacy NLM tests...")
    
    # Run legacy tests with improved output
    set(test_results)
    foreach(test_exe test_types test_config test_random test_clock test_neuron test_synapse test_stdp test_brain)
        message(STATUS "  Running ${test_exe}...")
        execute_process(
            COMMAND ${test_exe}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE result
            OUTPUT_VARIABLE output
            ERROR_VARIABLE error
        )
        
        if(result EQUAL 0)
            list(APPEND test_results "${test_exe}: PASSED")
        else()
            list(APPEND test_results "${test_exe}: FAILED - ${error}")
        endif()
    endforeach()
    
    message(STATUS "Legacy test execution completed")
    foreach(result ${test_results})
        message(STATUS "  ${result}")
    endforeach()
endfunction()

# Legacy test configuration
if(NOT DEFINED NLM_LEGACY_TESTS_ENABLED)
    option(NLM_LEGACY_TESTS_ENABLED "Enable legacy NLM tests" ON)
endif()

if(NLM_LEGACY_TESTS_ENABLED)
    run_legacy_tests()
endif()
