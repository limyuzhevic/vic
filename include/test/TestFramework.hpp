#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cassert>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testCategory;
    std::string testFile;
    int lineNumber;
    bool passed;
    double executionTime;
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
    
    TestResult(const std::string& name, const std::string& category, 
               const std::string& file, int line)
        : testName(name), testCategory(category), testFile(file), 
          lineNumber(line), passed(true), executionTime(0.0) {}
};

// Test case interface
class ITestCase {
public:
    virtual ~ITestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool run() = 0;
    virtual void setup() {};
    virtual void teardown() {};
    
    virtual bool shouldRun() const { return true; }
    virtual std::map<std::string, std::string> getTags() const { return {}; }
};

// Parameterized test case
class ParameterizedTestCase : public ITestCase {
public:
    virtual ~ParameterizedTestCase() = default;
    
    virtual std::vector<std::map<std::string, std::variant<int, double, std::string, bool>>> 
        getParameters() const = 0;
    
    virtual bool runWithParameters(const std::map<std::string, std::variant<int, double, std::string, bool>>& params) = 0;
    
    bool run() override {
        bool allPassed = true;
        for (const auto& params : getParameters()) {
            if (!runWithParameters(params)) {
                allPassed = false;
            }
        }
        return allPassed;
    }
};

// Test fixture for setup/teardown
class TestFixture {
public:
    virtual ~TestFixture() = default;
    
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual std::string getFixtureName() const { return "DefaultFixture"; }
};

// Test suite interface
class ITestSuite {
public:
    virtual ~ITestSuite() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::shared_ptr<ITestCase>> getTestCases() const = 0;
    
    virtual bool shouldRun() const { return true; }
    virtual void setupSuite() {}
    virtual void teardownSuite() {}
};

// Test runner interface
class ITestRunner {
public:
    virtual ~ITestRunner() = default;
    
    virtual bool runTest(std::shared_ptr<ITestCase> test) = 0;
    virtual bool runSuite(std::shared_ptr<ITestSuite> suite) = 0;
    
    virtual std::vector<TestResult> getResults() const = 0;
    virtual void clearResults() = 0;
    
    virtual void setVerbosity(int level) = 0;
    virtual void setOutputFile(const std::string& filepath) = 0;
    
    virtual std::string generateReport() const = 0;
    virtual std::string generateXMLReport() const = 0;
    virtual std::string generateJSONReport() const = 0;
};

// Simple test runner implementation
class SimpleTestRunner : public ITestRunner {
public:
    SimpleTestRunner();
    ~SimpleTestRunner() override;
    
    bool runTest(std::shared_ptr<ITestCase> test) override;
    bool runSuite(std::shared_ptr<ITestSuite> suite) override;
    
    std::vector<TestResult> getResults() const override;
    void clearResults() override;
    
    void setVerbosity(int level) override;
    void setOutputFile(const std::string& filepath) override;
    
    std::string generateReport() const override;
    std::string generateXMLReport() const override;
    std::string generateJSONReport() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test registry for discovering and loading tests
class TestRegistry {
public:
    TestRegistry();
    ~TestRegistry();
    
    // Register test suite
    bool registerSuite(std::shared_ptr<ITestSuite> suite);
    
    // Register test case
    bool registerTest(std::shared_ptr<ITestCase> test);
    
    // Get test suites
    std::vector<std::shared_ptr<ITestSuite>> getSuites() const;
    
    // Get test cases
    std::vector<std::shared_ptr<ITestCase>> getTestCases() const;
    
    // Get test cases by category
    std::vector<std::shared_ptr<ITestCase>> getTestCasesByCategory(const std::string& category) const;
    
    // Get test cases by tag
    std::vector<std::shared_ptr<ITestCase>> getTestCasesByTag(const std::string& tag) const;
    
    // Check if suite/test is registered
    bool hasSuite(const std::string& name) const;
    bool hasTest(const std::string& name) const;
    
    // Get suite by name
    std::shared_ptr<ITestSuite> getSuite(const std::string& name) const;
    
    // Get test by name
    std::shared_ptr<ITestCase> getTest(const std::string& name) const;
    
    // Load tests from directory
    bool loadTestsFromDirectory(const std::string& directoryPath);
    
    // Load tests from file
    bool loadTestsFromFile(const std::string& filepath);
    
    // Clear all tests
    void clear();
    
    // Get test statistics
    struct Statistics {
        size_t totalSuites;
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double totalExecutionTime;
        std::map<std::string, size_t> testsByCategory;
        std::map<std::string, size_t> testsByFile;
        
        double getPassRate() const {
            return totalTests > 0 ? (static_cast<double>(passedTests) / totalTests) * 100.0 : 0.0;
        }
    };
    
    Statistics getStatistics() const;
    
    // Export test list
    std::string exportTestList() const;
    
    // Get default test suites
    static std::vector<std::shared_ptr<ITestSuite>> getDefaultTestSuites();
    
    // Get test categories
    static std::vector<std::string> getDefaultCategories();
    
    // Get test tags
    static std::vector<std::string> getDefaultTags();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test configuration
class TestConfig {
public:
    TestConfig();
    ~TestConfig();
    
    // Load test configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save test configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get test configuration
    std::map<std::string, std::variant<int, double, std::string, bool>> 
        getTestConfig(const std::string& testName) const;
    
    // Set test configuration
    bool setTestConfig(const std::string& testName, 
                      const std::map<std::string, std::variant<int, double, std::string, bool>>& config);
    
    // Remove test configuration
    bool removeTestConfig(const std::string& testName);
    
    // Check if test configuration exists
    bool hasTestConfig(const std::string& testName) const;
    
    // Get all test configurations
    std::map<std::string, std::map<std::string, std::variant<int, double, std::string, bool>>> 
        getAllTestConfigs() const;
    
    // Clear test configuration
    void clear();
    
    // Get test metadata
    std::map<std::string, std::map<std::string, std::string>> getTestMetadata(const std::string& testName) const;
    
    // Set test metadata
    bool setTestMetadata(const std::string& testName,
                         const std::map<std::string, std::string>& metadata);
    
    // Get test categories
    std::vector<std::string> getCategories() const;
    
    // Set test category
    bool setCategory(const std::string& testName, const std::string& category);
    
    // Get test tags
    std::vector<std::string> getTags() const;
    
    // Set test tag
    bool setTag(const std::string& testName, const std::string& tag);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test factory for creating test instances
class TestFactory {
public:
    TestFactory();
    ~TestFactory();
    
    // Register test factory
    bool registerTestFactory(const std::string& testName, 
                            std::function<std::shared_ptr<ITestCase>()> factory);
    
    // Create test instance
    std::shared_ptr<ITestCase> createTest(const std::string& testName);
    
    // Get registered test factories
    std::vector<std::string> getRegisteredTestFactories() const;
    
    // Check if test factory is registered
    bool hasTestFactory(const std::string& testName) const;
    
    // Remove test factory
    bool removeTestFactory(const std::string& testName);
    
    // Clear all test factories
    void clear();
    
    // Get default test factories
    static std::map<std::string, std::function<std::shared_ptr<ITestCase>()>> getDefaultTestFactories();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test utilities
namespace TestUtils {
    // Validate test name
    bool isValidTestName(const std::string& name);
    
    // Sanitize test name
    std::string sanitizeTestName(const std::string& name);
    
    // Generate unique test name
    std::string generateTestName(const std::string& prefix = "test");
    
    // Format test result for output
    std::string formatTestResult(const TestResult& result);
    
    // Format test results for output
    std::string formatTestResults(const std::vector<TestResult>& results);
    
    // Get test execution time for performance analysis
    double getAverageExecutionTime(const std::vector<TestResult>& results);
    
    // Get test pass rate
    double getPassRate(const std::vector<TestResult>& results);
    
    // Get test count by category
    std::map<std::string, size_t> getTestCountByCategory(const std::vector<TestResult>& results);
    
    // Get test count by file
    std::map<std::string, size_t> getTestCountByFile(const std::vector<TestResult>& results);
    
    // Export test results to file
    bool exportTestResults(const std::vector<TestResult>& results, 
                          const std::string& filepath,
                          const std::string& format = "txt");
    
    // Import test results from file
    std::vector<TestResult> importTestResults(const std::string& filepath,
                                             const std::string& format = "txt");
    
    // Filter test results
    std::vector<TestResult> filterTestResults(const std::vector<TestResult>& results,
                                             const std::function<bool(const TestResult&)>& predicate);
    
    // Sort test results
    std::vector<TestResult> sortTestResults(const std::vector<TestResult>& results,
                                           const std::function<bool(const TestResult&, const TestResult&)>& comparator);
    
    // Generate test summary
    std::string generateTestSummary(const std::vector<TestResult>& results);
    
    // Generate test details
    std::string generateTestDetails(const std::vector<TestResult>& results);
    
    // Generate test recommendations
    std::vector<std::string> generateTestRecommendations(const std::vector<TestResult>& results);
}

// Global test registry instance
extern TestRegistry& getTestRegistry();

// Global test runner instance
extern ITestRunner& getTestRunner();

// Convenience functions
inline void registerTestSuite(std::shared_ptr<ITestSuite> suite) {
    getTestRegistry().registerSuite(suite);
}

inline void registerTestCase(std::shared_ptr<ITestCase> test) {
    getTestRegistry().registerTest(test);
}

inline bool runAllTests() {
    auto& registry = getTestRegistry();
    auto& runner = getTestRunner();
    
    bool allPassed = true;
    for (const auto& suite : registry.getSuites()) {
        if (suite->shouldRun()) {
            if (!runner.runSuite(suite)) {
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

inline std::string generateTestReport() {
    return getTestRunner().generateReport();
}

inline std::string generateTestXMLReport() {
    return getTestRunner().generateXMLReport();
}

inline std::string generateTestJSONReport() {
    return getTestRunner().generateJSONReport();
}

inline void setTestVerbosity(int level) {
    getTestRunner().setVerbosity(level);
}

inline void setTestOutputFile(const std::string& filepath) {
    getTestRunner().setOutputFile(filepath);
}

} // namespace nlm