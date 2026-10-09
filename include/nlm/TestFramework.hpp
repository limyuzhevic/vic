// NLM Test Framework
// Improved test runner with better reporting and structure

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class TestResult {
public:
    std::string name;
    bool passed;
    double durationMs;
    std::string message;
    std::string category;
    
    TestResult(const std::string& testName, bool testPassed, 
               double duration, const std::string& testMessage = "",
               const std::string& testCategory = "General")
        : name(testName), passed(testPassed), durationMs(duration),
          message(testMessage), category(testCategory) {}
};

class TestRunner {
public:
    TestRunner() : totalTests(0), passedTests(0), failedTests(0) {}
    
    template<typename TestFunc>
    void addTest(const std::string& name, TestFunc testFunc, 
                 const std::string& category = "General") {
        tests.push_back({name, testFunc, category});
    }
    
    void runAll() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Running " << tests.size() << " test suites..." << std::endl;
        std::cout << std::endl;
        
        for (const auto& test : tests) {
            runTest(test);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        printSummary(totalDuration);
    }
    
    void printSummary(double totalDuration) {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << " (" << 
            (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%)" << std::endl;
        std::cout << "Failed: " << failedTests << " (" <<
            (totalTests > 0 ? (failedTests * 100.0 / totalTests) : 0.0) << "%)" << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalDuration << " ms" << std::endl;
        std::cout << std::endl;
        
        if (failedTests > 0) {
            std::cout << "FAILED TESTS:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << " (" << result.category << ")" << std::endl;
                    if (!result.message.empty()) {
                        std::cout << "    Error: " << result.message << std::endl;
                    }
                }
            }
            std::cout << std::endl;
            std::cout << "OVERALL: TESTS FAILED" << std::endl;
        } else {
            std::cout << "OVERALL: ALL TESTS PASSED ✓" << std::endl;
        }
    }
    
    const std::vector<TestResult>& getResults() const { return results; }
    
private:
    struct TestSuite {
        std::string name;
        std::function<void()> testFunc;
        std::string category;
    };
    
    std::vector<TestSuite> tests;
    std::vector<TestResult> results;
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    
    void runTest(const TestSuite& test) {
        std::cout << "Running " << test.name << " (" << test.category << ")..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            test.testFunc();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            TestResult result(test.name, true, duration);
            results.push_back(result);
            passedTests++;
            
            std::cout << "  PASSED (" << std::fixed << std::setprecision(3) << duration << " ms)" << std::endl;
            
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            TestResult result(test.name, false, duration, e.what(), test.category);
            results.push_back(result);
            failedTests++;
            
            std::cout << "  FAILED (" << std::fixed << std::setprecision(3) << duration << " ms)" << std::endl;
            std::cout << "    Error: " << e.what() << std::endl;
        }
    }
};

// Helper macros for easier test registration
#define REGISTER_TEST(runner, category, name) \
    namespace { \
        class Test##__LINE__ { \
        public: \
            Test##__LINE__() { \
                runner.addTest(#name, &Test##__LINE__::runTest, category); \
            } \
            static void runTest() { \
                name(); \
            } \
        }; \
        Test##__LINE__ test##__LINE__##instance; \
    }

} // namespace nlm
