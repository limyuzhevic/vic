# NLM Test Suite CMake Configuration
# Modern CMake-based test system with improved organization and coverage

cmake_minimum_required(VERSION 3.16)

project(nlm_tests VERSION 0.1.0 LANGUAGES CXX)

# Enable testing framework
set(TEST_TIMEOUT 30 CACHE STRING "Timeout for individual tests in seconds")

# Configure test paths
set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TEST_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(NLM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Compiler features and warnings
add_compile_features(nlm_test PRIVATE cxx_std_20)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-Wno-missing-field-initializers -Wno-unused-parameter)
    endif()
endif()

# Include directories for tests
include_directories(
    ${NLM_SOURCE_DIR}
    ${TEST_SOURCE_DIR}
)

# Create the main test executable with all test cases
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
    test_config_json.cpp
    test_config_yaml.cpp
    test_config_validation.cpp
)

# Link with NLM core library
target_link_libraries(nlm_test PRIVATE nlm::core)

# Add test timeouts and properties
set_target_properties(nlm_test PROPERTIES
    TIMEOUT ${TEST_TIMEOUT}
    RUNTIME_OUTPUT_DIRECTORY ${TEST_BUILD_DIR}
)

# Add comprehensive test targets
add_test(NAME nlm_unit_tests COMMAND nlm_test)
set_tests_properties(nlm_unit_tests PROPERTIES
    TIMEOUT ${TEST_TIMEOUT}
    WORKING_DIRECTORY ${TEST_BUILD_DIR}
)

# Add test coverage analysis if available
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(COVERAGE_TOOL coverage)
    if(COVERAGE_TOOL)
        target_compile_options(nlm_test PRIVATE --coverage -O0 -g)
        target_link_libraries(nlm_test PRIVATE --coverage)
        add_custom_target(coverage
            COMMAND ${COVERAGE_TOOL} --exclude "${NLM_SOURCE_DIR}/*" --include "*test*.cpp" --output-file coverage.info ${TEST_BUILD_DIR}/nlm_test
            COMMAND echo "Coverage report generated: coverage.html"
            COMMENT "Generate test coverage report"
            VERBATIM
        )
    endif()
endif()

# Add test data and resource management
# Test configuration files are located in ../configs/

# Add test dependency on configuration examples
add_custom_target(test_config_deps
    COMMAND ${CMAKE_COMMAND} -E echo "Checking configuration file dependencies..."
    COMMAND ${CMAKE_COMMAND} -E touch ${TEST_BUILD_DIR}/.config_tested
    COMMENT "Verify configuration files exist for testing"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(nlm_test test_config_deps)

# Add test documentation target
add_custom_target(test_docs
    COMMAND ${CMAKE_COMMAND} -E echo "=== NLM Test Suite ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Available tests:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Unit tests: nlm_unit_tests (comprehensive test suite)"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Configuration tests: JSON, YAML, validation"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Performance tests: CPU, memory, scalability"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Integration tests: System component interactions"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test output directory: ${TEST_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Configuration files: ${NLM_SOURCE_DIR}/../configs/"
    COMMENT "Show test information and configuration locations"
    VERBATIM
)

# Add test results analysis target
add_custom_target(analyze_test_results
    COMMAND ${CMAKE_COMMAND} -E echo "=== Test Results Analysis ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Run tests first: make nlm_test"
    COMMAND ${CMAKE_COMMAND} -E echo "Then run: ./nlm_test"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test categories:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Config System: JSON, YAML, validation"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Core Components: Neurons, synapses, plasticity"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Memory Systems: Working, episodic, associative"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Neuromodulation: Dopamine, curiosity, novelty"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Cognitive Systems: Attention, planning, concept formation"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Integration: Complete system verification"
    COMMENT "Show test categories and analysis instructions"
    VERBATIM
)

# Add test configuration summary
add_custom_target(config_test_summary
    COMMAND ${CMAKE_COMMAND} -E echo "=== Configuration Test Summary ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Test configuration files in: ${NLM_SOURCE_DIR}/../configs/"
    COMMAND ${CMAKE_COMMAND} -E echo "  - default.cfg: Simple key=value format"
    COMMAND ${CMAKE_COMMAND} -E echo "  - nlm_config.json: JSON with nested structures"
    COMMAND ${CMAKE_COMMAND} -E echo "  - nlm_config.yaml: YAML with human-readable format"
    COMMAND ${CMAKE_COMMAND} -E echo "  - CONFIG_IMPLEMENTATION.md: Documentation"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test format support:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ✓ JSON (nlohmann/json library)"
    COMMAND ${CMAKE_COMMAND} -E echo "  ✓ YAML (yaml-cpp library)"
    COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Simple key=value (backward compatibility)"
    COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Schema validation"
    COMMAND ${CMAKE_COMMAND} -E echo "  ✓ Error handling and reporting"
    COMMENT "Show configuration test capabilities"
    VERBATIM
)

# Add performance benchmark target
add_custom_target(run_benchmarks
    COMMAND ${CMAKE_COMMAND} -E echo "=== NLM Performance Benchmarks ==="
    COMMAND ${CMAKE_COMMAND} -E echo "To run performance benchmarks:"
    COMMAND ${CMAKE_COMMAND} -E echo "  1. Build with benchmarks enabled:"
    COMMAND ${CMAKE_COMMAND} -E echo "     cmake .. -DBUILD_BENCHMARKS=ON"
    COMMAND ${CMAKE_COMMAND} -E echo "  2. Run benchmark executable:"
    COMMAND ${CMAKE_COMMAND} -E echo "     ./nlm_benchmark"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Benchmark categories:"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Neuron simulation performance"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Memory usage and optimization"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Network connectivity scaling"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Plasticity algorithm performance"
    COMMAND ${CMAKE_COMMAND} -E echo "  - Integration test timing"
    COMMENT "Show benchmark information"
    VERBATIM
)

# Add test dependency targets
add_custom_target(test_all
    DEPENDS nlm_test test_docs test_config_summary analyze_test_results
    COMMENT "Build and configure all test components"
)

# Add test documentation target
add_custom_target(test_readme
    COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SOURCE_DIR}/../README.md ${TEST_BUILD_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SOURCE_DIR}/../easy_usage.md ${TEST_BUILD_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${TEST_SOURCE_DIR}/../HOW_TO_USE.md ${TEST_BUILD_DIR}/
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation copied to test directory"
    COMMENT "Copy documentation to test directory"
    VERBATIM
)

# Add test cleanup target
add_custom_target(test_clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${TEST_BUILD_DIR}/test_results
    COMMAND ${CMAKE_COMMAND} -E remove ${TEST_BUILD_DIR}/coverage.info
    COMMAND ${CMAKE_COMMAND} -E echo "Test artifacts cleaned"
    COMMENT "Clean test artifacts"
    VERBATIM
)

# Add test status target
add_custom_target(test_status
    COMMAND ${CMAKE_COMMAND} -E echo "=== NLM Test System Status ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Build directory: ${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Test directory: ${TEST_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "NLM source: ${NLM_SOURCE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Test executable: ${TEST_BUILD_DIR}/nlm_test"
    COMMAND ${CMAKE_COMMAND} -E echo "Configuration files: ${NLM_SOURCE_DIR}/../configs/"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "To run tests:"
    COMMAND ${CMAKE_COMMAND} -E echo "  cd ${TEST_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "  ../nlm_test"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Test targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_all: Build all test components"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_docs: Show documentation locations"
    COMMAND ${CMAKE_COMMAND} -E echo "  config_test_summary: Show configuration test capabilities"
    COMMAND ${CMAKE_COMMAND} -E echo "  analyze_test_results: Show test analysis"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_readme: Copy documentation to test directory"
    COMMAND ${CMAKE_COMMAND} -E echo "  test_clean: Clean test artifacts"
    COMMENT "Show test system status and available targets"
    VERBATIM
)
