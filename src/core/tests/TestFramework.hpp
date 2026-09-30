// Test Framework - Improved test structure for NLM
// Provides a comprehensive test framework with better organization and reporting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>
#include <sstream>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    std::string description;
    bool passed;
    std::string errorMessage;
    double duration; // seconds
    int lineNumber;
    std::string fileName;
};

// Test category enumeration
enum class TestCategory {
    Unit,           // Individual component tests
    Integration,    // System integration tests
    Performance,    // Performance and benchmarking
    Stress,         // Stress testing
    Regression,     // Regression tests
    Documentation   // Documentation and examples
};

// Test framework interface
class TestFramework {
public:
    TestFramework() {
        Logger::getGlobal().info("TestFramework initialized");
    }
    
    virtual ~TestFramework() = default;
    
    // Register a test case
    void registerTest(
        const std::string& name,
        const std::string& description,
        TestCategory category,
        std::function<bool()> testFunction,
        int lineNumber,
        const std::string& fileName
    ) {
        TestCase testCase;
        testCase.name = name;
        testCase.description = description;
        testCase.category = category;
        testCase.testFunction = testFunction;
        testCase.lineNumber = lineNumber;
        testCase.fileName = fileName;
        
        testCases.push_back(testCase);
        Logger::getGlobal().debug("Test registered: " + name + " (" + categoryToString(category) + ")");
    }
    
    // Run all tests
    std::vector<TestResult> runAllTests() {
        Logger::getGlobal().info("Running all tests...");
        
        results.clear();
        passedTests = 0;
        failedTests = 0;
        totalDuration = 0.0;
        
        // Group tests by category
        std::map<TestCategory, std::vector<TestCase>> testsByCategory;
        for (const auto& testCase : testCases) {
            testsByCategory[testCase.category].push_back(testCase);
        }
        
        // Run tests by category
        for (const auto& [category, tests] : testsByCategory) {
            Logger::getGlobal().info("Running " + categoryToString(category) + " tests...");
            runTestsByCategory(category, tests);
        }
        
        return results;
    }
    
    // Run specific category
    std::vector<TestResult> runCategory(TestCategory category) {
        Logger::getGlobal().info("Running " + categoryToString(category) + " tests...");
        
        std::vector<TestCase> tests;
        for (const auto& testCase : testCases) {
            if (testCase.category == category) {
                tests.push_back(testCase);
            }
        }
        
        results.clear();
        passedTests = 0;
        failedTests = 0;
        totalDuration = 0.0;
        
        runTestsByCategory(category, tests);
        return results;
    }
    
    // Get test results
    const std::vector<TestResult>& getResults() const {
        return results;
    }
    
    // Get passed test count
    size_t getPassedCount() const {
        return passedTests;
    }
    
    // Get failed test count
    size_t getFailedCount() const {
        return failedTests;
    }
    
    // Get total duration
    double getTotalDuration() const {
        return totalDuration;
    }
    
    // Generate report
    std::string generateReport() const {
        std::ostringstream oss;
        
        oss << "=" << std::string(60, '=') << std::endl;
        oss << "NLM TEST REPORT" << std::endl;
        oss << "=" << std::string(60, '=') << std::endl;
        oss << "Generated: " << getCurrentTimestamp() << std::endl;
        oss << "Total Tests: " << testCases.size() << std::endl;
        oss << "Passed: " << passedTests << std::endl;
        oss << "Failed: " << failedTests << std::endl;
        oss << "Success Rate: " << std::fixed << std::setprecision(2) 
            << (testCases.empty() ? 0.0 : (static_cast<double>(passedTests) / testCases.size() * 100.0)) << "%" << std::endl;
        oss << "Total Duration: " << std::fixed << std::setprecision(3) << totalDuration << " seconds" << std::endl;
        oss << "Average Duration: " << std::fixed << std::setprecision(3) 
            << (testCases.empty() ? 0.0 : totalDuration / testCases.size()) << " seconds" << std::endl;
        oss << std::endl;
        
        // Group results by category
        std::map<TestCategory, std::vector<TestResult>> resultsByCategory;
        for (const auto& result : results) {
            resultsByCategory[categoryFromString(result.category)].push_back(result);
        }
        
        // Report by category
        for (const auto& [category, categoryResults] : resultsByCategory) {
            oss << "--- " << categoryToString(category) << " TESTS ---" << std::endl;
            oss << "Passed: " << categoryResults.size() - 
                count_failed_tests(categoryResults) << "/" << categoryResults.size() << std::endl;
            
            for (const auto& result : categoryResults) {
                oss << "  " << (result.passed ? "✓" : "✗") << " " << result.name << std::endl;
                if (!result.passed) {
                    oss << "    Error: " << result.errorMessage << std::endl;
                }
            }
            oss << std::endl;
        }
        
        oss << "=" << std::string(60, '=') << std::endl;
        
        return oss.str();
    }
    
    // Write report to file
    void writeReport(const std::string& filename) const {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << generateReport();
            file.close();
            Logger::getGlobal().info("Test report written to: " + filename);
        } else {
            Logger::getGlobal().error("Failed to write test report to: " + filename);
        }
    }
    
    // Clear all tests
    void clearTests() {
        testCases.clear();
        results.clear();
        Logger::getGlobal().info("Test cases cleared");
    }

protected:
    struct TestCase {
        std::string name;
        std::string description;
        TestCategory category;
        std::function<bool()> testFunction;
        int lineNumber;
        std::string fileName;
    };
    
    std::vector<TestCase> testCases;
    std::vector<TestResult> results;
    size_t passedTests;
    size_t failedTests;
    double totalDuration;
    
    // Convert category to string
    std::string categoryToString(TestCategory category) const {
        switch (category) {
            case TestCategory::Unit: return "Unit Tests";
            case TestCategory::Integration: return "Integration Tests";
            case TestCategory::Performance: return "Performance Tests";
            case TestCategory::Stress: return "Stress Tests";
            case TestCategory::Regression: return "Regression Tests";
            case TestCategory::Documentation: return "Documentation Tests";
            default: return "Unknown";
        }
    }
    
    // Convert string to category
    TestCategory categoryFromString(const std::string& categoryStr) const {
        if (categoryStr == "Unit Tests") return TestCategory::Unit;
        if (categoryStr == "Integration Tests") return TestCategory::Integration;
        if (categoryStr == "Performance Tests") return TestCategory::Performance;
        if (categoryStr == "Stress Tests") return TestCategory::Stress;
        if (categoryStr == "Regression Tests") return TestCategory::Regression;
        if (categoryStr == "Documentation Tests") return TestCategory::Documentation;
        return TestCategory::Unit;
    }
    
    // Count failed tests in category
    size_t count_failed_tests(const std::vector<TestResult>& testResults) const {
        size_t failed = 0;
        for (const auto& result : testResults) {
            if (!result.passed) failed++;
        }
        return failed;
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Run tests by category
    void runTestsByCategory(TestCategory category, const std::vector<TestCase>& tests) {
        for (const auto& testCase : tests) {
            TestResult result;
            result.name = testCase.name;
            result.category = categoryToString(testCase.category);
            result.description = testCase.description;
            result.lineNumber = testCase.lineNumber;
            result.fileName = testCase.fileName;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                result.passed = testCase.testFunction();
                if (!result.passed) {
                    result.errorMessage = "Test failed but no error message provided";
                }
            } catch (const std::exception& e) {
                result.passed = false;
                result.errorMessage = std::string("Exception: ") + e.what();
            } catch (...) {
                result.passed = false;
                result.errorMessage = "Unknown exception occurred";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration<double>(endTime - startTime).count();
            
            results.push_back(result);
            totalDuration += result.duration;
            
            if (result.passed) {
                passedTests++;
                Logger::getGlobal().info("Test passed: " + result.name + " (" + std::to_string(result.duration) + "s)");
            } else {
                failedTests++;
                Logger::getGlobal().error("Test failed: " + result.name + " - " + result.errorMessage + " (" + std::to_string(result.duration) + "s)");
            }
        }
    }
};

// Global test framework instance
inline TestFramework& getTestFramework() {
    static TestFramework framework;
    return framework;
}

// Convenience macros for test registration
#define REGISTER_TEST(name, description, category, testFunc) \
    namespace { \
        struct test_registrar_ ## name { \
            test_registrar_ ## name() { \
                nlm::getTestFramework().registerTest( \
                    #name, description, category, testFunc, \
                    __LINE__, __FILE__ \
                ); \
            } \
        }; \
        test_registrar_ ## name test_instance_ ## name; \
    }

// Convenience macros for test categories
#define UNIT_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Unit, testFunc)
#define INTEGRATION_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Integration, testFunc)
#define PERFORMANCE_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Performance, testFunc)
#define STRESS_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Stress, testFunc)
#define REGRESSION_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Regression, testFunc)
#define DOCUMENTATION_TEST(name, description, testFunc) REGISTER_TEST(name, description, nlm::TestCategory::Documentation, testFunc)

// Test runner function
inline void runAllTests() {
    auto& framework = nlm::getTestFramework();
    auto results = framework.runAllTests();
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "TEST EXECUTION SUMMARY" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "Total Tests: " << framework.getResults().size() << std::endl;
    std::cout << "Passed: " << framework.getPassedCount() << std::endl;
    std::cout << "Failed: " << framework.getFailedCount() << std::endl;
    
    if (framework.getFailedCount() > 0) {
        std::cout << std::endl;
        std::cout << "FAILED TESTS:" << std::endl;
        for (const auto& result : framework.getResults()) {
            if (!result.passed) {
                std::cout << "  - " << result.name << ": " << result.errorMessage << std::endl;
            }
        }
    }
    
    // Write detailed report
    framework.writeReport("test_report.html");
    
    // Exit with appropriate code
    if (framework.getFailedCount() > 0) {
        std::cout << std::endl;
        std::cout << "TESTS FAILED - exiting with error code 1" << std::endl;
        std::exit(1);
    } else {
        std::cout << std::endl;
        std::cout << "ALL TESTS PASSED - exiting with success code 0" << std::endl;
        std::exit(0);
    }
}

} // namespace nlm
