#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <memory>

/**
 * Simple unit test framework for NLM
 * 
 * Provides a standardized way to write and run tests with:
 * - Test discovery and registration
 * - Detailed test reporting with timing
 * - Assertion helpers with detailed error messages
 * - Test categorization
 * - Overall test suite execution
 */

namespace nlm {
namespace test {

/**
 * Test result structure
 */
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    std::chrono::microseconds duration;
};

/**
 * Base test class
 */
class Test {
public:
    virtual ~Test() = default;
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual void run() = 0;
    virtual bool shouldRun() const { return true; }
};

/**
 * Typed test class for type-safe assertions
 */
template<typename T>
class TypedTest : public Test {
public:
    using TestFunction = std::function<void()>;
    
    TypedTest(std::string name, std::string category, TestFunction testFunc)
        : name_(std::move(name)), category_(std::move(category)), testFunc_(std::move(testFunc)) {}
    
    std::string getName() const override { return name_; }
    std::string getCategory() const override { return category_; }
    void run() override { testFunc_(); }
    
private:
    std::string name_;
    std::string category_;
    TestFunction testFunc_;
};

/**
 * Test assertion helpers
 */
namespace assert {
    
    inline void fail(const std::string& message) {
        std::cerr << "ASSERT FAILED: " << message << std::endl;
        throw std::runtime_error("Test assertion failed: " + message);
    }
    
    template<typename T, typename U>
    inline void equal(const T& expected, const U& actual, const std::string& message = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << "Expected: " << expected << " " << "(type: " << typeid(T).name() << ")";
            ss << " Actual: " << actual << " " << "(type: " << typeid(U).name() << ")";
            if (!message.empty()) ss << " - " << message;
            fail(ss.str());
        }
    }
    
    template<typename T>
    inline void isTrue(const T& condition, const std::string& message = "") {
        if (!condition) {
            std::stringstream ss;
            ss << "Expected true, got: " << condition;
            if (!message.empty()) ss << " - " << message;
            fail(ss.str());
        }
    }
    
    template<typename T>
    inline void isFalse(const T& condition, const std::string& message = "") {
        if (condition) {
            std::stringstream ss;
            ss << "Expected false, got: " << condition;
            if (!message.empty()) ss << " - " << message;
            fail(ss.str());
        }
    }
    
    template<typename T>
    inline void isNotNull(const T* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            std::stringstream ss;
            ss << "Expected non-null pointer";
            if (!message.empty()) ss << " - " << message;
            fail(ss.str());
        }
    }
    
    template<typename T>
    inline void isNull(const T* ptr, const std::string& message = "") {
        if (ptr != nullptr) {
            std::stringstream ss;
            ss << "Expected null pointer";
            if (!message.empty()) ss << " - " << message;
            fail(ss.str());
        }
    }
}

/**
 * Test registry for automatic test discovery
 */
class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    template<typename T>
    void registerTest(std::string name, std::string category, std::function<void()> testFunc) {
        tests_.emplace_back(std::make_unique<TypedTest<T>>(std::move(name), std::move(category), std::move(testFunc)));
    }
    
    std::vector<std::unique_ptr<Test>>& getTests() { return tests_; }
    
private:
    TestRegistry() = default;
    std::vector<std::unique_ptr<Test>> tests_;
};

/**
 * Helper macro for registering tests
 */
#define REGISTER_TEST(testName, testCategory, testFunc) \
    namespace { \
        struct testName##_registrar { \
            testName##_registrar() { \
                nlm::test::TestRegistry::getInstance().registerTest<testName##_test>( \
                    #testName, testCategory, testFunc); \
            } \
        }; \
        testName##_registrar testName##_instance; \
    }

/**
 * Test runner for executing all tests
 */
class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    void runAllTests() {
        auto& tests = TestRegistry::getInstance().getTests();
        
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "NLM Unit Test Suite" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        
        std::cout << "Found " << tests.size() << " tests" << std::endl;
        std::cout << std::endl;
        
        std::vector<TestResult> results;
        int totalTests = 0;
        int passedTests = 0;
        
        for (const auto& test : tests) {
            if (!test->shouldRun()) continue;
            
            totalTests++;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            std::string testName = test->getName();
            std::string testCategory = test->getCategory();
            
            std::cout << "Running: " << testCategory << "." << testName << "...";
            std::cout.flush();
            
            bool testPassed = false;
            std::string testMessage = "";
            
            try {
                test->run();
                testPassed = true;
                testMessage = "PASSED";
            } catch (const std::exception& e) {
                testMessage = std::string("FAILED: ") + e.what();
            } catch (...) {
                testMessage = "FAILED: Unknown exception";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            std::cout << " " << duration.count() << "us - " << testMessage << std::endl;
            
            if (testPassed) {
                passedTests++;
            }
            
            results.push_back({testName, testCategory, testPassed, testMessage, duration});
        }
        
        std::cout << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "  Total tests: " << totalTests << std::endl;
        std::cout << "  Passed:      " << passedTests << std::endl;
        std::cout << "  Failed:      " << (totalTests - passedTests) << std::endl;
        std::cout << "  Success rate: " << std::fixed << std::setprecision(1) 
                  << (totalTests > 0 ? (100.0 * passedTests / totalTests) : 0.0) << "%" << std::endl;
        
        // Output detailed results for failed tests
        std::cout << std::endl;
        bool hasFailures = false;
        for (const auto& result : results) {
            if (!result.passed) {
                if (!hasFailures) {
                    std::cout << "Failed Tests:" << std::endl;
                    hasFailures = true;
                }
                std::cout << "  " << result.category << "." << result.name 
                          << " (" << result.duration.count() << "us)" << std::endl;
                std::cout << "    " << result.message << std::endl;
            }
        }
        
        if (hasFailures) {
            std::cout << std::endl;
            std::cout << "FAIL: Some tests failed" << std::endl;
            throw std::runtime_error("Test suite failed");
        } else {
            std::cout << std::endl;
            std::cout << "SUCCESS: All tests passed" << std::endl;
        }
    }
};

}} // namespace nlm::test

// Helper macro for backward compatibility with existing tests
#define RUN_TEST(testFunc) \
    namespace { \
        struct testFunc##_registrar { \
            testFunc##_registrar() { \
                nlm::test::TestRegistry::getInstance().registerTest<testFunc##_test>( \
                    #testFunc, "General", testFunc); \
            } \
        }; \
        testFunc##_registrar testFunc##_instance; \
    }\
    void testFunc##_test()

#define RUN_TEST_CATEGORY(testFunc, category) \
    namespace { \
        struct testFunc##_registrar { \
            testFunc##_registrar() { \
                nlm::test::TestRegistry::getInstance().registerTest<testFunc##_test>( \
                    #testFunc, category, testFunc); \
            } \
        }; \
        testFunc##_registrar testFunc##_instance; \
    }\
    void testFunc##_test()