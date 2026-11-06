// TestRunner.hpp - Unified testing framework for NLM

#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace tests {

/**
 * Test result structure for comprehensive reporting
 */
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    double executionTime; // in seconds
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& testName, const std::string& testCategory = "General")
        : name(testName), category(testCategory), passed(false), 
          executionTime(0.0), lineNumber(0) {}
};

/**
 * Test suite for organizing related tests
 */
class TestSuite {
public:
    TestSuite(const std::string& name) : name(name) {}
    virtual ~TestSuite() = default;
    
    const std::string& getName() const { return name; }
    const std::vector<TestResult>& getResults() const { return results; }
    size_t getPassedCount() const { return passedCount; }
    size_t getFailedCount() const { return failedCount; }
    size_t getTotalCount() const { return totalCount; }
    double getTotalExecutionTime() const { return totalExecutionTime; }
    
    void addResult(const TestResult& result) {
        results.push_back(result);
        totalCount++;
        if (result.passed) {
            passedCount++;
        } else {
            failedCount++;
        }
        totalExecutionTime += result.executionTime;
    }
    
    void clear() {
        results.clear();
        passedCount = 0;
        failedCount = 0;
        totalCount = 0;
        totalExecutionTime = 0.0;
    }
    
    virtual void runAllTests() = 0;
    
protected:
    std::string name;
    std::vector<TestResult> results;
    size_t passedCount = 0;
    size_t failedCount = 0;
    size_t totalCount = 0;
    double totalExecutionTime = 0.0;
};

/**
 * Simple assertion macros for testing
 */
#define ASSERT(condition, message) \
    nlm::tests::AssertHelper(condition, message, __FILE__, __LINE__)

class AssertHelper {
public:
    AssertHelper(bool condition, const std::string& message, 
                 const std::string& file, int line) {
        if (!condition) {
            std::cerr << "[FAIL] Assertion failed: " << message << std::endl;
            std::cerr << "  File: " << file << std::endl;
            std::cerr << "  Line: " << line << std::endl;
            throw std::runtime_error(message);
        }
    }
};

/**
 * Comprehensive test runner with reporting
 */
class TestRunner {
public:
    TestRunner() = default;
    ~TestRunner() = default;
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites.push_back(std::move(suite));
    }
    
    void runAllTests() {
        std::cout << "===========================================\n";
        std::cout << "         NLM NEURAL LEARNING MACHINE TESTS\n";
        std::cout << "===========================================\n" << std::endl;
        
        std::cout << "Running " << suites.size() << " test suites...\n" << std::endl;
        
        totalStartTime = std::chrono::high_resolution_clock::now();
        
        for (auto& suite : suites) {
            std::cout << "Running test suite: " << suite->getName() << "...\n";
            
            auto suiteStartTime = std::chrono::high_resolution_clock::now();
            
            try {
                suite->runAllTests();
            } catch (const std::exception& e) {
                std::cout << "  Exception during test execution: " << e.what() << std::endl;
            }
            
            auto suiteEndTime = std::chrono::high_resolution_clock::now();
            auto suiteDuration = std::chrono::duration<double>(suiteEndTime - suiteStartTime).count();
            
            printSuiteResults(*suite, suiteDuration);
        }
        
        totalEndTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double>(totalEndTime - totalStartTime).count();
        
        printSummary(totalDuration);
    }
    
    const std::vector<std::unique_ptr<TestSuite>>& getSuites() const { return suites; }
    
private:
    void printSuiteResults(const TestSuite& suite, double suiteDuration) {
        std::cout << "  " << suite.getName() << " Results:" << std::endl;
        std::cout << "    Tests: " << suite.getTotalCount() << " (" 
                  << suite.getPassedCount() << " passed, " 
                  << suite.getFailedCount() << " failed)" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(3) << suiteDuration << "s" << std::endl;
        
        if (suite.getFailedCount() > 0) {
            std::cout << "    Status: FAILED" << std::endl;
            std::cout << "      Failed tests:" << std::endl;
            for (const auto& result : suite.getResults()) {
                if (!result.passed) {
                    std::cout << "        - " << result.name << ": " << result.message << std::endl;
                }
            }
        } else {
            std::cout << "    Status: PASSED" << std::endl;
        }
        std::cout << std::endl;
    }
    
    void printSummary(double totalDuration) {
        std::cout << "===========================================\n";
        std::cout << "           TEST SUMMARY\n";
        std::cout << "===========================================\n" << std::endl;
        
        int totalPassed = 0;
        int totalFailed = 0;
        
        for (const auto& suite : suites) {
            totalPassed += suite->getPassedCount();
            totalFailed += suite->getFailedCount();
        }
        
        int totalTests = totalPassed + totalFailed;
        double totalTime = std::chrono::duration<double>(totalEndTime - totalStartTime).count();
        
        std::cout << "Overall Results:" << std::endl;
        std::cout << "  Total Tests: " << totalTests << std::endl;
        std::cout << "  Passed:      " << totalPassed << std::endl;
        std::cout << "  Failed:      " << totalFailed << std::endl;
        std::cout << "  Total Time:  " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        
        if (totalFailed == 0) {
            std::cout << std::endl;
            std::cout << "✓ ALL TESTS PASSED!" << std::endl;
            std::cout << "The NLM neural system is functioning correctly." << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "✗ TESTS FAILED" << std::endl;
            std::cout << "" << totalFailed << " test(s) failed. Please review the issues above." << std::endl;
        }
        
        std::cout << "===========================================\n";
    }
    
    std::vector<std::unique_ptr<TestSuite>> suites;
    std::chrono::high_resolution_clock::time_point totalStartTime;
    std::chrono::high_resolution_clock::time_point totalEndTime;
};

} // namespace tests
} // namespace nlm

#endif // NLM_TEST_RUNNER_HPP