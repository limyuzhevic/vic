// Test Framework - Core Infrastructure
// This provides a comprehensive testing framework for the NLM project

#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <functional>
#include <chrono>
#include <cassert>
#include <stdexcept>

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testSuite;
    bool passed;
    std::string message;
    double durationSeconds;
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& name, const std::string& suite, bool p, 
               const std::string& msg = "", double duration = 0.0,
               int line = 0, const std::string& file = "")
        : testName(name), testSuite(suite), passed(p), message(msg),
          durationSeconds(duration), lineNumber(line), fileName(file) {}
};

// Test exception for assertion failures
class TestException : public std::runtime_error {
public:
    TestException(const std::string& message, const std::string& file, int line)
        : std::runtime_error(message), fileName(file), lineNumber(line) {}
    
    std::string fileName;
    int lineNumber;
};

// Assertion macros with detailed error reporting
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw TestException("Assertion failed: " #condition, __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_EQ(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " != " #val2 " (" << (val1) << " != " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_NE(val1, val2) \
    do { \
        if ((val1) == (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " == " #val2 " (" << (val1) << " == " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_GT(val1, val2) \
    do { \
        if ((val1) <= (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " > " #val2 " (" << (val1) << " <= " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_GE(val1, val2) \
    do { \
        if ((val1) < (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " >= " #val2 " (" << (val1) << " < " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_LT(val1, val2) \
    do { \
        if ((val1) >= (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " < " #val2 " (" << (val1) << " >= " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_LE(val1, val2) \
    do { \
        if ((val1) > (val2)) { \
            std::stringstream ss; \
            ss << "Assertion failed: " #val1 " <= " #val2 " (" << (val1) << " > " << (val2) << ")"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_THROW(statement, exception_type) \
    do { \
        bool threw = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            threw = true; \
        } catch (...) {} \
        if (!threw) { \
            std::stringstream ss; \
            ss << "Assertion failed: Expected " #exception_type " to be thrown"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

#define TEST_ASSERT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (...) { \
            std::stringstream ss; \
            ss << "Assertion failed: Unexpected exception thrown"; \
            throw TestException(ss.str(), __FILE__, __LINE__); \
        } \
    } while(0)

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getSuite() const = 0;
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run() = 0;
    
    void execute() {
        setup();
        try {
            run();
        } catch (const TestException& e) {
            throw;
        } catch (const std::exception& e) {
            throw TestException(std::string("Unexpected exception: ") + e.what(), __FILE__, __LINE__);
        }
        teardown();
    }
};

// Template test case for easy test creation
class TemplateTestCase : public TestCase {
public:
    using TestFunction = std::function<void()>;
    
    TemplateTestCase(const std::string& name, const std::string& suite, 
                     const TestFunction& testFunc)
        : name_(name), suite_(suite), testFunc_(testFunc) {}
    
    std::string getName() const override { return name_; }
    std::string getSuite() const override { return suite_; }
    void run() override { testFunc_(); }
    
private:
    std::string name_;
    std::string suite_;
    TestFunction testFunc_;
};

// Test suite for organizing related tests
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    std::string getName() const { return name_; }
    
    void addTestCase(std::unique_ptr<TestCase> testCase) {
        testCases_.push_back(std::move(testCase));
    }
    
    void run() {
        for (auto& testCase : testCases_) {
            testCase->execute();
        }
    }
    
    const std::vector<std::unique_ptr<TestCase>>& getTestCases() const {
        return testCases_;
    }
    
private:
    std::string name_;
    std::vector<std::unique_ptr<TestCase>> testCases_;
};

// Test runner for managing and executing all tests
class TestRunner {
public:
    TestRunner() = default;
    
    void addSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }
    
    std::vector<TestResult> run() {
        std::vector<TestResult> results;
        
        std::cout << "=== NLM Test Framework ===" << std::endl;
        
        for (auto& suite : suites_) {
            std::cout << "Running test suite: " << suite->getName() << std::endl;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            suite->run();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            for (const auto& testCase : suite->getTestCases()) {
                try {
                    auto testStart = std::chrono::high_resolution_clock::now();
                    testCase->execute();
                    auto testEnd = std::chrono::high_resolution_clock::now();
                    auto testDuration = std::chrono::duration<double>(testEnd - testStart).count();
                    
                    results.emplace_back(testCase->getName(), suite->getName(), 
                                       true, "", testDuration, 
                                       testCase->getName() + "()", "test");
                    
                    std::cout << "  PASS: " << testCase->getName() << " (" << testDuration << "s)" << std::endl;
                    
                } catch (const TestException& e) {
                    auto testEnd = std::chrono::high_resolution_clock::now();
                    auto testDuration = std::chrono::duration<double>(testEnd - startTime).count();
                    
                    results.emplace_back(testCase->getName(), suite->getName(), 
                                       false, e.what(), testDuration, 
                                       e.lineNumber, e.fileName);
                    
                    std::cout << "  FAIL: " << testCase->getName() << " - " << e.what() << std::endl;
                } catch (const std::exception& e) {
                    auto testEnd = std::chrono::high_resolution_clock::now();
                    auto testDuration = std::chrono::duration<double>(testEnd - startTime).count();
                    
                    results.emplace_back(testCase->getName(), suite->getName(), 
                                       false, std::string("Unexpected exception: ") + e.what(),
                                       testDuration, 0, "unknown");
                    
                    std::cout << "  ERROR: " << testCase->getName() << " - " << e.what() << std::endl;
                }
            }
        }
        
        printResults(results);
        return results;
    }
    
    void printResults(const std::vector<TestResult>& results) const {
        std::cout << std::endl;
        std::cout << "=== Test Results Summary ===" << std::endl;
        
        int totalTests = results.size();
        int passedTests = 0;
        int failedTests = 0;
        double totalDuration = 0.0;
        
        for (const auto& result : results) {
            totalDuration += result.durationSeconds;
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Total duration: " << totalDuration << "s" << std::endl;
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== Failed Tests Details ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "Suite: " << result.testSuite << std::endl;
                    std::cout << "Test: " << result.testName << std::endl;
                    std::cout << "Message: " << result.message << std::endl;
                    if (!result.fileName.empty()) {
                        std::cout << "File: " << result.fileName << ":" << result.lineNumber << std::endl;
                    }
                    std::cout << "Duration: " << result.durationSeconds << "s" << std::endl;
                    std::cout << "---" << std::endl;
                }
            }
        }
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== TEST RUN FAILED ===" << std::endl;
            std::exit(1);
        } else {
            std::cout << std::endl;
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        }
    }
    
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
};

// Convenience functions for creating test cases
inline void addTest(TestRunner& runner, const std::string& suiteName, 
                   const std::string& testName, std::function<void()> testFunc) {
    static TestSuite* currentSuite = nullptr;
    
    if (!currentSuite) {
        currentSuite = new TestSuite("default");
        runner.addSuite(std::unique_ptr<TestSuite>(currentSuite));
    }
    
    currentSuite->addTestCase(std::make_unique<TemplateTestCase>(testName, suiteName, testFunc));
}

// Macro for creating test functions
#define TEST_CASE(suite_name, test_name) \
    void test_name(); \
    namespace { \
        struct test_name##_registration { \
            test_name##_registration() { \
                addTest(runner, suite_name, #test_name, test_name); \
            } \
        } test_name##_reg; \
    } \
    void test_name()

// Main test runner instance
inline TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

} // namespace test
} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_H
