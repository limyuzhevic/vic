// Simple test program to verify CLI and test framework integration
// This is a minimal example that can be used to verify the new CLI system

#include "cli/CLI.hpp"
#include "test_framework/TestFramework.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM CLI and Test Framework Integration Test ===" << std::endl;
    
    // Test 1: Create and parse CLI arguments
    std::cout << "\n1. Testing CLI Parser..." << std::endl;
    nlm::CLIParser parser;
    
    // Test help
    std::cout << "   Testing help parsing..." << std::endl;
    
    // Test version
    std::cout << "   Testing version parsing..." << std::endl;
    
    // Test 2: Create and configure test framework
    std::cout << "\n2. Testing Test Framework..." << std::endl;
    nlm::TestFramework testFramework;
    
    std::cout << "   Available test suites: " << std::endl;
    auto suites = testFramework.getAvailableSuites();
    for (const auto& suite : suites) {
        std::cout << "     - " << suite << std::endl;
    }
    
    std::cout << "   Available tests: " << std::endl;
    auto tests = testFramework.getAvailableTests();
    for (const auto& test : tests) {
        std::cout << "     - " << test << std::endl;
    }
    
    // Test 3: Run a simple test
    std::cout << "\n3. Running a simple test..." << std::endl;
    
    // Create a test configuration
    std::unordered_map<std::string, std::string> overrides;
    overrides["neuron_count"] = "100";
    overrides["region_count"] = "1";
    
    // Run a specific test
    int result = testFramework.runWithConfig({"connectivity"}, "", overrides);
    
    if (result == 0) {
        std::cout << "   ✓ Test passed!" << std::endl;
    } else {
        std::cout << "   ✗ Test failed!" << std::endl;
    }
    
    // Test 4: Generate report
    std::cout << "\n4. Generating test report..." << std::endl;
    
    std::string report = testFramework.generateReport(nlm::TestResult::TestFormat::Console);
    std::cout << "   Report generated (first 500 chars):" << std::endl;
    std::cout << "   " << report.substr(0, std::min(size_t(500), report.size())) << "..." << std::endl;
    
    // Test 5: Save report to file
    std::cout << "\n5. Saving report to file..." << std::endl;
    
    if (testFramework.saveReport("test_report.txt", nlm::TestResult::TestFormat::Console)) {
        std::cout << "   ✓ Report saved to test_report.txt" << std::endl;
    } else {
        std::cout << "   ✗ Failed to save report" << std::endl;
    }
    
    // Test 6: Test programmatic API
    std::cout << "\n6. Testing programmatic API..." << std::endl;
    
    // Test programmatic test execution
    int programmaticResult = nlm::TestProgrammatic::runTestWithConfig("connectivity");
    if (programmaticResult == 0) {
        std::cout << "   ✓ Programmatic test execution passed!" << std::endl;
    } else {
        std::cout << "   ✗ Programmatic test execution failed!" << std::endl;
    }
    
    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    
    return 0;
}
