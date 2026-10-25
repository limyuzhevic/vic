// Integration Test Executable
// This executable tests all the new advanced features added to NLM

#include "src/tests/ComprehensiveIntegrationTest.cpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool runQuickTest = false;
    bool runFullTest = false;
    bool helpRequested = false;
    
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "quick") {
            runQuickTest = true;
        } else if (arg == "full") {
            runFullTest = true;
        } else if (arg == "help" || arg == "--help" || arg == "-h") {
            helpRequested = true;
        }
    }
    
    if (helpRequested) {
        std::cout << "=== NLM Integration Test Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: integration_test [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  quick    Run quick integration tests (faster, fewer features)" << std::endl;
        std::cout << "  full     Run full integration tests (comprehensive, slower)" << std::endl;
        std::cout << "  help     Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "If no option is specified, full integration tests are run." << std::endl;
        return 0;
    }
    
    // Run integration tests
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << "Running comprehensive integration tests..." << std::endl;
    std::cout << std::endl;
    
    nlm::ComprehensiveIntegrationTest integrationTest;
    
    if (runQuickTest) {
        std::cout << "Note: Quick mode not fully implemented in this version." << std::endl;
        std::cout << "Running full integration tests instead..." << std::endl;
    }
    
    auto results = integrationTest.runAllTests();
    
    // Print final results
    std::cout << std::endl << "=== Final Results ===" << std::endl;
    
    size_t passedTests = 0;
    size_t failedTests = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            passedTests++;
            std::cout << "✓ " << result.testName << ": PASSED" << std::endl;
        } else {
            failedTests++;
            std::cout << "✗ " << result.testName << ": FAILED - " << result.errorMessage << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Summary: " << passedTests << " passed, " << failedTests << " failed" << std::endl;
    
    if (failedTests == 0) {
        std::cout << "All integration tests passed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << "Some integration tests failed." << std::endl;
        return 1;
    }
}