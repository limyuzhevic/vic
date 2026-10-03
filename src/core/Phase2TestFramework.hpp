// Phase 2 Test Framework Base Class
#pragma once

#include "core/Config/Config.hpp"
#include "core/TestConstants.hpp"
#include "core/Phase2ConfigManager.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>

namespace nlm {
namespace phase2 {

/**
 * Test result structure
 */
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
    std::vector<std::string> details;
    
    TestResult(const std::string& name) : 
        testName(name), passed(false), executionTime(0.0) {}
};

/**
 * Base test class for Phase 2 experiments
 */
class Phase2Test {
public:
    virtual ~Phase2Test() = default;
    
    // Run the test
    virtual TestResult run(std::shared_ptr<Config> config) = 0;
    
    // Get test name
    virtual std::string getName() const = 0;
    
    // Get test description
    virtual std::string getDescription() const = 0;
    
    // Check if test should run based on configuration
    virtual bool shouldRun(const Phase2TestConfig& config) const = 0;
    
    // Setup before test runs
    virtual void setup(std::shared_ptr<Config> config) {}
    
    // Teardown after test completes
    virtual void teardown(std::shared_ptr<Config> config) {}
    
    // Get test dependencies
    virtual std::vector<std::string> getDependencies() const { return {}; }
protected:
    // Helper function to format output
    template<typename T>
    std::string formatValue(const T& value, int precision = PHASE2_OUTPUT_PRECISION) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }
    
    // Helper function to measure execution time
    template<typename Func>
    double measureExecutionTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    // Helper function to log test progress
    void logProgress(const std::string& message) {
        std::cout << "[" << getName() << "] " << message << std::endl;
    }
    
    // Helper function to check if value is approximately equal
    bool approximatelyEqual(double a, double b, double epsilon = 1e-6) const {
        return std::abs(a - b) < epsilon;
    }
};

/**
 * Test framework manager
 */
class Phase2TestFramework {
public:
    Phase2TestFramework();
    ~Phase2TestFramework();
    
    // Disable copying
    Phase2TestFramework(const Phase2TestFramework&) = delete;
    Phase2TestFramework& operator=(const Phase2TestFramework&) = delete;
    
    // Add a test to the framework
    void addTest(std::shared_ptr<Phase2Test> test);
    
    // Run all tests
    std::vector<TestResult> runAllTests(std::shared_ptr<Config> config);
    
    // Run specific tests
    std::vector<TestResult> runTests(const std::vector<std::string>& testNames, 
                                     std::shared_ptr<Config> config);
    
    // Get all registered tests
    std::vector<std::shared_ptr<Phase2Test>> getAllTests() const { return tests_; }
    
    // Get test results
    std::vector<TestResult> getResults() const { return results_; }
    
    // Clear previous results
    void clearResults() { results_.clear(); }
    
    // Print test summary
    void printTestSummary() const;
    
    // Get the global test framework instance
    static Phase2TestFramework& getInstance();
    
    // Set global test framework
    static void setInstance(std::shared_ptr<Phase2TestFramework> framework);
    
    // Register default tests
    void registerDefaultTests();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::vector<std::shared_ptr<Phase2Test>> tests_;
    std::vector<TestResult> results_;
    
    // Internal helper functions
    void sortTestsByDependency(const std::vector<std::shared_ptr<Phase2Test>>& tests);
    bool areTestsCompatible(const std::vector<std::shared_ptr<Phase2Test>>& tests) const;
    std::vector<std::shared_ptr<Phase2Test>> getTestsToRun(const Phase2TestConfig& config) const;
};

} // namespace phase2
} // namespace nlm