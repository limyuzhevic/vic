// TestFramework.h - Test Framework Header
// Comprehensive test framework for the Neural Learning Machine
// 
// Provides unit testing, integration testing, performance testing, and
// automated testing capabilities for all neural systems and cognitive functions.

#pragma once

#include "Brain.hpp"
#include "../config/AdvancedConfig.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testSuite;
    std::chrono::duration<double> executionTime;
    bool passed;
    std::string message;
    std::string errorDetails;
    size_t testId;
    
    TestResult(const std::string& name, const std::string& suite, bool passed,
               const std::string& message = "", const std::string& error = "")
        : testName(name), testSuite(suite), passed(passed), message(message),
          errorDetails(error), testId(0) {}
    
    std::string toString() const;
    std::string toJSON() const;
};

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getSuite() const = 0;
    
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
    virtual bool run() = 0;
    
    virtual bool canRun() const { return true; }
    
    TestResult execute();
    
    // Helper assertion methods
    template<typename T, typename U>
    bool assertEqual(const T& expected, const U& actual, const std::string& message = "");
    
    template<typename T>
    bool assertTrue(const T& condition, const std::string& message = "");
    
    template<typename T>
    bool assertFalse(const T& condition, const std::string& message = "");
    
    template<typename T, typename... Args>
    bool assertThrows(const std::function<void(Args...)>& func, const std::string& message = "");
    
    bool assertNotNull(std::nullptr_t ptr, const std::string& message = "");
    
    bool assertNull(std::nullptr_t ptr, const std::string& message = "");
    
    template<typename T>
    bool assertNotNull(T* ptr, const std::string& message = "");
    
    template<typename T>
    bool assertNull(T* ptr, const std::string& message = "");
    
    template<typename Container>
    bool assertContains(const Container& container, const typename Container::value_type& value,
                       const std::string& message = "");
    
    template<typename Container>
    bool assertEmpty(const Container& container, const std::string& message = "");
    
    template<typename Container>
    bool assertNotEmpty(const Container& container, const std::string& message = "");
    
    template<typename Numeric>
    bool assertGreaterThan(const Numeric& actual, const Numeric& expected,
                          const std::string& message = "");
    
    template<typename Numeric>
    bool assertLessThan(const Numeric& actual, const Numeric& expected,
                       const std::string& message = "");
    
    template<typename Numeric>
    bool assertWithin(const Numeric& actual, const Numeric& min, const Numeric& max,
                     const std::string& message = "");
};

// Test suite base class
class TestSuite {
public:
    virtual ~TestSuite() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual std::vector<std::unique_ptr<TestCase>> getTests() = 0;
    
    virtual bool setup() { return true; }
    virtual bool teardown() { return true; }
    
    virtual bool canRun() const;
};

// Forward declarations for test suites
class NeuralSystemTestSuite;
class MemorySystemTestSuite;
class CognitiveSystemTestSuite;
class NeuromodulationTestSuite;
class DevelopmentTestSuite;
class ConfigurationTestSuite;
class IntegrationTestSuite;
class PerformanceTestSuite;

// Test runner
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    // Register test suites
    void registerSuite(std::unique_ptr<TestSuite> suite);
    
    // Run all tests
    bool runAllTests();
    bool runSuite(const std::string& suiteName);
    bool runTest(const std::string& suiteName, const std::string& testName);
    
    // Get test results
    const std::vector<TestResult>& getAllResults() const { return allResults_; }
    std::vector<TestResult> getResultsForSuite(const std::string& suiteName) const;
    std::vector<TestResult> getFailedTests() const;
    std::vector<TestResult> getPassedTests() const;
    
    // Statistics
    size_t getTotalTests() const { return totalTests_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    double getSuccessRate() const { return totalTests_ > 0 ? (double)passedTests_ / totalTests_ : 0.0; }
    
    // Configuration
    void setVerbose(bool verbose) { verboseMode_ = verbose; }
    void setParallel(bool parallel) { parallelMode_ = parallel; }
    void setMaxRetries(size_t retries) { maxRetries_ = retries; }
    
    // Reporting
    std::string generateReport() const;
    std::string generateJSONReport() const;
    std::string generateJUnitReport() const;
    
    // Export
    bool exportResults(const std::string& filepath, const std::string& format = "text");
    
private:
    std::vector<std::unique_ptr<TestSuite>> suites_;
    std::vector<TestResult> allResults_;
    
    bool verboseMode_;
    bool parallelMode_;
    size_t maxRetries_;
    size_t totalTests_;
    size_t passedTests_;
    size_t failedTests_;
    
    // Helper methods
    void setupSuites();
    bool runTestSuite(TestSuite& suite);
    bool runSingleTest(TestCase& test, TestSuite& suite);
    TestResult executeTestWithRetry(TestCase& test, TestSuite& suite);
    void updateStatistics(const TestResult& result);
};

// Test utilities
class TestUtils {
public:
    // Random data generation
    static std::vector<float> generateRandomPattern(size_t size, float min = 0.0f, float max = 1.0f);
    static std::vector<float> generateNoisyPattern(const std::vector<float>& pattern, float noiseLevel);
    
    // Statistical utilities
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    
    // Simulation utilities
    static std::shared_ptr<Brain> createTestBrain(size_t neuronCount = 100, size_t regionCount = 1);
    static std::shared_ptr<Config> createTestConfig();
    static std::shared_ptr<AdvancedConfig> createAdvancedTestConfig();
    
    // Comparison utilities
    static bool vectorsAreEqual(const std::vector<float>& a, const std::vector<float>& b,
                              float tolerance = 0.001f);
    static bool matricesAreEqual(const std::vector<std::vector<float>>& a,
                               const std::vector<std::vector<float>>& b,
                               float tolerance = 0.001f);
    
    // Performance measurement
    static double measureExecutionTime(const std::function<void()>& func);
    static double measureMemoryUsage(const std::function<void()>& func);
    
    // Logging utilities
    static void logTestStart(const std::string& testName, const std::string& suiteName);
    static void logTestResult(const std::string& testName, bool passed, const std::string& message = "");
    static void logTestEnd(const std::string& testName, const std::chrono::duration<double>& duration);
};

} // namespace nlm

// Test utility implementations
inline std::string TestResult::toString() const {
    std::ostringstream oss;
    oss << "Test: " << testName << " (" << testSuite << ") - ";
    if (passed) {
        oss << "PASS";
    } else {
        oss << "FAIL";
        if (!errorDetails.empty()) {
            oss << " - " << errorDetails;
        }
    }
    oss << " (" << executionTime.count() << "s)";
    if (!message.empty()) {
        oss << " - " << message;
    }
    return oss.str();
}

inline std::string TestResult::toJSON() const {
    std::ostringstream oss;
    oss << "{\"testName\": \"" << testName << "\", \"testSuite\": \"" << testSuite
        << "\", \"passed\": " << (passed ? "true" : "false")
        << ", \"executionTime\": " << executionTime.count()
        << ", \"message\": \"" << message << "\"";
    if (!errorDetails.empty()) {
        oss << ", \"errorDetails\": \"" << errorDetails << "\"";
    }
    oss << "}";
    return oss.str();
}

inline TestResult TestCase::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Setup
    if (!setup()) {
        return TestResult(getName(), getSuite(), false, "Setup failed");
    }
    
    // Run test
    bool passed = false;
    std::string message;
    
    try {
        passed = run();
        message = "Test completed successfully";
    } catch (const std::exception& e) {
        passed = false;
        message = std::string("Test threw exception: ") + e.what();
    } catch (...) {
        passed = false;
        message = "Test threw unknown exception";
    }
    
    // Teardown
    teardown();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    
    return TestResult(getName(), getSuite(), passed, message);
}

// Helper assertion implementations
template<typename T, typename U>
bool TestCase::assertEqual(const T& expected, const U& actual, const std::string& message) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << message << " - Expected: " << expected << ", Got: " << actual;
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename T>
bool TestCase::assertTrue(const T& condition, const std::string& message) {
    if (!condition) {
        std::ostringstream oss;
        oss << message << " - Condition is false";
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename T>
bool TestCase::assertFalse(const T& condition, const std::string& message) {
    if (condition) {
        std::ostringstream oss;
        oss << message << " - Condition is true";
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename T, typename... Args>
bool TestCase::assertThrows(const std::function<void(Args...)>& func, const std::string& message) {
    try {
        func(std::forward<Args>(Args())...);
    } catch (...) {
        return true;
    }
    throw std::runtime_error(message + " - Expected exception was not thrown");
}

inline bool TestCase::assertNotNull(std::nullptr_t ptr, const std::string& message) {
    if (ptr != nullptr) {
        throw std::runtime_error(message + " - Expected null pointer");
    }
    return true;
}

inline bool TestCase::assertNull(std::nullptr_t ptr, const std::string& message) {
    if (ptr == nullptr) {
        throw std::runtime_error(message + " - Expected non-null pointer");
    }
    return true;
}

template<typename T>
bool TestCase::assertNotNull(T* ptr, const std::string& message) {
    if (ptr == nullptr) {
        throw std::runtime_error(message + " - Expected non-null pointer");
    }
    return true;
}

template<typename T>
bool TestCase::assertNull(T* ptr, const std::string& message) {
    if (ptr != nullptr) {
        throw std::runtime_error(message + " - Expected null pointer");
    }
    return true;
}

template<typename Container>
bool TestCase::assertContains(const Container& container, const typename Container::value_type& value,
                          const std::string& message) {
    if (std::find(container.begin(), container.end(), value) == container.end()) {
        std::ostringstream oss;
        oss << message << " - Value not found in container";
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename Container>
bool TestCase::assertEmpty(const Container& container, const std::string& message) {
    if (!container.empty()) {
        std::ostringstream oss;
        oss << message << " - Container is not empty";
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename Container>
bool TestCase::assertNotEmpty(const Container& container, const std::string& message) {
    if (container.empty()) {
        std::ostringstream oss;
        oss << message << " - Container is empty";
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename Numeric>
bool TestCase::assertGreaterThan(const Numeric& actual, const Numeric& expected,
                         const std::string& message) {
    if (!(actual > expected)) {
        std::ostringstream oss;
        oss << message << " - Expected: " << expected << ", Got: " << actual;
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename Numeric>
bool TestCase::assertLessThan(const Numeric& actual, const Numeric& expected,
                        const std::string& message) {
    if (!(actual < expected)) {
        std::ostringstream oss;
        oss << message << " - Expected: " << expected << ", Got: " << actual;
        throw std::runtime_error(oss.str());
    }
    return true;
}

template<typename Numeric>
bool TestCase::assertWithin(const Numeric& actual, const Numeric& min, const Numeric& max,
                         const std::string& message) {
    if (!(actual >= min && actual <= max)) {
        std::ostringstream oss;
        oss << message << " - Expected range [" << min << ", " << max
            << "], Got: " << actual;
        throw std::runtime_error(oss.str());
    }
    return true;
}

} // namespace nlm