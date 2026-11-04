// NLM Test Framework
// A comprehensive test framework for NLM (Neural Learning Machine)
// Provides utilities for testing, validation, and reporting

#pragma once

#include "core/Config/Config.hpp"
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cassert>
#include <iomanip>

namespace nlm {

// Test status enumeration
enum class TestStatus {
    PENDING,
    RUNNING,
    PASSED,
    FAILED,
    SKIPPED
};

// Test result structure
struct TestResult {
    std::string name;
    TestStatus status;
    std::string message;
    double durationSeconds;
    std::string testId;
    
    TestResult() : status(TestStatus::PENDING), durationSeconds(0.0) {}
    TestResult(const std::string& n) : name(n), status(TestStatus::PENDING), durationSeconds(0.0) {}
};

// Test framework class
class TestFramework {
public:
    TestFramework() : totalTests(0), passedTests(0), failedTests(0), skippedTests(0) {}
    
    // Add a test case
    void addTest(const std::string& name, std::function<void()> testFunc, const std::string& category = "General") {
        TestCase testCase;
        testCase.name = name;
        testCase.testFunc = testFunc;
        testCase.category = category;
        testCase.result.status = TestStatus::PENDING;
        testCases.push_back(testCase);
        totalTests++;
    }
    
    // Add a test with configuration
    void addTest(const std::string& name, std::function<void()> testFunc, 
                 const std::string& category, const std::shared_ptr<Config>& config) {
        TestCase testCase;
        testCase.name = name;
        testCase.testFunc = testFunc;
        testCase.category = category;
        testCase.config = config;
        testCase.result.status = TestStatus::PENDING;
        testCases.push_back(testCase);
        totalTests++;
    }
    
    // Run all tests
    void runAllTests() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
        std::cout << "Running " << testCases.size() << " test cases..." << std::endl << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (auto& testCase : testCases) {
            runTest(testCase);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        printSummary(totalDuration.count());
    }
    
    // Run tests in a specific category
    void runTestsInCategory(const std::string& category) {
        std::cout << "=== NLM Test Framework - Category: " << category << " ===" << std::endl;
        
        for (auto& testCase : testCases) {
            if (testCase.category == category) {
                runTest(testCase);
            }
        }
        
        printSummary(0); // Summary without total time since we ran a subset
    }
    
    // Get test results
    const std::vector<TestCase>& getTestCases() const { return testCases; }
    
    // Get summary statistics
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Tests: " << totalTests
           << " | Passed: " << passedTests
           << " | Failed: " << failedTests
           << " | Skipped: " << skippedTests
           << " | Success Rate: " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%";
        return ss.str();
    }
    
    // Clear all tests
    void clear() {
        testCases.clear();
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        skippedTests = 0;
    }
    
    // Create test configuration
    static std::shared_ptr<Config> createTestConfig() {
        auto config = std::make_shared<Config>();
        
        // Set reasonable defaults for testing
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        return config;
    }
    
    // Test utilities
    template<typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error("Assertion failed: " + std::to_string(expected) + 
                                    " != " + std::to_string(actual) + 
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    template<typename T>
    void assertNotEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected == actual) {
            throw std::runtime_error("Assertion failed: " + std::to_string(expected) + 
                                    " == " + std::to_string(actual) + 
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    template<typename T>
    void assertGreaterThan(const T& expected, const T& actual, const std::string& message = "") {
        if (expected <= actual) {
            throw std::runtime_error("Assertion failed: " + std::to_string(expected) + 
                                    " <= " + std::to_string(actual) + 
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    template<typename T>
    void assertLessThan(const T& expected, const T& actual, const std::string& message = "") {
        if (expected >= actual) {
            throw std::runtime_error("Assertion failed: " + std::to_string(expected) + 
                                    " >= " + std::to_string(actual) + 
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    template<typename T>
    void assertInRange(const T& value, const T& min, const T& max, const std::string& message = "") {
        if (value < min || value > max) {
            throw std::runtime_error("Assertion failed: " + std::to_string(value) + 
                                    " not in range [" + std::to_string(min) + ", " + std::to_string(max) + "]" +
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed: condition is false" +
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            throw std::runtime_error("Assertion failed: condition is true" +
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    void assertNotNull(const void* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            throw std::runtime_error("Assertion failed: pointer is null" +
                                    (message.empty() ? "" : ": " + message));
        }
    }
    
    // Configuration validation utilities
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        bool valid = true;
        
        // Check required keys
        auto requiredKeys = config->getKeys();
        for (const auto& key : requiredKeys) {
            if (config->get<std::string>(key).is_null()) {
                std::cerr << "Warning: Config key \"" << key << "\" has no value" << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
    
    // Performance measurement
    template<typename Func>
    double measurePerformance(Func func, const std::string& name = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        func();
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        if (!name.empty()) {
            std::cout << "Performance measurement for \"" << name << "\": "
                     << duration.count() / 1000000.0 << "s" << std::endl;
        }
        
        return duration.count() / 1000000.0; // Return seconds
    }
    
private:
    struct TestCase {
        std::string name;
        std::string category;
        std::function<void()> testFunc;
        std::shared_ptr<Config> config;
        TestResult result;
    };
    
    std::vector<TestCase> testCases;
    int totalTests;
    int passedTests;
    int failedTests;
    int skippedTests;
    
    void runTest(TestCase& testCase) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        testCase.result.status = TestStatus::RUNNING;
        std::cout << "Running: " << testCase.name << " [" << testCase.category << "]" << std::endl;
        
        try {
            if (testCase.config) {
                // Use test configuration if provided
                testCase.testFunc();
            } else {
                testCase.testFunc();
            }
            
            // Test passed
            testCase.result.status = TestStatus::PASSED;
            testCase.result.message = "Test passed successfully";
            passedTests++;
            
        } catch (const std::runtime_error& e) {
            // Test failed
            testCase.result.status = TestStatus::FAILED;
            testCase.result.message = std::string("Test failed: ") + e.what();
            failedTests++;
            
            std::cerr << "FAILED: " << testCase.name << " [" << testCase.category << "]" 
                     << std::endl << "Error: " << e.what() << std::endl;
            
        } catch (...) {
            // Test failed with unknown exception
            testCase.result.status = TestStatus::FAILED;
            testCase.result.message = "Test failed with unknown exception";
            failedTests++;
            
            std::cerr << "FAILED: " << testCase.name << " [" << testCase.category << "]" 
                     << std::endl << "Error: Unknown exception" << std::endl;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        testCase.result.durationSeconds = duration.count() / 1000000.0;
    }
    
    void printSummary(double totalTime) {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << getSummary() << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        
        if (failedTests == 0) {
            std::cout << "✓ All tests passed!" << std::endl;
        } else {
            std::cout << "✗ " << failedTests << " tests failed." << std::endl;
        }
    }
};

} // namespace nlm
