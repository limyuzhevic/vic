"""
Improved test framework for NLM.

This module provides a comprehensive test framework for NLM, replacing the
existing simple `assert()`-based testing with a more robust and feature-rich
approach.
"""

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <sstream>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class Action;
    // Other NLM types...
}

// Test framework classes
class TestCase;
class TestSuite;
class TestReport;
class TestRunner;

enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

enum class TestSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct TestInfo {
    std::string name;
    std::string description;
    std::function<bool()> testFunction;
    TestSeverity severity;
    std::vector<std::string> tags;
    double executionTime;
    TestResult result;
    std::string failureMessage;
    std::string errorMessage;
};

struct TestSuiteInfo {
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<TestCase>> testCases;
    TestResult result;
    double totalExecutionTime;
    int passedCount;
    int failedCount;
    int skippedCount;
    int errorCount;
};

class TestCase {
public:
    TestCase(const std::string& name, const std::string& description,
             std::function<bool()> testFunction, TestSeverity severity = TestSeverity::INFO)
        : name(name), description(description), testFunction(testFunction),
          severity(severity), executionTime(0.0), result(TestResult::ERROR) {}
    
    virtual ~TestCase() = default;
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    TestResult getResult() const { return result; }
    double getExecutionTime() const { return executionTime; }
    std::string getFailureMessage() const { return failureMessage; }
    std::string getErrorMessage() const { return errorMessage; }
    
    TestResult run() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            bool success = testFunction();
            executionTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
            
            if (success) {
                result = TestResult::PASSED;
                return result;
            } else {
                result = TestResult::FAILED;
                failureMessage = "Test failed";
                return result;
            }
        } catch (const std::exception& e) {
            executionTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
            result = TestResult::ERROR;
            errorMessage = std::string("Exception: ") + e.what();
            return result;
        } catch (...) {
            executionTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
            result = TestResult::ERROR;
            errorMessage = "Unknown exception";
            return result;
        }
    }
    
    virtual void setup() {}
    virtual void teardown() {}
    
protected:
    std::string name;
    std::string description;
    std::function<bool()> testFunction;
    TestSeverity severity;
    double executionTime;
    TestResult result;
    std::string failureMessage;
    std::string errorMessage;
};

class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description)
        : name(name), description(description), result(TestResult::ERROR),
          totalExecutionTime(0.0) {}
    
    void addTestCase(std::shared_ptr<TestCase> testCase) {
        testCases.push_back(testCase);
    }
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    TestResult getResult() const { return result; }
    double getTotalExecutionTime() const { return totalExecutionTime; }
    int getPassedCount() const { return passedCount; }
    int getFailedCount() const { return failedCount; }
    int getSkippedCount() const { return skippedCount; }
    int getErrorCount() const { return errorCount; }
    
    TestResult run() {
        passedCount = 0;
        failedCount = 0;
        skippedCount = 0;
        errorCount = 0;
        totalExecutionTime = 0.0;
        
        for (auto& testCase : testCases) {
            testCase->run();
            totalExecutionTime += testCase->getExecutionTime();
            
            switch (testCase->getResult()) {
                case TestResult::PASSED:
                    passedCount++;
                    break;
                case TestResult::FAILED:
                    failedCount++;
                    break;
                case TestResult::SKIPPED:
                    skippedCount++;
                    break;
                case TestResult::ERROR:
                    errorCount++;
                    break;
            }
        }
        
        if (failedCount == 0 && errorCount == 0) {
            result = TestResult::PASSED;
        } else if (passedCount == 0) {
            result = TestResult::FAILED;
        } else {
            result = TestResult::ERROR;
        }
        
        return result;
    }
    
    TestResult runWithSetupTeardown() {
        for (auto& testCase : testCases) {
            testCase->setup();
            testCase->run();
            testCase->teardown();
            totalExecutionTime += testCase->getExecutionTime();
            
            switch (testCase->getResult()) {
                case TestResult::PASSED:
                    passedCount++;
                    break;
                case TestResult::FAILED:
                    failedCount++;
                    break;
                case TestResult::SKIPPED:
                    skippedCount++;
                    break;
                case TestResult::ERROR:
                    errorCount++;
                    break;
            }
        }
        
        if (failedCount == 0 && errorCount == 0) {
            result = TestResult::PASSED;
        } else if (passedCount == 0) {
            result = TestResult::FAILED;
        } else {
            result = TestResult::ERROR;
        }
        
        return result;
    }
    
    void clear() {
        testCases.clear();
        result = TestResult::ERROR;
        totalExecutionTime = 0.0;
        passedCount = 0;
        failedCount = 0;
        skippedCount = 0;
        errorCount = 0;
    }
    
    std::vector<std::shared_ptr<TestCase>>& getTestCases() {
        return testCases;
    }
    
private:
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<TestCase>> testCases;
    TestResult result;
    double totalExecutionTime;
    int passedCount;
    int failedCount;
    int skippedCount;
    int errorCount;
};

class TestReport {
public:
    TestReport(const std::string& title) : title(title) {}
    
    void addSuite(std::shared_ptr<TestSuite> suite) {
        suites.push_back(suite);
    }
    
    void generateReport() {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "TEST REPORT: " << title << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        double totalTime = 0.0;
        int totalPassed = 0;
        int totalFailed = 0;
        int totalSkipped = 0;
        int totalError = 0;
        
        for (auto& suite : suites) {
            std::cout << "\nSUITE: " << suite->getName() << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            std::cout << "Description: " << suite->getDescription() << std::endl;
            std::cout << "Result: " << getResultString(suite->getResult()) << std::endl;
            std::cout << "Execution Time: " << std::fixed << std::setprecision(3) << suite->getTotalExecutionTime() << "s" << std::endl;
            std::cout << "Tests: " << suite->getTestCases().size() << std::endl;
            std::cout << "Passed: " << suite->getPassedCount() << std::endl;
            std::cout << "Failed: " << suite->getFailedCount() << std::endl;
            std::cout << "Skipped: " << suite->getSkippedCount() << std::endl;
            std::cout << "Error: " << suite->getErrorCount() << std::endl;
            
            totalTime += suite->getTotalExecutionTime();
            totalPassed += suite->getPassedCount();
            totalFailed += suite->getFailedCount();
            totalSkipped += suite->getSkippedCount();
            totalError += suite->getErrorCount();
            
            // Detailed test results
            for (auto& testCase : suite->getTestCases()) {
                std::cout << "\n  Test: " << testCase->getName() << std::endl;
                std::cout << "    Description: " << testCase->getDescription() << std::endl;
                std::cout << "    Result: " << getResultString(testCase->getResult()) << std::endl;
                std::cout << "    Execution Time: " << std::fixed << std::setprecision(3) << testCase->getExecutionTime() << "s" << std::endl;
                
                if (testCase->getResult() == TestResult::FAILED) {
                    std::cout << "    Failure Message: " << testCase->getFailureMessage() << std::endl;
                } else if (testCase->getResult() == TestResult::ERROR) {
                    std::cout << "    Error Message: " << testCase->getErrorMessage() << std::endl;
                }
            }
        }
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Total Suites: " << suites.size() << std::endl;
        std::cout << "Total Tests: " << getTotalTestCount() << std::endl;
        std::cout << "Total Passed: " << totalPassed << std::endl;
        std::cout << "Total Failed: " << totalFailed << std::endl;
        std::cout << "Total Skipped: " << totalSkipped << std::endl;
        std::cout << "Total Error: " << totalError << std::endl;
        std::cout << "Total Execution Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) << 
            (totalPassed / (double)getTotalTestCount() * 100.0) << "%" << std::endl;
        
        if (totalFailed > 0 || totalError > 0) {
            std::cout << "\nOVERALL RESULT: " << (totalError > 0 ? "ERROR" : "FAILED") << std::endl;
        } else {
            std::cout << "\nOVERALL RESULT: PASSED" << std::endl;
        }
    }
    
    std::string getResultString(TestResult result) {
        switch (result) {
            case TestResult::PASSED: return "PASSED";
            case TestResult::FAILED: return "FAILED";
            case TestResult::SKIPPED: return "SKIPPED";
            case TestResult::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    int getTotalTestCount() const {
        int total = 0;
        for (auto& suite : suites) {
            total += suite->getTestCases().size();
        }
        return total;
    }
    
private:
    std::string title;
    std::vector<std::shared_ptr<TestSuite>> suites;
};

class TestRunner {
public:
    TestRunner(const std::string& title) : report(title) {}
    
    void addSuite(std::shared_ptr<TestSuite> suite) {
        report.addSuite(suite);
    }
    
    void runAllTests() {
        std::cout << "Running all tests..." << std::endl;
        
        for (auto& suite : report.suites) {
            std::cout << "Running suite: " << suite->getName() << std::endl;
            suite->runWithSetupTeardown();
        }
        
        report.generateReport();
    }
    
    TestReport getReport() {
        return report;
    }
    
private:
    TestReport report;
};

// Macro for easy test case creation
#define CREATE_TEST_CASE(suite, name, description, testFunction, severity) \
    do { \
        class name##_TestCase : public TestCase { \
        public: \
            name##_TestCase() : TestCase(#name, description, testFunction, severity) {} \
            void setup() override { setup_##name(); } \
            void teardown() override { teardown_##name(); } \
        private: \
            void setup_##name() {} \
            void teardown_##name() {} \
        }; \
        suite->addTestCase(std::make_shared<name##_TestCase>()); \
    } while (0)

// Macro for parameterized tests
#define CREATE_PARAMETERIZED_TEST_CASE(suite, name, description, testFunction, severity, ...) \
    do { \
        class name##_TestCase : public TestCase { \
        public: \
            name##_TestCase(int param) : TestCase(#name, description, \
                [=]() { return testFunction(param); }, severity) {} \
            void setup() override { setup_##name(); } \
            void teardown() override { teardown_##name(); } \
        private: \
            void setup_##name() {} \
            void teardown_##name() {} \
        }; \
        /* Parameterized test implementation would go here */ \
    } while (0)

// Convenience functions for common test assertions
inline bool assertEqual(double expected, double actual, double tolerance = 1e-6,
                       const std::string& message = "Values are not equal") {
    if (std::abs(expected - actual) > tolerance) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected: " << expected << std::endl;
        std::cerr << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertNotEqual(double expected, double actual, double tolerance = 1e-6,
                           const std::string& message = "Values are equal") {
    if (std::abs(expected - actual) <= tolerance) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected not equal to: " << expected << std::endl;
        std::cerr << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertGreaterThan(double expected, double actual,
                              const std::string& message = "Value is not greater than expected") {
    if (actual <= expected) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected greater than: " << expected << std::endl;
        std::cerr << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertLessThan(double expected, double actual,
                           const std::string& message = "Value is not less than expected") {
    if (actual >= expected) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected less than: " << expected << std::endl;
        std::cerr << "  Actual: " << actual << std::endl;
        return false;
    }
    return true;
}

inline bool assertInRange(double value, double min, double max,
                          const std::string& message = "Value is not in range") {
    if (value < min || value > max) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected range: [" << min << ", " << max << "]" << std::endl;
        std::cerr << "  Actual: " << value << std::endl;
        return false;
    }
    return true;
}

inline bool assertNotNull(void* pointer, const std::string& message = "Pointer is null") {
    if (pointer == nullptr) {
        std::cerr << "Assertion failed: " << message << std::endl;
        return false;
    }
    return true;
}

inline bool assertTrue(bool condition, const std::string& message = "Condition is false") {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << std::endl;
        return false;
    }
    return true;
}

inline bool assertFalse(bool condition, const std::string& message = "Condition is true") {
    if (condition) {
        std::cerr << "Assertion failed: " << message << std::endl;
        return false;
    }
    return true;
}

// Test helper functions
namespace test_helpers {
    // Time measurement
    inline double getCurrentTime() {
        return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    // String utilities
    inline std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    // File operations
    inline bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
    
    // Directory operations
    inline bool directoryExists(const std::string& dirname) {
        struct stat info;
        return (stat(dirname.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
    }
    
    // Random number generation
    inline int randomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
    
    inline double randomDouble(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dist(min, max);
        return dist(gen);
    }
}

// Test framework initialization and shutdown
inline TestRunner& getTestRunner(const std::string& title = "NLM Tests") {
    static TestRunner runner(title);
    return runner;
}

inline void registerTestSuite(std::shared_ptr<TestSuite> suite) {
    getTestRunner().addSuite(suite);
}

inline void runAllTests() {
    getTestRunner().runAllTests();
}

// Test case helpers
#define SETUP_TEST_CASE(name, description, testFunction, severity) \
    class name##_TestCase : public TestCase { \
    public: \
        name##_TestCase() : TestCase(#name, description, testFunction, severity) {} \
        void setup() override { setup_##name(); } \
        void teardown() override { teardown_##name(); } \
    private: \
        void setup_##name() {} \
        void teardown_##name() {} \
    }; \
    /* Test case registration would go here */

#define ADD_TEST_CASE(suite, testCase) \
    suite->addTestCase(std::make_shared<testCase>())

// Performance testing utilities
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name) : name(name), startTime(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        std::cout << "Performance: " << name << " took " << std::fixed << std::setprecision(3) << duration << "s" << std::endl;
    }
    
    double getElapsedTime() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
private:
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Memory usage tracking (simplified)
class MemoryTracker {
public:
    static void enable() {
        enabled = true;
    }
    
    static void disable() {
        enabled = false;
    }
    
    static void track(const std::string& name) {
        if (enabled) {
            std::cout << "Memory tracking: " << name << std::endl;
        }
    }
    
private:
    static bool enabled;
};

bool MemoryTracker::enabled = false;
