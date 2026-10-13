#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

namespace NLMTesting {

// Test result enumeration
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

// Test metadata
struct TestMetadata {
    std::string name;
    std::string category;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    double duration;
    TestResult result;
    
    TestMetadata(const std::string& n, const std::string& c = "", const std::string& desc = "")
        : name(n), category(c), description(desc), timestamp(std::chrono::system_clock::now()),
          duration(0.0), result(TestResult::SKIPPED) {}
};

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestResult run() = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
    
    // Common test assertions
    TestResult assertTrue(bool condition, const std::string& message = "");
    TestResult assertFalse(bool condition, const std::string& message = "");
    TestResult assertEqual(double expected, double actual, double tolerance = 1e-6, const std::string& message = "");
    TestResult assertNotEqual(double expected, double actual, double tolerance = 1e-6, const std::string& message = "");
    TestResult assertGreaterThan(double expected, double actual, const std::string& message = "");
    TestResult assertLessThan(double expected, double actual, const std::string& message = "");
    TestResult assertInRange(double value, double min, double max, const std::string& message = "");
    TestResult assertNotNull(const std::shared_ptr<void>& ptr, const std::string& message = "");
    TestResult assertStringContains(const std::string& str, const std::string& substring, const std::string& message = "");
    TestResult assertFileExists(const std::string& path, const std::string& message = "");
};

// Test suite for organizing related tests
class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual std::string getName() const = 0;
    virtual std::vector<std::shared_ptr<TestCase>> getTests() const = 0;
    virtual bool setupSuite() { return true; }
    virtual bool teardownSuite() { return true; }
};

// Test runner for executing test suites
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    // Add test suite
    void addSuite(std::shared_ptr<TestSuite> suite);
    
    // Run all tests
    bool runAll(bool verbose = false);
    
    // Run tests by category
    bool runCategory(const std::string& category, bool verbose = false);
    
    // Run specific test by name
    bool runTest(const std::string& testName, bool verbose = false);
    
    // Get test results
    std::vector<TestMetadata> getResults() const { return results; }
    size_t getTotalTests() const { return totalTests; }
    size_t getPassedTests() const { return passedTests; }
    size_t getFailedTests() const { return failedTests; }
    size_t getSkippedTests() const { return skippedTests; }
    double getOverallSuccessRate() const { return totalTests > 0 ? (static_cast<double>(passedTests) / totalTests * 100.0) : 0.0; }
    
    // Get test statistics
    std::map<std::string, size_t> getTestCountsByCategory() const;
    double getAverageTestDuration() const;
    
    // Export results
    bool exportResults(const std::string& filename, const std::string& format = "txt");
    bool exportResultsJSON(const std::string& filename);
    bool exportResultsCSV(const std::string& filename);
    
    // Configuration
    void setVerbose(bool verbose) { this->verbose = verbose; }
    void setTimeout(double timeoutSeconds) { timeoutSeconds = timeout; }
    void setParallel(bool parallel) { parallel = parallel; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<std::shared_ptr<TestSuite>> suites;
    std::vector<TestMetadata> results;
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    size_t skippedTests;
    bool verbose;
    double timeout;
    bool parallel;
    
    void runTestSuite(std::shared_ptr<TestSuite> suite, bool verbose = false);
    TestResult runTestCase(std::shared_ptr<TestCase> testCase, bool verbose = false);
};

// Configuration validation tests
class ConfigValidationTests : public TestCase {
public:
    std::string getName() const override { return "Configuration Validation Tests"; }
    std::string getCategory() const override { return "Configuration"; }
    std::string getDescription() const override { return "Test configuration validation and parameter checking"; }
    
    TestResult run() override;
};

// Brain functionality tests
class BrainFunctionalityTests : public TestCase {
public:
    std::string getName() const override { return "Brain Functionality Tests"; }
    std::string getCategory() const override { return "Brain"; }
    std::string getDescription() const override { return "Test brain initialization and basic functionality"; }
    
    TestResult run() override;
};

// Plasticity tests
class PlasticityTests : public TestCase {
public:
    std::string getName() const override { return "Plasticity Tests"; }
    std::string getCategory() const override { return "Plasticity"; }
    std::string getDescription() const override { return "Test synaptic plasticity mechanisms (STDP, Hebbian)"; }
    
    TestResult run() override;
};

// Neuromodulation tests
class NeuromodulationTests : public TestCase {
public:
    std::string getName() const override { return "Neuromodulation Tests"; }
    std::string getCategory() const override { return "Neuromodulation"; }
    std::string getDescription() const override { return "Test neuromodulatory systems (dopamine, curiosity, novelty)"; }
    
    TestResult run() override;
};

// Memory system tests
class MemorySystemTests : public TestCase {
public:
    std::string getName() const override { return "Memory System Tests"; }
    std::string getCategory() const override { return "Memory"; }
    std::string getDescription() const override { return "Test memory systems (working, episodic, associative)"; }
    
    TestResult run() override;
};

// Performance benchmarks
class PerformanceBenchmarks : public TestCase {
public:
    std::string getName() const override { return "Performance Benchmarks"; }
    std::string getCategory() const override { return "Performance"; }
    std::string getDescription() const override { return "Benchmark system performance and scalability"; }
    
    TestResult run() override;
};

// Stress tests
class StressTests : public TestCase {
public:
    std::string getName() const override { return "Stress Tests"; }
    std::string getCategory() const override { return "Stress"; }
    std::string getDescription() const override { return "Test system under stress conditions"; }
    
    TestResult run() override;
};

// Command line interface tests
class CommandLineInterfaceTests : public TestCase {
public:
    std::string getName() const override { return "Command Line Interface Tests"; }
    std::string getCategory() const override { return "CLI"; }
    std::string getDescription() const override { return "Test command line interface and argument parsing"; }
    
    TestResult run() override;
};

// Integration test suite
class IntegrationTestSuite : public TestSuite {
public:
    std::string getName() const override { return "Integration Test Suite"; }
    
    std::vector<std::shared_ptr<TestCase>> getTests() const override {
        return {
            std::make_shared<ConfigValidationTests>(),
            std::make_shared<BrainFunctionalityTests>(),
            std::make_shared<PlasticityTests>(),
            std::make_shared<NeuromodulationTests>(),
            std::make_shared<MemorySystemTests>(),
            std::make_shared<PerformanceBenchmarks>(),
            std::make_shared<StressTests>(),
            std::make_shared<CommandLineInterfaceTests>(),
        };
    }
};

// Test utilities
namespace TestUtils {
    // Timing utilities
    double getCurrentTime();
    void sleep(double seconds);
    
    // String utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    
    // File utilities
    bool createDirectory(const std::string& path);
    bool deleteFile(const std::string& path);
    std::string generateRandomFilename(const std::string& extension = "");
    
    // Math utilities
    double calculateMean(const std::vector<double>& values);
    double calculateStandardDeviation(const std::vector<double>& values, double mean);
    double calculateMedian(std::vector<double> values);
    
    // Random number generation
    int generateRandomInt(int min, int max);
    double generateRandomDouble(double min, double max);
    bool generateRandomBool(double probability = 0.5);
    
    // Configuration utilities
    std::string getTestConfigPath();
    std::string getTestDataPath();
    std::string getTestOutputPath();
    
    // Logging utilities
    void logTestInfo(const std::string& message);
    void logTestError(const std::string& message);
    void logTestWarning(const std::string& message);
    
    // System utilities
    bool isSystemAvailable();
    std::string getSystemInfo();
    size_t getAvailableMemory();
    size_t getCPUCoreCount();
}

} // namespace NLMTesting