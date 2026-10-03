# NLM Test Framework Documentation

This document provides comprehensive information about the new extensible test framework for the Neural Learning Machine (NLM) system.

## Overview

The NLM Test Framework replaces the previous hardcoded, basic test system with a flexible, configurable, and extensible testing solution. It addresses all the limitations identified in the original framework:

### Original Framework Issues (Solved):
1. ✅ **Hardcoded Tests**: Now uses a factory pattern for dynamic test creation
2. ✅ **No Configuration**: Supports JSON/YAML configuration files
3. ✅ **No Test Categories**: Tests are organized into logical categories
4. ✅ **No Test Results Aggregation**: Comprehensive result collection and analysis
5. ✅ **No Test Scheduling**: Advanced scheduling and dependency management
6. ✅ **No Test Dependencies**: Support for test dependencies and ordering
7. ✅ **No Test Parameters**: Parameterized testing with multiple scenarios
8. ✅ **No External Test Support**: Extensible factory pattern for external tests
9. ✅ **No Test Reporting**: Detailed reporting in multiple formats
10. ✅ **No Performance Measurement**: Comprehensive performance metrics collection

## Core Framework Components

### 1. TestBase Class (`src/test/TestFramework.hpp`)

Base class for all tests. Provides:

- **Test Metadata**: Name, description, severity, category, tags
- **Execution Control**: Setup/teardown, skipping, timeout management
- **Parameter Support**: Parameterized testing capabilities
- **Dependency Management**: Test dependencies and ordering
- **Performance Metrics**: Execution time, memory usage, CPU usage

```cpp
class TestBase {
public:
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestStatus execute() = 0;
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual bool shouldSkip() const { return false; }
};
```

### 2. TestSuite Class (`src/test/TestSuite.hpp`)

Groups related tests with filtering capabilities:

- **Test Organization**: Logical grouping of tests
- **Filtering**: Apply filters to select specific tests
- **Execution**: Batch execution of test suites
- **Metadata**: Suite name, description, configuration

```cpp
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description);
    void addTest(std::unique_ptr<TestBase> test);
    void setFilter(const std::function<bool(const TestBase&)>& filter);
    const std::vector<std::unique_ptr<TestBase>>& getTests() const;
};
```

### 3. TestRegistry (`src/test/TestFramework.hpp`)

Manages test discovery and registration:

- **Central Repository**: All tests and suites stored centrally
- **Discovery**: Automatic test discovery from files
- **Retrieval**: Quick lookup by test name
- **Management**: Clear and manage test registrations

```cpp
class TestRegistry {
public:
    static TestRegistry& getInstance();
    void registerTest(std::unique_ptr<TestBase> test);
    void registerSuite(std::unique_ptr<TestSuite> suite);
    std::unique_ptr<TestBase> getTest(const std::string& name);
};
```

### 4. TestRunner (`src/test/TestRunner.hpp`)

Executes tests and collects results:

- **Sequential Execution**: Default test execution mode
- **Parallel Execution**: Concurrent test execution
- **Timeout Management**: Automatic test timeout handling
- **Result Aggregation**: Comprehensive results collection
- **Reporting**: Multiple output formats (JSON, XML, CSV)

```cpp
class TestRunner {
public:
    std::vector<TestResult> runTests(const std::vector<std::unique_ptr<TestBase>>& tests);
    std::vector<TestResult> runSuites(const std::vector<std::unique_ptr<TestSuite>>& suites);
    double calculatePassRate(const std::vector<TestResult>& results) const;
    void printResults(const std::vector<TestResult>& results) const;
};
```

### 5. TestConfiguration (`src/test/TestConfiguration.hpp`)

Configuration-driven test setup:

- **JSON/YAML Support**: Load configuration from files
- **Test Suite Configuration**: Configure test suites, parameters, fixtures
- **Environment-Specific**: Different configurations for different environments
- **Validation**: Configuration validation and error reporting
- **Application**: Apply configuration to test instances

```cpp
class TestConfiguration {
public:
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    TestSuiteConfig* getSuiteConfig(const std::string& name);
    void applyToTest(TestBase& test) const;
};
```

### 6. TestFactory (`src/test/TestFactory.hpp`)

Dynamic test creation and registration:

- **Factory Pattern**: Create tests dynamically at runtime
- **Registration Macros**: Easy test registration with macros
- **Metadata**: Rich test metadata for discovery and documentation
- **Discovery**: Automatic test discovery from source files
- **Export**: Export test information for analysis

```cpp
// Registration macros
#define REGISTER_TEST(name, testClass)
#define REGISTER_SUITE(name, suiteClass)

// Usage example:
REGISTER_TEST(testClockCreation, TestClockCreation)
REGISTER_SUITE(core_tests, CoreTestSuite)
```

## Advanced Test Features

### Test Categories

Tests are organized into logical categories:

- **Memory**: Memory system and allocation tests
- **Plasticity**: Plasticity rule and mechanism tests
- **Neuromodulation**: Neuromodulator and signal tests
- **Development**: Development system and maturation tests
- **Integration**: Integration and end-to-end tests

### Test Parameters

Parameterized testing with multiple scenarios:

```cpp
struct TestParameter {
    std::string name;
    std::any value;
    std::string type;
    std::vector<std::any> possibleValues;
};
```

### Test Dependencies

Define test dependencies and ordering:

```cpp
struct TestDependency {
    std::string testName;
    bool mustPass{false};
    std::string description;
};
```

### Test Fixtures

Setup and teardown for test environments:

```cpp
struct TestFixtureConfig {
    std::string name;
    std::string type; // "setup", "teardown", "environment"
    std::string description;
    std::string code;
    std::chrono::nanoseconds timeout{std::chrono::seconds(10)};
};
```

## Configuration-Driven Testing

### Configuration File Formats

#### JSON Configuration (`examples/test_suite.json`)

```json
{
  "testSuite": {
    "name": "NLM Core Tests",
    "description": "Core NLM system tests",
    "category": "core",
    "enabled": true,
    "timeout": 30,
    "parallel": false
  },
  "testSuites": [
    {
      "name": "clock_tests",
      "description": "Simulation clock and timing tests",
      "category": "core",
      "enabled": true,
      "timeout": 30,
      "parameters": {
        "timestep": 0.001,
        "maxSteps": 1000
      }
    }
  ],
  "testParameters": {
    "defaultTimestep": 0.001,
    "maxIterations": 1000
  }
}
```

#### YAML Configuration (`examples/test_suite.yaml`)

```yaml
testSuite:
  name: "NLM Core Tests"
  description: "Core NLM system tests"
  category: "core"
  enabled: true
  timeout: 30
  parallel: false

testSuites:
- name: "clock_tests"
  description: "Simulation clock and timing tests"
  category: "core"
  enabled: true
  timeout: 30
  parameters:
    timestep: 0.001
    maxSteps: 1000
```

### Phase-Specific Configuration (`examples/test_phases.json`)

Configuration for different test phases:

```json
{
  "phaseName": "Phase 2: Real Neural Computation Tests",
  "description": "Core neural computation and brain simulation tests",
  "testPhases": {
    "initialization": {
      "name": "System Initialization",
      "enabled": true,
      "timeout": 30,
      "tests": ["testClockCreation", "testConfigCreation"]
    },
    "neuron_system": {
      "name": "Neuron System Tests",
      "enabled": true,
      "timeout": 60,
      "dependencies": ["initialization"],
      "tests": ["testNeuronCreation", "testNeuronMembranePotential"]
    }
  }
}
```

## Test Reporting and Analysis

### Result Aggregation

The TestRunner collects comprehensive test results:

```cpp
struct TestResult {
    std::string testName;
    std::string suiteName;
    TestStatus status;
    TestMetrics metrics;
    std::string errorMessage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<std::string> tags;
    std::string category;
    size_t parameterIndex{0};
};
```

### Performance Metrics

Collects detailed performance metrics:

- **Execution Time**: Individual and aggregate test timing
- **Memory Usage**: Memory consumption tracking
- **CPU Usage**: Processing time metrics
- **Throughput**: Tests per second metrics
- **Resource Usage**: System resource monitoring

### Reporting Formats

- **Console**: Human-readable summary
- **JSON**: Structured data format
- **XML**: Machine-readable format
- **CSV**: Spreadsheet-compatible format

## Integration Points

### Integration with Existing Tests

The framework integrates with existing test files (`tests/test_*.cpp`):

1. **Backward Compatibility**: Existing test functions continue to work
2. **Factory Registration**: Existing tests can be registered via factory
3. **Configuration Migration**: Existing test suites can be migrated to configuration

### Integration with NLM Components

- **Config System**: Uses NLM configuration system for test configuration
- **Logger System**: Integrated with NLM logging system for output
- **Brain Simulation**: Can test brain simulation components
- **Plugin System**: Tests plugin loading and functionality

### Integration with Configuration System

- **Schema Validation**: Uses NLM configuration schema validation
- **Environment Configuration**: Different configurations for development, testing, production
- **Parameter Override**: Override test parameters via configuration

### Integration with Logging System

- **Structured Logging**: Uses NLM structured logging for test output
- **Performance Logging**: Log performance metrics for analysis
- **Error Logging**: Detailed error logging for debugging

## Advanced Features

### Test Parallelization

Run independent tests in parallel:

```cpp
// Configuration
TestConfiguration config;
config.name = "Parallel Test Suite";
config.parallel = true;
config.parallelThreads = 4;

// Execution
TestRunner runner;
runner.setConfiguration(config);
auto results = runner.runTests(tests);
```

### Test Retry

Retry failed tests with different parameters:

- Automatic retry on failure
- Configurable retry attempts
- Different parameter values for retry
- Backoff strategy for repeated failures

### Test Batching

Group related tests for efficiency:

- Batch processing of related tests
- Resource optimization for batched tests
- Reduced overhead for grouped tests
- Parallel execution within batches

### Test Monitoring

Monitor test execution progress:

- Real-time progress reporting
- Resource usage monitoring
- Test completion tracking
- Alert on long-running tests

## Command-Line Tools

### NLM Test Runner (`tools/nlm_test_runner.py`)

Command-line interface for running tests:

```bash
./tools/nlm_test_runner.py --help

Usage:
  nlm_test_runner.py [options]

Options:
  --config FILE          Configuration file path
  --test TEST           Specific test to run
  --timeout SECONDS     Test timeout in seconds
  --parallel            Enable parallel test execution
  --threads N           Number of parallel threads
  --output FORMAT       Output format (console, json, xml, csv, none)
  --output-file FILE    Output file path
  --verbose             Enable verbose output
  --environment ENV     Test environment (development, testing, production)
  --fail-fast           Stop on first failure
```

Example usage:

```bash
# Run all tests with JSON output
./tools/nlm_test_runner.py --output json --output-file results.json

# Run specific tests in parallel
./tools/nlm_test_runner.py --test testClockCreation testConfigCreation --parallel --threads 4

# Run tests with custom configuration
./tools/nlm_test_runner.py --config examples/test_suite.json --verbose
```

### NLM Test Analyzer (`tools/test_analyzer.py`)

Analyze test results for insights:

```bash
./tools/test_analyzer.py --help

Usage:
  test_analyzer.py [options]

Options:
  RESULTS_FILE          Test results file path
  --report FILE         Analysis report file path
  --verbose             Enable verbose output
```

### NLM Test Coverage (`tools/test_coverage.py`)

Analyze test coverage for source files:

```bash
./tools/test_coverage.py --help

Usage:
  test_coverage.py [options]

Options:
  SOURCE_DIR           Source code directory path
  RESULTS_FILE         Test results file path
  --report FILE        Coverage report file path
  --verbose            Enable verbose output
```

## Usage Examples

### Basic Test Execution

```bash
# Run all tests with default configuration
./tools/nlm_test_runner.py

# Run tests with custom configuration
./tools/nlm_test_runner.py --config examples/test_suite.json

# Run specific tests
./tools/nlm_test_runner.py --test testClockCreation testConfigCreation testNeuronCreation
```

### Advanced Test Execution

```bash
# Run tests in parallel with JSON output
./tools/nlm_test_runner.py --parallel --threads 4 --output json --output-file results.json

# Run tests with environment-specific configuration
./tools/nlm_test_runner.py --environment testing

# Run tests and fail fast on first failure
./tools/nlm_test_runner.py --fail-fast
```

### Test Analysis

```bash
# Analyze existing test results
./tools/test_analyzer.py results.json --report analysis_report.txt

# Analyze with verbose output
./tools/test_analyzer.py results.json --verbose
```

### Coverage Analysis

```bash
# Analyze coverage for the source directory
./tools/test_coverage.py src --results results.json --report coverage_report.json

# Analyze with verbose output
./tools/test_coverage.py src results.json --verbose
```

## Migration Guide

### Migrating from Legacy Tests

1. **Create Test Classes**: Convert existing test functions to TestBase subclasses
2. **Register Tests**: Use factory macros to register tests
3. **Create Configuration**: Migrate test suites to configuration files
4. **Update Test Runner**: Update test runner configuration
5. **Run Tests**: Execute tests using the new framework

### Example Migration

Legacy test:

```cpp
// tests/test_clock.cpp
namespace test_clock {
void testClockCreation() {
    nlm::SimulationClock clock(0.001);
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    assert(clock.getTimestep() == 0.001);
    std::cout << "    testClockCreation passed" << std::endl;
}
}
```

Migrated test:

```cpp
// src/test/TestClock.cpp
#include "TestFramework.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {
namespace test {

class TestClockCreation : public TestBase {
public:
    std::string getName() const override { return "testClockCreation"; }
    std::string getDescription() const override { return "Test clock creation and initialization"; }
    std::string getCategory() const override { return "core"; }
    TestStatus execute() override {
        nlm::SimulationClock clock(0.001);
        assert(clock.getStep() == 0);
        assert(clock.getTime() == 0.0);
        assert(clock.getTimestep() == 0.001);
        setMetrics({std::chrono::milliseconds(10), 0, 0.0, true, ""});
        return TestStatus::PASSED;
    }
};

} // namespace test
} // namespace nlm

// Register test
REGISTER_TEST(testClockCreation, TestClockCreation)
```

## Development Guide

### Adding New Test Categories

1. **Create Test Headers**: Create new test category headers in `src/test/`
2. **Implement Test Classes**: Implement test classes for the new category
3. **Register Tests**: Register tests using factory macros
4. **Add to Configuration**: Add configuration for the new category
5. **Test Integration**: Integrate with existing test infrastructure

### Example: Adding Memory Tests

```cpp
// src/test/TestMemory.hpp
#pragma once
#include "TestFramework.hpp"

namespace nlm {
namespace test {

class TestMemoryCreation : public TestBase {
public:
    std::string getName() const override { return "TestMemoryCreation"; }
    std::string getDescription() const override { return "Test memory system creation"; }
    std::string getCategory() const override { return "Memory"; }
    TestStatus execute() override {
        // Implement memory creation test
        return TestStatus::PASSED;
    }
};

// Other memory tests...

} // namespace test
} // namespace nlm

// src/test/TestMemory.cpp
#include "TestMemory.hpp"

namespace nlm {
namespace test {

// Implement test methods...

} // namespace test
} // namespace nlm

// Register tests
REGISTER_TEST(TestMemoryCreation, TestMemoryCreation)
```

### Configuration Examples

Create configuration files for different testing scenarios:

- **Development Configuration**: Run all tests with detailed output
- **Testing Configuration**: Run subset of tests for CI/CD
- **Production Configuration**: Run minimal tests for deployment validation
- **Performance Configuration**: Run performance and stress tests

## Future Enhancements

### Planned Features

1. **Test Plugin System**: Extensible plugin system for custom test types
2. **Custom Assertions**: Custom assertion framework for domain-specific testing
3. **Test Templates**: Template-based test creation for common patterns
4. **Automatic Test Generation**: Generate tests from code coverage analysis
5. **Continuous Integration**: Integration with CI/CD pipelines
6. **Test Documentation**: Automatic documentation generation from test metadata
7. **Test History**: Track test execution history and trends
8. **Test Validation**: Advanced validation for complex test scenarios

### Contribution Guidelines

- **Follow Code Style**: Adhere to existing code style guidelines
- **Write Tests**: Include tests for new framework features
- **Update Documentation**: Keep documentation up to date
- **Maintain Backward Compatibility**: Ensure existing tests continue to work
- **Code Reviews**: Participate in code review process

## Conclusion

The NLM Test Framework provides a comprehensive, flexible, and extensible testing solution for the Neural Learning Machine system. It addresses all the limitations of the previous framework while maintaining backward compatibility and providing powerful new features for advanced testing scenarios.

The framework is designed to be:

- **Flexible**: Support multiple testing styles and approaches
- **Extensible**: Easy to add new test types and categories
- **Performant**: Minimal performance impact on simulation
- **Integrated**: Seamless integration with existing NLM components
- **Configurable**: Configuration-driven test setup and execution
- **Comprehensive**: Full-featured test execution and reporting

The framework makes it easy for both newcomers and advanced users to create and run comprehensive tests of the NLM system, ensuring high-quality, reliable neural computation simulations.
