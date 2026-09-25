# NLM Testing Framework Documentation
# ===================================

This document provides comprehensive information about the NLM testing framework, covering all testing features, the enhanced test framework, and usage examples.

## Overview

The NLM testing framework provides comprehensive testing capabilities for the NLM (Neural Learning Machine) project. It includes both legacy test support and a modern enhanced testing framework with advanced features.

### Key Features

- **Legacy Support**: Backward compatibility with existing test infrastructure
- **Enhanced Framework**: Modern C++ testing framework with advanced features
- **Parameterized Tests**: Test parameterization for comprehensive coverage
- **Performance Benchmarks**: Performance testing and benchmarking
- **Test Organization**: Categorization and tagging for better test management
- **Output Formatting**: Multiple output formats (console, JSON, XML)
- **Test Discovery**: Automatic test discovery and registration
- **Error Handling**: Comprehensive error reporting and recovery

## Testing Framework Architecture

### 1. Legacy Testing Framework
**Directory:** `tests/`

**Components:**
- `test_main.cpp` - Main test runner
- `test_config.cpp` - Configuration tests
- `test_types.cpp` - Type definition tests
- `test_random.cpp` - Random number generator tests
- `test_clock.cpp` - Simulation clock tests
- `test_neuron.cpp` - Neuron tests
- `test_synapse.cpp` - Synapse tests
- `test_stdp.cpp` - STDP plasticity tests
- `test_brain.cpp` - Brain integration tests

**Characteristics:**
- Simple assertion-based tests
- Manual test organization
- Basic error handling
- Limited output formatting

### 2. Enhanced Testing Framework
**Directory:** `test_framework/`

**Subdirectories:**
- `base/` - Core testing framework
- `types/` - Test data structures and results
- `utils/` - Test utilities and parameterization
- `benchmarks/` - Performance benchmarking
- `main/` - Framework integration

**Components:**
- `TestingFramework.hpp` - Core TestCase and TestSuite classes
- `TestTypes.hpp` - Test result types and metadata
- `TestUtils.hpp` - Test utilities and parameterization
- `Benchmarks.hpp` - Performance benchmarking framework
- `TestFrameworkMain.hpp` - Framework integration

## Enhanced Testing Framework Features

### 1. Core Testing Classes

#### TestCase Class
**Base class for all test cases.**

**Constructor:**
```cpp
TestCase(const std::string& name, const std::string& category, 
         const std::vector<std::string>& tags = {});
```

**Methods:**
```cpp
virtual void run() = 0;                    // Run the test
void setParameters(const TestParameters& params); // Set test parameters
void addTag(const std::string& tag);       // Add test tag
void addCategory(const std::string& category);    // Add test category
bool hasTag(const std::string& tag) const; // Check if test has tag
std::string getName() const;              // Get test name
std::string getCategory() const;          // Get test category
std::vector<std::string> getTags() const; // Get test tags
```

**Assertion Methods:**
```cpp
void assertEqual(T a, T b, const std::string& message = "");
void assertNotEqual(T a, T b, const std::string& message = "");
void assertLess(T a, T b, const std::string& message = "");
void assertGreater(T a, T b, const std::string& message = "");
void assertTrue(bool condition, const std::string& message = "");
void assertFalse(bool condition, const std::string& message = "");
void assertEqual(double a, double b, double tolerance = 1e-6, const std::string& message = "");
void assertInRange(T value, T min, T max, const std::string& message = "");
```

#### TestSuite Class
**Container for organizing related test cases.**

**Methods:**
```cpp
void addTest(TestCase* testCase);          // Add test case
void removeTest(TestCase* testCase);       // Remove test case
void clearTests();                         // Clear all tests
size_t getTestCount() const;               // Get test count
TestCase* getTest(size_t index) const;     // Get test by index
std::vector<TestCase*> getTests() const;   // Get all tests
```

### 2. Parameterized Testing

#### TestParameters Class
**Container for test parameters.**

**Constructor:**
```cpp
TestParameters();
TestParameters(const std::unordered_map<std::string, ConfigValue>& params);
```

**Methods:**
```cpp
void set(const std::string& key, const ConfigValue& value);
bool has(const std::string& key) const;
ConfigValue get(const std::string& key) const;
template<typename T> T get(const std::string& key) const;
std::vector<std::string> getKeys() const;
bool isEmpty() const;
size_t size() const;
```

**Parameter Generation:**
```cpp
// Grid generation for parameter combinations
TestParameters generateGrid(int start, int end, int step);
TestParameters generateGrid(double start, double end, double step);
TestParameters generateRandom(const std::string& type, 
                             size_t count = 100,
                             double mean = 0.0,
                             double stddev = 1.0);
```

### 3. Performance Benchmarking

#### Benchmark Class
**Base class for performance benchmarks.**

**Constructor:**
```cpp
Benchmark(const BenchmarkMetadata& metadata = BenchmarkMetadata());
```

**Methods:**
```cpp
virtual BenchmarkResult run(size_t iterations) = 0;  // Run benchmark
void setup();                                       // Setup before benchmark
void teardown();                                    // Cleanup after benchmark
BenchmarkMetadata getMetadata() const;             // Get benchmark metadata
void setMetadata(const BenchmarkMetadata& metadata); // Set benchmark metadata
```

**Benchmark Configuration:**
```cpp
struct BenchmarkConfig {
    size_t warmupIterations = 100;
    size_t benchmarkIterations = 1000;
    size_t minIterations = 100;
    size_t maxIterations = 10000;
    double timeout = 30.0; // seconds
    bool repeat = true;
    bool parallel = false;
    size_t parallelThreads = std::thread::hardware_concurrency();
    double confidenceInterval = 0.95;
    std::string outputFormat = "console";
    bool saveResults = false;
    std::string resultsFile;
};
```

### 4. Test Results and Analysis

#### TestResult Class
**Result of a single test execution.**

**Methods:**
```cpp
TestStatus getStatus() const;
std::chrono::microseconds getDuration() const;
std::string getMessage() const;
std::string getErrorMessage() const;
BenchmarkResult getBenchmarkResult() const;
std::unordered_map<std::string, std::string> getMetadata() const;
```

#### BenchmarkResult Class
**Result of a single benchmark execution.**

**Methods:**
```cpp
BenchmarkStatus getStatus() const;
std::chrono::microseconds getDuration() const;
std::string getMessage() const;
double getMean() const;
double getStandardDeviation() const;
double getMin() const;
double getMax() const;
size_t getCount() const;
BenchmarkClass getPerformanceClass() const;
```

### 5. Test Discovery and Registration

#### TestRegistry Class
**Global test registration and discovery.**

**Methods:**
```cpp
static TestRegistry& getInstance();                    // Get singleton instance
void registerTest(TestCase* testCase);                  // Register test
void unregisterTest(TestCase* testCase);                // Unregister test
std::vector<TestCase*> getAllTests() const;             // Get all registered tests
void clearAllTests();                                  // Clear all tests
```

#### BenchmarkRegistry Class
**Global benchmark registration and discovery.**

**Methods:**
```cpp
static BenchmarkRegistry& getInstance();               // Get singleton instance
void registerBenchmark(Benchmark* benchmark);          // Register benchmark
void unregisterBenchmark(Benchmark* benchmark);        // Unregister benchmark
std::vector<Benchmark*> getAllBenchmarks() const;      // Get all registered benchmarks
void clearAllBenchmarks();                             // Clear all benchmarks
```

### 6. Output Formatting

#### OutputFormat Enum
```cpp
enum class OutputFormat {
    Console,
    JSON,
    XML,
    Markdown,
    HTML
};
```

#### Output Formatter Class
**Format test results for different output formats.**

**Methods:**
```cpp
static std::string formatConsole(const TestResult& result);
static std::string formatJSON(const TestResult& result);
static std::string formatXML(const TestResult& result);
static std::string formatMarkdown(const TestResult& result);
static std::string formatHTML(const TestResult& result);
```

## Testing Framework Integration

### 1. Legacy Test Integration
**Backward Compatibility:**
- Legacy test functions are still supported
- Test discovery includes legacy tests
- Test execution supports both frameworks

**Legacy Test Examples:**
```cpp
// Legacy test function (still supported)
namespace test_config {
    void testConfigCreation() {
        Config config;
        assert(!config.has("test"));
    }
    
    void runAll() {
        testConfigCreation();
        // ... other tests
    }
}

// Legacy test execution
void runLegacyTests() {
    test_config::runAll();
    // ... other legacy test suites
}
```

### 2. Enhanced Test Integration
**Enhanced Test Examples:**
```cpp
class ConfigTest : public TestCase {
public:
    ConfigTest() : TestCase("ConfigTest", "Configuration") {}
    
    void run() override {
        Config config;
        config.set("int_value", 42);
        assertEqual(config.get<int>("int_value"), 42);
        
        config.set("string_value", std::string("hello"));
        assertEqual(config.get<std::string>("string_value"), "hello");
    }
};
```

**Parameterized Test Examples:**
```cpp
class ParameterizedConfigTest : public TestCase {
public:
    ParameterizedConfigTest() : TestCase("ParameterizedConfigTest", "Configuration") {}
    
    void withParameters(const TestParameters& params) override {
        this->neuronCount = params.get<int>("neuron_count");
        this->connectionProbability = params.get<double>("connection_probability");
    }
    
    void run() override {
        Config config;
        config.set("neuron_count", neuronCount);
        config.set("connection_probability", connectionProbability);
        
        assertEqual(config.getOr<int>("neuron_count", 100), neuronCount);
    }
    
private:
    int neuronCount = 100;
    double connectionProbability = 0.1;
};
```

### 3. Test Execution
**Test Execution:**
```cpp
// Run all tests
void runAllTests() {
    // Run legacy tests
    runLegacyTests();
    
    // Run enhanced tests
    runEnhancedTests();
}

// Run enhanced tests
void runEnhancedTests() {
    // Get all registered tests
    std::vector<TestCase*> tests = TestRegistry::getInstance().getAllTests();
    
    // Run each test
    for (TestCase* test : tests) {
        try {
            test->run();
            // Log success
        } catch (const std::exception& e) {
            // Log failure
        }
    }
}
```

## Test Framework Usage

### 1. Running Tests
**Command Line Options:**
```bash
# Run all tests
./nlm_test_suite

# Run specific test category
./nlm_test_suite --test config

# Run performance benchmarks
./nlm_test_suite --benchmark neuron_performance

# Run tests by tag
./nlm_test_suite --tag performance

# Run tests by name pattern
./nlm_test_suite --pattern "*config*"

# Run benchmark suite
./nlm_test_suite --benchmarks
```

**Test Categories:**
- `config` - Configuration tests
- `types` - Type definition tests
- `random` - Random number generator tests
- `clock` - Simulation clock tests
- `neuron` - Neuron tests
- `synapse` - Synapse tests
- `stdp` - STDP plasticity tests
- `brain` - Brain integration tests

### 2. Test Organization
**Test Tags:**
```cpp
// Test tags for categorization
TestCase test1("test1", "basic", {"performance", "regression"});
TestCase test2("test2", "advanced", {"performance", "stress"});
TestCase test3("test3", "basic", {"regression", "sanity"});
```

**Test Categories:**
- `basic` - Basic functionality tests
- `advanced` - Advanced functionality tests
- `performance` - Performance tests
- `regression` - Regression tests
- `sanity` - Sanity checks
- `stress` - Stress tests

### 3. Test Output
**Console Output:**
```
=== NLM Test Suite ===
Running Tests:
  config: ConfigTest
  types: NeuronIdTest
  random: RandomTest
  clock: ClockTest
  neuron: NeuronTest
  synapse: SynapseTest
  stdp: STDPTest
  brain: BrainTest

Running Benchmarks:
  neuron_performance: NeuronPerformanceBenchmark

Results:
  Tests: 8 passed, 0 failed
  Benchmarks: 1 passed, 0 failed
  Total Time: 2.5s
```

**JSON Output:**
```json
{
  "test_results": [
    {
      "name": "ConfigTest",
      "category": "config",
      "status": "passed",
      "duration": 1500,
      "message": "All tests passed"
    }
  ],
  "benchmark_results": [
    {
      "name": "NeuronPerformanceBenchmark",
      "status": "passed",
      "duration": 1000,
      "mean": 1234.5,
      "stddev": 123.4
    }
  ],
  "summary": {
    "total_tests": 8,
    "passed_tests": 8,
    "failed_tests": 0,
    "total_benchmarks": 1,
    "passed_benchmarks": 1,
    "failed_benchmarks": 0,
    "total_time": 2500
  }
}
```

## Test Framework Best Practices

### 1. Test Organization
- **Categorize tests**: Use categories for better organization
- **Tag tests**: Use tags for selective test execution
- **Organize by functionality**: Group related tests together
- **Maintain test independence**: Tests should not depend on each other

### 2. Test Writing
- **Follow naming conventions**: Use descriptive test names
- **Use assertions**: Use assertion methods for validation
- **Handle exceptions**: Catch and handle exceptions properly
- **Document tests**: Document test purpose and expected behavior

### 3. Test Maintenance
- **Keep tests up to date**: Update tests when functionality changes
- **Review test coverage**: Ensure comprehensive test coverage
- **Run tests regularly**: Run tests regularly to catch regressions
- **Optimize test performance**: Optimize test performance for fast execution

### 4. Test Performance
- **Use efficient assertions**: Use efficient assertion methods
- **Minimize I/O**: Minimize file I/O operations
- **Use caching**: Cache frequently used values
- **Use parallel execution**: Use parallel execution for tests

## Testing Framework Examples

### 1. Basic Test Example
```cpp
#include "test_framework/base/TestingFramework.hpp"

class SimpleTest : public nlm::test::TestCase {
public:
    SimpleTest() : nlm::test::TestCase("SimpleTest", "basic") {}
    
    void run() override {
        int a = 5;
        int b = 10;
        
        assertEqual(a, 5);
        assertEqual(b, 10);
        assertGreater(a, 0);
        assertLess(b, 20);
        assertTrue(a < b);
        assertFalse(a > b);
    }
};
```

### 2. Parameterized Test Example
```cpp
class ParameterizedMathTest : public nlm::test::TestCase {
public:
    ParameterizedMathTest() : nlm::test::TestCase("ParameterizedMathTest", "advanced") {}
    
    void withParameters(const nlm::test::TestParameters& params) override {
        this->x = params.get<int>("x");
        this->y = params.get<int>("y");
        this->expectedSum = params.get<int>("sum");
    }
    
    void run() override {
        int result = x + y;
        assertEqual(result, expectedSum);
    }
    
private:
    int x = 0;
    int y = 0;
    int expectedSum = 0;
};
```

### 3. Benchmark Example
```cpp
class NeuronPerformanceBenchmark : public nlm::test::Benchmark {
public:
    NeuronPerformanceBenchmark() {
        nlm::test::BenchmarkMetadata metadata;
        metadata.name = "NeuronPerformance";
        metadata.description = "Performance benchmark for neuron operations";
        metadata.category = "Performance";
        metadata.tags = "neuron,performance,basic";
        metadata.unit = "us";
        metadata.performanceClass = nlm::test::BenchmarkClass::Good;
        setMetadata(metadata);
    }
    
    void setup() override {
        // Setup for benchmark
        neuron = std::make_unique<Neuron>(NeuronId(1));
    }
    
    nlm::test::BenchmarkResult run(size_t iterations) override {
        return timeFunction("NeuronPerformance", [this]() {
            neuron->stepLIF(0.0, 0.001);
        }, iterations);
    }
    
    void teardown() override {
        neuron.reset();
    }
    
private:
    std::unique_ptr<Neuron> neuron;
};
```

### 4. Integration Example
```cpp
#include "test_framework/main/TestFrameworkMain.hpp"

int main(int argc, char** argv) {
    return nlm::test::EnhancedTestMain::runAllTests(argc, argv);
}
```

## Conclusion

The NLM testing framework provides comprehensive testing capabilities with both legacy support and modern enhanced features. It includes:

- **Legacy Testing**: Backward compatibility with existing test infrastructure
- **Enhanced Testing**: Modern C++ testing framework with advanced features
- **Parameterized Testing**: Test parameterization for comprehensive coverage
- **Performance Benchmarking**: Performance testing and benchmarking
- **Test Organization**: Categorization and tagging for better test management
- **Output Formatting**: Multiple output formats (console, JSON, XML)
- **Test Discovery**: Automatic test discovery and registration
- **Error Handling**: Comprehensive error reporting and recovery

The testing framework is designed for flexibility, reliability, and performance, providing a solid foundation for NLM neural simulation experiments and research.
