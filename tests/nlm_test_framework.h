// NLM Test Framework - Modern C++ Test Infrastructure
// Provides sophisticated test runner with multiple categories, parameterized tests, and detailed reporting

#ifndef NLM_TEST_FRAMEWORK_H
#define NLM_TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <type_traits>

namespace nlm {
namespace test {

// Test categories
enum class TestCategory {
    Unit,        // Individual component tests
    Integration, // System/component interactions
    Performance, // Performance and timing tests
    Stress,      // Stress/overload tests
    Compatibility // Backward compatibility tests
};

// Assertion result structure
struct AssertionResult {
    std::string testName;
    std::string expression;
    std::string message;
    bool passed;
    std::string file;
    int line;
    double elapsedTime; // Time taken for this assertion
};

// Test case statistics
struct TestStatistics {
    std::string name;
    std::string category;
    bool passed;
    int totalAssertions;
    int failedAssertions;
    double totalTime;
    double cpuTime;
    int iteration;
};

// Fixture base class for test setup/teardown
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void setUpParameters() {}
};

// Parameter class for parameterized tests
class TestParameter {
public:
    std::string name;
    std::string value;
    
    TestParameter(const std::string& n, const std::string& v) : name(n), value(v) {}
    
    template<typename T>
    TestParameter(const std::string& n, const T& v) {
        name = n;
        std::stringstream ss;
        ss << v;
        value = ss.str();
    }
};

// Modern assertion macros with context and failure reporting
#define NLM_TEST_ASSERT(expr, msg) \
    nlm::test::TestAssertion(__func__, #expr, msg, expr, __FILE__, __LINE__)

#define NLM_TEST_CHECK(expr) \
    nlm::test::TestAssertion(__func__, #expr, "", expr, __FILE__, __LINE__)

#define NLM_TEST_FAIL(msg) \
    nlm::test::TestAssertion(__func__, "", msg, false, __FILE__, __LINE__)

#define NLM_TEST_REQUIRE(expr, msg) \
    nlm::test::TestAssertion(__func__, #expr, msg, expr, __FILE__, __LINE__)

// Test assertion class
class TestAssertion {
public:
    TestAssertion(const std::string& testName, const std::string& expr, 
                  const std::string& msg, bool condition, 
                  const std::string& file, int line)
        : testName(testName), expression(expr), assertionMessage(msg), 
          passed(condition), fileName(file), lineNumber(line), 
          startTime(std::chrono::high_resolution_clock::now()) {}
    
    ~TestAssertion() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        elapsedTime = duration.count() / 1000.0; // Convert to milliseconds
        
        if (!passed) {
            recordFailure();
        }
    }
    
    explicit operator bool() const { return passed; }
    
    const std::string& getTestName() const { return testName; }
    const std::string& getExpression() const { return expression; }
    const std::string& getMessage() const { return assertionMessage; }
    bool isPassed() const { return passed; }
    const std::string& getFile() const { return fileName; }
    int getLine() const { return lineNumber; }
    double getElapsedTime() const { return elapsedTime; }
    
    std::string getFullLocation() const {
        std::stringstream ss;
        ss << fileName << ":" << lineNumber;
        return ss.str();
    }
    
    std::string getFormattedAssertion() const {
        std::stringstream ss;
        ss << "  " << expression;
        if (!assertionMessage.empty()) {
            ss << " (" << assertionMessage << ")";
        }
        return ss.str();
    }
    
private:
    void recordFailure() {
        FailureInfo failure;
        failure.testName = testName;
        failure.expression = expression;
        failure.message = assertionMessage;
        failure.location = getFullLocation();
        failure.time = elapsedTime;
        failures.push_back(failure);
    }
    
    struct FailureInfo {
        std::string testName;
        std::string expression;
        std::string message;
        std::string location;
        double time;
    };
    
    static inline std::vector<FailureInfo> failures;
    
    std::string testName;
    std::string expression;
    std::string assertionMessage;
    bool passed;
    std::string fileName;
    int lineNumber;
    double elapsedTime;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Parameterized test case template
class ParameterizedTestCase {
public:
    virtual ~ParameterizedTestCase() = default;
    virtual std::vector<TestParameter> getParameters() const = 0;
    virtual void run() = 0;
    virtual TestCategory getCategory() const = 0;
    virtual std::string getName() const = 0;
};

// Generic parameterized test implementation
class ParameterizedTest : public ParameterizedTestCase {
public:
    template<typename TestClass, typename TestMethod>
    ParameterizedTest(TestMethod method, TestCategory category, const std::string& name) 
        : testMethod(method), category(category), testName(name) {}
    
    template<typename T, typename TestMethod>
    ParameterizedTest(T* instance, TestMethod method, TestCategory category, 
                      const std::string& name, TestFixture* fixture)
        : testMethod(method), category(category), testName(name), instance(instance), fixture(fixture) {}
    
    std::vector<TestParameter> getParameters() const override {
        return std::vector<TestParameter>(); // Override for parameterized tests
    }
    
    void run() override {
        if (fixture) {
            fixture->setUp();
        }
        (instance->*testMethod)();
        if (fixture) {
            fixture->tearDown();
        }
    }
    
    TestCategory getCategory() const override { return category; }
    std::string getName() const override { return testName; }
    
private:
    std::function<void()> testMethod;
    TestCategory category;
    std::string testName;
    TestClass* instance = nullptr;
    TestFixture* fixture = nullptr;
};

// Global test registry and runner
class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    // Register a simple test
    void registerTest(const std::string& name, std::function<void()> testFunc,
                     TestCategory category = TestCategory::Unit,
                     const std::string& description = "") {
        TestInfo info;
        info.name = name;
        info.category = category;
        info.description = description;
        info.testFunc = testFunc;
        info.isParameterized = false;
        
        tests[name] = info;
    }
    
    // Register a parameterized test
    void registerParameterizedTest(const std::string& name,
                                  std::function<void(int)> testFunc,
                                  TestCategory category = TestCategory::Unit,
                                  const std::string& description = "",
                                  int iterationCount = 1) {
        TestInfo info;
        info.name = name;
        info.category = category;
        info.description = description;
        info.iterationCount = iterationCount;
        info.parameterizedFunc = testFunc;
        info.isParameterized = true;
        
        tests[name] = info;
    }
    
    // Register a fixture-based test
    void registerFixtureTest(const std::string& name,
                            TestFixture* fixture,
                            std::function<void(TestFixture*)> testFunc,
                            TestCategory category = TestCategory::Unit,
                            const std::string& description = "") {
        TestInfo info;
        info.name = name;
        info.category = category;
        info.description = description;
        info.fixture = fixture;
        info.fixtureFunc = testFunc;
        info.isFixture = true;
        
        tests[name] = info;
    }
    
    // Get all registered tests
    const std::map<std::string, TestInfo>& getTests() const { return tests; }
    
    // Clear all tests (for testing only)
    void clear() { tests.clear(); }
    
    // Get failures from all assertions
    static const std::vector<typename TestAssertion::FailureInfo>& getAllFailures() {
        return TestAssertion::failures;
    }
    
    // Clear failures (for testing only)
    static void clearFailures() {
        TestAssertion::failures.clear();
    }
    
    // Run all tests
    bool runAll(const std::string& filter = "", bool verbose = false) {
        bool allPassed = true;
        int totalTests = 0;
        int passedTests = 0;
        double totalStartTime = getCurrentTime();
        
        std::cout << "=== NLM Test Runner ===" << std::endl;
        
        for (const auto& pair : tests) {
            const TestInfo& info = pair.second;
            
            // Skip if filtering and doesn't match
            if (!filter.empty() && info.name.find(filter) == std::string::npos) {
                continue;
            }
            
            // Categorize tests
            if (info.isParameterized) {
                runParameterizedTest(info, verbose);
            } else if (info.isFixture) {
                runFixtureTest(info, verbose);
            } else {
                runSimpleTest(info, verbose);
            }
            
            totalTests++;
            if (info.passed) passedTests++;
        }
        
        double totalEndTime = getCurrentTime();
        double totalElapsed = totalEndTime - totalStartTime;
        
        // Print summary
        printSummary(totalTests, passedTests, allPassed, totalElapsed, verbose);
        
        return allPassed;
    }
    
private:
    struct TestInfo {
        std::string name;
        std::string description;
        TestCategory category;
        std::function<void()> testFunc;
        std::function<void(int)> parameterizedFunc;
        TestFixture* fixture = nullptr;
        std::function<void(TestFixture*)> fixtureFunc;
        bool isParameterized = false;
        bool isFixture = false;
        int iterationCount = 1;
        bool passed = false;
        double executionTime = 0.0;
    };
    
    TestRegistry() = default;
    TestRegistry(const TestRegistry&) = delete;
    TestRegistry& operator=(const TestRegistry&) = delete;
    
    void runSimpleTest(const TestInfo& info, bool verbose) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (verbose) {
            std::cout << "Running " << info.name << "..." << std::endl;
        }
        
        // Clear previous failures for this test
        std::vector<TestAssertion::FailureInfo> testFailures;
        
        try {
            info.testFunc();
        } catch (const std::exception& e) {
            recordFailure(info.name, "Exception: " + std::string(e.what()), "");
        } catch (...) {
            recordFailure(info.name, "Unknown exception", "");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        info.executionTime = duration.count() / 1000.0;
        
        info.passed = (TestAssertion::failures.size() == 0); // This is approximate
        
        if (verbose) {
            std::cout << (info.passed ? "  PASSED" : "  FAILED") << " ("
                     << std::fixed << std::setprecision(3) << info.executionTime << "ms)" << std::endl;
        }
    }
    
    void runParameterizedTest(const TestInfo& info, bool verbose) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool testPassed = true;
        
        for (int i = 0; i < info.iterationCount; ++i) {
            if (verbose) {
                std::cout << "Running " << info.name << " [iteration " << i << "]..." << std::endl;
            }
            
            try {
                info.parameterizedFunc(i);
            } catch (const std::exception& e) {
                recordFailure(info.name, "Exception: " + std::string(e.what()), "");
                testPassed = false;
            } catch (...) {
                recordFailure(info.name, "Unknown exception", "");
                testPassed = false;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        info.executionTime = duration.count() / 1000.0;
        
        info.passed = testPassed && (TestAssertion::failures.size() == 0);
        
        if (verbose) {
            std::cout << (info.passed ? "  PASSED" : "  FAILED") << " ("
                     << std::fixed << std::setprecision(3) << info.executionTime << "ms, "
                     << info.iterationCount << " iterations)" << std::endl;
        }
    }
    
    void runFixtureTest(const TestInfo& info, bool verbose) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (verbose) {
            std::cout << "Running " << info.name << " with fixture..." << std::endl;
        }
        
        // Clear previous failures for this test
        std::vector<TestAssertion::FailureInfo> testFailures;
        
        try {
            if (info.fixture) {
                info.fixture->setUpParameters();
                info.fixture->setUp();
            }
            info.fixtureFunc(info.fixture);
            if (info.fixture) {
                info.fixture->tearDown();
            }
        } catch (const std::exception& e) {
            recordFailure(info.name, "Exception: " + std::string(e.what()), "");
        } catch (...) {
            recordFailure(info.name, "Unknown exception", "");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        info.executionTime = duration.count() / 1000.0;
        
        info.passed = (TestAssertion::failures.size() == 0);
        
        if (verbose) {
            std::cout << (info.passed ? "  PASSED" : "  FAILED") << " ("
                     << std::fixed << std::setprecision(3) << info.executionTime << "ms)" << std::endl;
        }
    }
    
    void recordFailure(const std::string& testName, const std::string& expression, 
                      const std::string& message) {
        TestAssertion::FailureInfo failure;
        failure.testName = testName;
        failure.expression = expression;
        failure.message = message;
        failure.location = "";
        failure.time = 0.0;
        TestAssertion::failures.push_back(failure);
    }
    
    void printSummary(int totalTests, int passedTests, bool allPassed, 
                     double totalElapsed, bool verbose) {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Total time: " << std::fixed << std::setprecision(3) 
                 << totalElapsed << "ms" << std::endl;
        
        if (!TestAssertion::failures.empty()) {
            std::cout << std::endl;
            std::cout << "=== Failures ===" << std::endl;
            for (const auto& failure : TestAssertion::failures) {
                std::cout << failure.testName << ": " << failure.expression;
                if (!failure.message.empty()) {
                    std::cout << " - " << failure.message;
                }
                std::cout << std::endl;
            }
        }
        
        if (allPassed) {
            std::cout << std::endl;
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        }
    }
    
    double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto epoch = now.time_since_epoch();
        return std::chrono::duration<double>(epoch).count() * 1000.0; // milliseconds
    }
    
    std::map<std::string, TestInfo> tests;
};

// Convenience functions for backward compatibility
inline void testPassed(const std::string& testName, const std::string& message = "") {
    std::cout << "  " << testName << ": " << (message.empty() ? "passed" : message) << std::endl;
}

inline void testFailed(const std::string& testName, const std::string& errorMessage = "") {
    std::cout << "  " << testName << ": " << (errorMessage.empty() ? "failed" : errorMessage) << std::endl;
}

// Statistics tracking for performance tests
class PerformanceStatistics {
public:
    struct Metric {
        std::string name;
        double value;
        std::string unit;
        std::string description;
    };
    
    void addMetric(const std::string& name, double value, const std::string& unit,
                  const std::string& description = "") {
        Metric metric;
        metric.name = name;
        metric.value = value;
        metric.unit = unit;
        metric.description = description;
        metrics.push_back(metric);
    }
    
    void printReport() const {
        std::cout << "=== Performance Report ===" << std::endl;
        for (const auto& metric : metrics) {
            std::cout << metric.name << ": " << metric.value << " " << metric.unit;
            if (!metric.description.empty()) {
                std::cout << " - " << metric.description;
            }
            std::cout << std::endl;
        }
    }
    
private:
    std::vector<Metric> metrics;
};

// Test macros for parameterized tests
#define NLM_TEST_PARAM(className, testMethod, paramName, paramValue) \
    className::testMethod paramName paramValue

// Simple test runner for backward compatibility
inline bool runAllTests(const std::string& filter = "", bool verbose = false) {
    return TestRegistry::getInstance().runAll(filter, verbose);
}

} // namespace test
} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_H
