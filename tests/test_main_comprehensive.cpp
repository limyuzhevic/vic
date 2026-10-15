// NLM Test Main
// Comprehensive test runner for all test suites
// Integrates unit, integration, performance, and stress tests

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

// Include all test suites
#include "tests/utils/TestUtils.hpp"

// Unit tests
namespace test_config_management {
    void runAll();
}

namespace test_performance_monitoring {
    void runAll();
}

namespace test_experiment_management {
    void runAll();
}

namespace test_neuron_advanced {
    void runAll();
}

namespace test_stdp_advanced {
    void runAll();
}

namespace test_agent_brain_enhanced {
    void runAll();
}

// Integration tests
namespace test_system_integration {
    void runAll();
}

namespace test_python_bindings {
    void runAll();
}

namespace test_cli_integration {
    void runAll();
}

// Performance tests
namespace test_performance_benchmarks {
    void runAll();
}

namespace test_scalability_tests {
    void runAll();
}

namespace test_memory_usage {
    void runAll();
}

// Compatibility tests
namespace test_backward_compatibility {
    void runAll();
}

namespace test_data_format {
    void runAll();
}

namespace test_configuration {
    void runAll();
}

// Stress tests
namespace test_large_scale_simulation {
    void runAll();
}

namespace test_memory_extremes {
    void runAll();
}

namespace test_performance_extremes {
    void runAll();
}

// Performance measurement utilities
struct TestResult {
    std::string testName;
    std::string category;
    double durationMs;
    bool passed;
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
};

struct TestSuiteResult {
    std::string suiteName;
    int totalTests;
    int passedTests;
    int failedTests;
    double totalDurationMs;
    double avgDurationMs;
    std::vector<TestResult> results;
    std::string report;
};

class TestRunner {
public:
    TestRunner() : totalTests_(0), passedTests_(0), failedTests_(0), startTime_(std::chrono::steady_clock::now()) {}
    
    ~TestRunner() {
        generateReport();
    }
    
    void runTestSuite(const std::string& name, std::function<void()> testFunction) {
        std::cout << "Running " << name << "..." << std::endl;
        
        TestSuiteResult suiteResult;
        suiteResult.suiteName = name;
        
        auto suiteStart = std::chrono::steady_clock::now();
        
        try {
            testFunction();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        auto suiteEnd = std::chrono::steady_clock::now();
        double suiteDuration = std::chrono::duration<double, std::milli>(suiteEnd - suiteStart).count();
        
        // Update global statistics
        totalTests_ += suiteResult.totalTests;
        passedTests_ += suiteResult.passedTests;
        failedTests_ += suiteResult.failedTests;
        
        // Store result
        suites_.push_back(suiteResult);
        
        std::cout << "  Duration: " << std::fixed << std::setprecision(3) 
                  << suiteDuration << "ms" << std::endl;
    }
    
    void generateReport() {
        auto endTime = std::chrono::steady_clock::now();
        double totalDuration = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
        
        std::cout << std::endl;
        std::cout << "=== NLM Comprehensive Test Suite Report ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Total Tests: " << totalTests_ << std::endl;
        std::cout << "Passed: " << passedTests_ << std::endl;
        std::cout << "Failed: " << failedTests_ << std::endl;
        std::cout << "Success Rate: " << (totalTests_ > 0 ? (passedTests_ * 100.0 / totalTests_) : 0.0) << "%" << std::endl;
        std::cout << "Total Duration: " << std::fixed << std::setprecision(3) << totalDuration << "ms" << std::endl;
        std::cout << std::endl;
        
        for (const auto& suite : suites_) {
            std::cout << suite.suiteName << ": " << suite.passedTests << "/" << suite.totalTests << " passed" << std::endl;
        }
        
        std::cout << std::endl;
        
        // Save report to file
        saveReportToFile();
        
        if (failedTests_ > 0) {
            std::cout << "=== SOME TESTS FAILED ===" << std::endl;
            return 1; // Indicate failure
        } else {
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
            return 0; // Indicate success
        }
    }
    
    void saveReportToFile() {
        try {
            std::string reportPath = "/tmp/nlm_test_report_" +
                std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".txt";
            
            std::ofstream file(reportPath, std::ios::out | std::ios::trunc);
            if (!file.is_open()) return;
            
            auto endTime = std::chrono::steady_clock::now();
            double totalDuration = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
            
            file << "=== NLM Comprehensive Test Suite Report ===" << std::endl;
            file << std::endl;
            file << "Total Tests: " << totalTests_ << std::endl;
            file << "Passed: " << passedTests_ << std::endl;
            file << "Failed: " << failedTests_ << std::endl;
            file << "Success Rate: " << (totalTests_ > 0 ? (passedTests_ * 100.0 / totalTests_) : 0.0) << "%" << std::endl;
            file << "Total Duration: " << std::fixed << std::setprecision(3) << totalDuration << "ms" << std::endl;
            file << std::endl;
            
            for (const auto& suite : suites_) {
                file << suite.suiteName << ": " << suite.passedTests << "/" << suite.totalTests << " passed" << std::endl;
            }
            
            file.close();
            
            std::cout << "Report saved to: " << reportPath << std::endl;
        } catch (...) {
            // Ignore file writing errors
        }
    }
    
private:
    std::vector<TestSuiteResult> suites_;
    int totalTests_;
    int passedTests_;
    int failedTests_;
    std::chrono::steady_clock::time_point startTime_;
};

// Global test runner instance
TestRunner testRunner;

// Test categories
enum class TestCategory {
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    COMPATIBILITY,
    STRESS
};

// Macro-like functions for test registration
#define REGISTER_TEST_SUITE(name, testFunction) \
    testRunner.runTestSuite(name, testFunction)

// Main test entry point
int main() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing all new features and enhancements" << std::endl;
    std::cout << "Run: " << std::chrono::steady_clock::now().time_since_epoch().count() << std::endl;
    std::cout << std::endl;
    
    try {
        // Unit Tests
        REGISTER_TEST_SUITE("Unit Tests - Configuration Management", 
                           []() { test_config_management::runAll(); });
        
        REGISTER_TEST_SUITE("Unit Tests - Performance Monitoring", 
                           []() { test_performance_monitoring::runAll(); });
        
        REGISTER_TEST_SUITE("Unit Tests - Experiment Management", 
                           []() { test_experiment_management::runAll(); });
        
        REGISTER_TEST_SUITE("Unit Tests - Advanced Neuron Tests", 
                           []() { test_neuron_advanced::runAll(); });
        
        REGISTER_TEST_SUITE("Unit Tests - Advanced STDP Tests", 
                           []() { test_stdp_advanced::runAll(); });
        
        REGISTER_TEST_SUITE("Unit Tests - Enhanced AgentBrain Tests", 
                           []() { test_agent_brain_enhanced::runAll(); });
        
        // Integration Tests
        REGISTER_TEST_SUITE("Integration Tests - System Integration", 
                           []() { test_system_integration::runAll(); });
        
        REGISTER_TEST_SUITE("Integration Tests - Python Bindings", 
                           []() { test_python_bindings::runAll(); });
        
        REGISTER_TEST_SUITE("Integration Tests - CLI Integration", 
                           []() { test_cli_integration::runAll(); });
        
        // Performance Tests
        REGISTER_TEST_SUITE("Performance Tests - Benchmarks", 
                           []() { test_performance_benchmarks::runAll(); });
        
        REGISTER_TEST_SUITE("Performance Tests - Scalability Tests", 
                           []() { test_scalability_tests::runAll(); });
        
        REGISTER_TEST_SUITE("Performance Tests - Memory Usage Tests", 
                           []() { test_memory_usage::runAll(); });
        
        // Compatibility Tests
        REGISTER_TEST_SUITE("Compatibility Tests - Backward Compatibility", 
                           []() { test_backward_compatibility::runAll(); });
        
        REGISTER_TEST_SUITE("Compatibility Tests - Data Format", 
                           []() { test_data_format::runAll(); });
        
        REGISTER_TEST_SUITE("Compatibility Tests - Configuration", 
                           []() { test_configuration::runAll(); });
        
        // Stress Tests
        REGISTER_TEST_SUITE("Stress Tests - Large Scale Simulation", 
                           []() { test_large_scale_simulation::runAll(); });
        
        REGISTER_TEST_SUITE("Stress Tests - Memory Extremes", 
                           []() { test_memory_extremes::runAll(); });
        
        REGISTER_TEST_SUITE("Stress Tests - Performance Extremes", 
                           []() { test_performance_extremes::runAll(); });
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST SUITE CRASH ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    return testRunner.generateReport();
}