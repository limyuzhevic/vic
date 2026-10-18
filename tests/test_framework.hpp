// NLM Test Framework
// Comprehensive testing framework for NLM Brain Simulator

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <memory>
#include <stdexcept>

class TestResult {
public:
    enum Status { PASSED, FAILED, SKIPPED };
    
    TestResult(const std::string& name, Status status, 
               const std::string& message = "", 
               double executionTime = 0.0)
        : name_(name), status_(status), message_(message), 
          executionTime_(executionTime) {}
    
    std::string getName() const { return name_; }
    Status getStatus() const { return status_; }
    std::string getMessage() const { return message_; }
    double getExecutionTime() const { return executionTime_; }
    
    bool isPassed() const { return status_ == PASSED; }
    bool isFailed() const { return status_ == FAILED; }
    bool isSkipped() const { return status_ == SKIPPED; }
    
private:
    std::string name_;
    Status status_;
    std::string message_;
    double executionTime_;
};

class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    virtual ~TestSuite() {}
    
    std::string getName() const { return name_; }
    
    virtual void run() = 0;
    
protected:
    std::string name_;
};

class TestFramework {
private:
    std::vector<TestSuite*> suites_;
    std::vector<TestResult> results_;
    bool verbose_ = true;
    bool timing_ = true;
    
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void addSuite(TestSuite* suite) {
        suites_.push_back(suite);
    }
    
    bool runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Running " << suites_.size() << " test suites..." << std::endl;
        std::cout << std::endl;
        
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        int skippedTests = 0;
        
        for (auto* suite : suites_) {
            std::cout << "Running " << suite->getName() << "..." << std::endl;
            try {
                suite->run();
                std::cout << "  " << suite->getName() << " completed" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "  " << suite->getName() << " failed with exception: " << e.what() << std::endl;
                return false;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total suites: " << suites_.size() << std::endl;
        
        for (const auto& result : results_) {
            totalTests++;
            switch (result.getStatus()) {
                case TestResult::PASSED:
                    passedTests++;
                    break;
                case TestResult::FAILED:
                    failedTests++;
                    break;
                case TestResult::SKIPPED:
                    skippedTests++;
                    break;
            }
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "  PASSED: " << passedTests << std::endl;
        std::cout << "  FAILED: " << failedTests << std::endl;
        std::cout << "  SKIPPED: " << skippedTests << std::endl;
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== FAILED TESTS ===" << std::endl;
            for (const auto& result : results_) {
                if (result.isFailed()) {
                    std::cout << "- " << result.getName() << ": " << result.getMessage() << std::endl;
                }
            }
        }
        
        return (failedTests == 0);
    }
    
    void addResult(const TestResult& result) {
        results_.push_back(result);
    }
    
    bool getVerbose() const { return verbose_; }
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    bool getTiming() const { return timing_; }
    void setTiming(bool timing) { timing_ = timing; }
};

// Macro for creating tests
#define TEST_CASE(suite, name) \
    class suite##_##name##_##_Test : public TestSuite { \
    public: \
        suite##_##name##_##_Test() : TestSuite(#suite "::" #name) {} \
        void run() override { name##_##Test(); } \
    private: \
        void name##_##Test(); \
    }; \
    static suite##_##name##_##_Test suite##_##name##_##_instance; \
    void suite##_##name##_##_Test::name##_##Test()

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " != " #actual); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " == " #actual); \
        } \
    } while(0)

#define ASSERT_GT(expected, actual) \
    do { \
        if ((expected) <= (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " <= " #actual); \
        } \
    } while(0)

#define ASSERT_LT(expected, actual) \
    do { \
        if ((expected) >= (actual)) { \
            throw std::runtime_error("Assertion failed: " #expected " >= " #actual); \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition " is false"); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if ((condition)) { \
            throw std::runtime_error("Assertion failed: " #condition " is true"); \
        } \
    } while(0)

#define ASSERT_THROW(statement, exceptionType) \
    do { \
        bool caught = false; \
        try { statement; } catch (const exceptionType&) { caught = true; } \
        if (!caught) { \
            throw std::runtime_error("Assertion failed: " #statement " should have thrown " #exceptionType); \
        } \
    } while(0)

// Macro for timing tests
#define TIME_TEST(testName, testCode) \
    { \
        auto start = std::chrono::high_resolution_clock::now(); \
        testCode; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
        std::cout << "  " << testName << " took " << duration << " ms" << std::endl; \
    }