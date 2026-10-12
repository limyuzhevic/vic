#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

/**
 * Simple but effective C++ testing framework for NLM
 * Provides assertion macros, test organization, and reporting
 */

class TestCase {
public:
    TestCase(const std::string& name, const std::string& category = "General")
        : name_(name), category_(category), passed_(false), duration_ms_(0.0) {}
    
    virtual ~TestCase() = default;
    
    const std::string& getName() const { return name_; }
    const std::string& getCategory() const { return category_; }
    bool isPassed() const { return passed_; }
    double getDurationMs() const { return duration_ms_; }
    
    void setPassed(bool passed) { passed_ = passed; }
    void setDuration(double duration_ms) { duration_ms_ = duration_ms; }
    
    virtual void run() = 0;
    
protected:
    std::string name_;
    std::string category_;
    bool passed_;
    double duration_ms_;
};

class TestFramework {
public:
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void registerTest(TestCase* test) {
        tests_.push_back(test);
    }
    
    void runAllTests() {
        std::cout << "\n=== NLM Test Suite ===\n";
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        int total_tests = tests_.size();
        int passed_tests = 0;
        int failed_tests = 0;
        
        // Run tests in category order
        runTestsByCategory("Unit");
        runTestsByCategory("Integration");
        runTestsByCategory("Performance");
        runTestsByCategory("General");
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Report results
        std::cout << "\n=== Test Summary ===";
        std::cout << "\nTotal tests: " << total_tests;
        std::cout << "\nPassed: " << passed_tests;
        std::cout << "\nFailed: " << failed_tests;
        std::cout << "\nTotal time: " << total_duration.count() << " ms";
        std::cout << "\nSuccess rate: " << (total_tests > 0 ? (passed_tests * 100 / total_tests) : 0) << "%";
        
        if (failed_tests > 0) {
            std::cout << "\n\n=== FAILED TESTS ===";
            for (TestCase* test : tests_) {
                if (!test->isPassed()) {
                    std::cout << "  - " << test->getCategory() << ": " << test->getName();
                }
            }
        }
        
        // Cleanup
        for (TestCase* test : tests_) {
            delete test;
        }
        tests_.clear();
    }
    
    template<typename T, typename U>
    void assertEqual(const T& expected, const U& actual, const std::string& message = "Assertion failed") {
        if (expected != actual) {
            std::cerr << "FAIL: " << message << "\n  Expected: " << expected << "\n  Actual: " << actual << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    template<typename T, typename U>
    void assertNotEqual(const T& expected, const U& actual, const std::string& message = "Assertion failed") {
        if (expected == actual) {
            std::cerr << "FAIL: " << message << "\n  Expected not equal to: " << expected << "\n  But both are: " << actual << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "Assertion failed") {
        if (!condition) {
            std::cerr << "FAIL: " << message << " - condition is false" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "Assertion failed") {
        if (condition) {
            std::cerr << "FAIL: " << message << " - condition is true" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertNullptr(const void* ptr, const std::string& message = "Assertion failed") {
        if (ptr != nullptr) {
            std::cerr << "FAIL: " << message << "\n  Pointer should be null" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertNotNullptr(const void* ptr, const std::string& message = "Assertion failed") {
        if (ptr == nullptr) {
            std::cerr << "FAIL: " << message << "\n  Pointer should not be null" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertLessThan(double a, double b, const std::string& message = "Assertion failed") {
        if (a >= b) {
            std::cerr << "FAIL: " << message << "\n  " << a << " should be less than " << b << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertGreaterThan(double a, double b, const std::string& message = "Assertion failed") {
        if (a <= b) {
            std::cerr << "FAIL: " << message << "\n  " << a << " should be greater than " << b << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
    void assertInRange(double value, double min, double max, const std::string& message = "Assertion failed") {
        if (value < min || value > max) {
            std::cerr << "FAIL: " << message << "\n  Value " << value << " is not in range [" << min << ", " << max << "]" << std::endl;
            throw std::runtime_error("Test assertion failed");
        }
    }
    
private:
    TestFramework() = default;
    ~TestFramework() = default;
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    
    void runTestsByCategory(const std::string& category) {
        for (TestCase* test : tests_) {
            if (test->getCategory() == category) {
                try {
                    auto start = std::chrono::high_resolution_clock::now();
                    test->run();
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    
                    test->setPassed(true);
                    test->setDuration(duration.count());
                } catch (const std::exception& e) {
                    test->setPassed(false);
                    std::cerr << "Exception in test " << test->getName() << ": " << e.what() << std::endl;
                }
            }
        }
    }
    
    std::vector<TestCase*> tests_;
};

// Macros for easy test case creation
#define TEST_CASE_NAME(name) TestCase_##name
#define TEST_CASE_CATEGORY(name) "Unit"
#define TEST_CASE(test_class, test_name) \
    class test_class : public TestCase { \
    public: \
        test_class() : TestCase(#test_name, TEST_CASE_CATEGORY(#test_name)) {} \
        void run() override; \
    }; \
    static test_class TEST_CASE_NAME(test_name); \
    void test_class::run()

// Helper macro for easy assertion
define TEST_ASSERT_EQUAL(expected, actual, message) \
    TestFramework::getInstance().assertEqual(expected, actual, message)

// Include the framework
#endif // TEST_FRAMEWORK_HPP