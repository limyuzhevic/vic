// NLM Test Suite Runner
// 
// This file provides a comprehensive test runner that can execute all
// available tests in the NLM project, including both legacy tests and
// new error handling tests.

#include "tests/test_config.cpp"      // Legacy config tests
#include "tests/test_error_handling.cpp"  // New error handling tests
#include <iostream>
#include <chrono>
#include <string>
#include <vector>

namespace test_runner {

// Test suite information
struct TestSuiteInfo {
    std::string name;
    std::string description;
    std::string (*runFunction)();
    bool isActive;
};

// Timing and statistics
struct TestStats {
    int totalTests;
    int passedTests;
    int failedTests;
    double executionTime;
    std::vector<std::string> failedTestNames;
    std::vector<std::string> errorMessages;
};

// Global test statistics
TestStats globalStats;

// Forward declarations
std::string runConfigTests();
std::string runErrorHandlingTests();
std::string runAllTests();
std::string runSmokeTests();
std::string runPerformanceTests();

// Available test suites
const std::vector<TestSuiteInfo> testSuites = {
    {
        "config_tests",
        "Legacy configuration system tests",
        &runConfigTests,
        true
    },
    {
        "error_handling_tests",
        "Enhanced error handling and validation tests",
        &runErrorHandlingTests,
        true
    },
    {
        "smoke_tests",
        "Quick smoke tests for basic functionality",
        &runSmokeTests,
        true
    },
    {
        "performance_tests",
        "Performance and stress tests",
        &runPerformanceTests,
        false  // Disabled by default - can be time-consuming
    },
    {
        "all_tests",
        "Run all available tests",
        &runAllTests,
        true
    }
};

// Initialize test statistics
void initializeTestStats() {
    globalStats.totalTests = 0;
    globalStats.passedTests = 0;
    globalStats.failedTests = 0;
    globalStats.executionTime = 0.0;
    globalStats.failedTestNames.clear();
    globalStats.errorMessages.clear();
}

// Record test result
void recordTestResult(const std::string& testName, bool passed, const std::string& errorMsg = "") {
    ++globalStats.totalTests;
    if (passed) {
        ++globalStats.passedTests;
    } else {
        ++globalStats.failedTests;
        globalStats.failedTestNames.push_back(testName);
        if (!errorMsg.empty()) {
            globalStats.errorMessages.push_back("Test: " + testName + " - Error: " + errorMsg);
        }
    }
}

// Get formatted test statistics
std::string getTestStats() {
    std::ostringstream oss;
    oss << "\n=== Test Suite Statistics ===" << std::endl;
    oss << "Total tests executed: " << globalStats.totalTests << std::endl;
    oss << "Tests passed: " << globalStats.passedTests << std::endl;
    oss << "Tests failed: " << globalStats.failedTests << std::endl;
    if (globalStats.totalTests > 0) {
        double passRate = (static_cast<double>(globalStats.passedTests) / 
                          static_cast<double>(globalStats.totalTests)) * 100.0;
        oss << "Pass rate: " << std::fixed << std::setprecision(2) << passRate << "%" << std::endl;
    }
    oss << "Total execution time: " << std::fixed << std::setprecision(3) << 
        globalStats.executionTime << " seconds" << std::endl;
    
    if (!globalStats.failedTestNames.empty()) {
        oss << "\n=== Failed Tests ===" << std::endl;
        for (size_t i = 0; i < globalStats.failedTestNames.size(); ++i) {
            oss << (i + 1) << ". " << globalStats.failedTestNames[i];
            if (i < globalStats.errorMessages.size()) {
                oss << " - " << globalStats.errorMessages[i];
            }
            oss << std::endl;
        }
    }
    
    return oss.str();
}

// Run individual test suites
std::string runConfigTests() {
    std::ostringstream oss;
    oss << "Running legacy configuration tests..." << std::endl;
    
    try {
        test_config::runAll();
        oss << "Legacy configuration tests completed successfully!" << std::endl;
        recordTestResult("Legacy Config Tests", true);
    } catch (const std::exception& e) {
        oss << "Legacy configuration tests failed: " << e.what() << std::endl;
        recordTestResult("Legacy Config Tests", false, e.what());
    }
    
    return oss.str();
}

std::string runErrorHandlingTests() {
    std::ostringstream oss;
    oss << "Running enhanced error handling and validation tests..." << std::endl;
    
    try {
        test_error_handling::runAllErrorHandlingTests();
        oss << "Enhanced error handling tests completed successfully!" << std::endl;
        recordTestResult("Enhanced Error Handling Tests", true);
    } catch (const std::exception& e) {
        oss << "Enhanced error handling tests failed: " << e.what() << std::endl;
        recordTestResult("Enhanced Error Handling Tests", false, e.what());
    }
    
    return oss.str();
}

std::string runSmokeTests() {
    std::ostringstream oss;
    oss << "Running smoke tests..." << std::endl;
    
    try {
        // Quick smoke tests to verify basic functionality
        nlm::UnifiedConfig config;
        config.initialize("quick_test");
        assert(config.get<int>("neuron_count") == 100);
        
        nlm::RandomGenerator rng(42);
        uint32_t val = rng.uniformInt(1, 10);
        assert(val >= 1 && val <= 10);
        
        oss << "Smoke tests completed successfully!" << std::endl;
        recordTestResult("Smoke Tests", true);
    } catch (const std::exception& e) {
        oss << "Smoke tests failed: " << e.what() << std::endl;
        recordTestResult("Smoke Tests", false, e.what());
    }
    
    return oss.str();
}

std::string runPerformanceTests() {
    std::ostringstream oss;
    oss << "Running performance tests..." << std::endl;
    
    try {
        // Simple performance tests (can be time-consuming)
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::UnifiedConfig config;
        config.initialize("research");
        
        for (int i = 0; i < 100; ++i) {
            config.set<int>("neuron_count", 500 + i);
            config.get<int>("neuron_count");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        oss << "Performance test completed in " << duration.count() << " ms" << std::endl;
        recordTestResult("Performance Tests", true);
    } catch (const std::exception& e) {
        oss << "Performance tests failed: " << e.what() << std::endl;
        recordTestResult("Performance Tests", false, e.what());
    }
    
    return oss.str();
}

std::string runAllTests() {
    std::ostringstream oss;
    oss << "Running all available tests..." << std::endl;
    
    initializeTestStats();
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run selected test suites
    for (const auto& suite : testSuites) {
        if (suite.isActive && suite.name != "all_tests") {
            oss << "\n--- Executing: " << suite.name << " ---" << std::endl;
            oss << suite.runFunction();
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    globalStats.executionTime = 
        std::chrono::duration<double>(endTime - startTime).count();
    
    oss << getTestStats();
    
    if (globalStats.failedTests == 0) {
        oss << "\nAll tests passed successfully!" << std::endl;
    } else {
        oss << "\n" << globalStats.failedTests << " test(s) failed!" << std::endl;
    }
    
    recordTestResult("All Tests", globalStats.failedTests == 0);
    
    return oss.str();
}

// Print available test suites
void printAvailableTestSuites() {
    std::cout << "Available test suites:" << std::endl;
    std::cout << "=" * 50 << std::endl;
    
    for (size_t i = 0; i < testSuites.size(); ++i) {
        const auto& suite = testSuites[i];
        std::cout << (i + 1) << ". " << suite.name << std::endl;
        std::cout << "   " << suite.description << std::endl;
        if (!suite.isActive) {
            std::cout << "   [DISABLED]" << std::endl;
        }
        std::cout << std::endl;
    }
}

// Main test runner function
std::string runTests(const std::string& testSuiteName = "all_tests") {
    std::ostringstream oss;
    oss << "NLM Test Suite Runner" << std::endl;
    oss << "======================" << std::endl;
    
    // Find the requested test suite
    std::string selectedSuite = testSuiteName;
    bool suiteFound = false;
    
    for (const auto& suite : testSuites) {
        if (suite.name == testSuiteName) {
            selectedSuite = suite.name;
            suiteFound = true;
            break;
        }
    }
    
    if (!suiteFound) {
        oss << "Error: Unknown test suite: " << testSuiteName << std::endl;
        oss << "Available test suites:" << std::endl;
        printAvailableTestSuites();
        return oss.str();
    }
    
    oss << "Running test suite: " << selectedSuite << std::endl;
    oss << std::string(50, '=') << std::endl;
    
    // Find and run the selected test suite
    for (const auto& suite : testSuites) {
        if (suite.name == selectedSuite) {
            oss << suite.runFunction();
            break;
        }
    }
    
    return oss.str();
}

// Command-line interface for test runner
int main(int argc, char* argv[]) {
    std::cout << "NLM Test Suite Runner" << std::endl;
    std::cout << "======================" << std::endl;
    
    if (argc == 1) {
        // Run all tests by default
        std::string results = runTests("all_tests");
        std::cout << results;
        
        return (globalStats.failedTests > 0) ? 1 : 0;
    } else if (argc == 2) {
        std::string testSuite = argv[1];
        if (testSuite == "--help" || testSuite == "-h") {
            printAvailableTestSuites();
            return 0;
        }
        
        std::string results = runTests(testSuite);
        std::cout << results;
        
        return (globalStats.failedTests > 0) ? 1 : 0;
    } else {
        std::cerr << "Usage: test_runner [test_suite_name]" << std::endl;
        std::cerr << "Available test suites:" << std::endl;
        printAvailableTestSuites();
        return 1;
    }
}

} // namespace test_runner

// Entry point for the test runner
int main(int argc, char* argv[]) {
    return test_runner::main(argc, argv);
}
