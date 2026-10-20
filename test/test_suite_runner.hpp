#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace integration_test {

/**
 * @struct TestMetrics
 * @brief Results from a single test execution
 */
struct TestMetrics {
    std::string testName;
    std::string phaseName;
    double executionTimeMs;
    bool passed;
    std::string errorMessage;
    std::unordered_map<std::string, double> customMetrics;
};

/**
 * @class IntegrationTestFramework
 * @brief Base class for all integration tests
 * 
 * This framework provides common utilities for testing system integration,
 * performance measurement, and validation of cross-system compatibility.
 */
class IntegrationTestFramework {
public:
    IntegrationTestFramework() = default;
    virtual ~IntegrationTestFramework() = default;
    
    /**
     * @brief Run the integration test
     * @return TestMetrics containing test results
     */
    virtual TestMetrics run() = 0;
    
    /**
     * @brief Get the name of the test phase
     * @return Phase name (e.g., "Phase 2", "Phase 3", etc.)
     */
    virtual std::string getPhaseName() const = 0;
    
    /**
     * @brief Get a descriptive name for the test
     * @return Test name
     */
    virtual std::string getTestName() const = 0;
    
    /**
     * @brief Get the full name combining phase and test
     * @return Combined name
     */
    std::string getFullTestName() const {
        return getPhaseName() + ": " + getTestName();
    }
    
protected:
    /**
     * @brief Record a custom metric for the test
     * @param name Metric name
     * @param value Metric value
     */
    void recordMetric(const std::string& name, double value) {
        // Implementation will be used by derived classes
        // This is a placeholder for future enhancement
    }
    
    /**
     * @brief Get the current simulation time
     * @return Current time in milliseconds
     */
    double getCurrentTimeMs() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double, std::milli>(duration).count();
    }
};

/**
 * @class TestSuiteRunner
 * @brief Manages and runs multiple integration tests
 */
class TestSuiteRunner {
public:
    TestSuiteRunner() = default;
    ~TestSuiteRunner() = default;
    
    /**
     * @brief Add a test to the suite
     * @param test Unique pointer to test instance
     */
    void addTest(std::unique_ptr<IntegrationTestFramework> test) {
        tests_.push_back(std::move(test));
    }
    
    /**
     * @brief Run all tests in the suite
     * @return Vector of test metrics for all tests
     */
    std::vector<TestMetrics> runAllTests() {
        return runAllTestsImpl();
    }
    
    /**
     * @brief Print test suite summary
     * @param results Vector of test metrics
     */
    void printSummary(const std::vector<TestMetrics>& results) {
        printSummaryImpl(results);
    }
    
private:
    std::vector<std::unique_ptr<IntegrationTestFramework>> tests_;
    
    /**
     * @brief Implementation of running all tests
     */
    std::vector<TestMetrics> runAllTestsImpl() {
        std::vector<TestMetrics> results;
        
        std::cout << "=== NLM Integration Test Suite ===" << std::endl;
        std::cout << std::endl;
        
        for (size_t i = 0; i < tests_.size(); ++i) {
            auto& test = tests_[i];
            std::cout << "Running test " << (i + 1) << "/" << tests_.size() << ": " << test->getFullTestName() << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            TestMetrics metrics = test->run();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            metrics.executionTimeMs = duration;
            
            results.push_back(metrics);
            
            if (metrics.passed) {
                std::cout << "  PASSED (" << duration << " ms)" << std::endl;
            } else {
                std::cout << "  FAILED (" << duration << " ms)" << std::endl;
                if (!metrics.errorMessage.empty()) {
                    std::cout << "    Error: " << metrics.errorMessage << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        printSummary(results);
        return results;
    }
    
    /**
     * @brief Implementation of printing summary
     */
    void printSummaryImpl(const std::vector<TestMetrics>& results) {
        int totalTests = results.size();
        int passedTests = 0;
        double totalTime = 0.0;
        
        std::cout << "=== Test Summary ===" << std::endl;
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            }
            totalTime += result.executionTimeMs;
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Total time: " << totalTime << " ms" << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
    }
};

} // namespace integration_test
} // namespace nlm