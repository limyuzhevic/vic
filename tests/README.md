# NLM Test Framework

## Overview

This document provides information about the test framework used in the NLM (Neural Learning Machine) project.

## Test Categories

### Unit Tests
- **Location**: `tests/unit/`
- **Purpose**: Test individual components and functions in isolation
- **Examples**:
  - Configuration loading and validation
  - Brain initialization
  - Memory system operations
  - Neuromodulation system tests

### Integration Tests
- **Location**: `tests/integration/`
- **Purpose**: Test the integration of multiple systems
- **Examples**:
  - Complete brain integration with all systems
  - Memory system integration
  - Neuromodulation system integration
  - Sensorimotor loop integration

### Performance Tests
- **Location**: `tests/performance/`
- **Purpose**: Test performance characteristics and optimization
- **Examples**:
  - Configuration loading performance
  - Brain initialization performance
  - Simulation speed benchmarks

### Stress Tests
- **Location**: `tests/stress/`
- **Purpose**: Test system behavior under extreme conditions
- **Examples**:
  - Large-scale neural networks
  - High-frequency input streams
  - Memory usage under load

## Running Tests

### Prerequisites

1. Build the NLM project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

2. Install required dependencies (if any)

### Running the Test Framework

Run the comprehensive test suite:
```bash
cd build
./nlm_tests
```

Run individual test cases:
```bash
# Run a specific test case
./nlm_test_test_case_name

# Run all unit tests
./nlm_tests --category=unit

# Run all integration tests
./nlm_tests --category=integration
```

### Test Output

The test framework provides:

- **Summary**: Overall test results and statistics
- **Detailed results**: Individual test case results with timing
- **Error messages**: Detailed error information for failed tests
- **Log files**: Persistent test results in `tests/test_results.txt`

### Test Configuration

Configuration is managed through `tests/test_config.cfg`:

- **TEST_CATEGORY_UNIT**: Enable/disable unit tests (0/1)
- **TEST_CATEGORY_INTEGRATION**: Enable/disable integration tests (0/1)
- **TEST_CATEGORY_PERFORMANCE**: Enable/disable performance tests (0/1)
- **TEST_CATEGORY_STRESS**: Enable/disable stress tests (0/1)
- **TEST_OUTPUT_FORMAT**: Test output format (text, json, xml)
- **TEST_TIMEOUT**: Test timeout in seconds
- **TEST_VERBOSITY**: Verbosity level (0=quiet, 1=normal, 2=verbose)

## Test Development

### Adding New Test Cases

1. Create a new test file in the appropriate test directory
2. Include the `TestFramework.hpp` header
3. Implement test functions that return `true` for pass, `false` for fail
4. Register tests with the test framework

### Test Case Template

```cpp
#include "TestFramework.hpp"

// Test case implementation
TestSuite createUnitTests() {
    TestSuite suite("Example Unit Tests");
    
    suite.addTest(TestCase(
        "Example_Test",
        "Description of what this test does",
        TestCategory::Unit,
        []() {
            // Test implementation
            // Return true for pass, false for fail
            return true;
        }
    ));
    
    return suite;
}
```

### Test Categories

- **TestCategory::Unit**: Unit tests for individual components
- **TestCategory::Integration**: Integration tests for system interactions
- **TestCategory::Performance**: Performance and benchmark tests
- **TestCategory::Stress**: Stress and load tests

### Validation Callbacks

The configuration system supports validation callbacks:

```cpp
auto validator = [](const std::string& key, const nlm::ConfigValue& value) {
    nlm::ValidationError error;
    
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count < 1) {
                error = nlm::ValidationError(
                    nlm::ValidationResult::RangeViolation,
                    "Neuron count must be at least 1",
                    key, ">= 1", std::to_string(count)
                );
            }
        }
    }
    
    return error;
};

config->addValidator("neuron_count", validator);
```

## Configuration Testing

### Configuration Presets

The configuration system supports presets:

- **minimal**: Minimal configuration with small network
- **standard**: Standard configuration for most use cases
- **high_performance**: Optimized for performance
- **research**: Optimized for research scenarios

### Configuration Validation

Configuration validation includes:

- **Type validation**: Ensures values match expected types
- **Range validation**: Ensures numeric values are within bounds
- **Allowed values**: Validates against allowed value lists
- **Required fields**: Ensures required configuration parameters are present

### Configuration Templates

Configuration templates are available in `tests/config_templates/`:

- **minimal.cfg**: Minimal configuration template
- **standard.cfg**: Standard configuration template
- **research.cfg**: Research configuration template

## CLI Testing

The command-line interface can be tested using:

```bash
# Test CLI help
./nlm_cli help

# Test CLI status
./nlm_cli status

# Test CLI initialization
./nlm_cli init --neuron_count=100 --region_count=1

# Test CLI save/load
./nlm_cli save brain_state.bin
./nlm_cli load brain_state.bin
```

## Performance Testing

Performance tests measure:

- **Configuration loading time**: Time to load configuration from file
- **Brain initialization time**: Time to initialize the brain
- **Simulation performance**: Steps per second
- **Memory usage**: Memory consumption during operation
- **Scalability**: Performance with increasing network size

## Stress Testing

Stress tests evaluate:

- **Large networks**: Performance with thousands of neurons
- **High-frequency input**: Handling rapid input streams
- **Memory pressure**: Memory usage under extreme conditions
- **Resource limits**: Behavior under resource constraints

## Test Reporting

Test results are saved to:

- **`tests/test_results.txt`**: Plain text summary of all test results
- **`tests/test_results.json`**: JSON format for integration with CI/CD systems
- **`tests/test_results.xml`**: XML format for continuous integration

## Continuous Integration

The test framework is designed to work with continuous integration systems:

- **GitHub Actions**: Configure test workflows in `.github/workflows/`
- **Jenkins**: Use `junit` test reporting plugin
- **GitLab CI**: Use built-in test reporting

Example GitHub Actions workflow:

```yaml
name: Run Tests

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Build NLM
      run: |
        mkdir build
        cd build
        cmake ..
        make -j4
    - name: Run Tests
      run: |
        cd build
        ./nlm_tests
    - name: Upload Test Results
      uses: actions/upload-artifact@v2
      with:
        name: test-results
        path: tests/test_results.txt
```

## Troubleshooting

### Common Issues

1. **Configuration file not found**: Ensure test configuration files are in the correct location
2. **Test timeouts**: Increase `TEST_TIMEOUT` in `test_config.cfg`
3. **Memory issues**: Check memory limits and increase if necessary
4. **Compilation errors**: Ensure all dependencies are properly installed

### Debug Information

Enable verbose logging for debugging:
```bash
export NLM_LOG_LEVEL=DEBUG
./nlm_tests
```

### Test Failure Analysis

For detailed test failure analysis:

1. Check the detailed error messages in the test output
2. Review the `test_results.txt` file
3. Examine any crash dumps or core files
4. Run individual tests to isolate the problem

## Conclusion

The NLM test framework provides comprehensive testing capabilities for:

- **Unit testing**: Individual component validation
- **Integration testing**: System interaction verification
- **Performance testing**: Performance and scalability assessment
- **Stress testing**: System robustness under extreme conditions
- **Configuration testing**: Configuration validation and error handling

This framework ensures the NLM system is reliable, maintainable, and performs as expected in various scenarios.
