#pragma once

/**
 * NLM Unit Test Framework
 * 
 * Provides comprehensive unit testing infrastructure for the Neural Learning Machine project.
 * Includes test cases, assertions, test runners, and reporting mechanisms.
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <algorithm>

namespace nlm {
namespace testing {

// === TEST INFRASTRUCTURE ===

/**
 * Test case result
 */
struct TestResult {
    std::string name;
    std::string testSuite;
    std::string category;
    bool passed;
    std::string message;
    std::string failureType;
    std::chrono::duration<double> executionTime;
    
    TestResult(const std::string& testName, const std::string& suite, const std::string& cat)
        : name(testName), testSuite(suite), category(cat), passed(false), 
          failureType("NONE") {}
};

/**
 * Test suite result
 */
struct TestSuiteResult {
    std::string name;
    int totalTests;
    int passedTests;
    int failedTests;
    double totalTime;
    std::vector<TestResult> results;
    
    TestSuiteResult(const std::string& suiteName) 
        : name(suiteName), totalTests(0), passedTests(0), failedTests(0), totalTime(0.0) {}
    
    double getPassRate() const {
        return totalTests > 0 ? (double)passedTests / totalTests * 100.0 : 0.0;
    }
};

/**
 * Main test runner
 */
class TestRunner {
public:
    TestRunner() : outputLevel(OutputLevel::NORMAL) {}
    
    enum OutputLevel {
        QUIET,
        NORMAL,
        VERBOSE,
        DETAILED
    };
    
    void setOutputLevel(OutputLevel level) { outputLevel = level; }
    
    void addTestSuite(TestSuiteResult& suite) {
        suites.push_back(suite);
    }
    
    const std::vector<TestSuiteResult>& getSuites() const { return suites; }
    
    TestSuiteResult& getCurrentSuite() { return currentSuite; }
    
    void startSuite(const std::string& name) {
        currentSuite = TestSuiteResult(name);
        if (outputLevel >= OutputLevel::NORMAL) {
            std::cout << "\n=== Test Suite: " << name << " ===" << std::endl;
        }
    }
    
    void endSuite() {
        suites.push_back(currentSuite);
        if (outputLevel >= OutputLevel::NORMAL) {
            int total = currentSuite.totalTests;
            int passed = currentSuite.passedTests;
            int failed = currentSuite.failedTests;
            std::cout << "\n--- " << currentSuite.name << " Summary ---" << std::endl;
            std::cout << "Total Tests: " << total << std::endl;
            std::cout << "Passed: " << passed << std::endl;
            std::cout << "Failed: " << failed << std::endl;
            std::cout << "Pass Rate: " << std::fixed << std::setprecision(1) << currentSuite.getPassRate() << "%" << std::endl;
            std::cout << "Total Time: " << std::fixed << std::setprecision(3) << currentSuite.totalTime << "s" << std::endl;
        }
        currentSuite = TestSuiteResult("");
    }
    
    void runTest(const TestResult& result) {
        currentSuite.results.push_back(result);
        currentSuite.totalTests++;
        
        if (result.passed) {
            currentSuite.passedTests++;
            if (outputLevel >= OutputLevel::NORMAL) {
                std::cout << "✓ " << result.name << std::endl;
            }
        } else {
            currentSuite.failedTests++;
            if (outputLevel >= OutputLevel::NORMAL) {
                std::cout << "✗ " << result.name << std::endl;
                if (!result.message.empty()) {
                    std::cout << "  Error: " << result.message << std::endl;
                }
                if (result.failureType != "NONE") {
                    std::cout << "  Type: " << result.failureType << std::endl;
                }
            }
        }
        currentSuite.totalTime += result.executionTime.count();
    }
    
    void printSummary() const {
        std::cout << "\n=== Overall Test Summary ===" << std::endl;
        int totalTests = 0, totalPassed = 0, totalFailed = 0;
        double totalTime = 0.0;
        
        for (const auto& suite : suites) {
            totalTests += suite.totalTests;
            totalPassed += suite.passedTests;
            totalFailed += suite.failedTests;
            totalTime += suite.totalTime;
            
            if (outputLevel >= OutputLevel::VERBOSE) {
                std::cout << "\nSuite: " << suite.name << std::endl;
                std::cout << "  Tests: " << suite.totalTests << " (" << suite.passedTests << " passed, " << suite.failedTests << " failed)" << std::endl;
                std::cout << "  Rate: " << std::fixed << std::setprecision(1) << suite.getPassRate() << "%" << std::endl;
            }
        }
        
        std::cout << "\nOverall Results:" << std::endl;
        std::cout << "  Total Tests: " << totalTests << std::endl;
        std::cout << "  Passed: " << totalPassed << std::endl;
        std::cout << "  Failed: " << totalFailed << std::endl;
        std::cout << "  Pass Rate: " << std::fixed << std::setprecision(1) << (totalTests > 0 ? (double)totalPassed / totalTests * 100.0 : 0.0) << "%" << std::endl;
        std::cout << "  Total Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    }
    
private:
    std::vector<TestSuiteResult> suites;
    TestSuiteResult currentSuite;
    OutputLevel outputLevel;
};

// === ASSERTIONS ===

/**
 * Base assertion class
 */
class Assertion {
public:
    Assertion(const std::string& condition, const std::string& file, int line)
        : condition(condition), file(file), line(line) {}
    
    virtual ~Assertion() = default;
    
    virtual void evaluate() = 0;
    
protected:
    std::string condition;
    std::string file;
    int line;
};

/**
 * Boolean assertion
 */
class BoolAssertion : public Assertion {
public:
    BoolAssertion(const std::string& description, bool actual, const std::string& file, int line)
        : Assertion(description, file, line), actual(actual) {}
    
    void evaluate() override {
        if (!actual) {
            throw TestAssertionError(condition, file, line);
        }
    }
    
private:
    bool actual;
};

/**
 * Equality assertion
 */
class EqualAssertion : public Assertion {
public:
    template<typename T>
    EqualAssertion(const std::string& description, const T& expected, const T& actual, const std::string& file, int line)
        : Assertion(description, file, line), expected(expected), actual(actual) {}
    
    void evaluate() override {
        if (!(expected == actual)) {
            std::stringstream ss;
            ss << condition << "\n  Expected: " << expected << "\n  Actual: " << actual;
            throw TestAssertionError(ss.str(), file, line);
        }
    }
    
private:
    template<typename T>
    EqualAssertion(const std::string& description, const T& expected, const T& actual, const std::string& file, int line);
    
    template<typename T>
    EqualAssertion(const std::string& description, const T* expected, const T* actual, const std::string& file, int line);
    
    template<typename T>
    T expected;
    template<typename T>
    T actual;
};

/**
 * Comparison assertions
 */
class ComparisonAssertions {
public:
    static BoolAssertion assertTrue(const std::string& condition, const std::string& file, int line) {
        return BoolAssertion(condition, true, file, line);
    }
    
    static BoolAssertion assertFalse(const std::string& condition, const std::string& file, int line) {
        return BoolAssertion(condition, false, file, line);
    }
    
    template<typename T>
    static EqualAssertion assertEqual(const std::string& description, const T& expected, const T& actual, const std::string& file, int line) {
        return EqualAssertion(description, expected, actual, file, line);
    }
    
    template<typename T>
    static EqualAssertion assertNotEqual(const std::string& description, const T& expected, const T& actual, const std::string& file, int line) {
        return EqualAssertion(description + " (expected != actual)", actual, expected, file, line);
    }
    
    static EqualAssertion assertFloatEqual(const std::string& description, float expected, float actual, float epsilon = 0.0001f, const std::string& file = "", int line = 0) {
        std::stringstream desc;
        desc << description << " (within " << epsilon << ")";
        return EqualAssertion(desc.str(), actual, expected, file, line);
    }
    
    static EqualAssertion assertDoubleEqual(const std::string& description, double expected, double actual, double epsilon = 0.000001, const std::string& file = "", int line = 0) {
        std::stringstream desc;
        desc << description << " (within " << epsilon << ")";
        return EqualAssertion(desc.str(), actual, expected, file, line);
    }
};

/**
 * Test assertion error
 */
class TestAssertionError : public NLMError {
public:
    TestAssertionError(const std::string& message, const std::string& file, int line)
        : NLMError(formatMessage(message, file, line)) {}
    
    const char* getTypeName() const override { return "TestAssertionError"; }
    
private:
    static std::string formatMessage(const std::string& message, const std::string& file, int line) {
        std::stringstream ss;
        ss << "Assertion failed: " << message << "\n";
        if (!file.empty()) {
            ss << "File: " << file << "\n";
        }
        if (line > 0) {
            ss << "Line: " << line << "\n";
        }
        return ss.str();
    }
};

/**
 * Test case base class
 */
class TestCase {
public:
    TestCase(const std::string& name) : name(name) {}
    virtual ~TestCase() = default;
    
    const std::string& getName() const { return name; }
    
    virtual void run() = 0;
    
protected:
    template<typename T>
    void assertEqual(const std::string& description, const T& expected, const T& actual) {
        ComparisonAssertions::assertEqual(description, expected, actual, __FILE__, __LINE__);
    }
    
    template<typename T>
    void assertNotEqual(const std::string& description, const T& expected, const T& actual) {
        ComparisonAssertions::assertNotEqual(description, expected, actual, __FILE__, __LINE__);
    }
    
    void assertTrue(const std::string& condition) {
        ComparisonAssertions::assertTrue(condition, __FILE__, __LINE__);
    }
    
    void assertFalse(const std::string& condition) {
        ComparisonAssertions::assertFalse(condition, __FILE__, __LINE__);
    }
    
    void assertFloatEqual(const std::string& description, float expected, float actual, float epsilon = 0.0001f) {
        ComparisonAssertions::assertFloatEqual(description, expected, actual, epsilon, __FILE__, __LINE__);
    }
    
    void assertDoubleEqual(const std::string& description, double expected, double actual, double epsilon = 0.000001) {
        ComparisonAssertions::assertDoubleEqual(description, expected, actual, epsilon, __FILE__, __LINE__);
    }
    
    void fail(const std::string& message) {
        throw TestAssertionError(message, __FILE__, __LINE__);
    }
    
private:
    std::string name;
};

/**
 * Test fixture base class
 */
class TestFixture {
public:
    TestFixture() = default;
    virtual ~TestFixture() = default;
    
    virtual void setUp() {}
    virtual void tearDown() {}
    
    void runTestCase(TestCase* test) {
        setUp();
        try {
            test->run();
            test->passed = true;
        } catch (const TestAssertionError& e) {
            test->message = e.what();
            test->failureType = "ASSERTION";
        } catch (const NLMError& e) {
            test->message = e.what();
            test->failureType = e.getTypeName();
        } catch (const std::exception& e) {
            test->message = std::string("Unexpected error: ") + e.what();
            test->failureType = "RUNTIME_ERROR";
        }
        tearDown();
    }
};

/**
 * Macro-like assertion helpers for convenience
 */

#define ASSERT_TRUE(condition) \
    do { \
        ComparisonAssertions::assertTrue(condition, __FILE__, __LINE__); \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        ComparisonAssertions::assertFalse(condition, __FILE__, __LINE__); \
    } while(0)

#define ASSERT_EQUAL(expected, actual) \
    do { \
        ComparisonAssertions::assertEqual(#expected " == " #actual, expected, actual, __FILE__, __LINE__); \
    } while(0)

#define ASSERT_NOT_EQUAL(expected, actual) \
    do { \
        ComparisonAssertions::assertNotEqual(#expected " != " #actual, expected, actual, __FILE__, __LINE__); \
    } while(0)

#define ASSERT_FLOAT_EQUAL(expected, actual, epsilon) \
    do { \
        ComparisonAssertions::assertFloatEqual(#expected " == " #actual " ± " #epsilon, expected, actual, epsilon, __FILE__, __LINE__); \
    } while(0)

#define ASSERT_DOUBLE_EQUAL(expected, actual, epsilon) \
    do { \
        ComparisonAssertions::assertDoubleEqual(#expected " == " #actual " ± " #epsilon, expected, actual, epsilon, __FILE__, __LINE__); \
    } while(0)

#define FAIL(message) \
    do { \
        throw TestAssertionError(message, __FILE__, __LINE__); \
    } while(0)

/**
 * Test registration
 */
class TestRegistrar {
public:
    template<typename T>
    static TestRegistrar& registerTest(const std::string& suiteName, const std::string& testName, const std::string& category = "General") {
        static T* instance = nullptr;
        if (!instance) {
            instance = new T(testName);
            TestRegistrar::tests.push_back({suiteName, category, instance});
        }
        return *this;
    }
    
    static const std::vector<std::tuple<std::string, std::string, TestCase*>>& getRegisteredTests() {
        return tests;
    }
    
    static void clearRegisteredTests() {
        for (auto& test : tests) {
            delete std::get<2>(test);
        }
        tests.clear();
    }
    
private:
    struct RegisteredTest {
        std::string suiteName;
        std::string category;
        TestCase* test;
    };
    
    static std::vector<RegisteredTest> tests;
};

/**
 * Convenience macro for test registration
 */
#define TEST_CLASS(TestName, TestCategory) \
    class TestName; \
    TestRegistrar& register_##TestName = TestRegistrar::registerTest<TestName>(#TestName, #TestName, TestCategory); \
    class TestName : public TestCase

/**
 * Test framework factory
 */
class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    TestRunner& getRunner() { return runner; }
    
    void runAllTests() {
        runner.startSuite("All Tests");
        
        for (const auto& testInfo : TestRegistrar::getRegisteredTests()) {
            const std::string& suiteName = std::get<0>(testInfo);
            const std::string& category = std::get<1>(testInfo);
            TestCase* test = std::get<2>(testInfo);
            
            if (runner.getSuites().empty() || runner.getSuites().back().name != suiteName) {
                runner.startSuite(suiteName);
            }
            
            TestResult result(test->getName(), suiteName, category);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            try {
                test->run();
                result.passed = true;
            } catch (const TestAssertionError& e) {
                result.message = e.what();
                result.failureType = "ASSERTION";
            } catch (const NLMError& e) {
                result.message = e.what();
                result.failureType = e.getTypeName();
            } catch (const std::exception& e) {
                result.message = std::string("Unexpected error: ") + e.what();
                result.failureType = "RUNTIME_ERROR";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.executionTime = endTime - startTime;
            
            runner.runTest(result);
        }
        
        runner.endSuite();
        runner.printSummary();
    }
    
private:
    TestFramework() = default;
    TestRunner runner;
};

} // namespace testing
} // namespace nlm
