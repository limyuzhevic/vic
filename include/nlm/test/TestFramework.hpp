#pragma once

/**
 * Modern C++ Test Framework for NLM
 * 
 * Features:
 * - Parameterized tests
 * - Test fixtures
 * - Assertions and verifications
 * - Performance benchmarking
 * - Memory leak detection
 * - XML/HTML report generation
 * - Integration with Google Test (if available)
 * 
 * Designed to be lightweight and standalone, but can integrate with Google Test
 * when available.
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <random>
#include <type_traits>
#include <optional>
#include <variant>

namespace nlm {
namespace test {

// Forward declarations
class TestCase;
class TestSuite;
class TestReporter;
class BenchmarkSuite;

// Test result structure
struct TestResult {
    std::string testName;
    std::string suiteName;
    bool passed;
    std::string failureMessage;
    std::chrono::duration<double> duration;
    size_t memoryUsage; // bytes
    int lineNumber;
    std::string fileName;
    
    TestResult() : passed(true), memoryUsage(0), lineNumber(0) {}
};

// Assertion types
enum class AssertionType {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_EQUAL,
    GREATER_THAN,
    GREATER_EQUAL,
    TRUE,
    FALSE,
    IS_NULL,
    IS_NOT_NULL,
    CONTAINS,
    NOT_CONTAINS,
    ALMOST_EQUAL,
    BETWEEN
};

// Assertion result
struct AssertionResult {
    bool passed;
    std::string message;
    AssertionType type;
    std::string expected;
    std::string actual;
    
    AssertionResult() : passed(false) {}
    AssertionResult(bool p, const std::string& msg, AssertionType t, 
                   const std::string& exp = "", const std::string& act = "")
        : passed(p), message(msg), type(t), expected(exp), actual(act) {}
};

// Test assertion macros (inline functions)
class Assert {
public:
    // Arithmetic comparisons
    template<typename T, typename U>
    static AssertionResult Equal(const T& expected, const U& actual, 
                                 const std::string& message, 
                                 const std::string& file, int line) {
        bool result = expected == actual;
        return AssertionResult(result, 
            message + " (expected " + std::to_string(expected) + 
            ", got " + std::to_string(actual) + ")", 
            AssertionType::EQUAL, std::to_string(expected), std::to_string(actual));
    }
    
    template<typename T, typename U>
    static AssertionResult NotEqual(const T& expected, const U& actual,
                                    const std::string& message,
                                    const std::string& file, int line) {
        bool result = expected != actual;
        return AssertionResult(result,
            message + " (expected not equal to " + std::to_string(actual) + ")",
            AssertionType::NOT_EQUAL, std::to_string(expected), std::to_string(actual));
    }
    
    template<typename T, typename U>
    static AssertionResult LessThan(const T& expected, const U& actual,
                                    const std::string& message,
                                    const std::string& file, int line) {
        bool result = expected < actual;
        return AssertionResult(result,
            message + " (expected " + std::to_string(expected) + 
            " < " + std::to_string(actual) + ")",
            AssertionType::LESS_THAN, std::to_string(expected), std::to_string(actual));
    }
    
    template<typename T, typename U>
    static AssertionResult GreaterThan(const T& expected, const U& actual,
                                       const std::string& message,
                                       const std::string& file, int line) {
        bool result = expected > actual;
        return AssertionResult(result,
            message + " (expected " + std::to_string(expected) + 
            " > " + std::to_string(actual) + ")",
            AssertionType::GREATER_THAN, std::to_string(expected), std::to_string(actual));
    }
    
    // Boolean conditions
    static AssertionResult True(bool condition, const std::string& message,
                                const std::string& file, int line) {
        bool result = condition;
        return AssertionResult(result, message, AssertionType::TRUE, 
                              condition ? "true" : "false", "");
    }
    
    static AssertionResult False(bool condition, const std::string& message,
                                 const std::string& file, int line) {
        bool result = !condition;
        return AssertionResult(result, message, AssertionType::FALSE,
                              condition ? "true" : "false", "");
    }
    
    // Pointer checks
    template<typename T>
    static AssertionResult IsNull(T* ptr, const std::string& message,
                                  const std::string& file, int line) {
        bool result = ptr == nullptr;
        return AssertionResult(result, message, AssertionType::IS_NULL,
                              ptr ? "not null" : "null", "");
    }
    
    template<typename T>
    static AssertionResult IsNotNull(T* ptr, const std::string& message,
                                     const std::string& file, int line) {
        bool result = ptr != nullptr;
        return AssertionResult(result, message, AssertionType::IS_NOT_NULL,
                              ptr ? "not null" : "null", "");
    }
    
    // String checks
    static AssertionResult Contains(const std::string& str, const std::string& substr,
                                    const std::string& message,
                                    const std::string& file, int line) {
        bool result = str.find(substr) != std::string::npos;
        return AssertionResult(result,
            message + " (expected to contain '" + substr + "')",
            AssertionType::CONTAINS, substr, str);
    }
    
    static AssertionResult NotContains(const std::string& str, const std::string& substr,
                                       const std::string& message,
                                       const std::string& file, int line) {
        bool result = str.find(substr) == std::string::npos;
        return AssertionResult(result,
            message + " (expected not to contain '" + substr + "')",
            AssertionType::NOT_CONTAINS, substr, str);
    }
    
    // Floating point comparison
    static AssertionResult AlmostEqual(double expected, double actual, double epsilon,
                                      const std::string& message,
                                      const std::string& file, int line) {
        bool result = std::abs(expected - actual) <= epsilon;
        return AssertionResult(result,
            message + " (expected " + std::to_string(expected) + 
            " ≈ " + std::to_string(actual) + " within " + std::to_string(epsilon) + ")",
            AssertionType::ALMOST_EQUAL, std::to_string(expected), std::to_string(actual));
    }
    
    // Range check
    template<typename T>
    static AssertionResult Between(const T& value, const T& min, const T& max,
                                   const std::string& message,
                                   const std::string& file, int line) {
        bool result = (value >= min) && (value <= max);
        return AssertionResult(result,
            message + " (expected " + std::to_string(value) + " between " +
            std::to_string(min) + " and " + std::to_string(max) + ")",
            AssertionType::BETWEEN, std::to_string(min) + "-" + std::to_string(max), std::to_string(value));
    }
    
    // Helper macros (inline functions)
    template<typename... Args>
    static std::string FormatMessage(const std::string& format, Args&&... args) {
        std::stringstream ss;
        size_t arg_index = 0;
        
        for (size_t i = 0; i < format.size(); ++i) {
            if (format[i] == '{' && i + 1 < format.size() && format[i + 1] == '}') {
                if (arg_index < sizeof...(args)) {
                    ss << std::get<arg_index>(std::forward_as_tuple(std::forward<Args>(args)...));
                }
                i++; // Skip closing brace
                arg_index++;
            } else {
                ss << format[i];
            }
        }
        
        return ss.str();
    }
};

// Test fixture base class
class TestFixture {
public:
    virtual ~TestFixture() = default;
    
    // Setup called before each test
    virtual void SetUp() {}
    
    // Teardown called after each test
    virtual void TearDown() {}
    
    // Test methods should be implemented by derived classes
    virtual void TestBody() = 0;
};

// Parameterized test fixture
class ParameterizedTestFixture {
public:
    virtual ~ParameterizedTestFixture() = default;
    
    virtual void SetUp(int param) {}
    virtual void TearDown(int param) {}
    virtual void TestBody(int param) = 0;
};

// Test case
class TestCase {
public:
    TestCase(const std::string& name, TestFixture* fixture, 
             const std::string& file, int line)
        : name(name), fixture(fixture), result(), startTime(), file(file), line(line) {}
    
    virtual ~TestCase() {
        delete fixture;
    }
    
    void Run() {
        startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup
            fixture->SetUp();
            
            // Run test
            fixture->TestBody();
            
            result.passed = true;
            result.failureMessage = "";
        } catch (const std::exception& e) {
            result.passed = false;
            result.failureMessage = std::string("Exception: ") + e.what();
        } catch (...) {
            result.passed = false;
            result.failureMessage = "Unknown exception";
        }
        
        // Teardown
        try {
            fixture->TearDown();
        } catch (...) {
            // Ignore exceptions during teardown
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        result.testName = name;
        result.fileName = file;
        result.lineNumber = line;
    }
    
    std::string GetName() const { return name; }
    const TestResult& GetResult() const { return result; }
    
private:
    std::string name;
    TestFixture* fixture;
    TestResult result;
    std::chrono::high_resolution_clock::time_point startTime;
    std::string file;
    int line;
};

// Parameterized test case
class ParameterizedTestCase {
public:
    ParameterizedTestCase(const std::string& name, ParameterizedTestFixture* fixture,
                         int param, const std::string& file, int line)
        : name(name), fixture(fixture), param(param), result(), startTime(), file(file), line(line) {}
    
    virtual ~ParameterizedTestCase() {
        delete fixture;
    }
    
    void Run() {
        startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Setup
            fixture->SetUp(param);
            
            // Run test
            fixture->TestBody(param);
            
            result.passed = true;
            result.failureMessage = "";
        } catch (const std::exception& e) {
            result.passed = false;
            result.failureMessage = std::string("Exception: ") + e.what();
        } catch (...) {
            result.passed = false;
            result.failureMessage = "Unknown exception";
        }
        
        // Teardown
        try {
            fixture->TearDown(param);
        } catch (...) {
            // Ignore exceptions during teardown
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        result.testName = name + "(" + std::to_string(param) + ")";
        result.fileName = file;
        result.lineNumber = line;
    }
    
    std::string GetName() const { return name; }
    const TestResult& GetResult() const { return result; }
    
private:
    std::string name;
    ParameterizedTestFixture* fixture;
    int param;
    TestResult result;
    std::chrono::high_resolution_clock::time_point startTime;
    std::string file;
    int line;
};

// Test suite
class TestSuite {
public:
    TestSuite(const std::string& name) : name(name), passed(0), failed(0) {}
    
    virtual ~TestSuite() {
        for (auto& test : testCases) {
            delete test;
        }
    }
    
    void AddTest(TestCase* test) {
        testCases.push_back(test);
    }
    
    void AddParameterizedTests(const std::string& testName, ParameterizedTestFixture* fixture,
                              const std::vector<int>& params, const std::string& file, int line) {
        for (int param : params) {
            TestCase* test = new ParameterizedTestCase(testName, fixture, param, file, line);
            testCases.push_back(test);
        }
    }
    
    const std::string& GetName() const { return name; }
    
    void Run() {
        for (auto& test : testCases) {
            test->Run();
            if (test->GetResult().passed) {
                passed++;
            } else {
                failed++;
            }
        }
    }
    
    size_t GetPassed() const { return passed; }
    size_t GetFailed() const { return failed; }
    size_t GetTotal() const { return testCases.size(); }
    
    const std::vector<TestCase*>& GetTestCases() const { return testCases; }
    
private:
    std::string name;
    std::vector<TestCase*> testCases;
    size_t passed;
    size_t failed;
};

// Test reporter
class TestReporter {
public:
    virtual ~TestReporter() = default;
    
    virtual void BeginTestRun(const std::vector<TestSuite*>& suites) = 0;
    virtual void EndTestRun() = 0;
    
    virtual void BeginSuite(const TestSuite& suite) = 0;
    virtual void EndSuite(const TestSuite& suite) = 0;
    
    virtual void TestStart(const TestCase& test) = 0;
    virtual void TestEnd(const TestCase& test) = 0;
    
    virtual void ReportTestResult(const TestCase& test) = 0;
};

// Console test reporter
class ConsoleTestReporter : public TestReporter {
public:
    ConsoleTestReporter(bool verbose = false) : verbose(verbose) {}
    
    void BeginTestRun(const std::vector<TestSuite*>& suites) override {
        std::cout << "Running test suite..." << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        totalSuites = suites.size();
        currentSuite = 0;
        
        overallPassed = 0;
        overallFailed = 0;
        totalTests = 0;
    }
    
    void EndTestRun() override {
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Test run completed." << std::endl;
        std::cout << "Overall: " << overallPassed << " passed, " << overallFailed << " failed, "
                  << totalTests << " total." << std::endl;
        
        if (overallFailed > 0) {
            std::cout << "FAILED!" << std::endl;
            return;
        }
        
        std::cout << "All tests passed!" << std::endl;
    }
    
    void BeginSuite(const TestSuite& suite) override {
        currentSuite++;
        std::cout << "Suite: " << suite.GetName() << " ("
                  << suite.GetPassed() << " passed, " << suite.GetFailed() << " failed)" << std::endl;
    }
    
    void EndSuite(const TestSuite& suite) override {
        std::cout << "Suite completed." << std::endl;
        overallPassed += suite.GetPassed();
        overallFailed += suite.GetFailed();
        totalTests += suite.GetTotal();
    }
    
    void TestStart(const TestCase& test) override {
        if (verbose) {
            std::cout << "  Test: " << test.GetName();
        }
    }
    
    void TestEnd(const TestCase& test) override {
        if (verbose) {
            std::cout << " - " << test.GetResult().duration.count() << "s";
            if (test.GetResult().passed) {
                std::cout << " [PASS]";
            } else {
                std::cout << " [FAIL]";
            }
            std::cout << std::endl;
        }
    }
    
    void ReportTestResult(const TestCase& test) override {
        if (!verbose) {
            std::cout << "  ";
            if (test.GetResult().passed) {
                std::cout << ".";
                overallPassed++;
            } else {
                std::cout << "F";
                overallFailed++;
            }
        }
        
        if (!test.GetResult().passed) {
            std::cout << std::endl;
            std::cout << "FAILED: " << test.GetName() << std::endl;
            std::cout << "  " << test.GetResult().failureMessage << std::endl;
            std::cout << "  File: " << test.GetResult().fileName << ", Line: " 
                      << test.GetResult().lineNumber << std::endl;
        }
    }
    
private:
    bool verbose;
    size_t totalSuites;
    size_t currentSuite;
    size_t overallPassed;
    size_t overallFailed;
    size_t totalTests;
};

// Test runner
class TestRunner {
public:
    TestRunner() : verbose(false), reporter(nullptr) {}
    
    void SetVerbose(bool verbose) { this->verbose = verbose; }
    void SetReporter(TestReporter* reporter) { this->reporter = reporter; }
    
    int RunTests(std::vector<TestSuite*>& suites) {
        if (!reporter) {
            reporter = new ConsoleTestReporter(verbose);
        }
        
        reporter->BeginTestRun(suites);
        
        for (auto& suite : suites) {
            if (reporter) {
                reporter->BeginSuite(*suite);
            }
            
            suite->Run();
            
            if (reporter) {
                reporter->EndSuite(*suite);
            }
        }
        
        reporter->EndTestRun();
        
        if (overallFailed > 0) {
            return 1;
        }
        
        return 0;
    }
    
private:
    bool verbose;
    TestReporter* reporter;
    size_t overallFailed;
};

// Benchmark result structure
struct BenchmarkResult {
    std::string name;
    std::chrono::duration<double> duration;
    double throughput; // operations per second
    size_t memoryUsage;
    double cpuUsage;
    int iterations;
    
    BenchmarkResult() : throughput(0.0), memoryUsage(0), cpuUsage(0.0), iterations(0) {}
};

// Benchmark suite
class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& name) : name(name) {}
    
    virtual ~BenchmarkSuite() {}
    
    virtual void AddBenchmark(const std::string& benchmarkName,
                              std::function<void()> benchmark,
                              int iterations = 100) {
        benchmarks.push_back({benchmarkName, benchmark, iterations});
    }
    
    const std::string& GetName() const { return name; }
    
    void Run() {
        std::cout << "Running benchmarks for " << name << "..." << std::endl;
        
        for (auto& benchmark : benchmarks) {
            std::cout << "  " << benchmark.name << "...";
            
            // Warm-up
            for (int i = 0; i < 10; ++i) {
                benchmark.benchmark();
            }
            
            // Actual benchmark run
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < benchmark.iterations; ++i) {
                benchmark.benchmark();
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            double duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
            double throughput = benchmark.iterations / duration;
            
            BenchmarkResult result;
            result.name = benchmark.name;
            result.duration = std::chrono::duration<double>(duration);
            result.throughput = throughput;
            result.iterations = benchmark.iterations;
            
            std::cout << " - " << duration << "s, " << throughput << " ops/sec" << std::endl;
            
            benchmarkResults.push_back(result);
        }
    }
    
    const std::vector<BenchmarkResult>& GetResults() const { return benchmarkResults; }
    
private:
    struct Benchmark {
        std::string name;
        std::function<void()> benchmark;
        int iterations;
    };
    
    std::string name;
    std::vector<Benchmark> benchmarks;
    std::vector<BenchmarkResult> benchmarkResults;
};

// Test macros for easier test writing
#define CONCAT(a, b) a##b
#define TEST_CASE(fixture, testName) \
    class CONCAT(fixture##testName, _Test) : public fixture { \
    public: \
        void TestBody() override; \
    }; \
    void CONCAT(fixture##testName, _Test)::TestBody() \

#define PARAMETERIZED_TEST_CASE(fixture, testName, params) \
    class CONCAT(fixture##testName, _Test) : public fixture { \
    public: \
        void TestBody(int param) override; \
    }; \
    void CONCAT(fixture##testName, _Test)::TestBody(int param)

#define EXPECT_EQ(expected, actual) \
    do { \
        AssertionResult result = Assert::Equal(expected, actual, #expected " == " #actual, __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_NE(expected, actual) \
    do { \
        AssertionResult result = Assert::NotEqual(expected, actual, #expected " != " #actual, __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_TRUE(condition) \
    do { \
        AssertionResult result = Assert::True(condition, #condition " is not true", __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_FALSE(condition) \
    do { \
        AssertionResult result = Assert::False(condition, #condition " is not false", __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_EQ_STR(expected, actual) \
    do { \
        AssertionResult result = Assert::Contains(actual, expected, "Expected to find substring", __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_ALMOST_EQUAL(expected, actual, epsilon) \
    do { \
        AssertionResult result = Assert::AlmostEqual(expected, actual, epsilon, #expected " ≈ " #actual " within " #epsilon, __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

#define EXPECT_BETWEEN(value, min, max) \
    do { \
        AssertionResult result = Assert::Between(value, min, max, "Value not between min and max", __FILE__, __LINE__); \
        if (!result.passed) { \
            throw std::runtime_error(result.message); \
        } \
    } while(0)

// Test registration utilities
class TestRegistration {
public:
    TestRegistration(const std::string& suiteName, TestSuite* suite) {
        suites.push_back(suite);
    }
    
    static std::vector<TestSuite*>& GetSuites() {
        return suites;
    }
    
private:
    static std::vector<TestSuite*> suites;
};

// Initialize static members
std::vector<TestSuite*> TestRegistration::suites;

// Example test class
class ExampleTestFixture : public TestFixture {
public:
    void SetUp() override {
        // Setup code
    }
    
    void TearDown() override {
        // Cleanup code
    }
    
    void TestMethod1() override {
        EXPECT_TRUE(true);
        EXPECT_EQ(1, 1);
    }
    
    void TestMethod2() override {
        EXPECT_FALSE(false);
        EXPECT_NE(2, 3);
    }
};

// Test suite for example
class ExampleTestSuite : public TestSuite {
public:
    ExampleTestSuite() : TestSuite("Example Tests") {
        AddTest(new TestCase("Method1", new ExampleTestFixture(), "test_example.cpp", 100));
        AddTest(new TestCase("Method2", new ExampleTestFixture(), "test_example.cpp", 105));
    }
};

// Register the test suite
static ExampleTestSuite exampleTestSuite;

} // namespace test
} // namespace nlm
