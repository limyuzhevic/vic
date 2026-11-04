// test_framework.hpp - Comprehensive testing framework for NLM
//
// This provides a structured testing framework with unit tests, integration tests,
// performance benchmarking, and configuration validation.

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <thread>

using namespace nlm;

// Test result structure
struct TestResult {
    std::string name;
    std::string description;
    bool passed;
    std::string message;
    double executionTime;
    std::chrono::system_clock::time_point timestamp;
    
    TestResult(const std::string& n, const std::string& d) 
        : name(n), description(d), passed(false), executionTime(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    std::vector<TestResult>& getResults() { return results_; }
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run() = 0;
    
protected:
    TestCase(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    // Assertion methods
    void assertTrue(bool condition, const std::string& message = "Condition should be true") {
        if (!condition) {
            results_.emplace_back(name_ + "_assertTrue", message);
            results_.back().passed = false;
            results_.back().message = message;
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "Condition should be false") {
        assertTrue(!condition, message);
    }
    
    void assertEqual(double actual, double expected, double tolerance = 1e-6,
                     const std::string& message = "Values should be equal") {
        if (std::abs(actual - expected) > tolerance) {
            std::stringstream ss;
            ss << message << ": expected " << expected << ", got " << actual;
            results_.emplace_back(name_ + "_assertEqual", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertNotEqual(double actual, double expected, double tolerance = 1e-6,
                        const std::string& message = "Values should not be equal") {
        if (std::abs(actual - expected) <= tolerance) {
            std::stringstream ss;
            ss << message << ": expected different values, got " << actual;
            results_.emplace_back(name_ + "_assertNotEqual", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertGreaterThan(double actual, double expected,
                           const std::string& message = "Value should be greater than expected") {
        if (actual <= expected) {
            std::stringstream ss;
            ss << message << ": expected > " << expected << ", got " << actual;
            results_.emplace_back(name_ + "_assertGreaterThan", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertLessThan(double actual, double expected,
                        const std::string& message = "Value should be less than expected") {
        if (actual >= expected) {
            std::stringstream ss;
            ss << message << ": expected < " << expected << ", got " << actual;
            results_.emplace_back(name_ + "_assertLessThan", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertContains(const std::string& actual, const std::string& expected,
                        const std::string& message = "String should contain expected substring") {
        if (actual.find(expected) == std::string::npos) {
            std::stringstream ss;
            ss << message << ": expected to contain '" << expected << "', got '" << actual << "'";
            results_.emplace_back(name_ + "_assertContains", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertNotContains(const std::string& actual, const std::string& expected,
                           const std::string& message = "String should not contain expected substring") {
        if (actual.find(expected) != std::string::npos) {
            std::stringstream ss;
            ss << message << ": expected to not contain '" << expected << "', got '" << actual << "'";
            results_.emplace_back(name_ + "_assertNotContains", ss.str());
            results_.back().passed = false;
            results_.back().message = ss.str();
        }
    }
    
    void assertConfigExists(const std::string& key,
                            const std::string& message = "Configuration key should exist") {
        if (!config_.has(key)) {
            results_.emplace_back(name_ + "_assertConfigExists", message + ": " + key);
            results_.back().passed = false;
            results_.back().message = message + ": " + key;
        }
    }
    
    template<typename T>
    T getConfig(const std::string& key, const T& defaultValue = T{}) {
        auto value = config_.get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
    void setConfig(const Config& config) {
        config_ = config;
    }
    
    // Helper methods
    template<typename Func>
    double measureExecutionTime(Func&& func, const std::string& operationName = "operation") {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(end - start).count();
        logResult(operationName, "Completed in " + std::to_string(elapsed) + " seconds");
        return elapsed;
    }
    
    void logResult(const std::string& testName, const std::string& message) {
        logInfo("  " + testName + ": " + message);
    }
    
    void logInfo(const std::string& message) const {
        NLM_LOG_INFO(message);
    }
    
    void logError(const std::string& message) const {
        NLM_LOG_ERROR(message);
    }
    
    void logWarning(const std::string& message) const {
        NLM_LOG_WARNING(message);
    }
    
    // Get elapsed time since start time
    double getElapsedTime(const std::chrono::high_resolution_clock::time_point& startTime) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<TestResult> results_;
    Config config_;
};

// Test suite class
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    void addTest(std::unique_ptr<TestCase> test) {
        tests_.push_back(std::move(test));
    }
    
    void runAllTests() {
        logInfo("Running test suite: " + name_);
        logInfo("Description: " + description_);
        
        int totalTests = tests_.size();
        int passedTests = 0;
        
        for (auto& test : tests_) {
            logInfo("\nRunning test: " + test->getName());
            logInfo("Description: " + test->getDescription());
            
            try {
                test->setup();
                
                auto startTime = std::chrono::high_resolution_clock::now();
                test->run();
                auto endTime = std::chrono::high_resolution_clock::now();
                
                test->teardown();
                
                double executionTime = std::chrono::duration<double>(endTime - startTime).count();
                
                for (auto& result : test->getResults()) {
                    result.executionTime = executionTime;
                    if (!result.passed) {
                        logError("  FAILED: " + result.message);
                    } else {
                        logInfo("  PASSED: " + result.message);
                    }
                }
                
                // Count passed tests
                bool testPassed = true;
                for (auto& result : test->getResults()) {
                    if (!result.passed) {
                        testPassed = false;
                        break;
                    }
                }
                
                if (testPassed) {
                    passedTests++;
                    logInfo("  TEST PASSED");
                } else {
                    logError("  TEST FAILED");
                }
                
            } catch (const std::exception& e) {
                logError("  EXCEPTION: " + std::string(e.what()));
            } catch (...) {
                logError("  UNKNOWN EXCEPTION");
            }
        }
        
        logInfo("\nTest Suite Summary:");
        logInfo("  Total tests: " + std::to_string(totalTests));
        logInfo("  Passed tests: " + std::to_string(passedTests));
        logInfo("  Failed tests: " + std::to_string(totalTests - passedTests));
        
        if (passedTests == totalTests) {
            logInfo("  ALL TESTS PASSED");
        } else {
            logError("  SOME TESTS FAILED");
        }
        
        // Save test results to file
        saveTestResults("test_results_" + name_ + ".json");
    }
    
    void saveTestResults(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open test results file: " + filename);
            return;
        }
        
        file << "{\n";
        file << "  \"testSuite\": \"" << name_ << "\",\n";
        file << "  \"description\": \"" << description_ << "\",\n";
        
        int totalTests = tests_.size();
        int passedTests = 0;
        
        for (size_t i = 0; i < tests_.size(); ++i) {
            auto& test = tests_[i];
            bool testPassed = true;
            
            for (auto& result : test->getResults()) {
                if (!result.passed) {
                    testPassed = false;
                    break;
                }
            }
            
            if (testPassed) passedTests++;
            
            file << "  \"tests\": [\n";
            for (size_t j = 0; j < test->getResults().size(); ++j) {
                auto& result = test->getResults()[j];
                file << "    {\n";
                file << "      \"name\": \"" << result.name << "\",\n";
                file << "      \"description\": \"" << result.description << "\",\n";
                file << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
                file << "      \"message\": \"" << result.message << "\",\n";
                file << "      \"executionTime\": " << std::fixed << std::setprecision(6) << result.executionTime << ",\n";
                file << "      \"timestamp\": \"" << std::chrono::system_clock::to_time_t(result.timestamp) << "\"\n";
                
                if (j < test->getResults().size() - 1) {
                    file << "    },\n";
                } else {
                    file << "    }\n";
                }
            }
            file << "  ]\n";
            
            if (i < tests_.size() - 1) {
                file << "  },\n";
            } else {
                file << "  }\n";
            }
        }
        
        file << "}\n";
        file.close();
        
        logInfo("Test results saved to: " + filename);
    }
    
    void clear() {
        tests_.clear();
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::unique_ptr<TestCase>> tests_;
};

// Global test runner
class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    void addTestSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }
    
    void runAllTests() {
        logInfo("Starting NLM Comprehensive Test Suite");
        logInfo("=====================================");
        
        int totalSuites = suites_.size();
        int passedSuites = 0;
        
        for (auto& suite : suites_) {
            logInfo("\nRunning test suite: " + suite->getName());
            suite->runAllTests();
            passedSuites++;
        }
        
        logInfo("\nOverall Test Summary:");
        logInfo("  Total test suites: " + std::to_string(totalSuites));
        logInfo("  Passed test suites: " + std::to_string(passedSuites));
        logInfo("  Failed test suites: " + std::to_string(totalSuites - passedSuites));
        
        if (passedSuites == totalSuites) {
            logInfo("  ALL TEST SUITES PASSED");
        } else {
            logError("  SOME TEST SUITES FAILED");
        }
    }
    
    void clear() {
        suites_.clear();
    }
    
private:
    TestRunner() = default;
    std::vector<std::unique_ptr<TestSuite>> suites_;
};

// Macro for easy test case creation
#define TEST_CASE(SuiteName, TestName, Description) \
    class Test_##SuiteName##_##TestName : public TestCase { \
    public: \
        Test_##SuiteName##_##TestName() : TestCase( \
            "#SuiteName#_#TestName#", \
            Description \
        ) {} \
        \
        void run() override { \
            onTest(); \
        } \
    protected: \
        virtual void onTest() = 0; \
    }; \
    \
    namespace { \
        struct Registration_##SuiteName##_##TestName { \
            Registration_##SuiteName##_##TestName() { \
                auto test = std::make_unique<Test_##SuiteName##_##TestName>(); \
                TestRunner::getInstance().getTestSuite(#SuiteName)->addTest(std::move(test)); \
            } \
        }; \
        Registration_##SuiteName##_##TestName registrationInstance_##SuiteName##_##TestName; \
    }

// Macro for creating test suites
#define TEST_SUITE(SuiteName, Description) \
    class Suite_##SuiteName { \
    public: \
        Suite_##SuiteName() { \
            TestRunner::getInstance().addTestSuite( \
                std::make_unique<TestSuite>(#SuiteName, Description) \
            ); \
        } \
    }; \
    namespace { \
        Suite_##SuiteName suiteInstance_##SuiteName; \
    }

