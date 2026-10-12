#ifndef TEST_ASSERTIONS_HPP
#define TEST_ASSERTIONS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>

/**
 * Custom assertion macros and utilities for NLM testing
 */

// Assertion failure function
void testAssertionFailure(const std::string& message, const std::string& file, int line, const std::string& expected, const std::string& actual) {
    std::cerr << "\n=== TEST ASSERTION FAILED ===" << std::endl;
    std::cerr << "File: " << file << std::endl;
    std::cerr << "Line: " << line << std::endl;
    std::cerr << "Message: " << message << std::endl;
    std::cerr << "Expected: " << expected << std::endl;
    stdcerr << "Actual: " << actual << std::endl;
    std::cerr << "=================================" << std::endl;
    throw std::runtime_error("Test assertion failed");
}

// Basic assertion macros
#define ASSERT_EQ(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            std::ostringstream oss; \
            oss << #expected << " == " << #actual; \
            testAssertionFailure(message, __FILE__, __LINE__, #expected, #actual); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual, message) \
    do { \
        if ((expected) == (actual)) { \
            std::ostringstream oss; \
            oss << #expected << " != " << #actual; \
            testAssertionFailure(message, __FILE__, __LINE__, #expected, #actual); \
        } \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << #condition; \
            testAssertionFailure(message, __FILE__, __LINE__, #condition, "false"); \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) \
    do { \
        if (condition) { \
            std::ostringstream oss; \
            oss << #condition; \
            testAssertionFailure(message, __FILE__, __LINE__, "false", #condition); \
        } \
    } while(0)

#define ASSERT_NULL(ptr, message) \
    do { \
        if ((ptr) != nullptr) { \
            std::ostringstream oss; \
            oss << #ptr << " != nullptr"; \
            testAssertionFailure(message, __FILE__, __LINE__, #ptr " != nullptr", "not null"); \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == nullptr) { \
            std::ostringstream oss; \
            oss << #ptr << " == nullptr"; \
            testAssertionFailure(message, __FILE__, __LINE__, "not null", #ptr " == nullptr"); \
        } \
    } while(0)

// Floating point comparison
#define ASSERT_DOUBLE_EQ(expected, actual, epsilon, message) \
    do { \
        if (std::abs((expected) - (actual)) > epsilon) { \
            std::ostringstream oss; \
            oss << "std::abs(" << #expected << " - " << #actual << ") > " << epsilon; \
            testAssertionFailure(message, __FILE__, __LINE__, #expected " ≈ " #actual, #actual); \
        } \
    } while(0)

#define ASSERT_DOUBLE_NE(expected, actual, epsilon, message) \
    do { \
        if (std::abs((expected) - (actual)) <= epsilon) { \
            std::ostringstream oss; \
            oss << "std::abs(" << #expected << " - " << #actual << ") <= " << epsilon; \
            testAssertionFailure(message, __FILE__, __LINE__, #expected " ≠ " #actual, #actual); \
        } \
    } while(0)

// Range assertions
#define ASSERT_BETWEEN(value, min, max, message) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            std::ostringstream oss; \
            oss << #value << " not in [" << #min << ", " << #max << "]"; \
            testAssertionFailure(message, __FILE__, __LINE__, #value " ∈ [" #min ", " #max "]", #value); \
        } \
    } while(0)

// Container assertions
#define ASSERT_CONTAINERS_EQUAL(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            std::ostringstream oss; \
            oss << #expected << " != " << #actual; \
            testAssertionFailure(message, __FILE__, __LINE__, "containers equal", "different"); \
        } \
    } while(0)

#define ASSERT_CONTAINERS_NOT_EQUAL(expected, actual, message) \
    do { \
        if ((expected) == (actual)) { \
            std::ostringstream oss; \
            oss << #expected << " == " << #actual; \
            testAssertionFailure(message, __FILE__, __LINE__, "containers not equal", "equal"); \
        } \
    } while(0)

// Vector assertions
#define ASSERT_VECTOR_EQ(expected, actual, message) \
    do { \
        if ((expected).size() != (actual).size()) { \
            std::ostringstream oss; \
            oss << #expected << ".size() != " << #actual << ".size()"; \
            testAssertionFailure(message, __FILE__, __LINE__, "sizes equal", "different"); \
        } \
        for (size_t i = 0; i < (expected).size(); ++i) { \
            if ((expected)[i] != (actual)[i]) { \
                std::ostringstream oss; \
                oss << #expected << "[" << i << "] != " << #actual << "[" << i << "]"; \
                testAssertionFailure(message, __FILE__, __LINE__, "vector elements equal", "different"); \
            } \
        } \
    } while(0)

#define ASSERT_VECTOR_DOUBLE_EQ(expected, actual, epsilon, message) \
    do { \
        if ((expected).size() != (actual).size()) { \
            std::ostringstream oss; \
            oss << #expected << ".size() != " << #actual << ".size()"; \
            testAssertionFailure(message, __FILE__, __LINE__, "sizes equal", "different"); \
        } \
        for (size_t i = 0; i < (expected).size(); ++i) { \
            if (std::abs((expected)[i] - (actual)[i]) > epsilon) { \
                std::ostringstream oss; \
                oss << #expected << "[" << i << "] != " << #actual << "[" << i << "] within " << epsilon; \
                testAssertionFailure(message, __FILE__, __LINE__, #expected " ≈ " #actual, "different"); \
            } \
        } \
    } while(0)

// String assertions
#define ASSERT_STRING_EQ(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            std::ostringstream oss; \
            oss << #expected << " != " << #actual; \
            testAssertionFailure(message, __FILE__, __LINE__, #expected, #actual); \
        } \
    } while(0)

#define ASSERT_STRING_CONTAINS(haystack, needle, message) \
    do { \
        if ((haystack).find((needle)) == std::string::npos) { \
            std::ostringstream oss; \
            oss << #haystack << " does not contain " << #needle; \
            testAssertionFailure(message, __FILE__, __LINE__, #haystack " contains " #needle, "doesn't contain"); \
        } \
    } while(0)

// Pointer assertions
#define ASSERT_POINTER_EQ(ptr1, ptr2, message) \
    do { \
        if ((ptr1) != (ptr2)) { \
            std::ostringstream oss; \
            oss << #ptr1 << " != " << #ptr2; \
            testAssertionFailure(message, __FILE__, __LINE__, #ptr1, #ptr2); \
        } \
    } while(0)

#define ASSERT_POINTER_NOT_EQ(ptr1, ptr2, message) \
    do { \
        if ((ptr1) == (ptr2)) { \
            std::ostringstream oss; \
            oss << #ptr1 " == " << #ptr2; \
            testAssertionFailure(message, __FILE__, __LINE__, "not equal", "equal"); \
        } \
    } while(0)

// Custom exception for test failures
class TestAssertionException : public std::runtime_error {
public:
    TestAssertionException(const std::string& message) : std::runtime_error(message) {}
};

// Test result structure
struct TestResult {
    std::string test_name;
    std::string test_category;
    bool passed;
    double duration_ms;
    std::string failure_message;
    
    TestResult(const std::string& name, const std::string& category) 
        : test_name(name), test_category(category), passed(false), duration_ms(0.0), failure_message("") {}
};

// Test runner interface
class TestRunner {
public:
    virtual ~TestRunner() = default;
    virtual void runTests() = 0;
    virtual std::vector<TestResult> getResults() const = 0;
    virtual void clearResults() = 0;
};

#endif // TEST_ASSERTIONS_HPP