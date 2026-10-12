// NLM Test Framework
// Comprehensive test runner for NLM simulation testing

#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>
#include <memory>
#include <map>

namespace nlm {

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTimeMs;
    
    TestResult(const std::string& testName, bool testPassed, const std::string& testMessage = "")
        : name(testName), passed(testPassed), message(testMessage), executionTimeMs(0.0) {}
};

class TestRunner {
private:
    std::map<std::string, std::function<bool(std::shared_ptr<Brain>)>> testCases;
    std::vector<std::string> testOrder;
    bool verbose;
    
public:
    TestRunner() : verbose(true) {}
    
    void setVerbose(bool verbose) {
        this->verbose = verbose;
    }
    
    void addTest(const std::string& name, std::function<bool(std::shared_ptr<Brain>)> testFunc) {
        testCases[name] = testFunc;
        testOrder.push_back(name);
    }
    
    bool runTest(std::shared_ptr<Brain> brain, const std::string& testName) {
        if (testCases.find(testName) == testCases.end()) {
            std::cerr << "ERROR: Test case not found: " << testName << std::endl;
            return false;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = testCases[testName](brain);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double executionTime = duration.count();
        
        if (verbose) {
            std::cout << "=== " << testName << " ===" << std::endl;
            std::cout << "Result: " << (result ? "PASSED" : "FAILED") << std::endl;
            std::cout << "Execution time: " << executionTime << " ms" << std::endl;
            std::cout << std::endl;
        }
        
        // Store test result (could be extended to store in a results vector)
        TestResult testResult(testName, result, result ? "Test passed" : "Test failed");
        testResult.executionTimeMs = executionTime;
        
        return result;
    }
    
    std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;
        
        std::cout << "=== Running All NLM Tests ===" << std::endl;
        std::cout << std::endl;
        
        for (const auto& testName : testOrder) {
            bool passed = runTest(brain, testName);
            TestResult result(testName, passed);
            results.push_back(result);
            
            if (!passed && !verbose) {
                std::cout << "Test failed: " << testName << std::endl;
            }
        }
        
        return results;
    }
    
    void runAllTestsAndReport(std::shared_ptr<Brain> brain) {
        auto results = runAllTests(brain);
        
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << std::endl;
        
        size_t passed = 0;
        size_t failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            std::cout << result.name << ": " << (result.passed ? "PASSED" : "FAILED");
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << std::endl;
            
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            totalTime += result.executionTimeMs;
        }
        
        std::cout << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Total Time: " << totalTime << " ms" << std::endl;
        
        if (failed == 0) {
            std::cout << "✓ All tests passed!" << std::endl;
        } else {
            std::cout << "✗ " << failed << " tests failed." << std::endl;
        }
    }
    
    size_t getTestCount() const {
        return testCases.size();
    }
    
    const std::vector<std::string>& getTestNames() const {
        return testOrder;
    }
};

} // namespace nlm

#endif // NLM_TEST_RUNNER_HPP