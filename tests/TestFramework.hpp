// Test Framework Header
// Enhanced testing framework for NLM with better organization, integration tests, and comprehensive coverage

#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cassert>
#include <memory>
#include <algorithm>
#include <cmath>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

namespace test {

class TestResult {
public:
    std::string name;
    bool passed;
    double executionTime;
    std::string message;
    
    TestResult(const std::string& n, bool p, double t, const std::string& m = "")
        : name(n), passed(p), executionTime(t), message(m) {}
};

class TestSuite {
public:
    virtual ~TestSuite() = default;
    virtual void runAllTests() = 0;
    virtual std::string getName() const = 0;
    
    void addTest(const std::function<bool()>& test, const std::string& name) {
        tests.push_back({name, test});
    }
    
    std::vector<TestResult> runAll() {
        std::vector<TestResult> results;
        
        for (const auto& test : tests) {
            auto start = std::chrono::high_resolution_clock::now();
            bool passed = test.second();
            auto end = std::chrono::high_resolution_clock::now();
            
            double executionTime = std::chrono::duration<double>(end - start).count();
            results.emplace_back(test.first, passed, executionTime);
            
            if (!passed) {
                std::cout << "[FAIL] " << test.first << std::endl;
            } else {
                std::cout << "[PASS] " << test.first << " (" << executionTime << "s)" << std::endl;
            }
        }
        
        return results;
    }
    
protected:
    std::vector<std::pair<std::string, std::function<bool()>>> tests;
};

// Macro for test assertions
#define ASSERT_TRUE(condition, message) \
    if (!(condition)) { \
        std::cerr << "[ASSERTION FAILED] " << message << std::endl; \
        return false; \
    }

#define ASSERT_FALSE(condition, message) \
    if ((condition)) { \
        std::cerr << "[ASSERTION FAILED] " << message << std::endl; \
        return false; \
    }

#define ASSERT_EQUAL(actual, expected, message) \
    if ((actual) != (expected)) { \
        std::cerr << "[ASSERTION FAILED] " << message \
                  << ": expected " << (expected) \
                  << ", got " << (actual) << std::endl; \
        return false; \
    }

#define ASSERT_FLOAT_EQUAL(actual, expected, tolerance, message) \
    if (std::abs((actual) - (expected)) > (tolerance)) { \
        std::cerr << "[ASSERTION FAILED] " << message \
                  << ": expected " << (expected) \
                  << ", got " << (actual) << std::endl; \
        return false; \
    }

#define ASSERT_LESS_THAN(actual, max, message) \
    if ((actual) >= (max)) { \
        std::cerr << "[ASSERTION FAILED] " << message \
                  << ": expected less than " << (max) \
                  << ", got " << (actual) << std::endl; \
        return false; \
    }

#define ASSERT_GREATER_THAN(actual, min, message) \
    if ((actual) <= (min)) { \
        std::cerr << "[ASSERTION FAILED] " << message \
                  << ": expected greater than " << (min) \
                  << ", got " << (actual) << std::endl; \
        return false; \
    }

// Global test runner
class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites.push_back(std::move(suite));
    }
    
    void runAll() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "NLM TEST SUITE" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::vector<TestResult> allResults;
        double totalTime = 0.0;
        int passedCount = 0;
        int failedCount = 0;
        
        for (auto& suite : suites) {
            std::cout << "\nSuite: " << suite->getName() << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            
            auto results = suite->runAll();
            allResults.insert(allResults.end(), results.begin(), results.end());
            
            for (const auto& result : results) {
                totalTime += result.executionTime;
                if (result.passed) {
                    passedCount++;
                } else {
                    failedCount++;
                }
            }
        }
        
        // Summary
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total Tests: " << allResults.size() << std::endl;
        std::cout << "Passed: " << passedCount << std::endl;
        std::cout << "Failed: " << failedCount << std::endl;
        std::cout << "Total Time: " << totalTime << "s" << std::endl;
        
        if (failedCount > 0) {
            std::cout << "\nFAILED TESTS:" << std::endl;
            for (const auto& result : allResults) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << " (" << result.executionTime << "s)" << std::endl;
                    if (!result.message.empty()) {
                        std::cout << "    Message: " << result.message << std::endl;
                    }
                }
            }
            return;
        }
    }
    
private:
    TestRunner() = default;
    std::vector<std::unique_ptr<TestSuite>> suites;
};

} // namespace test

#endif // NLM_TEST_FRAMEWORK_HPP