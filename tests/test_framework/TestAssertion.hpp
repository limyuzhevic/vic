// Rich Assertion Framework for NLM Brain Simulation
// Enhanced assertion macros and error reporting for unit tests

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace test {

// Enhanced assertion exception with full context
class AssertionException : public std::runtime_error {
public:
    AssertionException(const std::string& message, const std::string& file, int line,
                      const std::string& function = "", const std::string& errorType = "Assertion")
        : std::runtime_error(message), fileName(file), lineNumber(line), functionName(function), errorType(errorType) {}
    
    std::string getFileName() const { return fileName; }
    int getLineNumber() const { return lineNumber; }
    std::string getFunctionName() const { return functionName; }
    std::string getErrorType() const { return errorType; }
    
private:
    std::string fileName;
    int lineNumber;
    std::string functionName;
    std::string errorType;
};

// Rich assertion framework with detailed error reporting
class TestAssertion {
public:
    // Basic value assertions
    static void equal(const std::string& expected, const std::string& actual,
                     const std::string& message, const std::string& file, int line,
                     const std::string& function = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << "\n  Actual: " << actual;
            fail(ss.str(), file, line, function);
        }
    }
    
    template<typename T>
    static void equal(T expected, T actual, const std::string& message,
                     const std::string& file, int line, const std::string& function = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << "\n  Actual: " << actual;
            fail(ss.str(), file, line, function);
        }
    }
    
    template<typename T>
    static void notEqual(T expected, T actual, const std::string& message,
                        const std::string& file, int line, const std::string& function = "") {
        if (expected == actual) {
            fail(message, file, line, function, "NotEqual");
        }
    }
    
    // Numeric assertions
    template<typename T>
    static void greaterThan(T expected, T actual, const std::string& message,
                            const std::string& file, int line, const std::string& function = "") {
        if (expected >= actual) {
            fail(message + " (expected > " + std::to_string(expected) + ", got " + std::to_string(actual) + ")",
                 file, line, function, "GreaterThan");
        }
    }
    
    template<typename T>
    static void lessThan(T expected, T actual, const std::string& message,
                         const std::string& file, int line, const std::string& function = "") {
        if (expected <= actual) {
            fail(message + " (expected < " + std::to_string(expected) + ", got " + std::to_string(actual) + ")",
                 file, line, function, "LessThan");
        }
    }
    
    template<typename T>
    static void greaterThanOrEqual(T expected, T actual, const std::string& message,
                                   const std::string& file, int line, const std::string& function = "") {
        if (expected > actual) {
            fail(message + " (expected >= " + std::to_string(expected) + ", got " + std::to_string(actual) + ")",
                 file, line, function, "GreaterThanOrEqual");
        }
    }
    
    template<typename T>
    static void lessThanOrEqual(T expected, T actual, const std::string& message,
                                const std::string& file, int line, const std::string& function = "") {
        if (expected < actual) {
            fail(message + " (expected <= " + std::to_string(expected) + ", got " + std::to_string(actual) + ")",
                 file, line, function, "LessThanOrEqual");
        }
    }
    
    // Floating point assertions
    template<typename T>
    static void floatEqual(T expected, T actual, T tolerance,
                          const std::string& message, const std::string& file, int line,
                          const std::string& function = "") {
        if (std::abs(expected - actual) > tolerance) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << " (within " << tolerance << ")\n  Actual: " << actual
               << "\n  Difference: " << std::abs(expected - actual);
            fail(ss.str(), file, line, function);
        }
    }
    
    template<typename T>
    static void floatNotEqual(T expected, T actual, T tolerance,
                             const std::string& message, const std::string& file, int line,
                             const std::string& function = "") {
        if (std::abs(expected - actual) <= tolerance) {
            fail(message + " (expected != " + std::to_string(expected) + " within " + std::to_string(tolerance) + ")",
                 file, line, function, "FloatNotEqual");
        }
    }
    
    // Boolean assertions
    static void true(bool condition, const std::string& message,
                    const std::string& file, int line, const std::string& function = "") {
        if (!condition) {
            fail(message, file, line, function);
        }
    }
    
    static void false(bool condition, const std::string& message,
                     const std::string& file, int line, const std::string& function = "") {
        if (condition) {
            fail(message, file, line, function);
        }
    }
    
    // Pointer assertions
    static void notNull(const void* ptr, const std::string& message,
                       const std::string& file, int line, const std::string& function = "") {
        if (!ptr) {
            fail(message, file, line, function);
        }
    }
    
    static void null(const void* ptr, const std::string& message,
                    const std::string& file, int line, const std::string& function = "") {
        if (ptr) {
            fail(message + " (pointer should be null)", file, line, function, "Null");
        }
    }
    
    // Collection assertions
    template<typename Container>
    static void contains(const Container& collection, const typename Container::value_type& value,
                         const std::string& message, const std::string& file, int line,
                         const std::string& function = "") {
        if (std::find(collection.begin(), collection.end(), value) == collection.end()) {
            fail(message + " (collection does not contain " + std::to_string(value) + ")",
                 file, line, function, "Contains");
        }
    }
    
    template<typename Container>
    static void notContains(const Container& collection, const typename Container::value_type& value,
                            const std::string& message, const std::string& file, int line,
                            const std::string& function = "") {
        if (std::find(collection.begin(), collection.end(), value) != collection.end()) {
            fail(message + " (collection should not contain " + std::to_string(value) + ")",
                 file, line, function, "NotContains");
        }
    }
    
    template<typename Container>
    static void empty(const Container& collection, const std::string& message,
                      const std::string& file, int line, const std::string& function = "") {
        if (!collection.empty()) {
            fail(message + " (collection should be empty)", file, line, function, "Empty");
        }
    }
    
    template<typename Container>
    static void notEmpty(const Container& collection, const std::string& message,
                         const std::string& file, int line, const std::string& function = "") {
        if (collection.empty()) {
            fail(message + " (collection should not be empty)", file, line, function, "NotEmpty");
        }
    }
    
    // Size assertions
    template<typename Container>
    static void sizeEqual(const Container& collection, size_t expectedSize,
                         const std::string& message, const std::string& file, int line,
                         const std::string& function = "") {
        if (collection.size() != expectedSize) {
            fail(message + " (expected size " + std::to_string(expectedSize) + ", got " + std::to_string(collection.size()) + ")",
                 file, line, function, "SizeEqual");
        }
    }
    
    template<typename Container>
    static void sizeNotEqual(const Container& collection, size_t expectedSize,
                            const std::string& message, const std::string& file, int line,
                            const std::string& function = "") {
        if (collection.size() == expectedSize) {
            fail(message + " (size should not be " + std::to_string(expectedSize) + ")",
                 file, line, function, "SizeNotEqual");
        }
    }
    
    // String assertions
    static void startsWith(const std::string& str, const std::string& prefix,
                          const std::string& message, const std::string& file, int line,
                          const std::string& function = "") {
        if (str.rfind(prefix, 0) != 0) {
            fail(message + " (string does not start with '" + prefix + "')",
                 file, line, function, "StartsWith");
        }
    }
    
    static void endsWith(const std::string& str, const std::string& suffix,
                        const std::string& message, const std::string& file, int line,
                        const std::string& function = "") {
        if (str.rfind(suffix) == std::string::npos || str.length() - suffix.length() != str.rfind(suffix)) {
            fail(message + " (string does not end with '" + suffix + "')",
                 file, line, function, "EndsWith");
        }
    }
    
    static void containsString(const std::string& str, const std::string& substring,
                               const std::string& message, const std::string& file, int line,
                               const std::string& function = "") {
        if (str.find(substring) == std::string::npos) {
            fail(message + " (string does not contain '" + substring + "')",
                 file, line, function, "ContainsString");
        }
    }
    
    // Exception assertions
    static void throws(std::function<void()> func, const std::string& message,
                       const std::string& file, int line, const std::string& function = "",
                       const std::string& expectedException = "std::exception") {
        bool threw = false;
        std::string exceptionMsg;
        std::string exceptionType;
        
        try {
            func();
        } catch (const std::exception& e) {
            threw = true;
            exceptionMsg = e.what();
            exceptionType = typeid(e).name();
        } catch (...) {
            threw = true;
            exceptionMsg = "Unknown exception";
            exceptionType = "unknown";
        }
        
        if (!threw) {
            fail(message + " (Expected exception " + expectedException + ")", file, line, function);
        } else if (expectedException != "any" && exceptionType != expectedException) {
            fail(message + " (Expected " + expectedException + ", got " + exceptionType + ")", file, line, function);
        }
    }
    
    static void notThrows(std::function<void()> func, const std::string& message,
                          const std::string& file, int line, const std::string& function = "") {
        try {
            func();
        } catch (...) {
            fail(message, file, line, function);
        }
    }
    
    // Time assertions
    static void timeout(std::function<void()> func, std::chrono::nanoseconds timeout,
                       const std::string& message, const std::string& file, int line,
                       const std::string& function = "") {
        std::future<void> future = std::async(std::launch::async, [&]() { func(); });
        if (future.wait_for(timeout) == std::future_status::timeout) {
            fail(message + " (Test timed out after " + 
                   std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()) + "ms)",
                 file, line, function, "Timeout");
        }
    }
    
    // Failure assertions
    static void fails(const std::string& message, const std::string& file, int line,
                     const std::string& function = "") {
        fail(message, file, line, function, "ExpectedFailure");
    }
    
    static void notFails(std::function<void()> func, const std::string& message,
                         const std::string& file, int line, const std::string& function = "") {
        try {
            func();
        } catch (...) {
            fail(message + " (Expected test to not throw, but it did)", file, line, function);
        }
    }
    
    // Helper assertion functions
    static void fail(const std::string& message, const std::string& file, int line,
                    const std::string& function = "", const std::string& errorType = "Assertion") {
        std::stringstream ss;
        ss << "[" << errorType << "] " << message;
        if (!file.empty()) ss << " (File: " << file;
        if (line > 0) ss << ", Line: " << line;
        ss << ")";
        throw AssertionException(ss.str(), file, line, function, errorType);
    }
};

// Convenience macros for easy test writing
namespace macros {
    #define ASSERT_EQ(expected, actual) nlm::test::TestAssertion::equal(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_EQ(expected, actual) nlm::test::TestAssertion::notEqual(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_GT(expected, actual) nlm::test::TestAssertion::greaterThan(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_LT(expected, actual) nlm::test::TestAssertion::lessThan(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_GE(expected, actual) nlm::test::TestAssertion::greaterThanOrEqual(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_LE(expected, actual) nlm::test::TestAssertion::lessThanOrEqual(expected, actual, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_FLOAT_EQ(expected, actual, tolerance) nlm::test::TestAssertion::floatEqual(expected, actual, tolerance, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_FLOAT_NOT_EQ(expected, actual, tolerance) nlm::test::TestAssertion::floatNotEqual(expected, actual, tolerance, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_TRUE(condition) nlm::test::TestAssertion::true(condition, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_FALSE(condition) nlm::test::TestAssertion::false(condition, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_NULL(ptr) nlm::test::TestAssertion::notNull(ptr, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NULL(ptr) nlm::test::TestAssertion::null(ptr, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_THROWS(statement, exceptionType) nlm::test::TestAssertion::throws([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__, exceptionType)
    #define ASSERT_NO_THROWS(statement) nlm::test::TestAssertion::notThrows([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_FAIL(message) nlm::test::TestAssertion::fail(message, __FILE__, __LINE__, __func__, "ExpectedFailure")
    #define ASSERT_TIMEOUT(statement, timeoutMs) nlm::test::TestAssertion::timeout(statement, std::chrono::milliseconds(timeoutMs), "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_CONTAINS(collection, value) nlm::test::TestAssertion::contains(collection, value, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_CONTAINS(collection, value) nlm::test::TestAssertion::notContains(collection, value, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_EMPTY(collection) nlm::test::TestAssertion::empty(collection, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_EMPTY(collection) nlm::test::TestAssertion::notEmpty(collection, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_SIZE_EQ(collection, expectedSize) nlm::test::TestAssertion::sizeEqual(collection, expectedSize, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_SIZE_NOT_EQ(collection, expectedSize) nlm::test::TestAssertion::sizeNotEqual(collection, expectedSize, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_STARTS_WITH(str, prefix) nlm::test::TestAssertion::startsWith(str, prefix, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_ENDS_WITH(str, suffix) nlm::test::TestAssertion::endsWith(str, suffix, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_CONTAINS_STRING(str, substring) nlm::test::TestAssertion::containsString(str, substring, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_FAILS(statement) nlm::test::TestAssertion::notFails([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__)
}

} // namespace test
} // namespace nlm
