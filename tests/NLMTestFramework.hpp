// Production Unit Test Framework
// Separate testing infrastructure for NLM with proper organization and reporting

#ifndef NLM_TEST_FRAMEWORK_HPP
#define NLM_TEST_FRAMEWORK_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace test {

// Test result structure for detailed reporting
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    std::chrono::microseconds duration;
    
    TestResult(const std::string& testName, const std::string& testCategory,
               bool testPassed, const std::string& testMessage = "")
        : name(testName), category(testCategory), passed(testPassed),
          message(testMessage), duration(0) {}
};

// Main test runner class with timing and reporting
class TestRunner {
public:
    TestRunner() : passedTests(0), failedTests(0), totalTests(0) {}
    
    // Register a test function
    void addTest(const std::string& name, const std::string& category,
                 std::function<void()> testFunction);
    
    // Run all registered tests
    void runAllTests();
    
    // Run tests from specific category
    void runCategory(const std::string& category);
    
    // Get test results
    const std::vector<TestResult>& getResults() const { return results; }
    
    // Get summary statistics
    size_t getPassedCount() const { return passedTests; }
    size_t getFailedCount() const { return failedTests; }
    size_t getTotalCount() const { return totalTests; }
    
    // Generate human-readable report
    std::string generateReport() const;
    
    // Generate machine-readable report (JSON)
    std::string generateJSONReport() const;
    
    // Clear results
    void clear() {
        results.clear();
        passedTests = 0;
        failedTests = 0;
        totalTests = 0;
    }
    
private:
    struct TestCase {
        std::string name;
        std::string category;
        std::function<void()> function;
    };
    
    std::vector<TestCase> testCases;
    std::vector<TestResult> results;
    size_t passedTests;
    size_t failedTests;
    size_t totalTests;
};

// Macro for easy test definition
#define TEST_CATEGORY(category) static std::vector<TestRunner::TestCase>& getTestCases(category)

// Test assertion macros
#define ASSERT_TRUE(condition) assertTest(condition, #condition, __FILE__, __LINE__)
#define ASSERT_FALSE(condition) assertTest(!condition, #condition, __FILE__, __LINE__)
#define ASSERT_EQ(actual, expected) assertEqual(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_NE(actual, expected) assertNotEqual(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_GT(actual, expected) assertGreaterThan(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_GE(actual, expected) assertGreaterOrEqual(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_LT(actual, expected) assertLessThan(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_LE(actual, expected) assertLessOrEqual(actual, expected, #actual, #expected, __FILE__, __LINE__)
#define ASSERT_THROW(expression, exception) assertThrows(expression, exception, __FILE__, __LINE__)

// Helper assertion functions
void assertTest(bool condition, const std::string& description,
                const std::string& file, int line);
void assertEqual(double actual, double expected,
                  const std::string& actualDesc, const std::string& expectedDesc,
                  const std::string& file, int line);
void assertNotEqual(double actual, double expected,
                     const std::string& actualDesc, const std::string& expectedDesc,
                     const std::string& file, int line);
void assertGreaterThan(double actual, double expected,
                        const std::string& actualDesc, const std::string& expectedDesc,
                        const std::string& file, int line);
void assertGreaterOrEqual(double actual, double expected,
                           const std::string& actualDesc, const std::string& expectedDesc,
                           const std::string& file, int line);
void assertLessThan(double actual, double expected,
                     const std::string& actualDesc, const std::string& expectedDesc,
                     const std::string& file, int line);
void assertLessOrEqual(double actual, double expected,
                        const std::string& actualDesc, const std::string& expectedDesc,
                        const std::string& file, int line);

// Test fixture base class
class TestFixture {
protected:
    // Common test setup and teardown methods
    virtual void setUp() {}
    virtual void tearDown() {}
    
    // Template for test methods
    void runTest(const std::string& testName, std::function<void()> testMethod);
};

// Integration test fixture for system-level testing
class IntegrationTestFixture : public TestFixture {
protected:
    virtual void setUp() override;
    virtual void tearDown() override;
};

} // namespace test
} // namespace nlm

#endif // NLM_TEST_FRAMEWORK_HPP
