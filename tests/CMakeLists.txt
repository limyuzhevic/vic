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
    test_pybindings.cpp
    test_cpp_enhancements.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Configure test execution
# Disable long-running tests in CI by default
if(NOT CI_BUILD)
    # Run all tests
    add_custom_target(run_all_tests
        COMMAND ./nlm_test
        DEPENDS nlm_test
        COMMENT "Running all NLM tests"
        VERBATIM
    )
endif()

# Add test filters for selective testing
add_custom_target(run_unit_tests
    COMMAND ./nlm_test
    DEPENDS nlm_test
    COMMENT "Running unit tests only"
    VERBATIM
)

add_custom_target(run_python_tests
    COMMAND ./nlm_test
    DEPENDS nlm_test
    COMMENT "Running Python binding tests"
    VERBATIM
)
