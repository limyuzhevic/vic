// Base test framework for NLM
// Provides common utilities and assertion macros

#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    std::string error;
    
    TestResult(const std::string& testName, bool isPassed, const std::string& msg = "", const std::string& err = "")
        : name(testName), passed(isPassed), message(msg), error(err) {}
};

// Simple test framework
class TestFramework {
private:
    std::vector<TestResult> results;
    int testsRun;
    int testsPassed;
    
public:
    TestFramework() : testsRun(0), testsPassed(0) {}
    
    // Assertion macros
    #define ASSERT_EQ(expected, actual) \
        testAssertEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__)
    
    #define ASSERT_NE(expected, actual) \
        testAssertNotEqual((expected), (actual), #expected, #actual, __FILE__, __LINE__)
    
    #define ASSERT_GT(actual, expected) \
        testAssertGreater((actual), (expected), #actual, #expected, __FILE__, __LINE__)
    
    #define ASSERT_GE(actual, expected) \
        testAssertGreaterEqual((actual), (expected), #actual, #expected, __FILE__, __LINE__)
    
    #define ASSERT_LT(actual, expected) \
        testAssertLess((actual), (expected), #actual, #expected, __FILE__, __LINE__)
    
    #define ASSERT_LE(actual, expected) \
        testAssertLessEqual((actual), (expected), #actual, #expected, __FILE__, __LINE__)
    
    #define ASSERT_TRUE(condition) \
        testAssertTrue((condition), #condition, __FILE__, __LINE__)
    
    #define ASSERT_FALSE(condition) \
        testAssertFalse((condition), #condition, __FILE__, __LINE__)
    
    #define ASSERT_THROW(statement, exceptionType) \
        testAssertThrow(statement, exceptionType, #statement, #exceptionType, __FILE__, __LINE__)
    
    #define ASSERT_NO_THROW(statement) \
        testAssertNoThrow(statement, #statement, __FILE__, __LINE__)
    
    #define ASSERT_FAIL(message) \
        testAssertFail(message, __FILE__, __LINE__)
    
private:
    void testAssertEqual(const auto& expected, const auto& actual, const char* expectedStr, const char* actualStr, const char* file, int line) {
        testsRun++;
        if (expected == actual) {
            testsPassed++;
            results.emplace_back(expectedStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << expectedStr << " (" << expected << ") != " << actualStr << " (" << actual << ")";
            results.emplace_back(expectedStr, false, "", ss.str());
        }
    }
    
    void testAssertNotEqual(const auto& expected, const auto& actual, const char* expectedStr, const char* actualStr, const char* file, int line) {
        testsRun++;
        if (expected != actual) {
            testsPassed++;
            results.emplace_back(expectedStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << expectedStr << " == " << actualStr;
            results.emplace_back(expectedStr, false, "", ss.str());
        }
    }
    
    void testAssertGreater(const auto& actual, const auto& expected, const char* actualStr, const char* expectedStr, const char* file, int line) {
        testsRun++;
        if (actual > expected) {
            testsPassed++;
            results.emplace_back(actualStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << actualStr << " (" << actual << ") > " << expectedStr << " (" << expected << ")";
            results.emplace_back(actualStr, false, "", ss.str());
        }
    }
    
    void testAssertGreaterEqual(const auto& actual, const auto& expected, const char* actualStr, const char* expectedStr, const char* file, int line) {
        testsRun++;
        if (actual >= expected) {
            testsPassed++;
            results.emplace_back(actualStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << actualStr << " (" << actual << ") >= " << expectedStr << " (" << expected << ")";
            results.emplace_back(actualStr, false, "", ss.str());
        }
    }
    
    void testAssertLess(const auto& actual, const auto& expected, const char* actualStr, const char* expectedStr, const char* file, int line) {
        testsRun++;
        if (actual < expected) {
            testsPassed++;
            results.emplace_back(actualStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << actualStr << " (" << actual << ") < " << expectedStr << " (" << expected << ")";
            results.emplace_back(actualStr, false, "", ss.str());
        }
    }
    
    void testAssertLessEqual(const auto& actual, const auto& expected, const char* actualStr, const char* expectedStr, const char* file, int line) {
        testsRun++;
        if (actual <= expected) {
            testsPassed++;
            results.emplace_back(actualStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << actualStr << " (" << actual << ") <= " << expectedStr << " (" << expected << ")";
            results.emplace_back(actualStr, false, "", ss.str());
        }
    }
    
    void testAssertTrue(bool condition, const char* conditionStr, const char* file, int line) {
        testsRun++;
        if (condition) {
            testsPassed++;
            results.emplace_back(conditionStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << conditionStr << " is not true";
            results.emplace_back(conditionStr, false, "", ss.str());
        }
    }
    
    void testAssertFalse(bool condition, const char* conditionStr, const char* file, int line) {
        testsRun++;
        if (!condition) {
            testsPassed++;
            results.emplace_back(conditionStr, true, "Passed", "");
        } else {
            std::stringstream ss;
            ss << "Assertion failed: " << conditionStr << " is not false";
            results.emplace_back(conditionStr, false, "", ss.str());
        }
    }
    
    void testAssertThrow(const std::function<void()>& statement, const std::string& exceptionType, const char* statementStr, const char* exceptionStr, const char* file, int line) {
        testsRun++;
        try {
            statement();
            results.emplace_back(statementStr, false, "", "Expected exception " + exceptionType + " was not thrown");
        } catch (const std::exception& e) {
            testsPassed++;
            results.emplace_back(statementStr, true, "Passed", "");
        } catch (...) {
            results.emplace_back(statementStr, false, "", "Expected exception " + exceptionType + " was not thrown (different exception type)");
        }
    }
    
    void testAssertNoThrow(const std::function<void()>& statement, const char* statementStr, const char* file, int line) {
        testsRun++;
        try {
            statement();
            testsPassed++;
            results.emplace_back(statementStr, true, "Passed", "");
        } catch (...) {
            results.emplace_back(statementStr, false, "", "Unexpected exception thrown");
        }
    }
    
    void testAssertFail(const std::string& message, const char* file, int line) {
        testsRun++;
        results.emplace_back("AssertFail", false, "", message);
    }
    
public:
    void runAll() {
        std::cout << "=== Running Tests ===" << std::endl;
        std::cout << std::endl;
        
        // Clear previous results
        results.clear();
        testsRun = 0;
        testsPassed = 0;
        
        // Run all test suites
        runTestTypes();
        runTestConfig();
        runTestRandom();
        runTestClock();
        runTestNeuron();
        runTestSynapse();
        runTestStdp();
        runTestBrain();
        
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Tests run: " << testsRun << std::endl;
        std::cout << "Tests passed: " << testsPassed << std::endl;
        std::cout << "Tests failed: " << (testsRun - testsPassed) << std::endl;
        
        if (testsRun == testsPassed) {
            std::cout << std::endl;
            std::cout << "✓ ALL TESTS PASSED" << std::endl;
            std::cout << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "✗ SOME TESTS FAILED" << std::endl;
            std::cout << std::endl;
            
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "FAILED: " << result.name << std::endl;
                    if (!result.error.empty()) {
                        std::cout << "  Error: " << result.error << std::endl;
                    }
                    if (!result.message.empty()) {
                        std::cout << "  Message: " << result.message << std::endl;
                    }
                }
            }
        }
    }
    
    // Individual test suites (to be overridden by concrete test runners)
    virtual void runTestTypes() {}
    virtual void runTestConfig() {}
    virtual void runTestRandom() {}
    virtual void runTestClock() {}
    virtual void runTestNeuron() {}
    virtual void runTestSynapse() {}
    virtual void runTestStdp() {}
    virtual void runTestBrain() {}
};

// Macro to register test suites
#define REGISTER_TEST_SUITE(className) \
    class className : public TestFramework { \
    public: \
        void runAll() override { \
            TestFramework::runAll(); \
        } \
    };

#endif // NLM_TEST_FRAMEWORK_H