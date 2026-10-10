// TestFramework.hpp - Base classes for the NLM test system
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <atomic>

// Test status enumeration
enum class TestStatus {
    Pending,
    Running,
    Passed,
    Failed,
    Skipped,
    Blocked
};

// Test case structure
struct TestCase {
    std::string name;
    std::function<void()> testFunction;
    std::string category;
    std::string description;
    bool isCritical = false;
    
    TestCase(const std::string& n, std::function<void()> func, const std::string& cat, 
             const std::string& desc = "", bool critical = false)
        : name(n), testFunction(std::move(func)), category(cat), 
          description(desc), isCritical(critical) {}
};

// Performance metrics
struct PerformanceMetrics {
    double executionTimeMs;
    size_t memoryUsageBytes;
    size_t allocationsCount;
    size_t deallocationsCount;
    
    PerformanceMetrics() : executionTimeMs(0.0), memoryUsageBytes(0), 
                           allocationsCount(0), deallocationsCount(0) {}
};

// Individual test result
class TestResult {
public:
    std::string testName;
    TestStatus status;
    std::string errorMessage;
    PerformanceMetrics metrics;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    TestResult(const std::string& name) 
        : testName(name), status(TestStatus::Pending) {}
    
    double getDurationMs() const {
        auto duration = endTime - startTime;
        return std::chrono::duration<double, std::milli>(duration).count();
    }
    
    bool passed() const { return status == TestStatus::Passed; }
    bool failed() const { return status == TestStatus::Failed; }
    bool skipped() const { return status == TestStatus::Skipped; }
    bool blocked() const { return status == TestStatus::Blocked; }
};

// Test framework base class
class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void registerTest(const TestCase& testCase) {
        tests_.push_back(testCase);
        categorizedTests_[testCase.category].push_back(testCase.name);
    }
    
    const std::vector<TestCase>& getAllTests() const { return tests_; }
    const std::vector<std::string>& getTestsByCategory(const std::string& category) const {
        auto it = categorizedTests_.find(category);
        if (it != categorizedTests_.end()) return it->second;
        static const std::vector<std::string> empty;
        return empty;
    }
    
    void runTest(const std::string& testName, TestResult& result) {
        for (const auto& test : tests_) {
            if (test.name == testName) {
                result.startTime = std::chrono::steady_clock::now();
                result.status = TestStatus::Running;
                
                try {
                    test.testFunction();
                    result.status = TestStatus::Passed;
                } catch (const std::exception& e) {
                    result.status = TestStatus::Failed;
                    result.errorMessage = e.what();
                } catch (...) {
                    result.status = TestStatus::Failed;
                    result.errorMessage = "Unknown exception";
                }
                
                result.endTime = std::chrono::steady_clock::now();
                return;
            }
        }
    }
    
    void runCategory(const std::string& category) {
        auto it = categorizedTests_.find(category);
        if (it != categorizedTests_.end()) {
            std::cout << "Running " << category << " tests..." << std::endl;
            for (const auto& testName : it->second) {
                TestResult result(testName);
                runTest(testName, result);
                printResult(result);
            }
        }
    }
    
    void printResults() const {
        std::cout << "\n=== Test Summary ===" << std::endl;
        int passed = 0, failed = 0, skipped = 0, blocked = 0;
        
        for (const auto& result : testResults_) {
            switch (result.status) {
                case TestStatus::Passed: passed++; break;
                case TestStatus::Failed: failed++; break;
                case TestStatus::Skipped: skipped++; break;
                case TestStatus::Blocked: blocked++; break;
                default: break;
            }
        }
        
        std::cout << "Total Tests: " << testResults_.size() << std::endl;
        std::cout << "  PASSED: " << passed << std::endl;
        std::cout << "  FAILED: " << failed << std::endl;
        std::cout << "  SKIPPED: " << skipped << std::endl;
        std::cout << "  BLOCKED: " << blocked << std::endl;
    }
    
    void addTestResult(const TestResult& result) {
        testResults_.push_back(result);
    }
    
protected:
    TestFramework() = default;
    
private:
    void printResult(const TestResult& result) {
        std::cout << "  " << result.testName << ": ";
        switch (result.status) {
            case TestStatus::Passed: std::cout << "PASSED"; break;
            case TestStatus::Failed: std::cout << "FAILED"; break;
            case TestStatus::Skipped: std::cout << "SKIPPED"; break;
            case TestStatus::Blocked: std::cout << "BLOCKED"; break;
            default: std::cout << "PENDING"; break;
        }
        std::cout << std::endl;
        
        if (result.failed() && !result.errorMessage.empty()) {
            std::cout << "    Error: " << result.errorMessage << std::endl;
        }
    }
    
    std::vector<TestCase> tests_;
    std::unordered_map<std::string, std::vector<std::string>> categorizedTests_;
    std::vector<TestResult> testResults_;
};