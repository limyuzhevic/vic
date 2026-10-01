#!/usr/bin/env bash
"""
NLM Testing Framework

This directory contains comprehensive testing infrastructure for the Neural Learning Machine (NLM) framework.

Contents:
- tests/                     - Core test files
- tests/unit/                - Unit tests for individual components
- tests/integration/         - Integration tests for system testing
- tests/performance/         - Performance and benchmarking tests
- tests/regression/          - Regression tests for detecting changes
- tests/common/               - Common test utilities and fixtures

Testing Framework:
- Catch2 testing framework
- Google Test integration
- Custom test utilities
- Test reporting and analysis
- Test coverage analysis

Getting Started:
1. Install Catch2 if not already installed: sudo apt-get install catch2
2. Build the NLM framework with testing enabled: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON && make -j4
3. Run tests: ./nlm_tests or ctest
4. View test results: tests/report.html or cat tests/test_results.txt

Test Categories:
- Unit Tests: Individual component testing
- Integration Tests: System integration testing
- Performance Tests: Performance and benchmarking
- Regression Tests: Regression testing for changes
- Stress Tests: Stress and load testing

Testing Features:
- Automated test execution
- Test result reporting
- Test coverage analysis
- Performance benchmarking
- Comparison testing
- Regression detection
- Continuous integration integration

Testing Infrastructure:
- Test framework: Catch2
- Test utilities: Common test fixtures and helpers
- Test reporting: Detailed test results with pass/fail status
- Test analysis: Performance metrics and analysis
- Test integration: Integration with CI/CD pipelines
- Test documentation: Comprehensive test documentation

Test Organization:
- tests/unit/: Unit tests for core components (Config, Brain, Memory, etc.)
- tests/integration/: Integration tests for system components
- tests/performance/: Performance and benchmarking tests
- tests/regression/: Regression tests for detecting changes
- tests/common/: Common test utilities and fixtures

Test Writing Guidelines:
- Each test file should have a descriptive name
- Tests should be independent and repeatable
- Use descriptive test names and messages
- Include assertions for expected behavior
- Document test scenarios and edge cases
- Include performance benchmarks where appropriate

Running Tests:
- Build with tests enabled: cmake -DENABLE_TESTS=ON
- Run all tests: ctest -V
- Run specific test: ctest -R test_name
- Generate test report: ctest -T test
- Generate coverage report: cset -E

Test Categories:
- Unit Tests (unit/): Individual component testing
- Integration Tests (integration/): System integration testing
- Performance Tests (performance/): Performance and benchmarking
- Regression Tests (regression/): Regression testing for detecting changes
- Stress Tests (stress/): Stress and load testing

Test Examples:
- test_config.cpp: Configuration testing
- test_brain.cpp: Brain component testing
- test_memory.cpp: Memory system testing
- test_plasticity.cpp: Plasticity rule testing
- test_neuromodulation.cpp: Neuromodulation testing
- test_prediction.cpp: Prediction system testing
- test_visualization.cpp: Visualization testing
- test_benchmark.cpp: Benchmarking tests
- test_integration.cpp: Integration tests
- test_regression.cpp: Regression tests

Test Reporting:
- Console output: Test results in human-readable format
- HTML report: Interactive HTML test report
- XML report: Machine-readable test results
- JSON report: Structured test results for CI/CD
- Coverage report: Code coverage analysis
- Performance report: Performance benchmarks and metrics

Test Maintenance:
- Update tests when changes are made
- Add tests for new features
- Fix failing tests
- Review test quality and coverage
- Update test documentation

Testing Best Practices:
- Write focused, single-purpose tests
- Use descriptive test names
- Test both happy path and error conditions
- Use fixtures for common test scenarios
- Mock dependencies when appropriate
- Measure and report test performance
- Document test scenarios and edge cases

Test Automation:
- CI/CD integration for automated testing
- Nightly test runs
- Test result aggregation
- Performance trend analysis
- Regression detection
- Test coverage monitoring

Test Quality Assurance:
- Code coverage analysis
- Performance regression detection
- Security testing
- Compatibility testing
- Stress testing
- Load testing

Test Support Tools:
- Test framework: Catch2
- Test utilities: Custom test helpers
- Test reporting: Multiple output formats
- Test analysis: Performance and quality analysis
- Test comparison: Comparison between test runs
- Test history: Historical test data

Test Documentation:
- API documentation: Test API documentation
- Usage documentation: How to write tests
- Example tests: Working test examples
- Test guidelines: Best practices for testing
- Test troubleshooting: Common test issues and solutions

Test Integration:
- Integration with build system
- Integration with CI/CD pipelines
1. Welcome to the NLM Testing Framework

This document provides comprehensive information about the NLM testing infrastructure.

Table of Contents:
1. Overview
2. Testing Framework
3. Test Categories
4. Test Structure
5. Running Tests
6. Test Writing Guidelines
7. Test Analysis
8. Continuous Integration
9. Test Reporting
10. Test Maintenance
11. Best Practices
12. Troubleshooting
13. Future Enhancements

## 1. Overview

The NLM testing framework provides comprehensive testing infrastructure for the Neural Learning Machine framework. It includes unit tests, integration tests, performance tests, and regression tests to ensure the quality and reliability of the NLM system.

### Key Features:
- **Comprehensive Coverage**: Tests for all major components and systems
- **Automated Testing**: Automated test execution and reporting
- **Performance Testing**: Performance benchmarking and analysis
- **Regression Testing**: Detection of performance regressions
- **Continuous Integration**: Integration with CI/CD pipelines
- **Test Analysis**: Detailed test results and analysis
- **Documentation**: Comprehensive documentation and examples

## 2. Testing Framework

### Framework Overview:
The NLM testing framework is built on the Catch2 testing framework, which provides a simple, flexible, and powerful way to write tests.

### Framework Features:
- **Simple API**: Easy-to-use test framework with minimal boilerplate
- **Flexible**: Supports various testing scenarios and requirements
- **Powerful**: Advanced testing features and capabilities
- **Portable**: Cross-platform testing support
- **Documented**: Comprehensive documentation and examples

### Framework Components:
- **Test Framework**: Core testing framework
- **Test Utilities**: Common test utilities and fixtures
- **Test Reporting**: Test result reporting and analysis
- **Test Coverage**: Code coverage analysis
- **Test Comparison**: Comparison between test runs

## 3. Test Categories

### Unit Tests (unit/):
Unit tests for individual components and functions. These tests focus on testing specific components in isolation.

#### Common Unit Test Topics:
- Configuration testing (Config, Preset, Template)
- Brain component testing (Neuron, Synapse, Memory)
- Plasticity rule testing (STDP, Hebbian)
- Neuromodulation testing (Dopamine, Curiosity, Novelty)
- Prediction system testing
- Memory system testing (Working, Episodic, Associicative)
- Development system testing

### Integration Tests (integration/):
Integration tests for system components and their interactions. These tests focus on testing the integration between different components.

#### Common Integration Test Topics:
- Brain-environment interaction testing
- Agent brain integration testing
- System performance testing
- Memory system integration testing
- Neuromodulation system integration testing
- Development system integration testing

### Performance Tests (performance/):
Performance testing and benchmarking. These tests measure and compare the performance of different components and configurations.

#### Common Performance Test Topics:
- Simulation performance testing
- Memory usage testing
- CPU usage testing
- Network performance testing
- Scalability testing
- Optimization testing

### Regression Tests (regression/):
Regression testing for detecting performance regressions. These tests compare test results over time to detect performance degradation.

#### Common Regression Test Topics:
- Performance regression detection
- Functionality regression detection
- Memory usage regression detection
- Integration regression testing

## 4. Test Structure

### Test File Structure:
Each test file follows a consistent structure:

```cpp
// Test file template

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include "path/to/component_under_test.hpp"

// Test fixtures
struct TestFixture {
    // Setup
    void setup() {
        // Initialize test components
    }
    
    // Teardown
    void teardown() {
        // Clean up test components
    }
    
    // Test components
    std::shared_ptr<ComponentType> component;
};

// Test cases
TEST_CASE("Basic test", "[basic][smoke]") {
    TEST_FIXTURE(TestFixture, fixture) {
        // Test implementation
        REQUIRE(fixture.component != nullptr);
        // Add more test assertions
    }
}

TEST_CASE("Performance test", "[performance]") {
    TEST_FIXTURE(TestFixture, fixture) {
        // Performance test implementation
        auto start = std::chrono::high_resolution_clock::now();
        // Test code
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        REQUIRE(duration.count() < 1000); // Test should complete within 1 second
    }
}
```

### Test Categories:
Catch2 supports multiple test categories for organizing tests:

#### Test Categories:
- **Basic**: Basic functionality tests
- **Smoke**: Smoke tests (quick validation)
- **Performance**: Performance tests
- **Integration**: Integration tests
- **Regression**: Regression tests
- **Stress**: Stress tests
- **Memory**: Memory tests
- **Config**: Configuration tests
- **Unit**: Unit tests

### Test Fixtures:
Test fixtures provide a common context for test cases. They include setup and teardown methods.

## 5. Running Tests

### Prerequisites:
- Install Catch2 if not already installed
- Build the NLM framework with testing enabled

### Building with Tests:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
make -j4
```

### Running Tests:
```bash
# Run all tests
./nlm_tests

# Run tests with verbose output
./nlm_tests -s

# Run specific test by name
./nlm_tests -n "test_name"

# Run tests by category
./nlm_tests -c "basic"

# Run tests with JUnit XML output
./nlm_tests --reporter junit

# Run tests with HTML output
./nlm_tests --reporter html

# Run tests with JSON output
./nlm_tests --reporter json

# Use ctest for test management
cmake --build . --target test
ctest -V
ctest -R test_name
ctest -C basic
```

### Test Reporting:
```bash
# Generate test report
./nlm_tests --report-file test_report.html

# Generate JUnit XML report
./nlm_tests --reporter junit --out test_report.xml

# Generate JSON report
./nlm_tests --reporter json --out test_report.json

# Generate coverage report
./nlm_tests --coverage
```

## 6. Test Writing Guidelines

### Writing Good Tests:
1. **Be Focused**: Write tests that focus on specific functionality
2. **Be Descriptive**: Use descriptive test names and messages
3. **Be Reliable**: Make tests reliable and repeatable
4. **Be Complete**: Test both happy path and error conditions
5. **Be Maintainable**: Write tests that are easy to maintain

### Test Structure Best Practices:
```cpp
// Good test example
TEST_CASE("Component initialization", "[component][initialization]") {
    GIVEN("A component with valid parameters") {
        WHEN("The component is initialized") {
            Component component;
            REQUIRE(component.isInitialized());
        }
        
        WHEN("The component is not initialized") {
            Component component;
            REQUIRE_FALSE(component.isInitialized());
        }
    }
    
    GIVEN("A component with invalid parameters") {
        WHEN("The component is initialized with invalid parameters") {
            REQUIRE_THROWS_AS(Component("-invalid-param"), std::invalid_argument);
        }
    }
}
```

### Test Categories:
Use test categories to organize and filter tests:

```cpp
TEST_CASE("Basic test", "[basic][smoke]") { ... }
TEST_CASE("Performance test", "[performance]") { ... }
TEST_CASE("Integration test", "[integration]") { ... }
TEST_CASE("Regression test", "[regression]") { ... }
TEST_CASE("Stress test", "[stress]") { ... }
TEST_CASE("Memory test", "[memory]") { ... }
TEST_CASE("Config test", "[config]") { ... }
```

## 7. Test Analysis

### Test Metrics:
- **Test Count**: Total number of tests
- **Pass Rate**: Percentage of tests that pass
- **Execution Time**: Total execution time for all tests
- **Average Time**: Average execution time per test
- **Slowest Test**: Longest-running test
- **Fastest Test**: Fastest-running test
- **Test Categories**: Breakdown by test category

### Performance Analysis:
- **Performance Benchmarks**: Performance measurements for different components
- **Memory Usage**: Memory usage statistics
- **CPU Usage**: CPU usage statistics
- **Scalability**: Performance scaling with input size
- **Resource Usage**: Resource utilization analysis

### Regression Analysis:
- **Historical Performance**: Comparison with previous test runs
- **Performance Trends**: Trends in performance over time
- **Regression Detection**: Detection of performance regressions
- **Improvement Tracking**: Tracking of performance improvements

## 8. Continuous Integration

### CI/CD Integration:
- **Automated Testing**: Automated test execution on every commit
- **Test Result Reporting**: Test result reporting to CI/CD systems
- **Performance Monitoring**: Performance monitoring and alerting
- **Regression Detection**: Automatic detection of regressions
- **Test Coverage Monitoring**: Continuous monitoring of test coverage

### CI/CD Configuration:
```yaml
# GitHub Actions example
hosts:
  - name: Run tests
    run: |
      mkdir build
      cd build
      cmake .. -DENABLE_TESTS=ON
      make -j4
      ./nlm_tests --reporter junit --out test-results.xml

  - name: Upload test results
    uses: actions/upload-artifact@v2
    with:
      name: test-results
      path: test-results.xml
```

## 9. Test Reporting

### Report Formats:
- **Console Output**: Human-readable test results
- **HTML Report**: Interactive HTML test report
- **JUnit XML**: Machine-readable test results for CI/CD
- **JSON**: Structured test results
- **Coverage Report**: Code coverage analysis
- **Performance Report**: Performance benchmarks and metrics

### Report Features:
- **Test Results**: Detailed test results with pass/fail status
- **Test Statistics**: Summary statistics and metrics
- **Performance Analysis**: Performance benchmarks and analysis
- **Coverage Analysis**: Code coverage analysis
- **Historical Data**: Comparison with previous test runs
- **Filtering**: Filtering by test category, name, or status

## 10. Test Maintenance

### Updating Tests:
- **Update Tests**: Update tests when changes are made
- **Add Tests**: Add tests for new features
- **Fix Failing Tests**: Fix failing tests
- **Review Test Quality**: Review test quality and coverage
- **Update Documentation**: Update test documentation

### Test Maintenance Best Practices:
- **Version Control**: Commit test changes with meaningful messages
- **Code Review**: Review test changes with other developers
- **Test Quality**: Ensure tests are well-written and maintained
- **Test Coverage**: Maintain adequate test coverage
- **Test Performance**: Monitor test performance and optimize when necessary

## 11. Best Practices

### Writing Good Tests:
1. **Be Simple**: Write tests that are easy to understand
2. **Be Reliable**: Make tests reliable and repeatable
3. **Be Fast**: Make tests execute quickly
4. **Be Comprehensive**: Test all important scenarios
5. **Be Maintainable**: Write tests that are easy to maintain

### Test Design Patterns:
```cpp
// Fixture pattern
struct TestFixture {
    Component component;
    
    void setup() {
        component = Component();
    }
    
    void teardown() {
        // Cleanup
    }
};

// Parameterized test
struct ParameterizedTest : public TestFixture, public ::testing::WithParamInterface<int> {
    void testWithParam() {
        int param = GetParam();
        // Test with parameter
    }
};

// Exception test
TEST_CASE("Exception handling", "[exception]") {
    REQUIRE_THROWS_AS(FunctionThatThrows(), std::exception);
}

// Performance test
TEST_CASE("Performance test", "[performance]") {
    auto start = std::chrono::high_resolution_clock::now();
    // Test code
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    REQUIRE(duration.count() < 1000);
}
```

## 12. Troubleshooting

### Common Issues:
- **Test Compilation Errors**: Ensure all test dependencies are properly linked
- **Test Execution Failures**: Check for test logic errors
- **Test Performance Issues**: Optimize test performance
- **Test Coverage Issues**: Add tests to improve coverage
- **Test Integration Issues**: Fix CI/CD integration

### Troubleshooting Commands:
```bash
# Debug test compilation
g++ -std=c++20 -I./src -I/usr/include/Catch2 tests/test_file.cpp -o test_file

# Run test with debug information
./nlm_tests --debug

# Check test results
./nlm_tests --reporter json --out test_results.json

# Analyze test performance
./nlm_tests --performance-analysis

# Check test coverage
./nlm_tests --coverage
```

## 13. Future Enhancements

### Planned Enhancements:
- **Enhanced Test Framework**: More advanced testing features and capabilities
- **Test Automation**: Automated test generation and execution
- **AI-Powered Testing**: AI-powered test generation and optimization
- **Continuous Learning**: Automatic test improvement based on test results
- **Performance Monitoring**: Real-time performance monitoring during testing
- **Cloud Testing**: Cloud-based testing with distributed resources
- **Mobile Testing**: Mobile device testing support

### Research Directions:
- **Test-Driven Development**: Test-driven development for NLM
- **Property-Based Testing**: Property-based testing for NLM
- **Fuzz Testing**: Fuzz testing for NLM
- **Mutation Testing**: Mutation testing for NLM
- **Contract Testing**: Contract testing for NLM

## Conclusion

The NLM testing framework provides comprehensive testing infrastructure for the Neural Learning Machine framework. It includes unit tests, integration tests, performance tests, and regression tests to ensure the quality and reliability of the NLM system.

The testing framework supports:
- **Comprehensive Coverage**: Tests for all major components and systems
- **Automated Testing**: Automated test execution and reporting
- **Performance Testing**: Performance benchmarking and analysis
- **Regression Testing**: Detection of performance regressions
- **Continuous Integration**: Integration with CI/CD pipelines
- **Test Analysis**: Detailed test results and analysis

The NLM testing framework is essential for maintaining the quality and reliability of the NLM system. It ensures that all changes are properly tested and that the system continues to meet quality standards.

## Installation and Usage

### Building with Tests:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
make -j4
```

### Running Tests:
```bash
# Run all tests
./nlm_tests

# Run tests with verbose output
./nlm_tests -s

# Run specific test by name
./nlm_tests -n "test_name"

# Run tests by category
./nlm_tests -c "basic"

# Generate test report
./nlm_tests --report-file test_report.html
```

### Test Categories:
- **Basic**: Basic functionality tests
- **Smoke**: Smoke tests (quick validation)
- **Performance**: Performance tests
- **Integration**: Integration tests
- **Regression**: Regression tests
- **Stress**: Stress tests
- **Memory**: Memory tests
- **Config**: Configuration tests

The NLM testing framework is a comprehensive testing infrastructure that ensures the quality and reliability of the NLM system. It provides automated testing, performance benchmarking, regression detection, and continuous integration capabilities.
"""