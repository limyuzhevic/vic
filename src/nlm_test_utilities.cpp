// Test utilities implementation

#include "nlm_test_utilities.h"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace nlm {
namespace test {

// TestResult implementation
TestResult::TestResult(const std::string& name, const std::string& cat, bool p,
                       const std::string& msg, double time,
                       int line, const std::string& file)
    : testName(name), category(cat), passed(p), message(msg),
      executionTime(time), lineNumber(line), fileName(file) {}

// TestFramework implementation
TestFramework::TestFramework() {}

void TestFramework::startTest(const std::string& name, TestCategory category) {
    currentTest = name;
    currentCategory = category;
    std::cout << "  Testing: " << name << "..." << std::endl;
}

void TestFramework::recordResult(bool passed, const std::string& message) {
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

void TestFramework::recordResultWithTime(bool passed, const std::string& message, double time) {
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

void TestFramework::assert(bool condition, const std::string& message) {
    if (!condition) {
        std::stringstream ss;
        ss << message << " at " << currentTest;
        recordResult(false, ss.str());
    }
}

void TestFramework::assertDetailed(bool condition, const std::string& expected, 
                                   const std::string& actual, const std::string& operation) {
    std::stringstream ss;
    ss << "Expected " << operation << " '" << expected << "', got '" << actual << "'";
    assert(condition, ss.str());
}

void TestFramework::assertInRange(double value, double min, double max, 
                                  const std::string& description) {
    std::stringstream ss;
    ss << description << " " << value << " not in range [" << min << ", " << max << "]";
    assert(value >= min && value <= max, ss.str());
}

template<typename T>
void TestFramework::assertContains(const std::vector<T>& container, const T& element, 
                                   const std::string& description) {
    bool found = std::find(container.begin(), container.end(), element) != container.end();
    std::stringstream ss;
    ss << "Vector does not contain " << description;
    assert(found, ss.str());
}

template<typename T>
void TestFramework::assertEqual(const std::vector<T>& vec1, const std::vector<T>& vec2,
                                const std::string& description) {
    std::stringstream ss;
    ss << "Vector sizes don't match for " << description;
    assert(vec1.size() == vec2.size(), ss.str());
    ss.str("");
    ss << "Vector contents don't match for " << description;
    assert(std::equal(vec1.begin(), vec1.end(), vec2.begin()), ss.str());
}

template<typename T>
void TestFramework::assertNotNull(T* ptr, const std::string& description) {
    std::stringstream ss;
    ss << "Null pointer access: " << description;
    assert(ptr != nullptr, ss.str());
}

template<typename T, typename Func>
void TestFramework::assertThrows(Func func, const std::string& description) {
    try {
        func();
        std::stringstream ss;
        ss << "Expected exception but none was thrown for " << description;
        assert(false, ss.str());
    } catch (const T&) {
        std::stringstream ss;
        ss << "Correctly threw " << typeid(T).name() << " for " << description;
        recordResult(true, ss.str());
    }
}

const std::vector<TestResult>& TestFramework::getResults() const { return results; }

std::vector<TestResult> TestFramework::getResultsForCategory(TestCategory category) const {
    std::vector<TestResult> filtered;
    std::string catStr = testCategoryToString(category);
    for (const auto& result : results) {
        if (result.category == catStr) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

std::string TestFramework::testCategoryToString(TestCategory category) {
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

// TestSummary implementation
TestFramework::TestSummary::TestSummary(int total, int passed, int failed, double time,
                                       const std::map<std::string, int>& failMap)
    : totalTests(total), passedTests(passed), failedTests(failed),
      totalTime(time), failuresByCategory(failMap) {}

// Timer implementation
Timer::Timer() : startTime(std::chrono::high_resolution_clock::now()) {}

double Timer::stop() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count() / 1000000.0;  // Convert to seconds
}

// Template instantiations
template void TestFramework::assertContains<int>(const std::vector<int>&, const int&, const std::string&);
template void TestFramework::assertContains<double>(const std::vector<double>&, const double&, const std::string&);
template void TestFramework::assertContains<std::string>(const std::vector<std::string>&, const std::string&, const std::string&);
template void TestFramework::assertEqual<int>(const std::vector<int>&, const std::vector<int>&, const std::string&);
template void TestFramework::assertNotNull<void>(void*, const std::string&);
template void TestFramework::assertThrows<std::exception, std::function<void>>(std::function<void>, const std::string&);

} // namespace test
} // namespace nlm