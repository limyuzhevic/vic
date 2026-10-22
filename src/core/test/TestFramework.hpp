#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <functional>

namespace nlm {
namespace test {

// Test framework classes for NLM

class TestFailureException : public std::runtime_error {
public:
    TestFailureException(const std::string& testName, const std::string& message)
        : std::runtime_error("Test Failed: " + testName + " - " + message) {}
};

class TestAssert {
public:
    static void equal(bool condition, const std::string& testName, 
                      const std::string& expected, const std::string& actual) {
        if (!condition) {
            std::ostringstream oss;
            oss << "Expected: " << expected << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void equal(float expected, float actual, const std::string& testName,
                      const std::string& message = "") {
        if (std::abs(expected - actual) > 1e-6f) {
            std::ostringstream oss;
            oss << message << " Expected: " << expected << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void equal(int expected, int actual, const std::string& testName,
                      const std::string& message = "") {
        if (expected != actual) {
            std::ostringstream oss;
            oss << message << " Expected: " << expected << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void equal(const std::string& expected, const std::string& actual,
                      const std::string& testName, const std::string& message = "") {
        if (expected != actual) {
            std::ostringstream oss;
            oss << message << " Expected: '" << expected << "', Actual: '" << actual << "'";
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void notNull(void* ptr, const std::string& testName,
                       const std::string& message = "") {
        if (ptr == nullptr) {
            throw TestFailureException(testName, "Pointer should not be null: " + message);
        }
    }
    
    static void greaterThan(float actual, float threshold, const std::string& testName,
                           const std::string& message = "") {
        if (actual <= threshold) {
            std::ostringstream oss;
            oss << message << " Expected greater than " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void greaterEqual(float actual, float threshold, const std::string& testName,
                            const std::string& message = "") {
        if (actual < threshold) {
            std::ostringstream oss;
            oss << message << " Expected greater than or equal to " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void lessThan(float actual, float threshold, const std::string& testName,
                        const std::string& message = "") {
        if (actual >= threshold) {
            std::ostringstream oss;
            oss << message << " Expected less than " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void lessEqual(float actual, float threshold, const std::string& testName,
                         const std::string& message = "") {
        if (actual > threshold) {
            std::ostringstream oss;
            oss << message << " Expected less than or equal to " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    // Integer comparison methods
    static void greaterEqual(int actual, int threshold, const std::string& testName,
                            const std::string& message = "") {
        if (actual < threshold) {
            std::ostringstream oss;
            oss << message << " Expected greater than or equal to " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
    
    static void lessEqual(int actual, int threshold, const std::string& testName,
                         const std::string& message = "") {
        if (actual > threshold) {
            std::ostringstream oss;
            oss << message << " Expected less than or equal to " << threshold << ", Actual: " << actual;
            throw TestFailureException(testName, oss.str());
        }
    }
};

class TestRunner {
private:
    std::vector<std::string> testNames;
    int passedTests = 0;
    int totalTests = 0;
    
public:
    TestRunner() : passedTests(0), totalTests(0) {}
    
    template<typename TestFunc>
    void addTest(const std::string& name, TestFunc testFunc) {
        testNames.push_back(name);
        totalTests++;
        try {
            testFunc();
            passedTests++;
            std::cout << "PASS: " << name << std::endl;
        } catch (const TestFailureException& e) {
            std::cout << "FAIL: " << name << " - " << e.what() << std::endl;
            throw; // Re-throw to stop execution on first failure
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << name << " - " << e.what() << std::endl;
            throw;
        }
    }
    
    void addTest(const std::string& name, std::function<void()> testFunc) {
        addTest(name, testFunc);
    }
    
    void runAllTests() {
        std::cout << "Running " << totalTests << " tests..." << std::endl;
        std::cout << "=========================" << std::endl;
        
        for (const auto& name : testNames) {
            // This is handled by addTest which runs immediately
            // Tests are run when they're added to allow for immediate feedback
        }
        
        std::cout << "=========================" << std::endl;
        std::cout << "Tests completed: " << passedTests << "/" << totalTests << " passed" << std::endl;
    }
    
    int getPassedTests() const { return passedTests; }
    int getTotalTests() const { return totalTests; }
    double getPassRate() const { return totalTests > 0 ? (double)passedTests / totalTests : 0.0; }
};

// Macro for easy test declaration
#define TEST_CASE(name, testFunc) \\
    namespace test_##name { \\
        void test_##name(); \\
        struct TestRegistration_##name { \\
            TestRegistration_##name() { \\
                TestRunner::getInstance().addTest(#name, test_##name::test_##name); \\
            } \\
        }; \\
        TestRegistration_##name registration_##name; \\
    } \\
    
    void test_##name::test_##name()

// Global test runner instance
TestRunner& getTestRunner();

} // namespace test
} // namespace nlm
