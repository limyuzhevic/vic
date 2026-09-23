// Test Framework Base Class
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

// Forward declarations for components
namespace nlm {
    class Config;
    class Logger;
    class HierarchicalLogger;
    class ComponentLogger;
    class BatchStepProcessor;
    class PerformanceProfiler;
}

// Test result structure
struct TestResult {
    std::string testName;
    std::string testDescription;
    std::string status; // "PASS", "FAIL", "ERROR", "SKIP"
    std::string message;
    double executionTimeMs;
    size_t memoryUsed;
    std::string file;
    int line;
    std::string stackTrace;
    
    TestResult(const std::string& name, const std::string& description) 
        : testName(name), testDescription(description), status("UNKNOWN"), 
          executionTimeMs(0.0), memoryUsed(0), file(""), line(0) {}
};

// Test category for organization
enum class TestCategory {
    Logging,
    Configuration,
    BatchProcessing,
    Performance,
    PythonBindings,
    SystemIntegration,
    MemoryManagement,
    ErrorHandling,
    PerformanceBenchmarks
};

// Base test fixture class providing common functionality
class TestFixture {
public:
    TestFixture(const std::string& name, TestCategory category, const std::string& description = "")
        : testName_(name), category_(category), description_(description),
          startTime_(std::chrono::high_resolution_clock::now()) {
        testResult_.testName = name;
        testResult_.testDescription = description;
    }
    
    virtual ~TestFixture() {
        auto endTime = std::chrono::high_resolution_clock::now();
        testResult_.executionTimeMs = 
            std::chrono::duration<double, std::milli>(endTime - startTime_).count();
    }
    
    // Virtual test method - override in derived classes
    virtual void run() = 0;
    
    // Test assertions
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            testResult_.status = "FAIL";
            testResult_.message = "Assertion failed: " + message;
            testResult_.file = "";
            testResult_.line = 0;
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        assertTrue(!condition, message);
    }
    
    void assertEqual(const std::string& expected, const std::string& actual, const std::string& message = "") {
        if (expected != actual) {
            testResult_.status = "FAIL";
            testResult_.message = "Expected \"" + expected + "\", got \"" + actual + "\": " + message;
        }
    }
    
    void assertEqual(int expected, int actual, const std::string& message = "") {
        if (expected != actual) {
            testResult_.status = "FAIL";
            testResult_.message = "Expected " + std::to_string(expected) + ", got " + std::to_string(actual) + ": " + message;
        }
    }
    
    void assertEqual(double expected, double actual, double epsilon = 1e-6, const std::string& message = "") {
        if (std::abs(expected - actual) > epsilon) {
            testResult_.status = "FAIL";
            testResult_.message = "Expected " + std::to_string(expected) + ", got " + std::to_string(actual) + ": " + message;
        }
    }
    
    void assertNotNull(const void* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            testResult_.status = "FAIL";
            testResult_.message = "Pointer is null: " + message;
        }
    }
    
    void assertThrows(std::function<void()> func, const std::string& message = "") {
        try {
            func();
            testResult_.status = "FAIL";
            testResult_.message = "Expected exception was not thrown: " + message;
        } catch (...) {
            // Exception expected
        }
    }
    
    // Get test result
    const TestResult& getResult() const { return testResult_; }
    
    // Get test name
    const std::string& getName() const { return testName_; }
    
    // Get test category
    TestCategory getCategory() const { return category_; }
    
    // Get test description
    const std::string& getDescription() const { return description_; }
    
    // Compare two values with epsilon tolerance
    template<typename T>
    bool equals(const T& a, const T& b, double epsilon = 1e-6) {
        return std::abs(a - b) <= epsilon;
    }
    
    // Get current timestamp
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Generate test report
    std::string generateReport() {
        std::stringstream ss;
        ss << "Test: " << testName_ << std::endl;
        ss << "Description: " << description_ << std::endl;
        ss << "Category: " << static_cast<int>(category_) << std::endl;
        ss << "Status: " << testResult_.status << std::endl;
        ss << "Message: " << testResult_.message << std::endl;
        ss << "Execution time: " << testResult_.executionTimeMs << " ms" << std::endl;
        ss << "Timestamp: " << getTimestamp() << std::endl;
        return ss.str();
    }
    
protected:
    std::string testName_;
    TestCategory category_;
    std::string description_;
    TestResult testResult_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Test runner for managing multiple tests
class TestRunner {
public:
    TestRunner() : passed_(0), failed_(0), errored_(0), skipped_(0) {}
    
    // Add a test to run
    void addTest(std::unique_ptr<TestFixture> test) {
        tests_.push_back(std::move(test));
    }
    
    // Run all tests
    void runAll() {
        std::cout << "=== Starting NLM Core Functionality Tests ===" << std::endl;
        std::cout << std::endl;
        
        for (auto& test : tests_) {
            std::cout << "Running test: " << test->getName() << std::endl;
            std::cout << "Description: " << test->getDescription() << std::endl;
            
            try {
                test->run();
                const auto& result = test->getResult();
                
                switch (result.status[0]) {
                    case 'P':
                        std::cout << "PASS" << std::endl;
                        passed_++;
                        break;
                    case 'F':
                        std::cout << "FAIL" << std::endl;
                        std::cout << "Reason: " << result.message << std::endl;
                        failed_++;
                        break;
                    case 'E':
                        std::cout << "ERROR" << std::endl;
                        std::cout << "Reason: " << result.message << std::endl;
                        errored_++;
                        break;
                    case 'S':
                        std::cout << "SKIP" << std::endl;
                        skipped_++;
                        break;
                    default:
                        std::cout << "UNKNOWN" << std::endl;
                        failed_++;
                        break;
                }
            } catch (...) {
                std::cout << "ERROR" << std::endl;
                std::cout << "Test crashed with unhandled exception" << std::endl;
                errored_++;
            }
            
            std::cout << std::endl;
        }
        
        // Generate final report
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << tests_.size() << std::endl;
        std::cout << "Passed: " << passed_ << std::endl;
        std::cout << "Failed: " << failed_ << std::endl;
        std::cout << "Errored: " << errored_ << std::endl;
        std::cout << "Skipped: " << skipped_ << std::endl;
        
        double successRate = tests_.size() > 0 ? 
            (static_cast<double>(passed_ + skipped_) / tests_.size()) * 100.0 : 0.0;
        std::cout << "Success rate: " << std::fixed << std::setprecision(2) 
                  << successRate << "%" << std::endl;
        std::cout << std::endl;
    }
    
    // Get test counts
    size_t getTotalCount() const { return tests_.size(); }
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    size_t getErroredCount() const { return errored_; }
    size_t getSkippedCount() const { return skipped_; }
    
private:
    std::vector<std::unique_ptr<TestFixture>> tests_;
    size_t passed_;
    size_t failed_;
    size_t errored_;
    size_t skipped_;
};

// Test data management for common test fixtures
class TestDataManager {
public:
    TestDataManager() = default;
    ~TestDataManager() = default;
    
    // Get a simple log message for testing
    static std::string getSimpleLogMessage() {
        return "This is a test log message for unit testing purposes.";
    }
    
    // Get a simple config value for testing
    static std::string getSimpleConfigValue() {
        return "test_value_" + std::to_string(std::rand());
    }
    
    // Get test data for batch processing
    static std::vector<float> getTestBatchData(size_t size = 100) {
        std::vector<float> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<float>(i) * 0.1f;
        }
        return data;
    }
    
    // Get test matrix for performance testing
    static std::vector<std::vector<float>> getTestMatrix(size_t rows = 10, size_t cols = 10) {
        std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                matrix[i][j] = static_cast<float>(i * cols + j) * 0.01f;
            }
        }
        return matrix;
    }
    
    // Get test string for configuration
    static std::string getTestConfigString() {
        return "{\"test\": \"value\", \"number\": 42, \"enabled\": true}";
    }
    
    // Get test performance data
    static std::vector<double> getTestPerformanceData(size_t size = 100) {
        std::vector<double> data(size);
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<double>(std::rand()) / RAND_MAX * 100.0;
        }
        return data;
    }
    
    // Get test components for integration testing
    static std::unordered_map<std::string, std::string> getTestComponents() {
        return {
            {"core", "Core neural components"},
            {"sensory", "Sensory processing"},
            {"motor", "Motor control"},
            {"memory", "Memory system"},
            {"plasticity", "Synaptic plasticity"}
        };
    }
};
