// NLM Test Framework
// Modern C++17 test framework for NLM project
#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <sstream>

namespace nlm::test {

// Forward declarations
class TestBase;
class TestFixture;
class TestSuite;
class TestRunner;
class TestRegistry;
class AssertFramework;

// Test result types
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR,
    DISABLED
};

// Test severity levels
enum class TestSeverity {
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    MEMORY,
    EDGE_CASE,
    STRESS,
    PROPERTY,
    REGRESSION
};

// Test metadata
struct TestInfo {
    std::string name;
    std::string description;
    TestSeverity severity;
    std::string file;
    int line;
    std::vector<std::string> tags;
    bool disabled = false;
    bool parameterized = false;
};

// Test statistics
struct TestStats {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    int errors = 0;
    double totalTime = 0.0;
    std::unordered_map<std::string, int> severityCounts;
};

// Test case structure
class TestCase {
public:
    TestCase(std::function<void()> testFunc, TestInfo info)
        : testFunc(std::move(testFunc)), info(std::move(info)) {}
    
    TestResult run();
    TestInfo getInfo() const { return info; }
    std::string getName() const { return info.name; }
    
private:
    std::function<void()> testFunc;
    TestInfo info;
    std::chrono::high_resolution_clock::time_point startTime;
    double duration = 0.0;
};

// Test fixture for resource management
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void setup() {}
    virtual void teardown() {}
};

// Abstract base class for all tests
class TestBase {
public:
    virtual ~TestBase() = default;
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void runTest() = 0;
    
    TestResult getResult() const { return result; }
    double getDuration() const { return duration; }
    std::string getName() const { return name; }
    
protected:
    void setName(const std::string& testName) { name = testName; }
    void setResult(TestResult res) { result = res; }
    void setDuration(double dur) { duration = dur; }
    
private:
    std::string name;
    TestResult result = TestResult::ERROR;
    double duration = 0.0;
};

// Test suite for grouping related tests
class TestSuite {
public:
    TestSuite(const std::string& name, std::function<void()> setup = nullptr,
              std::function<void()> teardown = nullptr)
        : suiteName(name), setup(setup), teardown(teardown) {}
    
    void addTest(std::unique_ptr<TestBase> test);
    void run();
    std::string getName() const { return suiteName; }
    
private:
    std::string suiteName;
    std::vector<std::unique_ptr<TestBase>> tests;
    std::function<void()> setup;
    std::function<void()> teardown;
    TestStats stats;
};

// Test registry for automatic test discovery
class TestRegistry {
public:
    static TestRegistry& getInstance();
    
    void registerTest(std::unique_ptr<TestBase> test);
    void registerSuite(std::unique_ptr<TestSuite> suite);
    void runAllTests();
    
    TestStats getStats() const { return stats; }
    void printReport(std::ostream& out = std::cout) const;
    
private:
    TestRegistry() = default;
    std::vector<std::unique_ptr<TestBase>> tests;
    std::vector<std::unique_ptr<TestSuite>> suites;
    TestStats stats;
};

// Test runner for orchestrating test execution
class TestRunner {
public:
    static TestRunner& getInstance();
    
    void runTests();
    void runTestSuite(const TestSuite& suite);
    void runIndividualTest(const TestBase& test);
    
    void setVerbose(bool verbose) { this->verbose = verbose; }
    void setShowProgress(bool progress) { this->showProgress = progress; }
    
private:
    TestRunner() : verbose(false), showProgress(true) {}
    bool verbose;
    bool showProgress;
};

// Assert framework implementation
class AssertFramework {
public:
    static void init();
    
    // Basic assertion macros
    template<typename T>
    static void assert(const std::string& message, T condition, const std::string& file = "", int line = 0);
    
    template<typename T, typename U>
    static void assertEq(const std::string& message, T actual, U expected, 
                         const std::string& file = "", int line = 0);
    
    template<typename T, typename U>
    static void assertNE(const std::string& message, T actual, U expected,
                         const std::string& file = "", int line = 0);
    
    // Comparison assertions
    template<typename T, typename U>
    static void assertLT(const std::string& message, T actual, U expected,
                         const std::string& file = "", int line = 0);
    
    template<typename T, typename U>
    static void assertGT(const std::string& message, T actual, U expected,
                         const std::string& file = "", int line = 0);
    
    template<typename T, typename U>
    static void assertLE(const std::string& message, T actual, U expected,
                         const std::string& file = "", int line = 0);
    
    template<typename T, typename U>
    static void assertGE(const std::string& message, T actual, U expected,
                         const std::string& file = "", int line = 0);
    
    // Boolean assertions
    static void assertTrue(const std::string& message, bool condition,
                           const std::string& file = "", int line = 0);
    
    static void assertFalse(const std::string& message, bool condition,
                            const std::string& file = "", int line = 0);
    
    // Pointer assertions
    template<typename T>
    static void assertNull(const std::string& message, T* ptr,
                           const std::string& file = "", int line = 0);
    
    template<typename T>
    static void assertNotNull(const std::string& message, T* ptr,
                              const std::string& file = "", int line = 0);
    
    // Exception assertions
    static void assertThrow(const std::string& message,
                            std::function<void()> testFunc,
                            const std::string& file = "", int line = 0);
    
    static void assertNoThrow(const std::string& message,
                              std::function<void()> testFunc,
                              const std::string& file = "", int line = 0);
    
    // Formatted output
    static std::string formatMessage(const std::string& message,
                                     const std::string& file = "",
                                     int line = 0);
    
    // Color output utilities
    static std::string colorize(const std::string& text, const std::string& color);
};

// Global assert framework instance
extern AssertFramework assertFramework;

// Test utility classes
class TestTimer {
public:
    TestTimer() : start(std::chrono::high_resolution_clock::now()) {}
    ~TestTimer() = default;
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count() / 1000.0;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start;
};

class TestMemoryTracker {
public:
    static TestMemoryTracker& getInstance();
    
    void startTracking();
    void stopTracking();
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    size_t getCurrentMemoryUsage() const;
    
private:
    TestMemoryTracker() = default;
    size_t peakMemoryUsage = 0;
    bool tracking = false;
};

class TestResourceMonitor {
public:
    struct ResourceUsage {
        double cpuTime = 0.0;
        size_t memoryUsage = 0;
        double wallTime = 0.0;
    };
    
    static TestResourceMonitor& getInstance();
    
    ResourceUsage measure(std::function<void()> testFunc);
    
private:
    TestResourceMonitor() = default;
};

// Parameterized test support
template<typename... Args>
class ParameterizedTestBase : public TestBase {
public:
    virtual void runTestWithParams(Args... args) = 0;
    
    void runTest() override {
        // Default implementation - should be overridden
        runTestWithParams();
    }
};

// Test macros for easy test creation
#define NLM_TEST_CASE(name, testClass) \
    class name##Test : public nlm::test::TestBase { \
    public: \
        name##Test() { setName(#name); } \
        void runTest() override { testClass().runTest(); } \
    }; \
    namespace { \
        struct name##TestRegistrar { \
            name##TestRegistrar() { nlm::test::TestRegistry::getInstance().registerTest( \
                std::make_unique<name##Test>()); } \
        }; \
        name##TestRegistrar name##TestInstance; \
    }

#define NLM_TEST_SUITE(name, setup, teardown) \
    class name##Suite : public nlm::test::TestSuite { \
    public: \
        name##Suite() : TestSuite(#name, setup, teardown) {} \
    }; \
    namespace { \
        struct name##SuiteRegistrar { \
            name##SuiteRegistrar() { nlm::test::TestRegistry::getInstance().registerSuite( \
                std::make_unique<name##Suite>()); } \
        }; \
        name##SuiteRegistrar name##SuiteInstance; \
    }

#define NLM_SETUP_TEST_CASE(name) \
    class name##SetupTest : public nlm::test::TestBase { \
    public: \
        name##SetupTest() { setName(#name " setup"); } \
        void setup() override { setup_##name(); } \
        void runTest() override { /* Setup test case implementation */ } \
    }; \
    namespace { \
        struct name##SetupTestRegistrar { \
            name##SetupTestRegistrar() { nlm::test::TestRegistry::getInstance().registerTest( \
                std::make_unique<name##SetupTest>()); } \
        }; \
        name##SetupTestRegistrar name##SetupTestInstance; \
    }

// Assertion macros
#define NLM_ASSERT(condition, message) \
    nlm::test::assertFramework.assert(message, condition, __FILE__, __LINE__)

#define NLM_ASSERT_EQ(actual, expected, message) \
    nlm::test::assertFramework.assertEq(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_NE(actual, expected, message) \
    nlm::test::assertFramework.assertNE(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_LT(actual, expected, message) \
    nlm::test::assertFramework.assertLT(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_GT(actual, expected, message) \
    nlm::test::assertFramework.assertGT(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_LE(actual, expected, message) \
    nlm::test::assertFramework.assertLE(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_GE(actual, expected, message) \
    nlm::test::assertFramework.assertGE(message, actual, expected, __FILE__, __LINE__)

#define NLM_ASSERT_TRUE(condition, message) \
    nlm::test::assertFramework.assertTrue(message, condition, __FILE__, __LINE__)

#define NLM_ASSERT_FALSE(condition, message) \
    nlm::test::assertFramework.assertFalse(message, condition, __FILE__, __LINE__)

#define NLM_ASSERT_NULL(ptr, message) \
    nlm::test::assertFramework.assertNull(message, ptr, __FILE__, __LINE__)

#define NLM_ASSERT_NOT_NULL(ptr, message) \
    nlm::test::assertFramework.assertNotNull(message, ptr, __FILE__, __LINE__)

#define NLM_ASSERT_THROW(testFunc, message) \
    nlm::test::assertFramework.assertThrow(message, testFunc, __FILE__, __LINE__)

#define NLM_ASSERT_NO_THROW(testFunc, message) \
    nlm::test::assertFramework.assertNoThrow(message, testFunc, __FILE__, __LINE__)

// Test utility macros
#define NLM_REGISTER_TEST_CASE(name, testClass) \
    NLM_TEST_CASE(name, testClass)

#define NLM_REGISTER_TEST_SUITE(name, setup, teardown) \
    NLM_TEST_SUITE(name, setup, teardown)

// Legacy compatibility macros
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

#define NLM_ASSERT_LEGACY(condition) \
    assert(condition)
}