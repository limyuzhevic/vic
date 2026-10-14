cmake_minimum_required(VERSION 3.21)

# Modern testing configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Modern testing framework options
option(NLM_ENABLE_UNIT_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_INTEGRATION_TESTS "Enable integration tests" ON)
option(NLM_ENABLE_PERFORMANCE_TESTS "Enable performance tests" OFF)
option(NLM_ENABLE_COVERAGE "Enable test coverage reporting" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory/thread sanitizers" OFF)

# Modern testing framework files
set(MODERN_TESTING_FILES
    nlm_testing_framework.hpp
    nlm_test_main.cpp
    nlm_test_runner.cpp
    nlm_test_discovery.cpp
)

# Component test files
set(CORE_TESTS
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
)

set(BRAIN_TESTS
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

# Component-specific tests
set(PLASTICITY_TESTS
    test_plasticity.cpp
    test_hebbian.cpp
    test_structural_plasticity.cpp
)

set(MEMORY_TESTS
    test_memory.cpp
    test_working_memory.cpp
    test_episodic_memory.cpp
    test_semantic_memory.cpp
)

set(NEUROMODULATION_TESTS
    test_neuromodulator.cpp
    test_reward.cpp
    test_prediction_error.cpp
    test_novelty.cpp
    test_curiosity.cpp
)

set(INTEGRATION_TESTS
    test_integration_brain_memory.cpp
    test_integration_neuromodulation.cpp
    test_integration_cognition.cpp
    test_integration_agent.cpp
)

set(PERFORMANCE_TESTS
    test_performance_benchmark.cpp
    test_performance.cpp
)

set(CONFIGURATION_TESTS
    test_config_loading.cpp
    test_config_validation.cpp
)

set(CLI_TESTS
    test_cli.cpp
)

set(ERROR_HANDLING_TESTS
    test_error_handling.cpp
    test_exception_safety.cpp
)

# Create modern test executable
add_executable(nlm_test
    ${MODERN_TESTING_FILES}
    ${CORE_TESTS}
    ${BRAIN_TESTS}
    ${PLASTICITY_TESTS}
    ${MEMORY_TESTS}
    ${NEUROMODULATION_TESTS}
    ${INTEGRATION_TESTS}
    ${PERFORMANCE_TESTS}
    ${CONFIGURATION_TESTS}
    ${CLI_TESTS}
    ${ERROR_HANDLING_TESTS}
    test_main_enhanced.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Modern compiler optimizations for tests
if(NOT MSVC)
    target_compile_options(nlm_test PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Coverage and sanitizer support
if(NLM_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE --coverage -O0 -g)
    target_link_options(nlm_test PRIVATE --coverage)
endif()

if(NLM_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE -fsanitize=address,undefined,thread)
    target_link_options(nlm_test PRIVATE -fsanitize=address,undefined,thread)
endif()

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add test categories as separate test suites
if(NLM_ENABLE_UNIT_TESTS)
    add_test(NAME nlm_unit_tests COMMAND nlm_test --category unit)
endif()

if(NLM_ENABLE_INTEGRATION_TESTS)
    add_test(NAME nlm_integration_tests COMMAND nlm_test --category integration)
endif()

if(NLM_ENABLE_PERFORMANCE_TESTS)
    add_test(NAME nlm_performance_tests COMMAND nlm_test --category performance)
endif()

# Install test infrastructure
install(FILES
    ${MODERN_TESTING_FILES}
    DESTINATION include/nlm/tests
)
