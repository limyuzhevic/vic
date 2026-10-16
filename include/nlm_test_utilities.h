#ifndef NLM_TEST_UTILITIES_H
#define NLM_TEST_UTILITIES_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <functional>

namespace nlm {
namespace test {

// Test result structure
struct TestResult {
    std::string testName;
    std::string category;
    bool passed;
    std::string message;
    double executionTime;
    int lineNumber;
    std::string fileName;
    
    TestResult(const std::string& name, const std::string& cat, bool p, 
               const std::string& msg = "", double time = 0.0, 
               int line = 0, const std::string& file = "")
        : testName(name), category(cat), passed(p), message(msg),
          executionTime(time), lineNumber(line), fileName(file) {}
};

// Test category enumeration
enum class TestCategory {
    Core,
    Brain,
    Plasticity,
    Config,
    Memory,
    Integration,
    Performance,
    Stress,
    Regression,
    PythonBindings,
    ErrorHandling,
    EdgeCases
};

// Test framework
class TestFramework {
private:
    std::vector<TestResult> results;
    std::string currentTest;
    TestCategory currentCategory;
    
public:
    TestFramework() {}
    
    // Start a test
    void startTest(const std::string& name, TestCategory category = TestCategory::Core) {
        currentTest = name;
        currentCategory = category;
        std::cout << "  Testing: " << name << "..." << std::endl;
    }
    
    // Record test result
    void recordResult(bool passed, const std::string& message = "") {
        auto result = TestResult(currentTest, 
                                testCategoryToString(currentCategory),
                                passed, message, 
                                0.0, 0, "");
        results.push_back(result);
        
        if (passed) {
            std::cout << "    PASSED" << std::endl;
        } else {
            std::cout << "    FAILED: " << message << std::endl;
        }
    }
    
    // Record result with execution time
    void recordResultWithTime(bool passed, const std::string& message, double time) {
        auto result = TestResult(currentTest,
                                testCategoryToString(currentCategory),
                                passed, message,
                                time, 0, "");
        results.push_back(result);
        
        if (passed) {
            std::cout << "    PASSED (" << std::fixed << std::setprecision(3) << time << "s)" << std::endl;
        } else {
            std::cout << "    FAILED: " << message << " (" << std::fixed << std::setprecision(3) << time << "s)" << std::endl;
        }
    }
    
    // Assert helper
    void assert(bool condition, const std::string& message = "Assertion failed") {
        if (!condition) {
            std::stringstream ss;
            ss << message << " at " << currentTest;
            recordResult(false, ss.str());
        }
    }
    
    // Assert with detailed message
    void assertDetailed(bool condition, const std::string& expected, 
                       const std::string& actual, const std::string& operation = "equals") {
        std::stringstream ss;
        ss << "Expected " << operation << " '" << expected << "', got '" << actual << "'";
        assert(condition, ss.str());
    }
    
    // Assert within range
    void assertInRange(double value, double min, double max, 
                      const std::string& description = "value") {
        std::stringstream ss;
        ss << description << " " << value << " not in range [" << min << ", " << max << "]";
        assert(value >= min && value <= max, ss.str());
    }
    
    // Assert vector contains element
    template<typename T>
    void assertContains(const std::vector<T>& container, const T& element, 
                       const std::string& description = "element") {
        bool found = std::find(container.begin(), container.end(), element) != container.end();
        assert(found, "Vector does not contain " + description);
    }
    
    // Assert vectors equal
    template<typename T>
    void assertEqual(const std::vector<T>& vec1, const std::vector<T>& vec2,
                    const std::string& description = "vectors") {
        assert(vec1.size() == vec2.size(), 
               "Vector sizes don't match for " + description);
        assert(std::equal(vec1.begin(), vec1.end(), vec2.begin()),
               "Vector contents don't match for " + description);
    }
    
    // Assert memory address (not null pointer)
    template<typename T>
    void assertNotNull(T* ptr, const std::string& description = "pointer") {
        assert(ptr != nullptr, "Null pointer access: " + description);
    }
    
    // Assert throw exception
    template<typename T, typename Func>
    void assertThrows(Func func, const std::string& description = "function") {
        try {
            func();
            assert(false, "Expected exception but none was thrown for " + description);
        } catch (const T&) {
            // Expected
            recordResult(true, "Correctly threw " + std::string(typeid(T).name()) + " for " + description);
        }
    }
    
    // Get all results
    const std::vector<TestResult>& getResults() const { return results; }
    
    // Get results for category
    std::vector<TestResult> getResultsForCategory(TestCategory category) const {
        std::vector<TestResult> filtered;
        std::string catStr = testCategoryToString(category);
        for (const auto& result : results) {
            if (result.category == catStr) {
                filtered.push_back(result);
            }
        }
        return filtered;
    }
    
    // Get summary statistics
    struct TestSummary {
        int totalTests;
        int passedTests;
        int failedTests;
        double totalTime;
        std::map<std::string, int> failuresByCategory;
        
        TestSummary(int total, int passed, int failed, double time, 
                   const std::map<std::string, int>& failMap)
            : totalTests(total), passedTests(passed), failedTests(failed),
              totalTime(time), failuresByCategory(failMap) {}
    };
    
    TestSummary getSummary() const {
        int total = results.size();
        int passed = 0;
        int failed = 0;
        double totalTime = 0.0;
        std::map<std::string, int> failMap;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed++;
            } else {
                failed++;
                failMap[result.category]++;
            }
            totalTime += result.executionTime;
        }
        
        return TestSummary(total, passed, failed, totalTime, failMap);
    }
    
    // Print report
    void printReport() const {
        TestSummary summary = getSummary();
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "NLM TEST REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Summary
        std::cout << "\nSUMMARY:" << std::endl;
        std::cout << "  Total Tests: " << summary.totalTests << std::endl;
        std::cout << "  Passed:      " << summary.passedTests << std::endl;
        std::cout << "  Failed:      " << summary.failedTests << std::endl;
        std::cout << "  Success Rate: " << std::fixed << std::setprecision(2) 
                  << (summary.totalTests > 0 ? (100.0 * summary.passedTests / summary.totalTests) : 0.0) << "%" << std::endl;
        std::cout << "  Total Time:  " << std::fixed << std::setprecision(3) << summary.totalTime << "s" << std::endl;
        
        // Failures by category
        if (!summary.failuresByCategory.empty()) {
            std::cout << "\nFAILURES BY CATEGORY:" << std::endl;
            for (const auto& pair : summary.failuresByCategory) {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        // Detailed results
        std::cout << "\nDETAILED RESULTS:" << std::endl;
        for (const auto& result : results) {
            std::string status = result.passed ? "PASS" : "FAIL";
            std::cout << "  " << status << ": " << result.testName << " (" << result.category << ")";
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            if (result.executionTime > 0) {
                std::cout << " - " << std::fixed << std::setprecision(3) << result.executionTime << "s";
            }
            std::cout << std::endl;
        }
    }
    
    // Clear results
    void clear() { results.clear(); }
    
    // Convert category to string
    static std::string testCategoryToString(TestCategory category) {
        switch (category) {
            case TestCategory::Core: return "Core";
            case TestCategory::Brain: return "Brain";
            case TestCategory::Plasticity: return "Plasticity";
            case TestCategory::Config: return "Config";
            case TestCategory::Memory: return "Memory";
            case TestCategory::Integration: return "Integration";
            case TestCategory::Performance: return "Performance";
            case TestCategory::Stress: return "Stress";
            case TestCategory::Regression: return "Regression";
            case TestCategory::PythonBindings: return "PythonBindings";
            case TestCategory::ErrorHandling: return "ErrorHandling";
            case TestCategory::EdgeCases: return "EdgeCases";
            default: return "Unknown";
        }
    }
};

// Macro for easier test writing (in test files)
#define TEST_SUITE(name, category) \
    namespace name { \
    inline void runAllTests(nlm::test::TestFramework& framework)

#define TEST_CASE(testName) \
    void testName() \

#define TEST_ASSERT(cond, msg) \
    if (!(cond)) { \
        std::cerr << "Assert failed in " << #testName << ": " << msg << std::endl; \
        return false; \
    }

// Template for measuring execution time
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
public:
    void start() { startTime = std::chrono::high_resolution_clock::now(); }
    double stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0;  // Convert to seconds
    }
};

} // namespace test
} // namespace nlm

#endif // NLM_TEST_UTILITIES_H