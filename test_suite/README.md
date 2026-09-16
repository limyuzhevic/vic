# NLM Test Suite Build and Run Instructions

This document provides comprehensive instructions for building, running, and maintaining the NLM test suite.

## Overview

The NLM test suite is a comprehensive testing framework designed to validate all improvements made to the Neural Learning Machine (NLM) system. It includes:

- **Unit Tests**: Component-level testing of individual classes and functions
- **Integration Tests**: System component interaction and end-to-end testing
- **Advanced Features Tests**: Template-based configuration, parallel execution, real-time monitoring
- **Edge Case & Stress Tests**: Memory pressure, neural overload, configuration validation
- **Performance & Load Tests**: Memory usage, neural computation, configuration parsing speed

## Build Instructions

### Prerequisites

The NLM test suite requires:

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8 or higher (for Python bindings)
- Optional: Python packages for development

### Standard Build Process

```bash
# Navigate to the project root
cd /path/to/nlm

# Create build directory
mkdir -p build && cd build

# Configure with test suite support
cmake -DCMAKE_BUILD_TESTING=ON -DCMAKE_BUILD_INTEGRATION_TESTS=ON -DCMAKE_BUILD_ADVANCED_TESTS=ON -DCMAKE_BUILD_EDGE_CASE_TESTS=ON -DCMAKE_BUILD_PERFORMANCE_TESTS=ON ..

# Build the test suite
make -j$(nproc)
```

### Build Options

The test suite can be configured with various build options:

```bash
# Minimal build (core tests only)
cmake -DCMAKE_BUILD_TESTING=ON ..

# Full build (all test suites)
cmake -DCMAKE_BUILD_TESTING=ON -DCMAKE_BUILD_INTEGRATION_TESTS=ON -DCMAKE_BUILD_ADVANCED_TESTS=ON -DCMAKE_BUILD_EDGE_CASE_TESTS=ON -DCMAKE_BUILD_PERFORMANCE_TESTS=ON ..

# Build with specific test suites
cmake -DCMAKE_BUILD_TESTING=ON -DCMAKE_BUILD_INTEGRATION_TESTS=ON -DCMAKE_BUILD_PERFORMANCE_TESTS=ON ..

# Custom build directory
cmake -B custom_build_dir -S . -DCMAKE_BUILD_TESTING=ON
make -C custom_build_dir
```

### Python Bindings Build

The NLM project includes Python bindings for testing. To build and test them:

```bash
# Navigate to the project root
cd /path/to/nlm

# Install development dependencies (if not already installed)
pip install -e .[dev]

# Build Python bindings
python -m pip install -e .

# Run Python tests (if available)
pytest tests/
```

## Running Tests

### Test Execution

After building the test suite, you can run tests using the `nlm_test_suite` executable:

```bash
# Navigate to build directory
cd build

# Run all tests
./nlm_test_suite

# Run specific test suites
./nlm_test_suite --suite=unit
./nlm_test_suite --suite=integration
./nlm_test_suite --suite=advanced
./nlm_test_suite --suite=edge_case
./nlm_test_suite --suite=performance
```

### Test Suites

The test suite is organized into several categories:

#### Unit Tests
- **test_brain.cpp**: Brain creation, initialization, stepping, reset, regions, action production, status
- **test_clock.cpp**: Simulation clock functionality
- **test_config.cpp**: Configuration system tests
- **test_main.cpp**: Main application tests
- **test_neuron.cpp**: Neuron behavior and properties
- **test_random.cpp**: Random number generator tests
- **test_stdp.cpp**: Spike-Timing-Dependent Plasticity tests
- **test_synapse.cpp**: Synapse connection and transmission tests
- **test_types.cpp**: Type system and data structure tests

#### Integration Tests
- **test_phase6_integration.cpp**: Complete Phase 6 system integration
- **test_e2e_learning.cpp**: End-to-end learning scenarios
- **test_e2e_navigation.cpp**: Navigation and exploration testing
- **test_e2e_interaction.cpp**: Agent-environment interaction testing
- **test_prediction_memory_integration.cpp**: Prediction and memory system integration
- **test_attention_working_memory.cpp**: Attention and working memory interaction
- **test_neuromodulation_plasticity.cpp**: Neuromodulation and plasticity integration
- **test_performance_regression.cpp**: Performance regression testing
- **test_memory_usage.cpp**: Memory usage validation
- **test_processing_speed.cpp**: Processing speed validation

#### Advanced Features Tests
- **test_advanced_config_validation.cpp**: Advanced configuration validation
- **test_config_constraints.cpp**: Configuration constraints testing
- **test_config_security.cpp**: Configuration security and integrity
- **test_template_management.cpp**: Template lifecycle management
- **test_template_expansion.cpp**: Template variable expansion
- **test_template_inheritance.cpp**: Template inheritance and overriding
- **test_parallel_safety.cpp**: Thread safety testing
- **test_parallel_execution.cpp**: Parallel execution performance
- **test_concurrent_access.cpp**: Concurrent access testing
- **test_batch_processing.cpp**: Batch processing operations
- **test_monitoring_system.cpp**: Real-time monitoring system
- **test_performance_monitoring.cpp**: Performance monitoring
- **test_alert_system.cpp**: Alert and notification system

#### Edge Case & Stress Tests
- **test_memory_exhaustion.cpp**: Memory exhaustion scenarios
- **test_memory_pressure.cpp**: System under memory pressure
- **test_memory_leaks.cpp**: Memory leak detection
- **test_neural_overload.cpp**: Neural network overload testing
- **test_extreme_conditions.cpp**: Extreme neural conditions
- **test_config_edge_cases.cpp**: Configuration validation edge cases
- **test_config_validation_extremes.cpp**: Extreme configuration values
- **test_config_corruption.cpp**: Configuration corruption handling
- **test_exception_safety.cpp**: Exception safety testing
- **test_error_recovery.cpp**: Error recovery mechanisms
- **test_graceful_degradation.cpp**: Graceful degradation behavior

#### Performance & Load Tests
- **test_memory_usage_profiling.cpp**: Comprehensive memory usage profiling
- **test_memory_efficiency.cpp**: Memory efficiency testing
- **test_memory_patterns.cpp**: Memory access patterns testing
- **test_neural_computation_speed.cpp**: Neural computation speed testing
- **test_neural_accuracy.cpp**: Neural network accuracy validation
- **test_neural_optimization.cpp**: Neural network optimization testing
- **test_config_parsing_speed.cpp**: Configuration parsing performance
- **test_config_validation_speed.cpp**: Configuration validation speed
- **test_config_complexity.cpp**: Configuration complexity testing
- **test_scalability_neural_networks.cpp**: Neural network scalability
- **test_scalability_simulation.cpp**: Simulation scalability
- **test_scalability_memory.cpp**: Memory scalability

### Test Configuration

The test suite supports various command-line options for configuring test execution:

```bash
# Run tests with specific options
./nlm_test_suite --suite=unit --iterations=100 --precision=double --memory-limit=8

# Run integration tests with stress testing
./nlm_test_suite --suite=integration --stress-level=high --enable-recovery-testing

# Run advanced features tests with monitoring
./nlm_test_suite --suite=advanced --enable-parallel-testing --enable-monitoring

# Run edge case tests with exception monitoring
./nlm_test_suite --suite=edge_case --enable-exception-monitoring --test-timeout=300

# Run performance tests with benchmarking
./nlm_test_suite --suite=performance --enable-benchmarking --performance-threshold=0.95
```

### Test Output

The test suite generates various output formats:

#### Console Output
Real-time progress display with test results:

```
Running NLM Test Suite...
Suite: unit
  test_brain.cpp: 10 tests - PASSED (100%)
  test_config.cpp: 5 tests - PASSED (100%)
Suite: integration
  test_phase6_integration.cpp: 3 tests - PASSED (100%)
  test_e2e_learning.cpp: 2 tests - PASSED (100%)
Suite: advanced
  test_advanced_config_validation.cpp: 4 tests - PASSED (100%)
  test_parallel_safety.cpp: 3 tests - PASSED (100%)

Total Tests: 47
Total Passed: 47
Total Failed: 0
Success Rate: 100%
```

#### Detailed Reports

The test suite can generate detailed reports in various formats:

```bash
# Generate HTML report
./nlm_test_suite --output=html --output-file=test_report.html

# Generate JSON report for CI/CD
./nlm_test_suite --output=json --output-file=test_results.json

# Generate XML JUnit report
./nlm_test_suite --output=xml --output-file=junit_results.xml

# Generate both HTML and JSON reports
./nlm_test_suite --output=html,json --output-dir=reports/
```

### Performance Analysis

The test suite includes performance analysis capabilities:

```bash
# Run performance analysis with memory profiling
./nlm_test_suite --suite=performance --profile-memory --precision=double

# Run performance analysis with neural benchmarking
./nlm_test_suite --suite=advanced --enable-neural-benchmarking

# Run performance regression testing
./nlm_test_suite --suite=integration --enable-performance-validation

# Run comprehensive performance analysis
./nlm_test_suite --suite=all --enable-benchmarking --enable-profiling --enable-monitoring
```

## Test Suite Management

### Running Individual Test Files

You can run individual test files directly:

```bash
# Navigate to build directory
cd build

# Run specific test file
./nlm_test_suite --test-file=test_brain.cpp --suite=unit
./nlm_test_suite --test-file=test_phase6_integration.cpp --suite=integration
./nlm_test_suite --test-file=test_advanced_config_validation.cpp --suite=advanced
```

### Test Filtering

The test suite supports filtering tests by various criteria:

```bash
# Filter tests by name pattern
./nlm_test_suite --suite=unit --filter="test_brain"

# Filter tests by performance category
./nlm_test_suite --suite=performance --filter="neural_computation"

# Filter tests by memory category
./nlm_test_suite --suite=edge_case --filter="memory"

# Filter tests by configuration category
./nlm_test_suite --suite=advanced --filter="config"
```

### Test Isolation

Each test suite runs independently:

```bash
# Run only unit tests
./nlm_test_suite --suite=unit

# Run only integration tests
./nlm_test_suite --suite=integration

# Run only advanced features tests
./nlm_test_suite --suite=advanced

# Run only edge case and stress tests
./nlm_test_suite --suite=edge_case

# Run only performance and load tests
./nlm_test_suite --suite=performance

# Run all test suites
./nlm_test_suite --suite=all
```

## Test Development

### Adding New Tests

To add new tests to the NLM test suite:

1. **Create Test File**: Create a new test file in the appropriate test directory (`src/unit_test/`, `src/integration_test/`, etc.)
2. **Follow Conventions**: Follow the existing test patterns and conventions
3. **Add Test Cases**: Implement test cases following the existing format
4. **Update Documentation**: Update relevant documentation files
5. **Build and Test**: Build the test suite and verify new tests work correctly

### Test Case Template

```cpp
// Example test case template
#include "test/TestBase.hpp"

namespace test_examples {

class ExampleTest : public TestBase {
protected:
    void SetUp() override {
        // Initialize test fixtures
        TestBase::SetUp();
    }
    
    void TearDown() override {
        // Clean up test fixtures
        TestBase::TearDown();
    }
    
public:
    void testBasicFunctionality() {
        // Test basic functionality
        EXPECT_TRUE(true);
    }
    
    void testEdgeCase() {
        // Test edge case
        EXPECT_EQ(1 + 1, 2);
    }
    
    void testErrorCondition() {
        // Test error condition
        EXPECT_THROW(divideByZero(), std::runtime_error);
    }
};

} // namespace test_examples
```

## Test Maintenance

### Regular Maintenance Tasks

1. **Update Test Dependencies**: Ensure test dependencies are up to date
2. **Review Test Results**: Regularly review test results for regressions
3. **Update Performance Baselines**: Update performance benchmarks as system improves
4. **Clean Up Test Files**: Remove obsolete or redundant test files
5. **Update Documentation**: Keep documentation current with test changes

### Troubleshooting

#### Common Issues and Solutions

1. **Build Failures**
   - **Issue**: CMake configuration errors
   - **Solution**: Check CMakeLists.txt files for syntax errors, ensure required dependencies are installed

2. **Test Failures**
   - **Issue**: Tests failing unexpectedly
   - **Solution**: Check test output for specific failure messages, run tests with verbose output

3. **Performance Issues**
   - **Issue**: Tests taking too long to run
   - **Solution**: Reduce test iteration count, disable performance profiling

4. **Memory Issues**
   - **Issue**: Tests running out of memory
   - **Solution**: Increase memory limit, reduce test load level

5. **Configuration Issues**
   - **Issue**: Configuration parsing errors
   - **Solution**: Check configuration file syntax, validate configuration against schema

#### Diagnostic Commands

```bash
# Check system information
./nlm_test_suite --system-info

# Run tests with verbose output
./nlm_test_suite --verbose --suite=all

# Run tests with debug output
./nlm_test_suite --debug --suite=all

# Run tests with performance profiling
./nlm_test_suite --profile --suite=all

# Check test results summary
./nlm_test_suite --summary

# Validate test configuration
./nlm_test_suite --validate-config
```

## Continuous Integration

The NLM test suite is designed for integration with continuous integration systems:

### GitHub Actions

Create a `.github/workflows/test.yml` file:

```yaml
name: NLM Test Suite

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      uses: actions/setup-cpp@v1
      with:
        cmake-version: '3.16'
        cpp-version: '20'
    
    - name: Create build directory
      run: mkdir -p build
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TESTING=ON
    
    - name: Build test suite
      run: cmake --build build -- -j$(nproc)
    
    - name: Run tests
      run: ./build/nlm_test_suite
    
    - name: Upload test results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results
        path: build/test_results.json
```

### Jenkins Pipeline

Create a Jenkins pipeline script:

```groovy
pipeline {
    agent any
    
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        
        stage('Build') {
            steps {
                sh 'mkdir -p build'
                sh 'cmake -B build -DCMAKE_BUILD_TESTING=ON'
                sh 'cmake --build build -- -j${env.NUMBER_OF_PARALLEL_BUILDS}'
            }
        }
        
        stage('Test') {
            steps {
                sh './build/nlm_test_suite'
            }
        }
    }
    
    post {
        always {
            archiveArtifacts artifacts: 'build/test_results.html', allowEmpty: true
        }
    }
}
```

## Test Suite Configuration

### Configuration Files

The test suite supports various configuration files:

- **`config/test_config.json`**: Test suite configuration
- **`config/test_defaults.json`**: Default test configuration
- **`config/performance_config.json`**: Performance testing configuration
- **`config/stress_config.json`**: Stress testing configuration

### Environment Variables

The test suite can be configured using environment variables:

```bash
# Test configuration
export NLM_TEST_SUITE_CONFIG=/path/to/config.json
export NLM_TEST_SUITE_ITERATIONS=100
export NLM_TEST_SUITE_PRECISION=double

# Performance configuration
export NLM_TEST_SUITE_MEMORY_LIMIT=8
export NLM_TEST_SUITE_PERFORMANCE_THRESHOLD=0.95

# Stress testing configuration
export NLM_TEST_SUITE_STRESS_LEVEL=high
export NLM_TEST_SUITE_ENABLE_RECOVERY=true

# Output configuration
export NLM_TEST_SUITE_OUTPUT_FORMAT=json
export NLM_TEST_SUITE_OUTPUT_FILE=reports/test_results.json
```

## Version Control

### Git Integration

The NLM test suite includes git integration for tracking test changes:

```bash
# Stage test files for commit
git add src/test/
git add src/integration_test/
git add src/advanced_test/
git add src/edge_case_test/
git add src/performance_test/

# Commit test changes with descriptive message
git commit -m "Add comprehensive test suite for NLM improvements

- Add unit tests for all new functionality
- Add integration tests for system components
- Add advanced features tests for configuration and parallel execution
- Add edge case and stress tests for robustness validation
- Add performance and load tests for scalability analysis

Co-authored-by: openhands <openhands@all-hands.dev>"

# Push to remote repository
git push origin main
```

### Test Results Tracking

Track test results over time:

```bash
# Create test results report
./nlm_test_suite --output=html,json --output-dir=reports/

# Generate performance trend analysis
./nlm_test_suite --analyze-trends --output=performance_trends.json

# Compare with previous test results
./nlm_test_suite --compare-results --baseline=reports/test_results_baseline.json --output=comparison_report.html
```

## Additional Resources

### Documentation

- **NLM Documentation**: https://nlm.readthedocs.io
- **API Reference**: https://nlm.readthedocs.io/api
- **Tutorial**: https://nlm.readthedocs.io/tutorial

### Examples

- **Test Examples**: `/path/to/nlm/test_suite/examples/`
- **Sample Configurations**: `/path/to/nlm/config/`
- **Sample Scripts**: `/path/to/nlm/scripts/`

### Support

For test suite issues, contact:

- **Issue Tracker**: https://github.com/nlm-project/nlm/issues
- **Community Forum**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

## Legal and Copyright

The NLM test suite is part of the Neural Learning Machine (NLM) project, released under the MIT License.

Copyright (c) 2026 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.