// NLM Test Framework
// Phase 2: Enhanced Test System with Better Isolation and Reporting

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Test result structure
enum class TestFormat {
    Console,
    JSON,
    CSV,
    Detailed,
    Compact
};

struct TestResult {
    std::string testName;
    std::string testDescription;
    bool passed;
    std::string failureMessage;
    std::chrono::duration<double> executionTime;
    size_t neuronsCreated;
    size_t synapsesCreated;
    size_t totalSpikes;
    std::vector<std::string> messages;
    
    TestResult(const std::string& name, const std::string& description)
        : testName(name), testDescription(description), passed(false), 
          neuronsCreated(0), synapsesCreated(0), totalSpikes(0) {}
};

// Test fixture base class
class TestFixture {
public:
    TestFixture() = default;
    virtual ~TestFixture() = default;
    
    // Setup before each test
    virtual void setup() {}
    
    // Teardown after each test
    virtual void teardown() {}
    
    // Test methods (override in derived classes)
    virtual void test_neural_connectivity() {}
    virtual void test_plasticity_learning() {}
    virtual void test_stdp_verification() {}
    virtual void test_brain_functionality() {}
    virtual void test_memory_systems() {}
    virtual void test_neuromodulation() {}
    virtual void test_development() {}
    
    // Utility methods
    template<typename T>
    T getConfigValue(const std::string& key, const T& defaultValue) {
        if (auto config = appConfig_.lock()) {
            return config->getOr<T>(key, defaultValue);
        }
        return defaultValue;
    }
    
    void logMessage(const std::string& message, LogLevel level = LogLevel::Info) {
        if (logger_) {
            logger_->log(level, message);
        }
    }
    
protected:
    std::weak_ptr<Config> appConfig_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    
    friend class TestFramework;
};

// Test suite for categorizing tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    void addTest(const std::string& testName, std::function<void(TestFixture&)> testFunction) {
        tests_.emplace_back(testName, testFunction);
    }
    
    const std::vector<std::pair<std::string, std::function<void(TestFixture&)>>>& getTests() const {
        return tests_;
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::pair<std::string, std::function<void(TestFixture&)>>> tests_;
};

// Enhanced test framework with isolation and reporting
class TestFramework {
public:
    TestFramework();
    ~TestFramework();
    
    // Register a test fixture
    void registerFixture(std::function<std::unique_ptr<TestFixture>()> fixtureFactory);
    
    // Register a test suite
    void registerSuite(std::shared_ptr<TestSuite> suite);
    
    // Run all tests
    int runAll(const std::string& configFile = "");
    
    // Run tests from specific suites
    int runSuites(const std::vector<std::string>& suiteNames);
    
    // Run specific test
    int runTest(const std::string& testName);
    
    // Run tests with custom configuration
    int runWithConfig(const std::vector<std::string>& testNames, 
                     const std::string& configFile = "",
                     const std::unordered_map<std::string, std::string>& overrides = {});
    
    // Get test results
    const std::vector<TestResult>& getResults() const { return results_; }
    
    // Get available suites
    std::vector<std::string> getAvailableSuites() const;
    
    // Get available tests
    std::vector<std::string> getAvailableTests() const;
    
    // Get test statistics
    struct TestStats {
        size_t totalTests = 0;
        size_t passedTests = 0;
        size_t failedTests = 0;
        double totalExecutionTime = 0.0;
        size_t totalNeuronsCreated = 0;
        size_t totalSynapsesCreated = 0;
        size_t totalSpikes = 0;
    };
    
    TestStats getStats() const;
    
    // Clear results
    void clearResults() { results_.clear(); }
    
    // Generate report
    std::string generateReport(TestResult::TestFormat format = TestResult::TestFormat::Console) const;
    
    // Save report to file
    bool saveReport(const std::string& filepath, TestResult::TestFormat format = TestResult::TestFormat::Console) const;
    
    // Set output stream
    void setOutputStream(std::ostream& stream) { outputStream_ = &stream; }
    
    // Enable/disable parallel execution
    void setParallelExecution(bool enabled) { parallelExecution_ = enabled; }
    
    // Set maximum memory usage
    void setMaxMemoryMB(size_t maxMemoryMB) { maxMemoryMB_ = maxMemoryMB; }
    
    // Set timeout
    void setTimeoutSeconds(double timeoutSeconds) { timeoutSeconds_ = timeoutSeconds; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Create brain for testing
    std::shared_ptr<Brain> createBrainForTesting(const std::shared_ptr<Config>& config);
    
    // Initialize configuration
    bool initializeConfiguration(const std::string& configFile, 
                                 const std::unordered_map<std::string, std::string>& overrides);
    
    // Run single test with isolation
    int runSingleTest(const std::string& testName, const std::shared_ptr<Config>& config);
    
    // Execute test with fixture
    TestResult executeTest(const std::string& testName, 
                          std::function<void(TestFixture&)> testFunction,
                          std::unique_ptr<TestFixture> fixture);
    
    // Format output based on configuration
    void printTestResults(const std::vector<TestResult>& results) const;
    
    // Validate test configuration
    bool validateTestConfiguration(const std::string& testName, const std::shared_ptr<Config>& config) const;
    
    // Monitor memory usage
    void monitorMemoryUsage() const;
    
    // Check timeout
    bool checkTimeout() const;
    
    // Get current time
    double getCurrentTime() const;
    
    std::vector<TestResult> results_;
    std::vector<std::function<std::unique_ptr<TestFixture>()>> fixtureFactories_;
    std::vector<std::shared_ptr<TestSuite>> suites_;
    std::ostream* outputStream_;
    bool parallelExecution_;
    size_t maxMemoryMB_;
    double timeoutSeconds_;
    std::chrono::steady_clock::time_point startTime_;
};

// Built-in test fixtures
class ConnectivityTestFixture : public TestFixture {
public:
    void test_neural_connectivity() override;
};

class PlasticityTestFixture : public TestFixture {
public:
    void test_plasticity_learning() override;
};

class STDPTestFixture : public TestFixture {
public:
    void test_stdp_verification() override;
};

class BrainFunctionalityTestFixture : public TestFixture {
public:
    void test_brain_functionality() override;
    void test_memory_systems() override;
    void test_neuromodulation() override;
    void test_development() override;
};

// Convenience functions for programmatic test execution
namespace TestProgrammatic {
    // Run test with configuration file
    int runTestWithConfig(const std::string& testName, const std::string& configFile = "");
    
    // Run multiple tests
    int runTests(const std::vector<std::string>& testNames, const std::string& configFile = "");
    
    // Run all tests
    int runAllTests(const std::string& configFile = "");
    
    // Get test results
    std::string getTestResults(const std::vector<std::string>& testNames, const std::string& configFile = "");
    
    // Save test results to file
    bool saveTestResults(const std::vector<std::string>& testNames, 
                        const std::string& configFile,
                        const std::string& filepath,
                        TestResult::TestFormat format = TestResult::TestFormat::JSON);
}

} // namespace nlm
