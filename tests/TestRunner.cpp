// ==============================================================================
// Simple Test Runner for NLM
// ==============================================================================
// This is the main entry point for running the test suite.
// It provides a simple command-line interface for running tests.
// ==============================================================================

#include "tests/TestFramework.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Test Runner ===" << std::endl;
    std::cout << "Running comprehensive test suite for NLM (熙然)" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    bool runAllTests = true;
    std::string reportFile = "test_report.txt";
    std::string testCategory = "all";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_test [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                    Show this help message" << std::endl;
            std::cout << "  --report <file>             Output test report to file" << std::endl;
            std::cout << "  --category <cat>             Run tests from specific category" << std::endl;
            std::cout << "  --quiet                      Run tests silently (no output)" << std::endl;
            std::cout << "  --list-categories           List all test categories" << std::endl;
            return 0;
        } else if (arg == "--list-categories") {
            std::cout << "Available test categories:" << std::endl;
            std::cout << "  Configuration - Tests for configuration system" << std::endl;
            std::cout << "  Brain        - Tests for brain functionality" << std::endl;
            std::cout << "  Benchmark    - Performance benchmarks" << std::endl;
            std::cout << "  General      - General purpose tests" << std::endl;
            return 0;
        } else if (arg == "--quiet") {
            // Run tests silently - we'll implement this by modifying the test framework
            // For now, we'll just note that quiet mode is requested
        } else if (arg.substr(0, 9) == "--report=") {
            reportFile = arg.substr(9);
        } else if (arg.substr(0, 12) == "--category=") {
            testCategory = arg.substr(12);
            runAllTests = false; // Filter by category
        }
    }
    
    // Run the tests
    try {
        nlm::test::runTests();
        
        std::cout << std::endl;
        std::cout << "Test execution completed." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred during test execution." << std::endl;
        return 1;
    }
}