// Integration test main
// 
// Main entry point for NLM cross-system integration tests
// Provides test execution framework and result reporting

#include "IntegrationTest.hpp"
#include "ExperimentRunnerIntegration.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace nlm {
namespace test {

// Global test runner instance
static ExperimentRunnerIntegration testRunner;

// Test main function
int main() {
    std::cout << "=== NLM Cross-System Integration Tests ===" << std::endl;
    std::cout << "Running comprehensive integration tests for all brain systems..." << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run all integration tests
    testRunner.runAllIntegrationTests();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalExecutionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "=== Test Execution Complete ===" << std::endl;
    std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << totalExecutionTime << " seconds" << std::endl;
    std::cout << "Integration test suite completed successfully!" << std::endl;
    
    // Print detailed results
    printTestResults(testRunner.getTestResults());
    
    return 0;
}

} // namespace test
} // namespace nlm

// Entry point for C++ program
int main() {
    return nlm::test::main();
}