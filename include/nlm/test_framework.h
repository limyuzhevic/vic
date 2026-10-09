// Simple test framework to replace raw assertions
// Provides Google Test-like assertions without requiring external dependencies

#ifndef SIMPLE_TEST_FRAMEWORK
#define SIMPLE_TEST_FRAMEWORK

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string failureMessage;
    
    TestResult(const std::string& name, bool pass, const std::string& message = "")
        : testName(name), passed(pass), failureMessage(message) {}
};

// Simple assertion macros
#define ASSERT_TRUE(condition) \
    assertCondition(condition, #condition, __FILE__, __LINE__)

#define ASSERT_FALSE(condition) \
    assertCondition(!(condition), #condition, __FILE__, __LINE__)

#define ASSERT_EQ(expected, actual) \
    assertEqual(expected, actual, #expected, #actual, __FILE__, __LINE__)

#define ASSERT_NE(expected, actual) \
    assertEqual(!(expected == actual), (expected != actual), #expected, #actual, __FILE__, __LINE__)

#define ASSERT_GT(expected, actual) \
    assertCompare(expected, actual, #expected, #actual, ">", __FILE__, __LINE__)

#define ASSERT_GE(expected, actual) \
    assertCompare(expected, actual, #expected, #actual, ">=", __FILE__, __LINE__)

#define ASSERT_LT(expected, actual) \
    assertCompare(expected, actual, #expected, #actual, "<", __FILE__, __LINE__)

#define ASSERT_LE(expected, actual) \
    assertCompare(expected, actual, #expected, #actual, "<=", __FILE__, __LINE__)

#define ASSERT_FLOAT_EQ(expected, actual, tolerance) \
    assertFloatEqual(expected, actual, tolerance, #expected, #actual, __FILE__, __LINE__)

// Test macros
#define TEST_SUITE(suiteName) \
    namespace suiteName { \
        std::vector<TestResult> runAllTests() { \
            std::vector<TestResult> results;

#define TEST_CASE(testName) \
            results.push_back(testName()); \
        } \
        TestResult testName()

#define ASSERT_NO_THROW(code) \
    try { code; } catch (...) { \
        assertCondition(false, "No exception expected", __FILE__, __LINE__); \
    }

// Helper functions
inline bool assertCondition(bool condition, const std::string& conditionStr, const char* file, int line) {
    if (!condition) {
        std::cerr << "FAIL: Assertion '" << conditionStr << "' failed at " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

inline bool assertEqual(bool expected, bool actual, const std::string& expectedStr, const std::string& actualStr, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "FAIL: Equality assertion failed at " << file << ":" << line << std::endl;
        std::cerr << "  Expected: " << expectedStr << " (" << (expected ? "true" : "false") << ")" << std::endl;
        std::cerr << "  Actual:   " << actualStr << " (" << (actual ? "true" : "false") << ")" << std::endl;
        return false;
    }
    return true;
}

template<typename T>
bool assertEqual(const T& expected, const T& actual, const std::string& expectedStr, const std::string& actualStr, const char* file, int line) {
    if (expected != actual) {
        std::cerr << "FAIL: Equality assertion failed at " << file << ":" << line << std::endl;
        std::cerr << "  Expected: " << expectedStr << " (" << expected << ")" << std::endl;
        std::cerr << "  Actual:   " << actualStr << " (" << actual << ")" << std::endl;
        return false;
    }
    return true;
}

template<typename T>
bool assertCompare(const T& expected, const T& actual, const std::string& expectedStr, const std::string& actualStr, const std::string& op, const char* file, int line) {
    bool condition = false;
    if (op == ">") condition = (expected > actual);
    else if (op == ">=") condition = (expected >= actual);
    else if (op == "<") condition = (expected < actual);
    else if (op == "<=") condition = (expected <= actual);
    
    if (!condition) {
        std::cerr << "FAIL: Comparison assertion '" << expectedStr << " " << op << " " << actualStr << "' failed at " << file << ":" << line << std::endl;
        std::cerr << "  Expected: " << expectedStr << " (" << expected << ")" << std::endl;
        std::cerr << "  Actual:   " << actualStr << " (" << actual << ")" << std::endl;
        return false;
    }
    return true;
}

inline bool assertFloatEqual(float expected, float actual, float tolerance, const std::string& expectedStr, const std::string& actualStr, const char* file, int line) {
    if (std::abs(expected - actual) > tolerance) {
        std::cerr << "FAIL: Float equality assertion failed at " << file << ":" << line << std::endl;
        std::cerr << "  Expected: " << expectedStr << " (" << expected << ")" << std::endl;
        std::cerr << "  Actual:   " << actualStr << " (" << actual << ")" << std::endl;
        std::cerr << "  Tolerance: " << tolerance << std::endl;
        return false;
    }
    return true;
}

inline bool assertFloatEqual(double expected, double actual, double tolerance, const std::string& expectedStr, const std::string& actualStr, const char* file, int line) {
    if (std::abs(expected - actual) > tolerance) {
        std::cerr << "FAIL: Float equality assertion failed at " << file << ":" << line << std::endl;
        std::cerr << "  Expected: " << expectedStr << " (" << expected << ")" << std::endl;
        std::cerr << "  Actual:   " << actualStr << " (" << actual << ")" << std::endl;
        std::cerr << "  Tolerance: " << tolerance << std::endl;
        return false;
    }
    return true;
}

// Test runner function
inline std::vector<TestResult> runAllTests() {
    std::vector<TestResult> results;
    // This would be populated by individual test suites
    return results;
}

inline void printTestSummary(const std::vector<TestResult>& results) {
    int passed = 0;
    int failed = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
            std::cout << "PASS: " << result.testName << std::endl;
        } else {
            failed++;
            std::cout << "FAIL: " << result.testName << " - " << result.failureMessage << std::endl;
        }
    }
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << results.size() << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    
    if (failed > 0) {
        std::cerr << "\nSome tests failed!" << std::endl;
        exit(1);
    }
}

#endif // SIMPLE_TEST_FRAMEWORK
