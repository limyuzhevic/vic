#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <sstream>

namespace nlm {

// Forward declaration - Brain.hpp includes brain/Brain.hpp
class Brain;
class Config;

// Test status enumeration
enum class TestStatus {
    PASSED = 0,
    FAILED = 1,
    SKIPPED = 2,
    ERROR = 3
};

// Test result structure
struct TestResult {
    std::string testName;
    TestStatus status;
    std::string message;
    std::chrono::milliseconds executionTime;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<std::string> errorMessages;
    
    TestResult() : testName(), status(TestStatus::SKIPPED), message(), 
                   executionTime(0), metrics(), errorMessages() {}
    
    TestResult(const std::string& name, TestStatus stat) 
        : testName(name), status(stat), message(), executionTime(0) {}
    
    // Convenience methods
    void addMessage(const std::string& level, const std::string& msg) {
        errorMessages.push_back(level + ": " + msg);
    }
    
    void addMetric(const std::string& name, double value) {
        metrics.push_back({name, value});
    }
    
    // Convert to JSON string
    std::string toJson() const {
        std::stringstream ss;
        ss << "{\"test_name\": \"" << testName << "\", \"status\": " << 
            (int)status << ", \"message\": \"" << message << "\", \"execution_time_ms\": " << 
            executionTime.count() << ", \"metrics\": [";
        
        for (size_t i = 0; i < metrics.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "{\"name\": \"" << metrics[i].first << "\", \"value\": " << metrics[i].second;
        }
        
        ss << "], \"errors\": [";
        for (size_t i = 0; i < errorMessages.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << errorMessages[i] << "\"";
        }
        ss << "]}";
        return ss.str();
    }
};

// Complete test results container
class TestResults {
public:
    TestResults() = default;
    ~TestResults() = default;
    
    // Disable copying, enable moving
    TestResults(const TestResults&) = delete;
    TestResults& operator=(const TestResults&) = delete;
    TestResults(TestResults&&) noexcept = default;
    TestResults& operator=(TestResults&&) noexcept = default;
    
    // Add a test result
    void addResult(const TestResult& result) {
        results_.push_back(result);
    }
    
    // Get all results
    const std::vector<TestResult>& getResults() const { return results_; }
    
    // Summary statistics
    size_t getTotalTests() const { return results_.size(); }
    size_t getPassedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& r) { return r.status == TestStatus::PASSED; });
    }
    
    size_t getFailedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& r) { return r.status == TestStatus::FAILED; });
    }
    
    size_t getSkippedTests() const {
        return std::count_if(results_.begin(), results_.end(),
                           [](const TestResult& r) { return r.status == TestStatus::SKIPPED; });
    }
    
    // Generate summary report
    std::string generateSummary() const {
        std::stringstream ss;
        ss << "=== Test Summary ===" << std::endl;
        ss << "Total Tests: " << getTotalTests() << std::endl;
        ss << "Passed: " << getPassedTests() << std::endl;
        ss << "Failed: " << getFailedTests() << std::endl;
        ss << "Skipped: " << getSkippedTests() << std::endl;
        ss << std::endl;
        
        for (const auto& result : results_) {
            std::string statusStr;
            switch (result.status) {
                case TestStatus::PASSED: statusStr = "PASSED"; break;
                case TestStatus::FAILED: statusStr = "FAILED"; break;
                case TestStatus::SKIPPED: statusStr = "SKIPPED"; break;
                case TestStatus::ERROR: statusStr = "ERROR"; break;
            }
            ss << result.testName << ": " << statusStr;
            if (!result.message.empty()) {
                ss << " - " << result.message;
            }
            ss << std::endl;
            if (!result.errorMessages.empty()) {
                for (const auto& error : result.errorMessages) {
                    ss << "  " << error << std::endl;
                }
            }
        }
        
        return ss.str();
    }
    
    // Convert to JSON
    std::string toJson() const {
        std::stringstream ss;
        ss << "{\"tests\": [";
        
        for (size_t i = 0; i < results_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << results_[i].toJson();
        }
        
        ss << "]}";
        return ss.str();
    }
    
private:
    std::vector<TestResult> results_;
};

// Base Test class
class Test {
public:
    Test(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    virtual ~Test() = default;
    
    // Disable copying, enable moving
    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;
    Test(Test&&) noexcept = default;
    Test& operator=(Test&&) noexcept = default;
    
    // Get test information
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    // Execute test
    virtual TestResult execute(std::shared_ptr<Brain> brain, const Config& config) = 0;
    
protected:
    std::string name_;
    std::string description_;
};

// Test framework implementation
class TestFramework {
public:
    TestFramework() = default;
    ~TestFramework() = default;
    
    // Disable copying, enable moving
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    TestFramework(TestFramework&&) noexcept = default;
    TestFramework& operator=(TestFramework&&) noexcept = default;
    
    // Register a test
    void registerTest(std::shared_ptr<Test> test) {
        tests_[test->getName()] = test;
    }
    
    // Set test execution parameters
    void setTestsToRun(const std::vector<std::string>& testNames) {
        if (testNames.empty()) {
            runAllTests_ = true;
            return;
        }
        
        std::unordered_set<std::string> testSet(testNames.begin(), testNames.end());
        runAllTests_ = (testSet.find("all") != testSet.end());
        testSet_ = testSet;
    }
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    
    // Run all selected tests
    TestResults runAllTests(std::shared_ptr<Brain> brain, const Config& config) {
        TestResults results;
        
        for (const auto& pair : tests_) {
            const auto& testName = pair.first;
            auto test = pair.second;
            
            // Check if this test should be run
            bool shouldRun = false;
            if (runAllTests_) {
                shouldRun = true;
            } else if (testSet_.empty()) {
                shouldRun = true;  // Default behavior: run all tests
            } else {
                shouldRun = (testSet_.find(testName) != testSet_.end());
            }
            
            if (!shouldRun) {
                TestResult result(testName, TestStatus::SKIPPED);
                result.addMessage("INFO", "Test skipped by command line selection");
                results.addResult(result);
                continue;
            }
            
            // Run the test
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                TestResult result = test->execute(brain, *config_);
                auto endTime = std::chrono::high_resolution_clock::now();
                
                result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                results.addResult(result);
                
                if (verbose_) {
                    std::cout << "Test: " << testName << " - Status: " << 
                        (result.status == TestStatus::PASSED ? "PASSED" : 
                         result.status == TestStatus::FAILED ? "FAILED" : 
                         result.status == TestStatus::ERROR ? "ERROR" : "SKIPPED") << std::endl;
                }
                
            } catch (const std::exception& e) {
                TestResult result(testName, TestStatus::ERROR);
                result.addMessage("ERROR", std::string("Exception: ") + e.what());
                results.addResult(result);
                
                if (verbose_) {
                    std::cout << "Test: " << testName << " - ERROR: " << e.what() << std::endl;
                }
            }
        }
        
        return results;
    }
    
    // Get all registered test names
    std::vector<std::string> getRegisteredTestNames() const {
        std::vector<std::string> names;
        names.reserve(tests_.size());
        for (const auto& pair : tests_) {
            names.push_back(pair.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    }
    
private:
    std::unordered_map<std::string, std::shared_ptr<Test>> tests_;
    std::unordered_set<std::string> testSet_;
    bool runAllTests_ = false;
    bool verbose_ = false;
    std::shared_ptr<Config> config_;
};
