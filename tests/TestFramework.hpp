// Enhanced test framework with better structure
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <chrono>
#include <thread>

class TestResult {
public:
    std::string name;
    bool passed = false;
    std::string errorMessage;
    double duration = 0.0;
    
    TestResult(const std::string& testName) : name(testName) {}
};

class TestSuite {
public:
    TestSuite(const std::string& suiteName) : suiteName_(suiteName) {}
    
    void addTest(const std::string& testName, std::function<void()> testFunc) {
        tests_.push_back({testName, testFunc});
    }
    
    void runAll() {
        std::cout << "=== Test Suite: " << suiteName_ << " ===" << std::endl;
        
        for (auto& test : tests_) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                test.testFunc();
                test.passed = true;
                std::cout << "  ✓ " << test.name << std::endl;
            } catch (const std::exception& e) {
                test.passed = false;
                test.errorMessage = e.what();
                std::cout << "  ✗ " << test.name << " - " << e.what() << std::endl;
            } catch (...) {
                test.passed = false;
                test.errorMessage = "Unknown exception";
                std::cout << "  ✗ " << test.name << " - Unknown exception" << std::endl;
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            test.duration = std::chrono::duration<double>(endTime - startTime).count();
        }
        
        int passed = 0;
        int total = tests_.size();
        double totalDuration = 0.0;
        
        for (const auto& test : tests_) {
            if (test.passed) passed++;
            totalDuration += test.duration;
        }
        
        std::cout << "=== Results: " << passed << "/" << total << " passed ===" << std::endl;
        std::cout << "Total time: " << totalDuration << " seconds" << std::endl;
        
        if (passed == total) {
            std::cout << "All tests passed!" << std::endl;
        } else {
            std::cout << "Test failures:" << std::endl;
            for (const auto& test : tests_) {
                if (!test.passed) {
                    std::cout << "  - " << test.name << ": " << test.errorMessage << std::endl;
                }
            }
        }
    }
    
    std::vector<TestResult> getResults() const {
        std::vector<TestResult> results;
        for (const auto& test : tests_) {
            TestResult result(test.name);
            result.passed = test.passed;
            result.errorMessage = test.errorMessage;
            result.duration = test.duration;
            results.push_back(result);
        }
        return results;
    }
    
private:
    struct TestInfo {
        std::string name;
        std::function<void()> func;
        bool passed = false;
        std::string errorMessage;
        double duration = 0.0;
    };
    
    std::string suiteName_;
    std::vector<TestInfo> tests_;
};

// Enhanced assertion macros with detailed error messages
#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + " (condition: " #condition ")"); \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) \
    do { \
        if (condition) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + " (condition: !" #condition ")"); \
        } \
    } while(0)

#define ASSERT_EQUAL(expected, actual, message) \
    do { \
        if ((expected) != (actual)) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + \
                                    " (expected: " + std::to_string(expected) + \
                                    ", actual: " + std::to_string(actual) + ")"); \
        } \
    } while(0)

#define ASSERT_DOUBLE_EQUAL(expected, actual, tolerance, message) \
    do { \
        if (std::abs((expected) - (actual)) > (tolerance)) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + \
                                    " (expected: " + std::to_string(expected) + \
                                    ", actual: " + std::to_string(actual) + \
                                    ", tolerance: " + std::to_string(tolerance) + ")"); \
        } \
    } while(0)

#define ASSERT_CONTAINS(container, value, message) \
    do { \
        if (std::find((container).begin(), (container).end(), (value)) == (container).end()) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + \
                                    " (value not found in container)"); \
        } \
    } while(0)

#define ASSERT_NOT_CONTAINS(container, value, message) \
    do { \
        if (std::find((container).begin(), (container).end(), (value)) != (container).end()) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + \
                                    " (value found in container)"); \
        } \
    } while(0)

#define ASSERT_NO_THROW(statement, message) \
    do { \
        try { \
            statement; \
        } catch (...) { \
            throw std::runtime_error("ASSERTION FAILED: " + std::string(message) + \
                                    " (unexpected exception thrown)"); \
        } \
    } while(0)

// Performance testing utilities
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start_).count();
        std::cout << "  [PERF] " << name_ << ": " << duration << "s" << std::endl;
    }
    
    double getDuration() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
    
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_;
};

// Test data generators
class TestDataGenerator {
public:
    static std::vector<int> generateRandomInts(size_t count, int min = 0, int max = 100) {
        std::vector<int> data;
        data.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            data.push_back(min + std::rand() % (max - min + 1));
        }
        return data;
    }
    
    static std::vector<double> generateRandomDoubles(size_t count, double min = 0.0, double max = 1.0) {
        std::vector<double> data;
        data.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            data.push_back(min + (static_cast<double>(std::rand()) / RAND_MAX) * (max - min));
        }
        return data;
    }
    
    static std::vector<std::string> generateRandomStrings(size_t count, size_t length) {
        std::vector<std::string> data;
        data.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            std::string str;
            for (size_t j = 0; j < length; ++j) {
                str.push_back('a' + (std::rand() % 26));
            }
            data.push_back(str);
        }
        return data;
    }
};
