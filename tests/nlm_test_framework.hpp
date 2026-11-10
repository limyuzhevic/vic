// Comprehensive NLM Test Framework
// This provides a modern, robust testing infrastructure for the NLM Brain Simulator

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include <algorithm>

namespace nlm_test {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    std::chrono::milliseconds duration;
    std::string file;
    int line;
};

// Test suite class
class TestSuite {
public:
    using TestFunction = std::function<void()>;
    
    TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(const std::string& testName, TestFunction test, const std::string& category = "General") {
        tests_.push_back({testName, category, test});
    }
    
    const std::string& getName() const { return name_; }
    
    const std::vector<std::tuple<std::string, std::string, TestFunction>>& getTests() const { return tests_; }
    
private:
    std::string name_;
    std::vector<std::tuple<std::string, std::string, TestFunction>> tests_;
};

// Global test runner
class TestRunner {
public:
    TestRunner() : passedTests_(0), failedTests_(0), totalTests_(0) {}
    
    void addSuite(TestSuite suite) {
        suites_.push_back(std::move(suite));
    }
    
    void runAll() {
        std::cout << "===================================\n";
        std::cout << "  NLM BRAIN SIMULATOR TEST SUITE\n";
        std::cout << "===================================\n\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (auto& suite : suites_) {
            runSuite(suite);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        printSummary(totalDuration);
    }
    
    const std::vector<TestResult>& getResults() const { return results_; }
    
private:
    void runSuite(TestSuite& suite) {
        std::cout << "Test Suite: " << suite.getName() << "\n";
        std::cout << "=" << std::string(suite.getName().length() + 15, '=') << "\n\n";
        
        for (auto& test : suite.getTests()) {
            runTest(std::get<0>(test), std::get<1>(test), std::get<2>(test));
        }
        
        std::cout << "\n";
    }
    
    void runTest(const std::string& testName, const std::string& category, TestFunction test) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::string testId = category + "::" + testName;
        TestResult result;
        result.name = testName;
        result.category = category;
        result.file = "";
        result.line = 0;
        
        try {
            test();
            result.passed = true;
            result.message = "Test passed";
            passedTests_++;
        } catch (const std::exception& e) {
            result.passed = false;
            result.message = std::string("Exception: ") + e.what();
            failedTests_++;
        } catch (...) {
            result.passed = false;
            result.message = "Unknown exception";
            failedTests_++;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        results_.push_back(result);
        
        // Print test result
        std::string status = result.passed ? "PASS" : "FAIL";
        std::cout << "  " << std::left << std::setw(30) << testId 
                  << " [" << std::right << std::setw(6) << result.duration.count() << "ms] "
                  << status << " - " << result.message << "\n";
    }
    
    void printSummary(const std::chrono::milliseconds& totalDuration) {
        totalTests_ = passedTests_ + failedTests_;
        
        std::cout << "\n";
        std::cout << "===================================\n";
        std::cout << "  TEST SUMMARY\n";
        std::cout << "===================================\n";
        std::cout << "  Total Tests:   " << totalTests_ << "\n";
        std::cout << "  Passed:        " << passedTests_ << "\n";
        std::cout << "  Failed:        " << failedTests_ << "\n";
        std::cout << "  Total Time:    " << totalDuration.count() << "ms\n";
        
        if (failedTests_ > 0) {
            std::cout << "\n";
            std::cout << "FAILED TESTS:\n";
            std::cout << "------------\n";
            
            for (const auto& result : results_) {
                if (!result.passed) {
                    std::cout << "  " << result.category << "::" << result.name 
                              << " - " << result.message << "\n";
                }
            }
        }
        
        std::cout << "\n";
        if (failedTests_ == 0) {
            std::cout << "🎉 ALL TESTS PASSED!\n";
        } else {
            std::cout << "❌ " << failedTests_ << " TESTS FAILED\n";
        }
    }
    
    std::vector<TestSuite> suites_;
    std::vector<TestResult> results_;
    size_t passedTests_;
    size_t failedTests_;
    size_t totalTests_;
};

// Global test runner instance
inline TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

// Helper macros for test definitions
#define TEST_CATEGORY(name) namespace test_##name { void runAllTests(); }
#define TEST_SUITE(name) TestSuite suite_##name(#name)
#define TEST(testName, testFunc, category) suite_##testName.addTest(testName, testFunc, category)
#define ASSERT(condition, message) if (!(condition)) throw std::runtime_error(message)

// Register a test suite
define inline void registerSuite(TestSuite suite) {
    getTestRunner().addSuite(std::move(suite));
}

// Auto-register test suites (called at program exit)
inline void autoRegister() {
    // This will be filled in by test implementations
}

// Convenience function to run all tests
inline void runAllTests() {
    getTestRunner().runAll();
}

} // namespace nlm_test

// Convenience macro for creating test suites
#define NLM_TEST_MAIN() int main() {\
    nlm_test::autoRegister();\
    nlm_test::runAllTests();\
    return 0;\
}