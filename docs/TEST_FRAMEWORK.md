# Test Framework Documentation

This document describes the enhanced test framework for NLM (Neural Learning Machine) Phase 6, including advanced test suites, assertions, and test runner functionality.

## Overview

The test framework has been completely redesigned to provide:

- **Advanced test suites**: Organized test collections with setup/teardown capabilities
- **Powerful assertions**: Comprehensive assertion functions with detailed error messages
- **Performance measurement**: Built-in timing and performance metrics
- **Error reporting**: Detailed error reporting with error codes and types
- **Test runner**: Unified test runner with statistics and result reporting
- **Integration**: Seamless integration with existing configuration and error handling systems

## Test Framework Architecture

### TestSuite Class

The `TestSuite` base class provides the foundation for organizing tests:

```cpp
class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<TestResult> runTests() = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
};
```

### TestResult Structure

The `TestResult` structure captures detailed information about each test:

```cpp
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double duration; // in seconds
    std::string errorType;
    int errorCode;
};
```

### TestRunner Class

The `TestRunner` class manages and executes test suites:

```cpp
class TestRunner {
public:
    TestRunner();
    void addSuite(std::unique_ptr<TestSuite> suite);
    std::vector<TestResult> runAllTests();
    void printResults(const std::vector<TestResult>& results);
    // ... statistics methods
};
```

## Assertions

The `Assertions` class provides comprehensive assertion functions:

### Basic Assertions

| Function | Description |
|----------|-------------|
| `equals(actual, expected, message)` | Check if actual equals expected |
| `notEquals(actual, expected, message)` | Check if actual does not equal expected |
| `isTrue(condition, message)` | Check if condition is true |
| `isFalse(condition, message)` | Check if condition is false |
| `inRange(value, min, max, message)` | Check if value is within range |

### Pointer Assertions

| Function | Description |
|----------|-------------|
| `isNull(ptr, message)` | Check if pointer is null |
| `isNotNull(ptr, message)` | Check if pointer is not null |

### Exception Assertions

| Function | Description |
|----------|-------------|
| `throws(func, message)` | Check if function throws exception |

### Container Assertions

| Function | Description |
|----------|-------------|
| `isEmpty(container, message)` | Check if container is empty |
| `isNotEmpty(container, message)` | Check if container is not empty |
| `hasSize(container, expectedSize, message)` | Check if container has expected size |

## Macros

The framework provides convenient macros for test case definition:

```cpp
#define TEST_CASE(suiteName, testName) \\
    class testName##_test : public TestSuite { \\
    public: \\
        std::string getName() const override { return #suiteName; } \\
        std::vector<TestResult> runTests() override { \\
            std::vector<TestResult> results; \\
            auto start = std::chrono::high_resolution_clock::now(); \\
            testName(); \\
            auto end = std::chrono::high_resolution_clock::now(); \\
            auto duration = std::chrono::duration<double>(end - start).count(); \\
            results.emplace_back(#testName, true, "Test passed", duration); \\
            return results; \\
        } \\
    private: \\
        void testName()

#define ASSERT_EQ(actual, expected) Assertions::equals(actual, expected, "Assertion failed: " #actual " == " #expected)
#define ASSERT_NE(actual, expected) Assertions::notEquals(actual, expected, "Assertion failed: " #actual " != " #expected)
#define ASSERT_TRUE(condition) Assertions::isTrue(condition, "Assertion failed: " #condition)
#define ASSERT_FALSE(condition) Assertions::isFalse(condition, "Assertion failed: not " #condition)
#define ASSERT_RANGE(value, min, max) Assertions::inRange(value, min, max, "Assertion failed: " #value " not in range [" #min ", " #max "]")
#define ASSERT_NULL(ptr) Assertions::isNull(ptr, "Assertion failed: " #ptr " should be null")
#define ASSERT_NOT_NULL(ptr) Assertions::isNotNull(ptr, "Assertion failed: " #ptr " should not be null")
#define ASSERT_THROWS(func) Assertions::throws(func, "Assertion failed: " #func " should throw")
#define ASSERT_EMPTY(container) Assertions::isEmpty(container, "Assertion failed: " #container " should be empty")
#define ASSERT_NOT_EMPTY(container) Assertions::isNotEmpty(container, "Assertion failed: " #container " should not be empty")
#define ASSERT_SIZE(container, expected) Assertions::hasSize(container, expected, "Assertion failed: " #container " size should be " #expected)
```

## Test Suites

### ConfigHelper Test Suite

Tests the configuration helper system:

```cpp
class ConfigHelperTestSuite : public TestSuite {
public:
    std::string getName() const override { return "ConfigHelper"; }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        // Test default configuration creation
        testDefaultConfig();
        
        // Test loading from JSON
        testLoadFromJSON();
        
        // Test setting and getting values
        testSetAndGet();
        
        // Test key existence
        testKeyExistence();
        
        // Test JSON conversion
        testToJSON();
        
        return results;
    }
};
```

### Error Handling Test Suite

Tests the error handling system:

```cpp
class ErrorHandlingTestSuite : public TestSuite {
public:
    std::string getName() const override { return "ErrorHandling"; }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        // Test exception creation
        testExceptionCreation();
        
        // Test error code conversion
        testErrorCodeConversion();
        
        return results;
    }
};
```

### Neuromodulation Test Suite

Tests neuromodulation components:

```cpp
class NeuromodulationTestSuite : public TestSuite {
public:
    std::string getName() const override { return "Neuromodulation"; }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        // Test Dopamine functionality
        testDopamineFunctionality();
        
        // Test Acetylcholine functionality
        testAcetylcholineFunctionality();
        
        return results;
    }
};
```

## Test Execution

### Running Tests

To run all tests, compile and link with the test framework:

```bash
nlm_test
```

The test runner will display:

- Test suite names
- Individual test results
- Performance metrics
- Error details for failed tests
- Summary statistics

### Test Output Example

```
Test Results Summary
====================

Total Tests:  15
Passed:       12
Failed:       3
Success Rate: 80.00%

Failed Tests:
  - DefaultConfig
    Error: Assertion failed: isTrue(flag) should be true
    Type: std::exception
    Code: 0
    Duration: 0.000123s
  - LoadFromJSON
    Error: Assertion failed: isNull(ptr) should be null
    Type: std::exception
    Code: 0
    Duration: 0.000456s

Total Duration: 0.001234s
```

## Integration with Other Components

### Integration with Configuration System

```cpp
nlm::ConfigHelper config;
config.set("test.value", 42);
auto value = config.get<int>("test.value");
```

### Integration with Error Handling System

```cpp
try {
    NLM_THROW_ERROR(NLMErrors::ConfigFileNotFound, "Cannot open config file");
} catch (const nlm::NLMException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Error Code: " << static_cast<int>(e.getErrorCode()) << std::endl;
}
```

### Integration with Neuromodulation

```cpp
nlm::Dopamine dopamine;
dopamine.setLevel(0.5f);
dopamine.signalReward(1.0f);
```

## Performance Considerations

### Test Performance

- **Setup/Teardown**: Use `setup()` and `teardown()` for expensive operations
- **Test isolation**: Each test runs independently
- **Timing**: Built-in timing for performance measurement

### Memory Usage

- **Test data**: Tests should clean up memory after completion
- **Container tests**: Use lightweight test data
- **Assertion overhead**: Assertions are inlined for minimal overhead

### Best Practices

1. **Keep tests simple**: Test one thing per test
2. **Use meaningful names**: Test names should describe what they test
3. **Check for leaks**: Run tests with memory leak detection
4. **Parallel testing**: Consider parallel test execution for large test suites
5. **Test coverage**: Ensure comprehensive test coverage

## Test Categories

### Unit Tests

- Configuration system
- Error handling
- Neuromodulation
- Assertions

### Integration Tests

- Configuration with neuromodulation
- Error handling with configuration
- Memory system integration

### Performance Tests

- Configuration loading performance
- Error handling performance
- Neuromodulation performance

## Future Enhancements

### Planned Features

1. **Parameterized tests**: Support for test case parameters
2. **Fixture system**: Test fixture support
3. **Test filtering**: Filter tests by name, tags, or categories
4. **Test reporting**: Advanced test reporting with HTML/JSON output
5. **Continuous integration**: Integration with CI/CD systems
6. **Test coverage analysis**: Automatic test coverage calculation

### Performance Optimizations

1. **Test parallelization**: Run tests in parallel
2. **Caching**: Cache test results and expensive operations
3. **Instrumentation**: Fine-grained performance measurement
4. **Profiling**: Integration with profiling tools

## Conclusion

The enhanced test framework provides:

- **Comprehensive testing**: Coverage of all NLM components
- **Advanced assertions**: Powerful assertion functions with detailed error reporting
- **Performance measurement**: Built-in timing and performance metrics
- **Error handling**: Integration with NLM error handling system
- **Test organization**: Organized test suites with setup/teardown capabilities
- **Extensibility**: Easy to extend with new test types and features

This test framework provides a solid foundation for testing NLM Phase 6 components with improved reliability, performance, and maintainability.
