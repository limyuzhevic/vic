// TestFramework.hpp - Core testing framework classes
#ifndef TESTFRAMEWORK_HPP
#define TESTFRAMEWORK_HPP

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <variant>

namespace nlm::test {

// Test result enumeration
enum class TestResult {
    PASSED = 0,
    FAILED = 1,
    SKIPPED = 2,
    ERROR = 3
};

// Test info structure
struct TestInfo {
    std::string name;
    std::string category;
    std::string description;
    std::chrono::duration<double> duration;
    TestResult result;
    std::string errorMessage;
    int lineNumber;
    std::string fileName;
    
    TestInfo(const std::string& testName, const std::string& testCategory,
             const std::string& testDesc = "")
        : name(testName), category(testCategory), description(testDesc),
          duration(0), result(TestResult::SKIPPED), lineNumber(0) {}
};

// Parameterization for tests
class TestParameters {
private:
    std::map<std::string, std::variant<int, double, std::string, bool>> params;
    
public:
    template<typename T>
    void add(const std::string& name, const T& value) {
        params[name] = value;
    }
    
    template<typename T>
    T get(const std::string& name) const {
        auto it = params.find(name);
        if (it != params.end()) {
            return std::get<T>(it->second);
        }
        throw std::runtime_error("Parameter not found: " + name);
    }
    
    bool has(const std::string& name) const {
        return params.find(name) != params.end();
    }
    
    const std::map<std::string, std::variant<int, double, std::string, bool>>& getAll() const {
        return params;
    }
};

// Base TestCase class
class TestCase {
protected:
    std::string name;
    std::string category;
    
public:
    TestCase(const std::string& testName, const std::string& testCategory = "General")
        : name(testName), category(testCategory) {}
    
    virtual ~TestCase() = default;
    
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run() = 0;
    
    virtual bool setupTestCase() { return true; }
    virtual bool teardownTestCase() { return true; }
    
    virtual void withParameters(const TestParameters& params) {}
    
    // Assertion helpers
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            throw std::runtime_error("Assertion failed: " + message);
        }
    }
    
    void assertEqual(const std::string& actual, const std::string& expected, 
                     const std::string& message = "") {
        if (actual != expected) {
            throw std::runtime_error("Assertion failed: " + message +
                                   " (expected: '" + expected + "', got: '" + actual + "')");
        }
    }
    
    void assertNotEqual(const std::string& actual, const std::string& expected,
                        const std::string& message = "") {
        if (actual == expected) {
            throw std::runtime_error("Assertion failed: " + message +
                                   " (unexpected: '" + expected + "')");
        }
    }
    
    template<typename T>
    void assertEqual(const T& actual, const T& expected, const std::string& message = "") {
        if (actual != expected) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (expected: " << expected << ", got: " << actual << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    template<typename T>
    void assertNotEqual(const T& actual, const T& expected, const std::string& message = "") {
        if (actual == expected) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (unexpected: " << expected << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    void assertGreater(const double actual, const double expected,
                      const std::string& message = "") {
        if (actual <= expected) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (expected > " << expected << ", got " << actual << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    void assertLess(const double actual, const double expected,
                   const std::string& message = "") {
        if (actual >= expected) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (expected < " << expected << ", got " << actual << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    void assertInRange(const double actual, const double min, const double max,
                      const std::string& message = "") {
        if (actual < min || actual > max) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (expected between " << min << " and " << max
               << ", got " << actual << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    void assertApproximately(const double actual, const double expected,
                           const double tolerance,
                           const std::string& message = "") {
        double diff = std::abs(actual - expected);
        if (diff > tolerance) {
            std::stringstream ss;
            ss << "Assertion failed: " << message
               << " (expected approximately " << expected
               << " within tolerance " << tolerance
               << ", got " << actual << ")";
            throw std::runtime_error(ss.str());
        }
    }
    
    void assertNotNull(const void* ptr, const std::string& message = "") {
        if (ptr == nullptr) {
            throw std::runtime_error("Assertion failed: " + message + " (pointer is null)");
        }
    }
    
    void assertNull(const void* ptr, const std::string& message = "") {
        if (ptr != nullptr) {
            throw std::runtime_error("Assertion failed: " + message + " (pointer is not null)");
        }
    }
    
    // Timing utilities
    template<typename Func>
    std::chrono::duration<double> timeFunction(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return end - start;
    }
    
    // Performance benchmarking
    template<typename Func>
    double benchmark(Func&& func, int iterations = 1000) {
        auto totalTime = std::chrono::duration<double>::zero();
        
        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();
            totalTime += (end - start);
        }
        
        return (totalTime.count() / iterations) * 1e6; // microseconds per operation
    }
};

// TestSuite class for organizing related tests
class TestSuite {
private:
    std::string name;
    std::vector<std::unique_ptr<TestCase>> testCases;
    std::map<std::string, TestParameters> parameterSets;
    
public:
    TestSuite(const std::string& suiteName) : name(suiteName) {}
    
    const std::string& getName() const { return name; }
    
    void addTest(std::unique_ptr<TestCase> testCase) {
        testCases.push_back(std::move(testCase));
    }
    
    void addParameterSet(const std::string& name, const TestParameters& params) {
        parameterSets[name] = params;
    }
    
    const std::vector<std::unique_ptr<TestCase>>& getTestCases() const {
        return testCases;
    }
    
    const std::map<std::string, TestParameters>& getParameterSets() const {
        return parameterSets;
    }
    
    // Run all tests in the suite
    std::vector<TestInfo> run() {
        std::vector<TestInfo> results;
        
        // Setup test case if needed
        for (auto& paramSet : parameterSets) {
            for (auto& testCase : testCases) {
                // Copy test case for parameterized runs
                auto testCopy = std::make_unique<TestCase>(*testCase);
                testCopy->withParameters(paramSet.second);
                
                TestInfo info(testCase->getName(), testCase->getCategory(), "");
                info.fileName = "test_framework_base.cpp";
                
                try {
                    if (testCopy->setupTestCase()) {
                        auto start = std::chrono::high_resolution_clock::now();
                        testCopy->setup();
                        testCopy->run();
                        testCopy->teardown();
                        auto end = std::chrono::high_resolution_clock::now();
                        
                        info.duration = end - start;
                        info.result = TestResult::PASSED;
                    } else {
                        info.result = TestResult::SKIPPED;
                        info.errorMessage = "Test case setup failed";
                    }
                } catch (const std::exception& e) {
                    auto end = std::chrono::high_resolution_clock::now();
                    info.duration = end - start;
                    info.result = TestResult::FAILED;
                    info.errorMessage = e.what();
                }
                
                results.push_back(info);
            }
        }
        
        return results;
    }
};

// Global test registry
class TestRegistry {
private:
    std::vector<std::unique_ptr<TestSuite>> suites;
    std::vector<TestInfo> results;
    
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    void registerSuite(std::unique_ptr<TestSuite> suite) {
        suites.push_back(std::move(suite));
    }
    
    const std::vector<std::unique_ptr<TestSuite>>& getSuites() const {
        return suites;
    }
    
    const std::vector<TestInfo>& getResults() const {
        return results;
    }
    
    void setResults(const std::vector<TestInfo>& testResults) {
        results = testResults;
    }
    
    // Run all registered tests
    void runAllTests() {
        results.clear();
        
        for (auto& suite : suites) {
            auto suiteResults = suite->run();
            results.insert(results.end(), suiteResults.begin(), suiteResults.end());
        }
    }
    
    // Generate test report
    std::string generateReport() {
        std::stringstream ss;
        ss << "=== Test Suite Report ===" << std::endl;
        ss << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, errored = 0;
        for (const auto& result : results) {
            switch (result.result) {
                case TestResult::PASSED: passed++; break;
                case TestResult::FAILED: failed++; break;
                case TestResult::SKIPPED: skipped++; break;
                case TestResult::ERROR: errored++; break;
            }
        }
        
        ss << "Total Tests: " << results.size() << std::endl;
        ss << "Passed: " << passed << std::endl;
        ss << "Failed: " << failed << std::endl;
        ss << "Skipped: " << skipped << std::endl;
        ss << "Errored: " << errored << std::endl;
        ss << std::endl;
        
        // Detailed results
        ss << "=== Detailed Results ===" << std::endl;
        for (const auto& result : results) {
            ss << "Test: " << result.name << std::endl;
            ss << "Category: " << result.category << std::endl;
            ss << "Duration: " << std::fixed << std::setprecision(6) << result.duration.count() << "s" << std::endl;
            
            switch (result.result) {
                case TestResult::PASSED:
                    ss << "Result: PASSED" << std::endl;
                    break;
                case TestResult::FAILED:
                    ss << "Result: FAILED" << std::endl;
                    if (!result.errorMessage.empty()) {
                        ss << "Error: " << result.errorMessage << std::endl;
                    }
                    break;
                case TestResult::SKIPPED:
                    ss << "Result: SKIPPED" << std::endl;
                    break;
                case TestResult::ERROR:
                    ss << "Result: ERROR" << std::endl;
                    if (!result.errorMessage.empty()) {
                        ss << "Error: " << result.errorMessage << std::endl;
                    }
                    break;
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
};

} // namespace nlm::test

#endif // TESTFRAMEWORK_HPP
