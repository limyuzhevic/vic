// Unit Test Framework for NLM
// Comprehensive testing framework for Neural Learning Machine

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <functional>

// Test result structure
struct TestResult {
    std::string testName;
    std::string testCategory;
    bool passed;
    std::string message;
    double executionTime;
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& name, const std::string& category = "General")
        : testName(name), testCategory(category), passed(false), 
          executionTime(0.0), lineNumber(0) {}
};

// Test category enumerator
enum class TestCategory {
    UNIT,      // Individual component tests
    INTEGRATION, // Integration tests between components
    PERFORMANCE, // Performance benchmarks
    BEHAVIORAL, // Behavioral/algorithmic tests
    STRESS,     // Stress/load tests
    MEMORY,     // Memory leak detection
    ALL        // All categories
};

// Base test case class
class TestCase {
protected:
    std::vector<TestResult> results;
    
public:
    TestCase() {}
    virtual ~TestCase() {}
    
    // Run all tests in this test case
    virtual void runAllTests() = 0;
    
    // Get all test results
    const std::vector<TestResult>& getResults() const { return results; }
    
    // Add a test result
    void addTestResult(const TestResult& result) {
        results.push_back(result);
    }
    
    // Helper assertion methods
    void assertEqual(double expected, double actual, const std::string& message = "") {
        if (std::abs(expected - actual) > 1e-9) {
            TestResult result("Assertion", "UNIT");
            result.passed = false;
            result.message = message.empty() ? 
                "Assertion failed: " + std::to_string(expected) + " != " + std::to_string(actual) :
                message;
            addTestResult(result);
        } else {
            TestResult result("Assertion", "UNIT");
            result.passed = true;
            addTestResult(result);
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            TestResult result("Assertion", "UNIT");
            result.passed = false;
            result.message = message.empty() ? "Assertion failed: condition is false" : message;
            addTestResult(result);
        } else {
            TestResult result("Assertion", "UNIT");
            result.passed = true;
            addTestResult(result);
        }
    }
    
    void assertNotNull(void* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            TestResult result("Assertion", "UNIT");
            result.passed = false;
            result.message = message.empty() ? "Assertion failed: pointer is null" : message;
            addTestResult(result);
        } else {
            TestResult result("Assertion", "UNIT");
            result.passed = true;
            addTestResult(result);
        }
    }
    
    void assertGreaterThan(double value, double threshold, const std::string& message = "") {
        if (value <= threshold) {
            TestResult result("Assertion", "UNIT");
            result.passed = false;
            result.message = message.empty() ? 
                "Assertion failed: " + std::to_string(value) + " <= " + std::to_string(threshold) :
                message;
            addTestResult(result);
        } else {
            TestResult result("Assertion", "UNIT");
            result.passed = true;
            addTestResult(result);
        }
    }
    
    void assertLessThan(double value, double threshold, const std::string& message = "") {
        if (value >= threshold) {
            TestResult result("Assertion", "UNIT");
            result.passed = false;
            result.message = message.empty() ? 
                "Assertion failed: " + std::to_string(value) + " >= " + std::to_string(threshold) :
                message;
            addTestResult(result);
        } else {
            TestResult result("Assertion", "UNIT");
            result.passed = true;
            addTestResult(result);
        }
    }
};

// Main test runner class
class TestRunner {
private:
    std::vector<TestCase*> testCases;
    TestCategory runCategory;
    std::map<std::string, std::vector<TestResult>> resultsByCategory;
    double totalExecutionTime;
    size_t totalTests;
    size_t passedTests;
    
public:
    TestRunner() : runCategory(TestCategory::ALL), totalExecutionTime(0.0), 
                   totalTests(0), passedTests(0) {}
    
    // Add a test case
    void addTestCase(TestCase* testCase) {
        testCases.push_back(testCase);
    }
    
    // Set test category to run
    void setCategory(TestCategory category) {
        runCategory = category;
    }
    
    // Run all tests
    void runAllTests() {
        totalTests = 0;
        passedTests = 0;
        totalExecutionTime = 0.0;
        
        std::cout << "=" << std::string(80, '=') << std::endl;
        std::cout << "NLM UNIT TEST RUNNER" << std::endl;
        std::cout << "=" << std::string(80, '=') << std::endl;
        
        for (TestCase* testCase : testCases) {
            std::cout << "Running test case: " << getTestCaseName(testCase) << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            testCase->runAllTests();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            totalExecutionTime += duration;
            
            const auto& testResults = testCase->getResults();
            totalTests += testResults.size();
            
            for (const auto& result : testResults) {
                if (result.passed) {
                    passedTests++;
                    std::cout << "  ✓ " << result.testName << " (" << result.testCategory << ")";
                } else {
                    std::cout << "  ✗ " << result.testName << " (" << result.testCategory << ") - " << result.message;
                }
                std::cout << " [" << std::fixed << std::setprecision(4) << result.executionTime << "s]" << std::endl;
                
                if (result.passed) {
                    resultsByCategory[result.testCategory].push_back(result);
                }
            }
        }
        
        std::cout << "=" << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << "=" << std::string(80, '=') << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success rate: " << (passedTests * 100.0 / totalTests) << "%" << std::endl;
        std::cout << "Total execution time: " << totalExecutionTime << "s" << std::endl;
        std::cout << "Average time per test: " << (totalTests > 0 ? totalExecutionTime / totalTests : 0.0) << "s" << std::endl;
        
        if (totalTests > 0 && passedTests < totalTests) {
            std::cout << std::endl;
            std::cout << "FAILED TESTS:" << std::endl;
            for (const auto& category : resultsByCategory) {
                for (const auto& result : category.second) {
                    if (!result.passed) {
                        std::cout << "  " << result.testName << " (" << result.testCategory << ")";
                        std::cout << " - " << result.message << std::endl;
                        std::cout << "    File: " << result.fileName << ", Line: " << result.lineNumber << std::endl;
                    }
                }
            }
        }
    }
    
    // Get test statistics
    struct TestStatistics {
        size_t totalTests;
        size_t passedTests;
        double totalTime;
        double successRate;
        std::map<std::string, size_t> testsByCategory;
    };
    
    TestStatistics getStatistics() const {
        TestStatistics stats;
        stats.totalTests = totalTests;
        stats.passedTests = passedTests;
        stats.totalTime = totalExecutionTime;
        stats.successRate = totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0;
        
        for (const auto& category : resultsByCategory) {
            stats.testsByCategory[category.first] += category.second.size();
        }
        
        return stats;
    }
    
private:
    std::string getTestCaseName(TestCase* testCase) {
        // Use RTTI to get the class name
        std::string name = typeid(*testCase).name();
        
        // Remove common prefixes
        if (name.find("class ") == 0) {
            name = name.substr(6);
        } else if (name.find("struct ") == 0) {
            name = name.substr(7);
        }
        
        // Remove template parameters (simplified)
        size_t pos = name.find("<");
        if (pos != std::string::npos) {
            name = name.substr(0, pos);
        }
        
        return name;
    }
};

// Macro for test case registration
#define REGISTER_TEST_CASE(TestClass) \
    namespace { \
        TestClass test_instance; \
        struct TestRegistrar { \
            TestRegistrar() { \
                test_runner.addTestCase(&test_instance); \
            } \
        } test_registrar_instance; \
    }

// Macro for individual test functions
#define TEST_CASE(ClassName, TestName) \
    class TestClassName { \
    public: \
        TestClassName() {} \
        void TestName(); \
    }; \
    void TestClassName::TestName()

// Macro for assertions with file/line information
#define ASSERT_EQUAL(expected, actual, message) \
    do { \
        if (std::abs((expected) - (actual)) > 1e-9) { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = false; \
            result.message = message.empty() ? \
                "Assertion failed: " + std::to_string(expected) + " != " + std::to_string(actual) : \
                message; \
            result.lineNumber = __LINE__; \
            result.fileName = __FILE__; \
            test_case_instance.addTestResult(result); \
        } else { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = true; \
            test_case_instance.addTestResult(result); \
        } \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = false; \
            result.message = message.empty() ? \
                "Assertion failed: condition is false" : message; \
            result.lineNumber = __LINE__; \
            result.fileName = __FILE__; \
            test_case_instance.addTestResult(result); \
        } else { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = true; \
            test_case_instance.addTestResult(result); \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == nullptr) { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = false; \
            result.message = message.empty() ? \
                "Assertion failed: pointer is null" : message; \
            result.lineNumber = __LINE__; \
            result.fileName = __FILE__; \
            test_case_instance.addTestResult(result); \
        } else { \
            TestResult result("Assertion", "UNIT"); \
            result.passed = true; \
            test_case_instance.addTestResult(result); \
        } \
    } while(0)

// Global test runner instance
test_runner global_test_runner;

// Helper function for timing
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::string testName;
    
public:
    Timer(const std::string& name = "Timer") : testName(name) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        std::cout << "[" << testName << ": " << std::fixed << std::setprecision(6) 
                  << duration << "s]" << std::endl;
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        return duration;
    }
};

// Test fixture for common setup/teardown
class TestFixture {
protected:
    TestCase* testCaseInstance;
    
public:
    TestFixture(TestCase* testCase) : testCaseInstance(testCase) {}
    
    virtual ~TestFixture() {}
    
    // Override these in derived classes
    virtual void setUp() {}
    virtual void tearDown() {}
};

// Macro for test fixture
#define FIXTURE_TEST_CASE(ClassName, TestName) \
    class TestClassName : public TestFixture { \
    public: \
        TestClassName(TestCase* testCase) : TestFixture(testCase) {} \
        void TestName(); \
    }; \
    void TestClassName::TestName()

// Helper macro for timed tests
#define TIMED_TEST(test_func, test_name) \
    do { \
        Timer timer(test_name); \
        test_func; \
    } while(0)