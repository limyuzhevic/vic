#include "tests/TestFramework.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

namespace test {

TestResult runTests(const std::string& testName) {
    std::cout << "=========================================" << std::endl;
    std::cout << "Running tests: " << testName << std::endl;
    std::cout << "=========================================" << std::endl;
    
    TestResult result;
    
    // Test basic types
    test_types(result);
    
    // Test neuron functionality
    test_neuron(result);
    
    // Test synapse functionality
    test_synapse(result);
    
    // Test brain functionality
    test_brain(result);
    
    // Test configuration system
    test_config(result);
    
    // Test plasticity mechanisms
    test_stdp(result);
    
    // Test memory systems
    test_working_memory(result);
    
    // Test neuromodulation
    test_neuromodulation(result);
    
    // Test prediction system
    test_prediction(result);
    
    return result;
}

void printTestSummary(const TestResult& result) {
    std::cout << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Test Summary" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Total Tests: " << result.totalTests << std::endl;
    std::cout << "Passed: " << result.passedTests << std::endl;
    std::cout << "Failed: " << result.failedTests << std::endl;
    std::cout << "Success Rate: " << (result.passedTests * 100.0 / result.totalTests) << "%" << std::endl;
    
    if (result.failedTests > 0) {
        std::cout << std::endl;
        std::cout << "FAILED TESTS:" << std::endl;
        for (const auto& failed : result.failedTestNames) {
            std::cout << "  - " << failed << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Individual test results:" << std::endl;
    for (const auto& test : result.testResults) {
        std::cout << "  " << (test.passed ? "✓" : "✗") << " " << test.name << ": " << (test.message.empty() ? "passed" : test.message) << std::endl;
    }
}

bool shouldRunTests(const std::vector<std::string>& testNames, const std::string& filter) {
    if (filter.empty()) return true;
    
    for (const auto& name : testNames) {
        if (name.find(filter) != std::string::npos) return true;
    }
    
    return false;
}

} // namespace test

// Test main entry point
int main(int argc, char** argv) {
    std::string testFilter;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--filter") {
            if (arg.find('=') != std::string::npos) {
                testFilter = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                testFilter = argv[++i];
            }
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_test [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --filter <name>   Filter tests by name" << std::endl;
            std::cout << "  --list             List all available tests" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            return 0;
        } else if (arg == "--list") {
            std::cout << "Available tests:" << std::endl;
            std::cout << "  types - Basic type tests" << std::endl;
            std::cout << "  neuron - Neuron functionality tests" << std::endl;
            std::cout << "  synapse - Synapse functionality tests" << std::endl;
            std::cout << "  brain - Brain integration tests" << std::endl;
            std::cout << "  config - Configuration system tests" << std::endl;
            std::cout << "  stdp - STDP plasticity tests" << std::endl;
            std::cout << "  working_memory - Working memory tests" << std::endl;
            std::cout << "  neuromodulation - Neuromodulation tests" << std::endl;
            std::cout << "  prediction - Prediction system tests" << std::endl;
            return 0;
        }
    }
    
    // Run tests
    test::TestResult result = test::runTests("NLM Comprehensive Test Suite");
    
    // Print summary
    test::printTestSummary(result);
    
    // Return exit code based on test results
    if (result.failedTests > 0) {
        return 1; // Some tests failed
    }
    
    return 0; // All tests passed
}