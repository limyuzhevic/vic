// Test utilities for NLM framework
// Provides reusable test macros and utilities to reduce code duplication

#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <functional>

namespace nlm {
namespace test {

// Test result tracking
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double durationMs;
    
    TestResult(const std::string& n, bool p, const std::string& msg = "") 
        : name(n), passed(p), message(msg), durationMs(0.0) {}
};

// Test framework base class
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
};

// Macro for registering tests automatically
#define REGISTER_TEST(test) \
    namespace { \
        class test##_reg { \
        public: \
            test##_reg() { test::registerTest(new test()); } \
        }; \
        test##_reg test##_reg_instance; \
    }

// Macro for creating simple test cases
#define TEST_CASE(name, test_body) \
    class name : public TestCase { \
    public: \
        std::string getName() const override { return #name; } \
        void run() override { test_body } \
    };

// Test utilities
class TestUtils {
public:
    // Floating point comparison
    static bool approxEqual(float a, float b, float epsilon = 1e-5f) {
        return std::abs(a - b) <= epsilon;
    }
    
    static bool approxEqual(double a, double b, double epsilon = 1e-8) {
        return std::abs(a - b) <= epsilon;
    }
    
    // Vector equality
    template<typename T>
    static bool vectorEqual(const std::vector<T>& a, const std::vector<T>& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) return false;
        }
        return true;
    }
    
    // Vector approximate equality
    template<typename T>
    static bool vectorApproxEqual(const std::vector<T>& a, const std::vector<T>& b, T epsilon) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (!approxEqual(a[i], b[i], epsilon)) return false;
        }
        return true;
    }
    
    // String comparison with case sensitivity
    static bool stringEqual(const std::string& a, const std::string& b, bool caseSensitive = true) {
        if (caseSensitive) {
            return a == b;
        } else {
            return std::tolower(a) == std::tolower(b);
        }
    }
    
    // Format message
    static std::string format(const std::string& format, ...) {
        // Simplified implementation - in production, use proper formatting
        return format;
    }
    
    // Time measurement
    class Timer {
    public:
        Timer() : start_(std::chrono::high_resolution_clock::now()) {}
        ~Timer() {}
        double elapsedMs() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
        }
    private:
        std::chrono::high_resolution_clock::time_point start_;
    };
    
    // Test assertion with detailed reporting
    static void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            std::cerr << "FAILED: " << message << std::endl;
            assert(condition);
        }
    }
    
    static void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            std::cerr << "FAILED: " << message << std::endl;
            assert(!condition);
        }
    }
    
    static void assertEqual(int a, int b, const std::string& message = "") {
        if (a != b) {
            std::cerr << "FAILED: " << message << " (expected " << a << ", got " << b << ")" << std::endl;
            assert(a == b);
        }
    }
    
    static void assertApproxEqual(float a, float b, float epsilon, const std::string& message = "") {
        if (!approxEqual(a, b, epsilon)) {
            std::cerr << "FAILED: " << message << " (expected " << a << ", got " << b << ", epsilon " << epsilon << ")" << std::endl;
            assert(approxEqual(a, b, epsilon));
        }
    }
    
    static void assertEqual(const std::string& a, const std::string& b, const std::string& message = "") {
        if (a != b) {
            std::cerr << "FAILED: " << message << " (expected \"" << a << "\", got \"" << b << "\")" << std::endl;
            assert(a == b);
        }
    }
};

// Simple test runner
class TestRunner {
public:
    static std::vector<TestResult> runTests(const std::vector<std::unique_ptr<TestCase>>& tests) {
        std::vector<TestResult> results;
        
        for (auto& test : tests) {
            std::cout << "Running test: " << test->getName() << "..." << std::endl;
            
            TestUtils::Timer timer;
            bool setupSuccess = test->setup();
            
            TestResult result(test->getName(), false, "Setup failed");
            
            if (setupSuccess) {
                try {
                    test->run();
                    result.passed = true;
                    result.message = "Test passed";
                } catch (const std::exception& e) {
                    result.message = std::string("Exception: ") + e.what();
                } catch (...) {
                    result.message = "Unknown exception";
                }
            }
            
            result.durationMs = timer.elapsedMs();
            results.push_back(result);
            
            test->teardown();
        }
        
        return results;
    }
    
    static void printResults(const std::vector<TestResult>& results) {
        std::cout << "=== Test Results ===" << std::endl;
        int passed = 0, failed = 0;
        
        for (const auto& result : results) {
            std::cout << (result.passed ? "✓ PASS" : "✗ FAIL") << ": " << result.name;
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << " (" << result.durationMs << " ms)" << std::endl;
            
            if (result.passed) passed++; else failed++;
        }
        
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "Total: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        
        if (failed > 0) {
            std::cout << std::endl << "FAILED TESTS:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.name << ": " << result.message << std::endl;
                }
            }
        }
    }
    
    static bool allPassed(const std::vector<TestResult>& results) {
        for (const auto& result : results) {
            if (!result.passed) return false;
        }
        return true;
    }
};

// Test registry
class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    void registerTest(std::unique_ptr<TestCase> test) {
        tests.push_back(std::move(test));
    }
    
    const std::vector<std::unique_ptr<TestCase>>& getTests() const {
        return tests;
    }
    
    void clear() {
        tests.clear();
    }
    
    size_t getTestCount() const {
        return tests.size();
    }
    
private:
    TestRegistry() {}
    std::vector<std::unique_ptr<TestCase>> tests;
};

} // namespace test
} // namespace nlm