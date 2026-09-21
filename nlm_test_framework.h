// NLM Test Framework
// Modern, robust testing framework for NLM (Phase 6: Integrated Artificial Brain)
// 
// This framework provides:
// - Multiple test categories (Unit, Integration, Performance, Stress, Compatibility)
// - Advanced assertion macros with detailed failure reporting
// - Test fixtures for setup/teardown
// - Parameterized test support
// - Performance statistics tracking
// - Backward compatibility with existing tests
//
// Usage:
// 1. Include this header in your test file
// 2. Register tests using the TEST() or TEST_F() macros
// 3. Run the test suite using nlm::test::TestSuite::runAllTests()

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <sstream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <set>
#include <random>

namespace nlm {
namespace test {

// Test result structure containing comprehensive test metadata
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string failureMessage;
    std::string file;
    int line;
    std::chrono::nanoseconds duration;
    size_t memoryUsage;
    bool skipped;
    std::string tags;
    
    TestResult(const std::string& testName, const std::string& testCategory) 
        : name(testName), category(testCategory), passed(false), line(0), 
          duration(0), memoryUsage(0), skipped(false) {}
};

// Test failure record for detailed reporting
struct TestFailure {
    std::string testName;
    std::string expression;
    std::string location;
    std::string message;
    double time;
    
    TestFailure(const std::string& name, const std::string& expr, const std::string& loc, 
                const std::string& msg, double timeMs)
        : testName(name), expression(expr), location(loc), message(msg), time(timeMs) {}
};

// Test fixture base class for setup/teardown
class TestFixture {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
    
public:
    virtual ~TestFixture() {}
    void setup() { SetUp(); }
    void teardown() { TearDown(); }
};

// Test case class - base class for all tests
class TestCase {
protected:
    std::string name;
    std::string category;
    bool setupComplete;
    std::shared_ptr<TestFixture> fixture;
    
public:
    TestCase(const std::string& testName, const std::string& testCategory = "Unit")
        : name(testName), category(testCategory), setupComplete(false) {}
    
    virtual ~TestCase() {}
    
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    
    virtual TestResult run() = 0;
    
    void setFixture(std::shared_ptr<TestFixture> f) {
        fixture = f;
        if (fixture) {
            fixture->setup();
            setupComplete = true;
        }
    }
    
    void clearFixture() {
        if (fixture && setupComplete) {
            fixture->teardown();
            setupComplete = false;
        }
    }
};

// Performance metrics collection
struct PerformanceMetrics {
    std::chrono::nanoseconds totalDuration;
    double averagePerTest;
    std::chrono::nanoseconds minDuration;
    std::chrono::nanoseconds maxDuration;
    size_t testCount;
    
    PerformanceMetrics() : totalDuration(0), averagePerTest(0), minDuration(0), 
                          maxDuration(0), testCount(0) {}
    
    void addTestTime(std::chrono::nanoseconds duration) {
        totalDuration += duration;
        if (minDuration == 0 || duration < minDuration) minDuration = duration;
        if (duration > maxDuration) maxDuration = duration;
        ++testCount;
    }
    
    double getAveragePerTest() const {
        return testCount > 0 ? static_cast<double>(totalDuration.count()) / testCount : 0.0;
    }
};

// Modern assertion macros with detailed failure reporting
#define NLM_TEST_ASSERT(condition, message) \
do { \
    if (!(condition)) { \
        std::ostringstream oss; \
        oss << "Assertion failed: " << message; \
        throw std::runtime_error(oss.str()); \
    } \
} while(0)

#define NLM_TEST_CHECK(condition, message) \
do { \
    if (!(condition)) { \
        std::ostringstream oss; \
        oss << "Check failed: " << message; \
        throw std::runtime_error(oss.str()); \
    } \
} while(0)

#define NLM_TEST_FAIL(message) \
do { \
    std::ostringstream oss; \
    oss << "Test failed: " << message; \
    throw std::runtime_error(oss.str()); \
} while(0)

// Simple test case macro for backward compatibility
#define SIMPLE_TEST(testFunc, testName) \
static bool testFunc##_active = []() { \
    auto test = std::make_unique<LegacyTestCase>(#testFunc, []() { testFunc(); }); \
    TestSuite::registerTest(std::move(test)); \
    return true; \
}(); \
\
class LegacyTestCase : public TestCase { \
public: \
    std::function<void()> testFunction; \
    \
    LegacyTestCase(const std::string& name, std::function<void()> func) \
        : TestCase(name, "Compatibility"), testFunction(func) {} \
    \
    virtual TestResult run() { \
        TestResult result(name, category); \
        auto start = std::chrono::high_resolution_clock::now(); \
        try { \
            testFunction(); \
            result.passed = true; \
        } catch (const std::exception& e) { \
            result.passed = false; \
            result.failureMessage = e.what(); \
        } \
        auto end = std::chrono::high_resolution_clock::now(); \
        result.duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start); \
        return result; \
    } \
};

// Test suite manager - central registry and runner
class TestSuite {
private:
    static std::vector<std::unique_ptr<TestCase>> testCases;
    static std::map<std::string, std::unique_ptr<TestFixture>> fixtures;
    static PerformanceMetrics performanceMetrics;
    static std::vector<std::string> testCategories;
    static std::map<std::string, std::vector<std::string>> categoryFilter;
    static bool verboseMode;
    static bool performanceMode;
    
public:
    static void initialize() {
        testCases.clear();
        fixtures.clear();
        performanceMetrics = PerformanceMetrics();
        testCategories = {"Unit", "Integration", "Performance", "Stress", "Compatibility"};
        verboseMode = false;
        performanceMode = false;
    }
    
    static void setVerbose(bool verbose) { verboseMode = verbose; }
    static void setPerformanceMode(bool performance) { performanceMode = performance; }
    
    static void registerFixture(const std::string& name, std::unique_ptr<TestFixture> fixture) {
        fixtures[name] = std::move(fixture);
    }
    
    static void registerTest(std::unique_ptr<TestCase> test) {
        testCases.push_back(std::move(test));
        std::string category = test->getCategory();
        if (std::find(testCategories.begin(), testCategories.end(), category) == testCategories.end()) {
            testCategories.push_back(category);
        }
    }
    
    static void filterByCategory(const std::string& category) {
        categoryFilter[category] = {};
    }
    
    static std::vector<std::string> getAvailableCategories() {
        return testCategories;
    }
    
    static TestResult runTest(TestCase* test, bool useFixture) {
        TestResult result(test->getName(), test->getCategory());
        result.file = "unknown";
        result.line = 0;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            if (useFixture && test->getCategory() != "Compatibility") {
                for (const auto& pair : fixtures) {
                    test->setFixture(pair.second.get());
                    break;
                }
            }
            
            result = test->run();
            
            if (result.passed) {
                std::cout << "  ✓ " << test->getName();
                if (performanceMode) {
                    double durationMs = result.duration.count() / 1000000.0;
                    std::cout << " (" << std::fixed << std::setprecision(3) << durationMs << " ms)";
                }
                std::cout << std::endl;
            }
            
        } catch (const std::exception& e) {
            result.passed = false;
            result.failureMessage = e.what();
            std::cout << "  ✗ " << test->getName() << " - " << e.what() << std::endl;
        }
        
        test->clearFixture();
        
        auto end = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        performanceMetrics.addTestTime(result.duration);
        
        return result;
    }
    
    static std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        std::cout << std::endl;
        std::cout << "=== NLM Test Suite ===" << std::endl;
        if (performanceMode) {
            std::cout << "Performance mode: ON" << std::endl;
        }
        
        // Filter tests by category if specified
        std::vector<TestCase*> testsToRun;
        for (auto& test : testCases) {
            bool shouldRun = true;
            
            if (!categoryFilter.empty()) {
                bool categoryMatches = false;
                for (const auto& filter : categoryFilter) {
                    if (test->getCategory() == filter.first) {
                        categoryMatches = true;
                        break;
                    }
                }
                shouldRun = categoryMatches;
            }
            
            if (shouldRun) {
                testsToRun.push_back(test.get());
            }
        }
        
        // Run tests with fixture support
        for (auto test : testsToRun) {
            bool useFixture = (test->getCategory() != "Compatibility" && 
                              test->getCategory() != "Simple");
            results.push_back(runTest(test, useFixture));
        }
        
        return results;
    }
    
    static void printReport(const std::vector<TestResult>& results) {
        std::cout << std::endl;
        std::cout << "=== Test Report ===" << std::endl;
        
        size_t totalTests = results.size();
        size_t passedTests = 0;
        size_t failedTests = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                ++passedTests;
            } else {
                ++failedTests;
            }
        }
        
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        
        if (performanceMode) {
            std::cout << "Performance metrics:" << std::endl;
            std::cout << "  Total time: " << performanceMetrics.totalDuration.count() / 1000000.0 << " ms" << std::endl;
            std::cout << "  Average per test: " << performanceMetrics.getAveragePerTest() / 1000000.0 << " ms" << std::endl;
            std::cout << "  Min time: " << performanceMetrics.minDuration.count() / 1000000.0 << " ms" << std::endl;
            std::cout << "  Max time: " << performanceMetrics.maxDuration.count() / 1000000.0 << " ms" << std::endl;
        }
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== Failed Tests ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  " << result.name << ": " << result.failureMessage << std::endl;
                }
            }
        }
        
        std::cout << std::endl;
    }
};

// Registry for managing test cases and backward compatibility
class TestRegistry {
private:
    struct RegisteredTest {
        std::string name;
        std::function<void()> testFunc;
        std::string category;
        std::string description;
    };
    
    static std::vector<RegisteredTest> registeredTests;
    static std::vector<TestFailure> testFailures;
    
public:
    static void registerTest(const std::string& name, std::function<void()> testFunc,
                           const std::string& category = "Unit",
                           const std::string& description = "") {
        registeredTests.push_back({name, testFunc, category, description});
    }
    
    static std::vector<RegisteredTest> getAllTests() {
        return registeredTests;
    }
    
    static void clearFailures() {
        testFailures.clear();
    }
    
    static void addFailure(const TestFailure& failure) {
        testFailures.push_back(failure);
    }
    
    static std::vector<TestFailure> getAllFailures() {
        return testFailures;
    }
};

// Test categories for easy filtering
namespace Categories {
    const std::string Unit = "Unit";
    const std::string Integration = "Integration";
    const std::string Performance = "Performance";
    const std::string Stress = "Stress";
    const std::string Compatibility = "Compatibility";
}

// Export important symbols
inline TestSuite& getTestSuite() { return testSuite; }
inline void initializeTestSuite() { TestSuite::initialize(); }

// Helper functions for backward compatibility
inline void runLegacyTest(void (*testFunc)()) {
    try {
        testFunc();
    } catch (const std::exception& e) {
        std::cout << "Legacy test failed: " << e.what() << std::endl;
        throw;
    }
}

// Global test suite instance
inline TestSuite testSuite;

// Static member definitions
inline std::vector<std::unique_ptr<TestCase>> TestSuite::testCases;
inline std::map<std::string, std::unique_ptr<TestFixture>> TestSuite::fixtures;
inline PerformanceMetrics TestSuite::performanceMetrics;
inline std::vector<std::string> TestSuite::testCategories;
inline std::map<std::string, std::vector<std::string>> TestSuite::categoryFilter;
inline bool TestSuite::verboseMode = false;
inline bool TestSuite::performanceMode = false;

// TestRegistry static member definitions
inline std::vector<TestRegistry::RegisteredTest> TestRegistry::registeredTests;
inline std::vector<TestFailure> TestRegistry::testFailures;

} // namespace test
} // namespace nlm
