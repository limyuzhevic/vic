# NLM Testing Framework Guide

## Overview

This document provides comprehensive information about the NLM (Neural Learning Machine) Testing Framework, which includes extensive test coverage and validation capabilities for neural computation systems.

## Overview

The NLM Testing Framework is a comprehensive testing suite designed to ensure reliability, performance, and biological fidelity of neural computation systems. It includes:

1. **Enhanced Unit Testing Framework**: Template-based assertions, performance benchmarking, memory leak detection, and exception validation
2. **Integration Testing Framework**: System integration tests, cross-component validation, and real-time testing
3. **Performance Testing Framework**: Regression testing, profiling integration, load testing, and scalability analysis
4. **Validation and Verification Tools**: Configuration validation, brain validation, neural dynamics validation, and memory system validation
5. **Automated Testing Tools**: Automated test suite execution, coverage analysis, and continuous integration integration

## Key Components

### 1. TestFramework

**File**: `include/nlm/testing/TestFramework.hpp`

The core unit testing framework providing:

- Template-based assertions with detailed failure reporting
- Performance-aware testing with benchmark integration
- Memory leak detection and tracking
- Exception testing and validation
- Parameter-aware configuration testing
- Automatic test result aggregation

**Key Features**:
- Exception-safe resource management
- Thread-safe operation
- Comprehensive reporting
- Template-based assertion macros
- Performance benchmarking

**Usage Example**:
```cpp
#include "nlm/testing/TestFramework.hpp"

void testNeuronFunctionality() {
    nlm::Neuron neuron;
    neuron.initialize(100.0);
    
    ASSERT_TRUE(neuron.isActive(), "Neuron should be active");
    ASSERT_EQUALS(neuron.getMembranePotential(), 20.0, "0.1");
    
    neuron.spike();
    ASSERT_TRUE(neuron.hasFired(), "Neuron should have fired");
}

BENCHMARK("neuron_spike_benchmark", [&]() {
    nlm::Neuron neuron;
    neuron.initialize(100.0);
    neuron.spike();
});
```

### 2. IntegrationTestSuite

**File**: `include/nlm/testing/IntegrationTestSuite.hpp`

System integration testing framework providing:

- System integration testing for all NLM components
- Cross-component integration validation
- Performance integration testing
- Real-time system integration tests
- Plugin integration testing

**Key Features**:
- Component-agnostic testing architecture
- Dependency injection for test isolation
- State management for system-wide tests
- Performance-aware integration testing

**Usage Example**:
```cpp
#include "nlm/testing/IntegrationTestSuite.hpp"

void runIntegrationTests() {
    IntegrationTestSuite suite;
    
    suite.registerComponent("brain", ComponentStatus::READY);
    suite.registerComponent("memory", ComponentStatus::READY);
    suite.registerComponent("dynamics", ComponentStatus::READY);
    
    suite.registerIntegrationTest(
        "brain_memory_integration",
        "Test brain-memory communication",
        []() { return initializeBrainMemoryConnection(); },
        []() { return testBrainMemoryDataExchange(); },
        []() { cleanupBrainMemoryConnection(); }
    );
    
    suite.initializeSystem();
    suite.runIntegrationTests();
    suite.shutdownSystem();
}
```

### 3. PerformanceTestSuite

**File**: `include/nlm/testing/PerformanceTestSuite.hpp`

Performance testing framework providing:

- Performance regression testing with baselines
- Performance profiling integration
- Performance regression detection
- Load testing and stress testing
- Scalability testing

**Key Features**:
- Baseline-driven testing approach
- Statistical analysis of performance data
- Automated regression detection
- Multi-dimensional performance profiling

**Usage Example**:
```cpp
#include "nlm/testing/PerformanceTestSuite.hpp"

void runPerformanceTests() {
    PerformanceTestSuite suite;
    
    // Create baseline
    suite.createBaseline("performance_baseline", "Default performance baseline");
    suite.addBaselineMetric("cpu_usage", 50.0);
    suite.addBaselineMetric("memory_usage", 200.0);
    
    // Run benchmarks
    auto result = suite.benchmark("brain_step_small", []() {
        simulateBrainStep(100);
    }, 1000, "ms");
    
    // Compare with baseline
    suite.runRegressionTests();
    
    // Generate report
    suite.generatePerformanceReport();
}
```

### 4. ConfigurationTestSuite

**File**: `include/nlm/testing/ConfigurationTestSuite.hpp`

Configuration testing framework providing:

- Configuration validation testing
- Parameter-aware testing for configuration validation
- Configuration schema validation
- Configuration value range validation
- Configuration dependency validation

**Key Features**:
- Parameter-aware testing approach
- Schema-driven validation
- Dependency-aware validation
- Comprehensive error reporting

**Usage Example**:
```cpp
#include "nlm/testing/ConfigurationTestSuite.hpp"

void testConfigurations() {
    ConfigurationTestSuite suite;
    
    // Register configuration tests
    suite.registerConfigurationTest(
        "brain_config_valid",
        "Validate brain configuration parameters",
        {{"neuron_count", "100"}, {"region_count", "2"}}
    );
    
    suite.registerConfigurationTest(
        "memory_config_valid",
        "Validate memory configuration parameters", 
        {{"capacity_mb", "1024"}, {"retention_period_ms", "1000"}}
    );
    
    // Run tests
    suite.runConfigurationTests();
}
```

### 5. BrainValidationTestSuite

**File**: `include/nlm/testing/BrainValidationTestSuite.hpp`

Brain validation testing framework providing:

- Brain validation and verification
- Neural dynamics validation
- Brain state consistency validation
- Neural computation fidelity testing

**Key Features**:
- Biological fidelity validation
- Neural dynamics preservation
- State consistency validation
- Multi-region validation

**Usage Example**:
```cpp
#include "nlm/testing/BrainValidationTestSuite.hpp"

void validateBrain() {
    BrainValidationTestSuite suite;
    
    // Register validation tests
    suite.registerBrainValidationTest(
        "brain_state_valid",
        "Validate brain state consistency",
        [](const BrainStateSnapshot& snapshot) {
            return validateBrainState(snapshot);
        }
    );
    
    suite.registerNeuralDynamicsTest(
        "neural_dynamics_valid",
        "Validate neural dynamics measurements",
        [](const NeuralDynamicsMeasurement& dynamics) {
            return validateNeuralDynamics(dynamics);
        }
    );
    
    // Run validation
    suite.runBrainValidationTests();
}
```

### 6. MemTestFramework

**File**: `include/nlm/testing/MemTestFramework.hpp`

Memory leak detection framework providing:

- Memory leak detection
- Memory tracking
- Memory performance testing
- Memory stress testing

**Key Features**:
- Memory leak detection
- Memory allocation tracking
- Performance analysis
- Memory stress validation

**Usage Example**:
```cpp
#include "nlm/testing/MemTestFramework.hpp"

void testMemoryLeaks() {
    MemTestFramework suite;
    
    // Register memory tests
    suite.registerMemoryLeakTest(
        "memory_leak_basic",
        "Test for basic memory leaks",
        []() {
            // Allocate memory
            std::vector<double> data(1000000);
            std::fill(data.begin(), data.end(), 1.0);
        },
        10 * 1024 * 1024, // 10MB
        30.0 // 30 seconds timeout
    );
    
    // Run memory tests
    suite.runMemoryLeakTests();
    suite.analyzeMemoryLeaks();
}
```

### 7. BenchmarkSuite

**File**: `include/nlm/testing/BenchmarkSuite.hpp`

Benchmark suite for comprehensive performance measurements providing:

- Benchmark registration
- Benchmark execution
- Benchmark categorization
- Benchmark reporting
- Benchmark comparison

**Key Features**:
- Benchmark management
- Performance measurement
- Categorization support
- Detailed reporting

**Usage Example**:
```cpp
#include "nlm/testing/BenchmarkSuite.hpp"

void runBenchmarks() {
    BenchmarkSuite suite;
    
    // Register benchmarks
    suite.registerBenchmark(
        "brain_step_small",
        "Benchmark brain step with small network",
        []() { simulateBrainStep(100); },
        1000,
        "ms",
        "brain"
    );
    
    // Run benchmarks
    suite.runBenchmark("brain_step_small");
    suite.runAllBenchmarks();
    
    // Generate report
    suite.generateBenchmarkReport();
}
```

### 8. TestRunner

**File**: `include/nlm/testing/TestRunner.hpp`

Comprehensive test runner providing:

- Test suite management
- Test execution coordination
- Result aggregation
- Comprehensive reporting

**Key Features**:
- Test orchestration
- Result management
- Progress tracking
- Multiple output formats

**Usage Example**:
```cpp
#include "nlm/testing/TestRunner.hpp"

void runAllTests() {
    TestRunner runner;
    
    // Register test suites
    runner.registerUnitTestSuite(std::make_shared<TestSuiteManager>());
    runner.registerIntegrationTestSuite(std::make_shared<IntegrationTestSuite>());
    runner.registerPerformanceTestSuite(std::make_shared<PerformanceTestSuite>());
    
    // Run all tests
    runner.runAllTests();
    
    // Generate comprehensive report
    runner.generateComprehensiveReport();
}
```

### 9. CoverageReporter

**File**: `include/nlm/testing/CoverageReporter.hpp`

Test coverage analysis and reporting providing:

- Coverage collection
- Coverage calculation
- Coverage reporting
- Coverage validation
- Coverage comparison

**Key Features**:
- Coverage measurement
- Multiple coverage types
- HTML report generation
- Threshold validation

**Usage Example**:
```cpp
#include "nlm/testing/CoverageReporter.hpp"

void generateCoverageReport() {
    CoverageReporter reporter;
    
    // Record test executions
    reporter.recordTestExecution("test_brain_creation", {"brain.cpp:100", "brain.cpp:101"});
    reporter.recordFunctionCall("neuron_initialize", "test_brain_creation");
    
    // Generate report
    reporter.generateCoverageReport();
    
    // Export coverage data
    reporter.exportCoverageData("coverage_results.txt");
}
```

### 10. TestDataGenerator

**File**: `include/nlm/testing/TestDataGenerator.hpp`

Automated test data generation providing:

- Configuration data generation
- Neural data generation
- Performance data generation
- Plugin data generation
- Test data validation

**Key Features**:
- Automated data generation
- Multiple data types
- Data validation
- Data export

**Usage Example**:
```cpp
#include "nlm/testing/TestDataGenerator.hpp"

void generateTestData() {
    TestDataGenerator generator;
    
    // Generate configuration data
    auto configs = generator.generateConfigurationData("brain", 100);
    
    // Generate neural data
    auto neuralData = generator.generateNeuralData("spiking", 20, 100);
    
    // Generate performance data
    auto perfData = generator.generatePerformanceData("brain_step", 500);
    
    // Validate generated data
    bool isValid = generator.validateGeneratedData("configurations", configs);
    
    // Export test data
    generator.exportTestData("test_data.txt");
}
```

## Example Usage

### Basic Validation Workflow

The following example demonstrates a basic validation workflow using the NLM testing framework:

```cpp
#include "nlm/testing/TestRunner.hpp"

int main() {
    // Create test runner
    TestRunner runner;
    
    // Run unit tests
    runner.runTestSuites({"unit"});
    
    // Run integration tests
    runner.runTestSuites({"integration"});
    
    // Generate report
    runner.generateComprehensiveReport();
    
    return 0;
}
```

### Performance Validation Workflow

The following example demonstrates performance validation and regression testing:

```cpp
#include "nlm/testing/PerformanceTestSuite.hpp"

void validatePerformance() {
    PerformanceTestSuite suite;
    
    // Create performance baseline
    suite.createBaseline("performance_baseline", "Default performance baseline");
    
    // Run regression tests
    suite.runRegressionTests();
    
    // Compare with baseline
    suite.compareWithBaseline();
    
    // Generate performance report
    suite.generatePerformanceReport();
}
```

### Configuration Validation Workflow

The following example demonstrates configuration validation:

```cpp
#include "nlm/testing/ConfigurationTestSuite.hpp"

void validateConfiguration() {
    ConfigurationTestSuite suite;
    
    // Register configuration tests
    suite.registerConfigurationTest(
        "brain_config_valid",
        "Validate brain configuration parameters",
        {{"neuron_count", "100"}, {"region_count", "2"}}
    );
    
    // Run configuration tests
    suite.runConfigurationTests();
    
    // Validate consistency
    bool isConsistent = suite.validateConfigurationConsistency();
}
```

## Configuration

### Test Configuration File

Create a `kilo.json` file in your project directory to configure the testing framework:

```json
{
    "nlm": {
        "testing": {
            "framework": {
                "enable_unit_tests": true,
                "enable_integration_tests": true,
                "enable_performance_tests": true,
                "enable_configuration_tests": true,
                "enable_brain_validation_tests": true,
                "enable_memory_tests": true,
                "test_timeout": 30.0,
                "parallel_execution": false,
                "output_level": 1,
                "report_format": "text"
            },
            "performance": {
                "baseline_file": "baseline_perf.txt",
                "performance_threshold": 20.0,
                "load_test_duration": 60.0
            },
            "coverage": {
                "line_coverage_threshold": 0.8,
                "branch_coverage_threshold": 0.7,
                "statement_coverage_threshold": 0.9
            }
        }
    }
}
```

### Test Build Configuration

The NLM testing framework integrates with CMake. Add the following to your CMakeLists.txt:

```cmake
# Enable testing framework
option(NLM_ENABLE_TESTING "Enable NLM testing framework" ON)

if(NLM_ENABLE_TESTING)
    # Add testing framework sources
    add_subdirectory(src/testing)
    
    # Add test executables
    add_test(NAME nlm_unit_tests COMMAND nlm_unit_tests)
    add_test(NAME nlm_integration_tests COMMAND nlm_integration_tests)
    add_test(NAME nlm_performance_tests COMMAND nlm_performance_tests)
    add_test(NAME nlm_configuration_tests COMMAND nlm_configuration_tests)
    add_test(NAME nlm_brain_validation_tests COMMAND nlm_brain_validation_tests)
    add_test(NAME nlm_memory_tests COMMAND nlm_memory_tests)
endif()
```

## Running Tests

### Basic Test Execution

Run the NLM test suite using the following command:

```bash
./nlm_test_runner --help
```

For basic testing, run:

```bash
./nlm_test_runner --run-all
```

### Specific Test Suites

Run specific test suites:

```bash
./nlm_test_runner --run-suite unit
./nlm_test_runner --run-suite integration
./nlm_test_runner --run-suite performance
./nlm_test_runner --run-suite configuration
./nlm_test_runner --run-suite brain_validation
./nlm_test_runner --run-suite memory
```

### Performance Testing

Run performance testing with specific configurations:

```bash
./nlm_test_runner --run-performance --load-test
./nlm_test_runner --run-performance --stress-test
./nlm_test_runner --run-performance --scalability-test
```

### Configuration Testing

Run configuration testing:

```bash
./nlm_test_runner --run-configuration --validate-consistency
./nlm_test_runner --run-configuration --cross-component-validation
```

### Brain Validation

Run brain validation:

```bash
./nlm_test_runner --run-brain-validation --biological-fidelity
./nlm_test_runner --run-brain-validation --neural-dynamics
```

### Memory Testing

Run memory testing:

```bash
./nlm_test_runner --run-memory --memory-leak-detection
./nlm_test_runner --run-memory --memory-performance
```

### Output Formats

Generate reports in different formats:

```bash
./nlm_test_runner --output-format text --output-file test_report.txt
./nlm_test_runner --output-format xml --output-file test_report.xml
./nlm_test_runner --output-format json --output-file test_report.json
```

### Coverage Reporting

Generate coverage reports:

```bash
./nlm_test_runner --generate-coverage --output-dir coverage_report
./nlm_test_runner --coverage-thresholds --line-threshold 0.9 --branch-threshold 0.8
```

### Automated Testing

Run automated testing with test data generation:

```bash
./nlm_test_runner --automated-testing
./nlm_test_runner --test-data-generation --generate brain_configs --generate neural_data
```

## Python Examples

The `examples/testing/` directory contains Python examples for using the NLM testing framework:

### Automated Validation Examples

`examples/testing/auto_validation.py` - Demonstrates automated validation workflows including:
- Basic validation
- Performance validation
- Configuration validation
- Brain validation
- Memory validation
- Automated test data generation

### Performance Benchmark Examples

`examples/testing/performance_benchmarks.py` - Demonstrates performance benchmarking including:
- Benchmark suite creation
- Performance test suite
- Load testing
- Performance profiling
- Benchmark comparison
- Performance analysis

### Configuration Test Examples

`examples/testing/configuration_test.py` - Demonstrates configuration testing including:
- Configuration test suite creation
- Configuration schema usage
- Parameterized configuration tests
- Configuration validation
- Error detection
- Automated configuration testing

## Integration with CI/CD

### GitHub Actions

Add the following to your `.github/workflows/test.yml`:

```yaml
name: NLM Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Set up CMake
      uses: actions/setup-cmake@v1
      
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
    - name: Build tests
      run: cmake --build build --target all
      
    - name: Run unit tests
      run: ./build/nlm_unit_tests
      
    - name: Run integration tests
      run: ./build/nlm_integration_tests
      
    - name: Run performance tests
      run: ./build/nlm_performance_tests
      
    - name: Run configuration tests
      run: ./build/nlm_configuration_tests
      
    - name: Run brain validation tests
      run: ./build/nlm_brain_validation_tests
      
    - name: Run memory tests
      run: ./build/nlm_memory_tests
      
    - name: Generate coverage report
      run: ./build/generate_coverage --output-dir coverage_report
      
    - name: Upload coverage report
      uses: actions/upload-artifact@v2
      with:
        name: coverage-report
        path: coverage_report/
```

### GitLab CI

Add the following to your `.gitlab-ci.yml`:

```yaml
stages:
  - build
  - test
  - report

build:
  stage: build
  script:
    - cmake -B build -DCMAKE_BUILD_TYPE=Release
    - cmake --build build --target all
  only:
    - merge_requests
    - main

test_unit:
  stage: test
  script:
    - ./build/nlm_unit_tests
  only:
    - merge_requests
    - main

test_integration:
  stage: test
  script:
    - ./build/nlm_integration_tests
  only:
    - merge_requests
    - main

test_performance:
  stage: test
  script:
    - ./build/nlm_performance_tests
  only:
    - merge_requests
    - main

test_configuration:
  stage: test
  script:
    - ./build/nlm_configuration_tests
  only:
    - merge_requests
    - main

test_brain_validation:
  stage: test
  script:
    - ./build/nlm_brain_validation_tests
  only:
    - merge_requests
    - main

test_memory:
  stage: test
  script:
    - ./build/nlm_memory_tests
  only:
    - merge_requests
    - main

coverage:
  stage: report
  script:
    - ./build/generate_coverage --output-dir coverage_report
  artifacts:
    paths:
      - coverage_report/
  only:
    - merge_requests
    - main
```

## Usage in Real Projects

### Project Setup

1. **Add NLM Testing Framework to CMakeLists.txt**

2. **Configure Testing Options**

3. **Add Test Data Generation**

4. **Set Up Continuous Integration**

5. **Monitor Test Results**

### Performance Optimization

1. **Optimize Test Performance**
   - Use parallel execution for independent tests
   - Profile test performance
   - Identify and optimize slow tests

2. **Memory Management**
   - Monitor memory usage
   - Detect memory leaks
   - Optimize memory allocation

3. **Test Data Management**
   - Generate synthetic test data
   - Use parameterized tests
   - Validate test data quality

### Best Practices

1. **Test Organization**
   - Group related tests together
   - Use consistent naming conventions
   - Document test purpose and behavior

2. **Test Data Management**
   - Use automated test data generation
   - Validate test data before use
   - Keep test data separate from code

3. **Test Reporting**
   - Generate comprehensive test reports
   - Use multiple output formats
   - Integrate with CI/CD systems

4. **Test Maintenance**
   - Keep tests up-to-date with code changes
   - Remove obsolete tests
   - Update test data as needed

## Troubleshooting

### Common Issues

1. **Test Failures**
   - Check test configuration
   - Verify test environment
   - Review test output for error details

2. **Performance Issues**
   - Profile test execution
   - Identify slow tests
   - Optimize test execution

3. **Memory Issues**
   - Check for memory leaks
   - Monitor memory usage
   - Optimize memory allocation

4. **Configuration Issues**
   - Validate configuration files
   - Check required sections
   - Verify parameter ranges

### Debugging Tips

1. **Enable Verbose Output**
   ```bash
   ./nlm_test_runner --verbose --output-file debug.log
   ```

2. **Run Tests with Debug Information**
   ```bash
   ./nlm_test_runner --debug --run-suite unit
   ```

3. **Check System Resources**
   ```bash
   ./nlm_test_runner --check-resources --run-suite performance
   ```

## Contact

For questions, issues, or feature requests, please refer to the AGENTS.md file for contact information.

## License

The NLM Testing Framework is provided under the NLM license. See the LICENSE file for details.

## Acknowledgements

The NLM Testing Framework is developed by the NLM project team and is continuously improved through contributions from the open-source community.