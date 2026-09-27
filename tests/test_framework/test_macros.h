// Test Macros for NLM Brain Simulation
// Comprehensive test macros for easy test writing

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>

namespace nlm {
namespace test {

// Forward declarations
class TestBase;
class TestFixture;

// Test class macro
#define TEST_CLASS(className) class className : public TestBase

// Test method macro
#define TEST_METHOD(methodName) methodName

// Test fixture macro
#define TEST_FIXTURE(fixtureName) class fixtureName : public TestFixture

// Enhanced assertion macros
#define ASSERT_EQ(expected, actual) nlm::test::TestAssertion::equal(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_FLOAT_EQ(expected, actual, tolerance) nlm::test::TestAssertion::floatEqual(expected, actual, tolerance, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_TRUE(condition) nlm::test::TestAssertion::true(condition, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_FALSE(condition) nlm::test::TestAssertion::false(condition, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_NOT_NULL(ptr) nlm::test::TestAssertion::notNull(ptr, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_THROW(statement, exceptionType) nlm::test::TestAssertion::throws([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__, exceptionType)
#define ASSERT_NO_THROW(statement) nlm::test::TestAssertion::notThrows([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_FAIL(message) nlm::test::TestAssertion::fail(message, __FILE__, __LINE__, __func__, "ExpectedFailure")
#define ASSERT_TIMEOUT(statement, timeoutMs) nlm::test::TestAssertion::timeout(statement, std::chrono::milliseconds(timeoutMs), "Assertion failed", __FILE__, __LINE__, __func__)

// Collection assertions
#define ASSERT_CONTAINS(collection, value) nlm::test::TestAssertion::contains(collection, value, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_NOT_CONTAINS(collection, value) nlm::test::TestAssertion::notContains(collection, value, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_EMPTY(collection) nlm::test::TestAssertion::empty(collection, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_NOT_EMPTY(collection) nlm::test::TestAssertion::notEmpty(collection, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_SIZE_EQ(collection, expectedSize) nlm::test::TestAssertion::sizeEqual(collection, expectedSize, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_SIZE_NOT_EQ(collection, expectedSize) nlm::test::TestAssertion::sizeNotEqual(collection, expectedSize, "Assertion failed", __FILE__, __LINE__, __func__)

// String assertions
#define ASSERT_STARTS_WITH(str, prefix) nlm::test::TestAssertion::startsWith(str, prefix, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_ENDS_WITH(str, suffix) nlm::test::TestAssertion::endsWith(str, suffix, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_CONTAINS_STRING(str, substring) nlm::test::TestAssertion::containsString(str, substring, "Assertion failed", __FILE__, __LINE__, __func__)

// Numeric comparisons
#define ASSERT_GT(a, b) nlm::test::TestAssertion::greaterThan(a, b, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_LT(a, b) nlm::test::TestAssertion::lessThan(a, b, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_GE(a, b) nlm::test::TestAssertion::greaterThanOrEqual(a, b, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_LE(a, b) nlm::test::TestAssertion::lessThanOrEqual(a, b, "Assertion failed", __FILE__, __LINE__, __func__)
#define ASSERT_NOT_EQ(a, b) nlm::test::TestAssertion::notEqual(a, b, "Assertion failed", __FILE__, __LINE__, __func__)

// Benchmark macros
#define BENCHMARK(name, func, iterations) nlm::test::Benchmark::run(name, func, iterations)

// Test registration macros
#define REGISTER_TEST(testClass, testSuite, testName, description, ...) \\
class TestClass##testName : public testClass { \\
public: \\
    TestClass##testName() : testClass(testName, testSuite, #testClass, {"unit", "integration"}, description) {} \\
    void executeTest() override { testName(__VA_ARGS__); } \\
};

#define NLM_TEST_CLASS(className, testSuite, description, ...) \\
class className : public TestBase { \\
public: \\
    className() : TestBase(#className, testSuite, #className, {"unit", "integration"}, description) {} \\
    void executeTest() override { testBody(__VA_ARGS__); } \\
private: \\
    void testBody(__VA_ARGS__) \\

#define NLM_TEST_FIXTURE(fixtureName) class fixtureName : public TestFixture

// Test result tracking
#define ADD_TEST_RESULT(testName, passed, executionTime) {\\
    nlm::test::TestResult result;\\
    result.name = testName;\\
    result.status = passed ? nlm::test::TestResult::Status::PASSED : nlm::test::TestResult::Status::FAILED;\\
    result.executionTime = executionTime;\\
    // Add to test results collection\\
}

// Conditional test execution
#define SKIP_TEST_IF(condition, reason) \\
    if (condition) { \\\n        std::cout << "SKIP: " << reason << std::endl; \\
        return; \\
    }

#define ONLY_RUN_IF(condition, reason) \\
    if (!(condition)) { \\\n        std::cout << "SKIP: " << reason << std::endl; \\
        return; \\
    }

// Parameterized test macros
#define PARAMETERIZED_TEST_CASE(testName, paramName, ...) \\
class testName : public ParameterizedTestBase { \\
public: \\
    testName() : ParameterizedTestBase(testName) {} \\
    void executeTestWithParameter(const std::string& paramName, const std::string& paramValue) override { \\\n        if (paramName == #paramName) {\\
            runTestForParameter(paramName, paramValue, __VA_ARGS__); \\
        } \\
    } \\
private: \\
    template<typename... Args> \\
    void runTestForParameter(const std::string& name, const std::string& value, Args... args) { \\\n        // Implementation for parameterized test \\
    } \\
};

// Performance test macros
#define PERFORMANCE_TEST(benchmarkName, testFunc, thresholdMs) \\
{ \\\n    auto start = std::chrono::high_resolution_clock::now(); \\\n    testFunc; \\\n    auto end = std::chrono::high_resolution_clock::now(); \\\n    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); \\\n    if (duration.count() > thresholdMs) { \\\n        std::cout << "PERFORMANCE WARNING: " << benchmarkName \\
                  << " took " << duration.count() << " ms (threshold: " << thresholdMs << " ms)" << std::endl; \\\n    } \\
}

} // namespace test
} // namespace nlm
