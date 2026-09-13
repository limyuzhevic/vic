# NLM Test Framework

This document describes the comprehensive test framework for the NLM (Neural Learning Machine) system, designed to validate the improvements made to the codebase and ensure the functionality works correctly.

## Overview

The NLM test framework provides comprehensive testing capabilities to validate:

1. Core neural computation functionality
2. Memory system integration
3. Neuromodulation systems
4. Phase 6 integration achievements
5. Configuration and command-line interface
6. Performance monitoring capabilities
7. Cross-platform compatibility

The test framework is organized into multiple test suites with configurable test levels and reporting options.

## Test Framework Structure

### 1. Test Suite Organization

```
include/nlm/
├── test/
│   ├── framework/
│   │   ├── TestFramework.hpp          # Core test framework
│   │   ├── TestBase.hpp               # Base test class
│   │   ├── TestSuite.hpp              # Test suite container
│   │   └── TestReport.hpp             # Test reporting
│   ├── core/
│   │   ├── TestBrain.hpp             # Brain functionality tests
│   │   ├── TestMemory.hpp            # Memory system tests
│   │   ├── TestNeuromodulation.hpp   # Neuromodulation tests
│   │   ├── TestPlasticity.hpp        # Plasticity tests
│   │   └── TestSensory.hpp           # Sensory processing tests
│   ├── integration/
│   │   ├── TestPhase6.hpp            # Phase 6 integration tests
│   │   ├── TestConfiguration.hpp     # Configuration tests
│   │   ├── TestCLI.hpp              # Command-line interface tests
│   │   └── TestPerformance.hpp      # Performance tests
│   └── performance/
│       ├── TestPerformanceMonitor.hpp # Performance monitoring tests
│       ├── TestProfiling.hpp         # Detailed profiling tests
│       └── TestBenchmarking.hpp      # Benchmarking tests
└── tests/
    ├── main.cpp                      # Main test runner
    ├── test_main.cpp                 # Legacy test runner compatibility
    └── test_brain.cpp                # Legacy brain tests
```

### 2. Core Test Framework

#### 2.1 Test Framework Classes

```cpp
// Core test framework classes
class TestFramework {
public:
    TestFramework(const TestFrameworkConfig& config);
    ~TestFramework();
    
    // Add test suites
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    
    // Run all tests
    TestResults runAllTests();
    
    // Run specific test suites
    TestResults runTestSuite(const std::string& name);
    
    // Get test statistics
    TestStatistics getStatistics() const;
    
    // Generate reports
    void generateReport(const std::string& filename);
    
    // Export results
    void exportResults(const std::string& filename);
    
private:
    std::vector<std::unique_ptr<TestSuite>> testSuites_;
    TestFrameworkConfig config_;
    TestResults currentResults_;
};

// Test suite container
class TestSuite {
public:
    TestSuite(const std::string& name, TestLevel level);
    
    void addTest(std::unique_ptr<TestCase> test);
    
    std::string getName() const { return name_; }
    TestLevel getLevel() const { return level_; }
    size_t getTestCount() const { return tests_.size(); }
    
    TestResults run();
    
private:
    std::string name_;
    TestLevel level_;
    std::vector<std::unique_ptr<TestCase>> tests_;
};

// Base test case
class TestCase {
public:
    TestCase(const std::string& name);
    virtual ~TestCase() = default;
    
    std::string getName() const { return name_; }
    TestStatus getStatus() const { return status_; }
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run() = 0;
    
    void assertTrue(bool condition, const std::string& message = "");
    void assertEqual(double expected, double actual, double epsilon = 1e-6, const std::string& message = "");
    void assertContains(const std::string& container, const std::string& value, const std::string& message = "");
    void assertNotNull(void* ptr, const std::string& message = "");
    
protected:
    std::string name_;
    TestStatus status_ = TestStatus::Pending;
    std::vector<std::string> errorMessages_;
};
```

#### 2.2 Test Configuration

```cpp
// Test configuration structure
struct TestFrameworkConfig {
    // Test levels
    TestLevel defaultTestLevel = TestLevel::Standard;
    std::unordered_map<std::string, TestLevel> suiteLevels;
    
    // Reporting options
    bool enableVerboseOutput = true;
    bool enableProgressOutput = true;
    bool generateJUnitXML = true;
    bool generateHTMLReport = true;
    bool generateJSONReport = true;
    std::string outputDirectory = "./test_results";
    
    // Performance testing
    bool enablePerformanceTests = true;
    double performanceTimeout = 60.0;  // seconds
    size_t maxMemoryUsage = 1024;  // MB
    
    // Test filtering
    std::vector<std::string> excludeSuites;
    std::vector<std::string> excludeTests;
    std::unordered_map<std::string, std::vector<std::string>> suiteFilters;
    
    // Randomization
    bool randomizeTestOrder = false;
    size_t randomSeed = 42;
    
    // Parallel testing
    bool enableParallelTesting = false;
    size_t parallelThreadCount = std::thread::hardware_concurrency();
};
```

#### 2.3 Test Results and Reporting

```cpp
// Test results structure
struct TestResults {
    size_t totalTests = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    size_t skippedTests = 0;
    double totalTime = 0.0;
    
    std::vector<TestSuiteResult> suiteResults;
    std::vector<TestCaseResult> caseResults;
    std::vector<std::string> errors;
    
    double getPassRate() const { return totalTests > 0 ? (double)passedTests / totalTests : 0.0; }
};

// Test suite result
struct TestSuiteResult {
    std::string name;
    TestLevel level;
    size_t testCount = 0;
    size_t passedTests = 0;
    size_t failedTests = 0;
    size_t skippedTests = 0;
    double executionTime = 0.0;
    std::vector<TestCaseResult> caseResults;
    std::vector<std::string> errors;
};

// Test case result
struct TestCaseResult {
    std::string name;
    std::string suiteName;
    TestStatus status;
    double executionTime = 0.0;
    std::string errorMessage;
    std::vector<std::string> assertions;
};
```

### 3. Core Functionality Tests

#### 3.1 Brain Tests

```cpp
// Brain functionality tests
class TestBrain : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testBrainCreation();
    void testBrainInitialization();
    void testBrainStep();
    void testBrainReset();
    void testBrainRegions();
    void testBrainActionProduction();
    void testBrainStatus();
    void testBrainIntegration();
};
```

#### 3.2 Memory System Tests

```cpp
// Memory system tests
class TestMemorySystems : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testWorkingMemoryIntegration();
    void testEpisodicMemoryIntegration();
    void testAssociativeMemoryIntegration();
    void testMemoryReplay();
    void testMemoryConsolidation();
    void testMemoryCapacity();
    void testMemoryDecay();
};
```

#### 3.3 Neuromodulation Tests

```cpp
// Neuromodulation tests
class TestNeuromodulation : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testDopamineSystem();
    void testCuriositySystem();
    void testNoveltyDetection();
    void testPredictionError();
    void testNeuromodulationIntegration();
    void testRewardModulation();
    void testExplorationBehavior();
};
```

#### 3.4 Plasticity Tests

```cpp
// Plasticity tests
class TestPlasticity : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testSTDP();
    void testHebbianLearning();
    void testStructuralPlasticity();
    void testPlasticityIntegration();
    void testDevelopmentPlasticity();
    void testActivityDependentPlasticity();
};
```

#### 3.5 Sensory and Motor Tests

```cpp
// Sensory and motor processing tests
class TestSensoryMotor : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testSensoryProcessing();
    void testMotorControl();
    void testAgentBrain();
    void testActionSelection();
    void testSensoryIntegration();
    void testMotorIntegration();
};
```

### 4. Integration Tests

#### 4.1 Phase 6 Integration Tests

```cpp
// Phase 6 integration tests
class TestPhase6Integration : public TestCase {
public:
    TestPhase6Integration();
    
    void run() override;
    
private:
    void testPhase6Verification();
    void testMemoryIntegration();
    void testNeuromodulationIntegration();
    void testPredictionIntegration();
    void testDevelopmentIntegration();
    void testCheckpointing();
    void testReplaySystem();
    void testFullIntegration();
};
```

#### 4.2 Configuration Tests

```cpp
// Configuration tests
class TestConfiguration : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testConfigFileLoading();
    void testConfigValidation();
    void testConfigDefaults();
    void testConfigOverrides();
    void testCommandLineArguments();
    void testAdvancedConfigOptions();
    void testConfigSchema();
};
```

#### 4.3 Command Line Interface Tests

```cpp
// Command-line interface tests
class TestCLI : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testHelpOption();
    void testVersionOption();
    void testConfigOption();
    void testSeedOption();
    void testNeuronOption();
    void testTimestepOption();
    void testPlasticityOption();
    void testInvalidOptions();
    void testOptionParsing();
};
```

### 5. Performance Tests

#### 5.1 Performance Monitoring Tests

```cpp
// Performance monitoring tests
class TestPerformanceMonitor : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testPerformanceMonitorInitialization();
    void testMetricsCollection();
    void testProfiling();
    void testReporting();
    void testPerformanceIntegration();
    void testPerformanceAccuracy();
};
```

#### 5.2 Profiling Tests

```cpp
// Detailed profiling tests
class TestProfiling : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testOperationProfiling();
    void testDetailedTiming();
    void testPerformanceAnalysis();
    void testProfileExport();
    void testProfileAccuracy();
};
```

#### 5.3 Benchmarking Tests

```cpp
// Benchmarking tests
class TestBenchmarking : public TestCase {
public:
    TestBrain();
    
    void run() override;
    
private:
    void testScalabilityBenchmark();
    void testPerformanceBenchmark();
    void testMemoryBenchmark();
    void testCPUBenchmark();
    void testEnergyEfficiency();
    void testRealTimePerformance();
};
```

### 6. Test Utilities

#### 6.1 Test Data Management

```cpp
// Test data management
class TestDataManager {
public:
    TestDataManager(const std::string& dataDirectory);
    
    // Load test data
    std::vector<float> loadFloatVector(const std::string& filename);
    std::vector<std::vector<float>> load2DArray(const std::string& filename);
    std::string loadString(const std::string& filename);
    
    // Save test data
    void saveFloatVector(const std::string& filename, const std::vector<float>& data);
    void saveTestResults(const std::string& filename, const TestResults& results);
    
    // Get default test data paths
    std::string getDefaultTestDataPath(const std::string& testName) const;
    
private:
    std::string dataDirectory_;
    std::unordered_map<std::string, std::string> defaultPaths_;
};
```

#### 6.2 Performance Benchmarking Utilities

```cpp
// Performance benchmarking utilities
class BenchmarkUtils {
public:
    struct BenchmarkResult {
        double elapsedTime;
        size_t memoryUsage;
        double throughput;
        double efficiency;
    };
    
    static BenchmarkResult benchmarkOperation(std::function<void()> operation, 
                                              size_t iterations = 100);
    
    static std::vector<BenchmarkResult> benchmarkMultipleOperations(
        const std::vector<std::function<void()>>& operations,
        size_t iterations = 100);
    
    static double calculateEfficiency(double work, double time, double resources);
    
    static std::string formatBenchmarkResult(const BenchmarkResult& result);
};
```

#### 6.3 Test Assertions

```cpp
// Enhanced test assertions
class TestAssertions {
public:
    static void assertWithinRange(double value, double min, double max, 
                                 const std::string& message = "");
    
    static void assertIncreasing(const std::vector<double>& values,
                                 const std::string& message = "");
    
    static void assertDecreasing(const std::vector<double>& values,
                                 const std::string& message = "");
    
    static void assertConverging(const std::vector<double>& values,
                                 double threshold = 1e-3,
                                 size_t window = 10,
                                 const std::string& message = "");
    
    static void assertPerformanceWithinLimits(double actual, double limit,
                                              const std::string& metricName = "");
    
    static void assertMemoryWithinLimits(size_t actual, size_t limit,
                                         const std::string& metricName = "");
};
```

### 7. Test Execution and Reporting

#### 7.1 Test Runner

```cpp
// Main test runner
class TestRunner {
public:
    TestRunner(const TestFrameworkConfig& config);
    
    // Add test suites
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    
    // Run all tests
    TestResults runAllTests();
    
    // Run tests with specific filter
    TestResults runTests(const std::vector<std::string>& filter);
    
    // Run tests for specific suite
    TestResults runSuite(const std::string& suiteName);
    
    // Generate comprehensive report
    void generateReport(const std::string& outputDirectory);
    
    // Export results
    void exportResults(const std::string& filename);
    
    // Get current test results
    TestResults getResults() const { return results_; }
    
private:
    std::unique_ptr<TestFramework> testFramework_;
    TestResults results_;
    TestFrameworkConfig config_;
};
```

#### 7.2 Report Generation

```cpp
// Comprehensive report generation
class ReportGenerator {
public:
    void generateTextReport(const TestResults& results, const std::string& filename);
    void generateHTMLReport(const TestResults& results, const std::string& filename);
    void generateJSONReport(const TestResults& results, const std::string& filename);
    void generateJUnitXMLReport(const TestResults& results, const std::string& filename);
    void generatePerformanceReport(const TestResults& results, const std::string& filename);
    
    void generateAllReports(const TestResults& results, const std::string& outputDirectory);
    
private:
    void createReportDirectory(const std::string& outputDirectory);
    std::string escapeHTML(const std::string& text) const;
    std::string formatDuration(double seconds) const;
    std::string formatMemorySize(size_t bytes) const;
};
```

### 8. Example Test Implementation

#### 8.1 Simple Test Example

```cpp
// Simple test example
class ExampleBrainTest : public TestCase {
public:
    ExampleBrainTest() : TestCase("ExampleBrainTest") {}
    
    void run() override {
        // Setup
        setup();
        
        try {
            // Test 1: Brain creation
            testBrainCreation();
            
            // Test 2: Brain initialization
            testBrainInitialization();
            
            // Test 3: Brain step
            testBrainStep();
            
            // Mark as passed
            status_ = TestStatus::Passed;
            
        } catch (const std::exception& e) {
            // Mark as failed with error
            status_ = TestStatus::Failed;
            errorMessages_.push_back(std::string("Test failed: ") + e.what());
        }
        
        // Teardown
        teardown();
    }
    
private:
    void testBrainCreation() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        
        // Verify brain created successfully
        assertNotNull(&brain, "Brain should not be null");
        assertEqual(1, brain.getRegionCount(), "Should have 1 region");
    }
    
    void testBrainInitialization() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        
        assertTrue(success, "Brain initialization should succeed");
        assertEqual(2, brain.getRegionCount(), "Should have 2 regions after initialization");
        assertTrue(brain.getTotalNeuronCount() > 0, "Should have neurons after initialization");
    }
    
    void testBrainStep() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        size_t initialSpikes = brain.getTotalSpikeCount();
        
        // Run a few steps
        for (size_t step = 0; step < 10; ++step) {
            brain.step(step);
        }
        
        size_t finalSpikes = brain.getTotalSpikeCount();
        
        // Spike count should increase or stay the same
        assertGreaterThanEqual(finalSpikes, initialSpikes, "Spike count should not decrease");
    }
};
```

#### 8.2 Performance Test Example

```cpp
// Performance test example
class PerformanceExampleTest : public TestCase {
public:
    PerformanceExampleTest() : TestCase("PerformanceExampleTest") {}
    
    void run() override {
        // Setup performance monitoring
        PerformanceConfig perfConfig;
        perfConfig.enableProfiling = true;
        perfConfig.enableMetrics = true;
        perfConfig.samplingInterval = 0.1;
        
        // Run performance test
        setupPerformanceMonitoring(perfConfig);
        
        try {
            // Run simulation with performance monitoring
            runPerformanceTest();
            
            // Analyze performance results
            analyzePerformanceResults();
            
            status_ = TestStatus::Passed;
            
        } catch (const std::exception& e) {
            status_ = TestStatus::Failed;
            errorMessages_.push_back(std::string("Performance test failed: ") + e.what());
        }
        
        teardownPerformanceMonitoring();
    }
    
private:
    void setupPerformanceMonitoring(const PerformanceConfig& config) {
        // Initialize performance monitor
        auto perfMonitor = std::make_unique<RealTimePerformanceMonitor>(brain_.get(), config);
        perfMonitor_ = std::move(perfMonitor);
        
        // Start performance monitoring
        perfMonitor_->setCallback([this](const PerformanceStats& stats) {
            onPerformanceUpdate(stats);
        });
    }
    
    void runPerformanceTest() {
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < 1000; ++step) {
            brain_->step(step, step * 0.001);
            perfMonitor_->update(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime_ = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    void analyzePerformanceResults() {
        PerformanceStats stats = perfMonitor_->getCurrentStats();
        
        // Calculate performance metrics
        double stepsPerSecond = 1000.0 / executionTime_;
        double avgFiringRate = stats.neural.averageFiringRate;
        double eiRatio = stats.neural.eiRatio;
        
        // Assert performance criteria
        assertGreaterThan(stepsPerSecond, 100.0, "Should process at least 100 steps per second");
        assertGreaterThan(avgFiringRate, 0.0, "Should have some neural activity");
        assertGreaterThan(eiRatio, 0.0, "Should have some excitation/inhibition balance");
        
        // Store results
        performanceResults_.stepsPerSecond = stepsPerSecond;
        performanceResults_.averageFiringRate = avgFiringRate;
        performanceResults_.eiRatio = eiRatio;
    }
    
    void onPerformanceUpdate(const PerformanceStats& stats) {
        // Handle real-time performance updates
        // This can be used for adaptive testing or early termination
        if (stats.computational.cpuTimePercentage > 90.0) {
            // High CPU usage - log warning
            performanceWarnings_.push_back("High CPU usage detected");
        }
    }
    
    // Test data
    std::shared_ptr<nlm::Brain> brain_;
    std::unique_ptr<RealTimePerformanceMonitor> perfMonitor_;
    double executionTime_ = 0.0;
    
    struct PerformanceResults {
        double stepsPerSecond;
        double averageFiringRate;
        double eiRatio;
    } performanceResults_;
    
    std::vector<std::string> performanceWarnings_;
};
```

### 9. Test Configuration Examples

#### 9.1 Quick Test Configuration

```cpp
// Quick test configuration
TestFrameworkConfig quickTestConfig;
quickTestConfig.defaultTestLevel = TestLevel::Quick;
quickTestConfig.enableVerboseOutput = false;
quickTestConfig.enableProgressOutput = true;
quickTestConfig.generateJUnitXML = false;
quickTestConfig.generateHTMLReport = false;
quickTestConfig.generateJSONReport = true;
quickTestConfig.outputDirectory = "./quick_test_results";
quickTestConfig.randomizeTestOrder = true;
```

#### 9.2 Comprehensive Test Configuration

```cpp
// Comprehensive test configuration
TestFrameworkConfig comprehensiveConfig;
comprehensiveConfig.defaultTestLevel = TestLevel::Comprehensive;
comprehensiveConfig.enableVerboseOutput = true;
comprehensiveConfig.enableProgressOutput = true;
comprehensiveConfig.generateJUnitXML = true;
comprehensiveConfig.generateHTMLReport = true;
comprehensiveConfig.generateJSONReport = true;
comprehensiveConfig.outputDirectory = "./comprehensive_test_results";
comprehensiveConfig.enablePerformanceTests = true;
comprehensiveConfig.performanceTimeout = 120.0;
comprehensiveConfig.maxMemoryUsage = 2048;
comprehensiveConfig.randomizeTestOrder = true;
comprehensiveConfig.enableParallelTesting = true;
```

### 10. Test Integration with Build System

#### 10.1 CMake Integration

```cmake
# CMake integration for NLM tests
include(CTest)

# Enable testing
enable_testing()

# Add test executable
add_executable(nlm_test
    test_main.cpp
    test_framework/TestFramework.cpp
    test_framework/TestBase.cpp
    test_framework/TestSuite.cpp
    test_framework/TestReport.cpp
    
    core/TestBrain.cpp
    core/TestMemory.cpp
    core/TestNeuromodulation.cpp
    core/TestPlasticity.cpp
    core/TestSensory.cpp
    
    integration/TestPhase6.cpp
    integration/TestConfiguration.cpp
    integration/TestCLI.cpp
    integration/TestPerformance.cpp
    
    performance/TestPerformanceMonitor.cpp
    performance/TestProfiling.cpp
    performance/TestBenchmarking.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)

# Add tests to CTest
add_test(NAME nlm_unit_tests COMMAND nlm_test)

# Add performance tests
add_test(NAME nlm_performance_tests COMMAND nlm_test --performance-only)

# Add integration tests
add_test(NAME nlm_integration_tests COMMAND nlm_test --integration-only)

# Set test properties
set_tests_properties(nlm_unit_tests PROPERTIES TIMEOUT 300)
set_tests_properties(nlm_performance_tests PROPERTIES TIMEOUT 600)
set_tests_properties(nlm_integration_tests PROPERTIES TIMEOUT 1200)
```

#### 10.2 Python Integration

```python
# Python bindings for test framework
import ctypes
import json
import os
from pathlib import Path

class NLMPythonTestFramework:
    def __init__(self, library_path):
        self.library_path = library_path
        self.lib = ctypes.CDLL(library_path)
        
        # Initialize test framework
        self.lib.nlm_test_framework_init.argtypes = [ctypes.c_char_p]
        self.lib.nlm_test_framework_run.argtypes = None
        self.lib.nlm_test_framework_get_results.argtypes = None
        
    def run_tests(self, test_config_path=None):
        """Run NLM tests from Python"""
        if test_config_path:
            self.lib.nlm_test_framework_init(test_config_path.encode())
        
        self.lib.nlm_test_framework_run()
        
        results = self.lib.nlm_test_framework_get_results()
        return self.parse_results(results)
    
    def parse_results(self, results_ptr):
        """Parse test results from C structure"""
        # Parse JSON from results
        with open(results_ptr, 'r') as f:
            return json.load(f)
    
    def run_performance_tests(self):
        """Run performance tests"""
        self.lib.nlm_test_framework_run_performance_tests()
    
    def run_integration_tests(self):
        """Run integration tests"""
        self.lib.nlm_test_framework_run_integration_tests()
```

## Conclusion

The NLM test framework provides a comprehensive, extensible testing solution for the neural simulation system. It enables:

1. **Comprehensive Testing**: From unit tests to integration tests, covering all aspects of the NLM system
2. **Performance Testing**: Built-in performance monitoring and benchmarking capabilities
3. **Configurable Testing**: Flexible test configuration for different testing scenarios
4. **Detailed Reporting**: Multiple output formats for test results and analysis
5. **Integration**: Seamless integration with the build system and existing testing infrastructure
6. **Extensibility**: Easy addition of new test cases and test suites

The framework is designed to help developers ensure the correctness, performance, and reliability of the NLM system while providing detailed feedback for optimization and validation.
