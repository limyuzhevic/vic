# NLM Tests CMakeLists.txt

# Find GTest
find_package(GTest REQUIRED)

# Create test executables
function(add_nlm_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE nlm GTest::gtest GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add comprehensive tests
add_nlm_test(test_neuron)
add_nlm_test(test_synapse)
add_nlm_test(test_brain)
add_nlm_test(test_stdp)
add_nlm_test(test_config)
add_nlm_test(test_clock)
add_nlm_test(test_random)
add_nlm_test(test_types)

# Add integration and performance tests if needed
# add_nlm_test(test_integration)
# add_nlm_test(test_performance)
