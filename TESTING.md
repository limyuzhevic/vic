# NLM Testing Framework Documentation

## Overview

The NLM Testing Framework provides comprehensive testing capabilities for NLM (Neural Learning Machine) with support for legacy tests, enhanced tests, benchmarks, and integration with the existing test suite.

## Framework Architecture

### Core Components

1. **TestingFramework.hpp** - Main testing infrastructure
2. **TestFramework.cpp** - Test execution logic
3. **TestTypes.hpp** - Test data structures and enums
4. **TestUtils.hpp** - Testing utilities

### Enhanced Testing Framework

The enhanced testing framework adds:

- **Test Case Management**: Support for individual test cases
- **Test Suite Management**: Grouping tests into logical suites
- **Test Filtering**: Select tests by category, tags, and patterns
- **Test Output Formatting**: Multiple output formats (console, JSON, HTML)
- **Performance Monitoring**: Measure test execution time and memory usage
- **Integration Support**: Seamless integration with legacy tests

## Test Framework Structure

### Test Categories
Tests are organized by functional areas:

- **Memory Integration**: Memory system connectivity
- **Neuromodulation**: Neuromodulator integration and effects
- **Checkpointing**: State save/load functionality
- **Replay**: Memory replay and consolidation
- **Performance**: Performance benchmarks and scaling
- **Configuration**: Configuration validation and error handling
- **Compatibility**: Legacy and enhanced test compatibility

### Test Tags
Tags provide additional organization:

- **Unit**: Individual component tests
- **Integration**: System-level integration tests
- **Performance**: Performance and stress tests
- **Regression**: Regression test suites
- **Compliance**: Specification compliance tests

## Test Execution

### Command Line Interface
```bash
# Run all tests
./nlm --test all

# Run tests by category
./nlm --test memory --test neuromodulation

# Run tests by tag
./nlm --test tag:integration --test tag:performance

# Run tests by name pattern
./nlm --test pattern:"test_.*_integration"

# Run legacy tests
./nlm --test legacy

# Run enhanced tests
./nlm --test enhanced

# Save test results
./nlm --test all --export results.json

# Export results in different formats
./nlm --test all --format json
./nlm --test all --format xml
./nlm --test all --format html
```

### Test Framework Integration
```cpp
#include "TestFrameworkMain.hpp"

int main(int argc, char* argv[]) {
    return nlm::test::testFrameworkMain(argc, argv);
}
```

## Test Case Implementation

### Basic Test Case
```cpp
#include "TestingFramework.hpp"

class MemoryIntegrationTest : public nlm::test::TestCase {
public:
    MemoryIntegrationTest() : TestCase("MemoryIntegration", "Memory") {}
    
    bool setupTestCase() override {
        // Setup for all tests in this suite
        return true;
    }
    
    void setup() override {
        // Setup for each test
        brain = nlm::createDefaultConfig();
        agent = nlm::createAgentBrain(brain);
    }
    
    void run() override {
        // Test logic
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testMemoryConnectivity();
    }
    
    void teardown() override {
        // Cleanup after each test
        agent.reset();
        brain.reset();
    }
    
    bool teardownTestCase() override {
        // Cleanup after all tests
        return true;
    }
    
private:
    std::shared_ptr<Config> brain;
    std::shared_ptr<AgentBrain> agent;
    
    void testWorkingMemoryIntegration() {
        ASSERT_NOT_NULL(brain);
        ASSERT(agent->getWorkingMemoryState() == WorkingMemoryState::Connected);
    }
    
    void testEpisodicMemoryIntegration() {
        // Test episodic memory integration
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        ASSERT(agent->isRewardModulationEnabled());
    }
};
```

### Test Suite Implementation
```cpp
#include "TestingFramework.hpp"

class EnhancedTestSuite : public nlm::test::TestSuite {
public:
    EnhancedTestSuite() : TestSuite("Enhanced", "Integration") {}
    
protected:
    void registerTests() override {
        // Register individual test cases
        registerTest(new MemoryIntegrationTest());
        registerTest(new NeuromodulationIntegrationTest());
        registerTest(new CheckpointTest());
        registerTest(new ReplayTest());
        registerTest(new DevelopmentTest());
    }
};
```

## Legacy Test Support

### Legacy Test Integration
```cpp
#include "TestFrameworkMain.hpp"

void runLegacyTest(const std::string& testName, std::function<void()> testFunction) {
    nlm::test::TestFrameworkIntegration::runLegacyTest(testName, testFunction);
}

void runLegacyTestSuite(const std::vector<std::string>& testNames,
                       const std::vector<std::function<void()>>& testFunctions) {
    nlm::test::TestFrameworkIntegration::runLegacyTestSuite(testNames, testFunctions);
}
```

### Legacy Test Examples
```cpp
// Legacy test implementation
void testNeuronDynamics() {
    auto config = nlm::createDefaultConfig();
    config.set("brain.neuron_count", 100);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    ASSERT(brain->getTotalSpikeCount() > 0);
}
```

## Enhanced Test Features

### Test Assertions
```cpp
#include "TestingFramework.hpp"

// Basic assertions
ASSERT_TRUE(condition);
ASSERT_FALSE(condition);
ASSERT_NULL(ptr);
ASSERT_NOT_NULL(ptr);
ASSERT_EQUAL(actual, expected, "Message");
ASSERT_NOT_EQUAL(actual, expected, "Message");
ASSERT_GREATER(actual, expected, "Message");
ASSERT_LESSER(actual, expected, "Message");

// Floating point assertions
ASSERT_APPROX_EQUAL(actual, expected, epsilon, "Message");

// Custom assertions
ASSERT_WITHIN_RANGE(value, min, max, "Message");
```

### Test Output Formatting
```cpp
// Console output
nlm::test::TestFrameworkIntegration::printTestResults(results);
nlm::test::TestFrameworkIntegration::printTestSummary(results);

// JSON output
std::string jsonOutput = nlm::test::TestFrameworkIntegration::
    formatTestResults(results, "json");

// XML output
std::string xmlOutput = nlm::test::TestFrameworkIntegration::
    formatTestResults(results, "xml");

// HTML output
std::string htmlOutput = nlm::test::TestFrameworkIntegration::
    formatTestResults(results, "html");
```

### Test Result Saving
```cpp
// Save test results
bool saveResults = nlm::test::TestFrameworkIntegration::
    saveTestResults(results, "test_results.json", "json");

// Load test configuration
nlm::test::TestFrameworkIntegration::
    loadTestConfiguration("tests/config.json");

// Save test configuration
nlm::test::TestFrameworkIntegration::
    saveTestConfiguration("tests/config.json");
```

## Test Filtering and Selection

### Test Filtering
```cpp
// Filter by category
auto memoryTests = nlm::test::TestFrameworkIntegration::
    filterTestResults(results, "Memory", "", nlm::test::TestStatus::Passed);

// Filter by tag
auto integrationTests = nlm::test::TestFrameworkIntegration::
    filterTestResults(results, "", "integration", nlm::test::TestStatus::Failed);

// Filter by status
auto failedTests = nlm::test::TestFrameworkIntegration::
    filterTestResults(results, "", "", nlm::test::TestStatus::Failed);
```

### Test Selection
```cpp
// Select by name pattern
auto matchingTests = nlm::test::TestFrameworkIntegration::
    selectTestResults(results, "test_.*_integration");

// Select by category and tag
auto selectedTests = nlm::test::TestFrameworkIntegration::
    selectTestResults(results, "Memory.*", 10);
```

## Performance Monitoring

### Test Performance Measurement
```cpp
// Measure execution time
auto executionTime = nlm::test::TestFrameworkIntegration::
    measureTestExecutionTime(testCase);

// Measure memory usage
double memoryUsage = nlm::test::TestFrameworkIntegration::
    measureTestMemoryUsage(testCase);

// Profile test execution
auto profile = nlm::test::TestFrameworkIntegration::
    profileTestExecution(testCase);
```

### Performance Benchmarking
```cpp
// Run benchmarks
auto benchmarks = nlm::test::TestFrameworkIntegration::
    executeBenchmarks(nlm::test::BenchmarkConfig());

// Convert test suite to benchmark suite
auto benchmarkSuite = nlm::test::TestFrameworkIntegration::
    convertTestSuiteToBenchmarkSuite(testSuite);
```

## Test Comparison and Analysis

### Test Comparison
```cpp
// Compare test results
auto comparison = nlm::test::TestFrameworkIntegration::
    compareTestResults(results1, results2);

// Calculate pass rate
double passRate = nlm::test::TestFrameworkIntegration::
    calculateTestPassRate(results);

// Calculate total execution time
auto totalTime = nlm::test::TestFrameworkIntegration::
    calculateTotalTestTime(results);
```

## Test Framework Configuration

### Test Parameters
```cpp
struct TestParameters {
    std::string outputFormat = "console";
    bool enablePerformanceMonitoring = true;
    bool enableLegacySupport = true;
    bool enableEnhancedTests = true;
    int maxRetries = 3;
    double timeoutSeconds = 30.0;
    std::vector<std::string> enabledCategories;
    std::vector<std::string> enabledTags;
};
```

### Benchmark Configuration
```cpp
struct BenchmarkConfig {
    size_t neuronCount = 1000;
    size_t steps = 1000;
    std::string benchmarkType = "simulation";
    double durationSeconds = 10.0;
    bool enableWarmup = true;
    int warmupSteps = 100;
    std::string outputFormat = "console";
};
```

## Test Error Handling

### Error Handling
```cpp
bool handleTestError(const std::string& testName,
                   const std::string& errorMessage,
                   const std::string& file = "",
                   int line = 0) {
    return nlm::test::TestFrameworkIntegration::
        handleTestError(testName, errorMessage, file, line);
}
```

### Error Recovery
```cpp
// Recover from test errors
nlm::test::TestFrameworkIntegration::
    recoverFromTestError(errorResult);

// Check if test error is recoverable
bool isRecoverable = nlm::test::TestFrameworkIntegration::
    isTestErrorRecoverable(errorResult);

// Retry failed tests
nlm::test::TestFrameworkIntegration::
    retryFailedTests(results, 3);
```

## Test Result Validation

### Validation
```cpp
// Validate individual test result
bool isValid = nlm::test::TestFrameworkIntegration::
    validateTestResult(testResult);

// Validate all test results
bool allValid = nlm::test::TestFrameworkIntegration::
    validateTestResults(results);

// Validate benchmark results
bool benchmarkValid = nlm::test::TestFrameworkIntegration::
    validateBenchmarkResult(benchmarkResult);
```

### Test Result Comparison
```cpp
// Compare test results
bool equivalent = nlm::test::TestFrameworkIntegration::
    areTestResultsEquivalent(result1, result2);

// Calculate result similarity
double similarity = nlm::test::TestFrameworkIntegration::
    calculateTestResultSimilarity(result1, result2);
```

## Test Framework Statistics

### Test Statistics
```cpp
auto statistics = nlm::test::TestFrameworkIntegration::
    calculateTestStatistics(results);

std::cout << "Total tests: " << statistics.totalTests << std::endl;
std::cout << "Passed tests: " << statistics.passedTests << std::endl;
std::cout << "Failed tests: " << statistics.failedTests << std::endl;
std::cout << "Skipped tests: " << statistics.skippedTests << std::endl;
std::cout << "Average execution time: " << statistics.avgExecutionTime << std::endl;
```

### Benchmark Statistics
```cpp
auto benchmarkStats = nlm::test::TestFrameworkIntegration::
    calculateBenchmarkStatistics(benchmarkResults);
```

## Test Report Generation

### Report Generation
```cpp
// Generate HTML report
std::string htmlReport = nlm::test::TestFrameworkIntegration::
    generateTestReport(testResults, benchmarkResults, "html");

// Generate detailed report
std::string detailedReport = nlm::test::TestFrameworkIntegration::
    generateTestReport(testResults, benchmarkResults, "detailed");
```

## Testing Best Practices

### Writing Tests
1. **Test Independently**: Each test should be independent of others
2. **Test Edge Cases**: Include tests for boundary conditions and error cases
3. **Test Both Success and Failure**: Verify both expected success and failure scenarios
4. **Use Appropriate Assertions**: Use assertions that provide clear error messages
5. **Clean Up**: Properly clean up resources after each test

### Running Tests
1. **Run Tests Regularly**: Integrate tests into your build process
2. **Monitor Test Performance**: Track test execution time and identify bottlenecks
3. **Keep Tests Updated**: Update tests as the codebase evolves
4. **Use Test Categories**: Organize tests by functionality for better test selection

## Testing Examples

### Configuration Tests
```cpp
class ConfigurationTest : public nlm::test::TestCase {
public:
    ConfigurationTest() : TestCase("Configuration", "Configuration") {}
    
    void run() override {
        testConfigurationLoading();
        testConfigurationValidation();
        testConfigurationDiffing();
    }
    
private:
    void testConfigurationLoading() {
        Config config;
        ASSERT_TRUE(config.loadFromFile("tests/config.json"));
        ASSERT(config.has("brain.neuron_count"));
        ASSERT_EQUAL(config.get<int>("brain.neuron_count"), 1000);
    }
    
    void testConfigurationValidation() {
        Config config;
        config.set("brain.neuron_count", -100);
        
        std::vector<ConfigSchema> schema = {
            ConfigSchema("brain.neuron_count", "int", true, "Positive neuron count")
        };
        
        ASSERT_FALSE(config.validateSchema(schema));
    }
};
```

### Performance Tests
```cpp
class PerformanceTest : public nlm::test::TestCase {
public:
    PerformanceTest() : TestCase("Performance", "Performance") {}
    
    void run() override {
        testNeuronPerformance();
        testSynapsePerformance();
    }
    
private:
    void testNeuronPerformance() {
        auto config = nlm::createDefaultConfig();
        config.set("brain.neuron_count", 1000);
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        ASSERT_GREATER(duration, 0.0);  // Should take some time
    }
};
```

### Integration Tests
```cpp
class IntegrationTest : public nlm::test::TestCase {
public:
    IntegrationTest() : TestCase("Integration", "Integration") {}
    
    void run() override {
        testBrainWorldIntegration();
        testAgentIntegration();
        testNeuromodulationIntegration();
    }
    
private:
    void testBrainWorldIntegration() {
        auto config = nlm::createDefaultConfig();
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        auto world = nlm::createSimpleWorld();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(world);
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        // Verify integration
        ASSERT_TRUE(agent->isRewardModulationEnabled());
        ASSERT_TRUE(agent->isDevelopmentEnabled());
    }
};
```

## Test Framework Commands

### Available Commands
```bash
# Run all tests
./nlm --test all

# Run tests by category
./nlm --test memory
./nlm --test neuromodulation
./nlm --test performance

# Run tests by tag
./nlm --test tag:integration
./nlm --test tag:performance

# Run tests by name pattern
./nlm --test pattern:"test_.*_integration"

# Run legacy tests
./nlm --test legacy

# Run enhanced tests
./nlm --test enhanced

# Run benchmarks
./nlm --benchmark neuron_performance
./nlm --benchmark synapse_performance

# Export test results
./nlm --test all --export results.json
./nlm --test all --export results.html --format html

# Show help
./nlm --test --help
```

## Test Framework Status

### Framework Information
```cpp
// Get framework status
std::string status = nlm::test::TestFrameworkIntegration::
    getFrameworkStatus();

// Get framework version
std::string version = nlm::test::TestFrameworkIntegration::
    getFrameworkVersion();

// Get framework info
std::string info = nlm::test::TestFrameworkIntegration::
    getFrameworkInfo();

// Check if framework is healthy
bool healthy = nlm::test::TestFrameworkIntegration::
    isFrameworkHealthy();
```

## Test Framework Limitations

### Known Issues
1. **Memory Usage**: Some tests may consume significant memory
2. **Execution Time**: Complex integration tests may take longer to run
3. **Resource Requirements**: Some tests require specific hardware resources
4. **Test Isolation**: Tests may have side effects on shared resources

### Recommendations
1. **Run Tests in Isolation**: Run tests in separate processes when possible
2. **Monitor Resources**: Monitor memory and CPU usage during test execution
3. **Clean Up After Tests**: Ensure all test resources are properly cleaned up
4. **Use Test Environments**: Use isolated test environments to avoid conflicts

## Future Enhancements

1. **Parallel Test Execution**: Support for parallel test execution
2. **Test Sharding**: Partition tests across multiple machines
3. **Continuous Integration**: Integration with CI/CD pipelines
4. **Test Coverage Analysis**: Automated test coverage analysis
5. **Performance Regression Testing**: Automated performance regression detection

## Conclusion

The NLM Testing Framework provides comprehensive testing capabilities for the NLM project. It supports:

- Legacy tests for backward compatibility
- Enhanced tests for new functionality
- Performance benchmarks and profiling
- Test filtering and selection
- Multiple output formats
- Error handling and recovery
- Performance monitoring
- Integration with other components

By following the best practices outlined in this documentation, you can create effective tests that verify NLM functionality, ensure quality, and maintain system stability.

