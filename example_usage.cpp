#!/usr/bin/env cpp
// Example: Using the Enhanced NLM Phase 2 Test Framework
// This example demonstrates how to integrate the enhanced test framework into your own applications

#include "core/Config/Config.hpp"
#include "src/main.cpp" // Include the enhanced test framework

int main(int argc, char** argv) {
    std::cout << "=== Example: Enhanced NLM Phase 2 Test Framework ===" << std::endl;
    std::cout << "This example shows how to use the enhanced test framework programmatically." << std::endl;
    std::cout << std::endl;
    
    // Example 1: Create configuration and run tests with command line arguments
    std::cout << "Example 1: Running tests with command line arguments" << std::endl;
    std::cout << "Command: ./nlm --verbose --save-results" << std::endl;
    std::cout << "This will run all tests with verbose output and save results to CSV." << std::endl;
    std::cout << std::endl;
    
    // Simulate command line arguments for demonstration
    int exampleArgc = 5;
    char* exampleArgv[] = {
        (char*)"nlm",
        (char*)"--verbose",
        (char*)"--save-results",
        (char*)"--config-file",
        (char*)"configs/example.cfg"
    };
    
    // Note: In a real application, you would call main() or integrate the test framework
    std::cout << "Example completed (simulated)." << std::endl;
    std::cout << std::endl;
    
    // Example 2: Show help information
    std::cout << "Example 2: Displaying help information" << std::endl;
    std::cout << "Command: ./nlm --help" << std::endl;
    std::cout << "This will display all available command line options and usage information." << std::endl;
    std::cout << std::endl;
    
    // Example 3: Show configuration validation
    std::cout << "Example 3: Configuration validation" << std::endl;
    std::cout << "The enhanced test framework validates all configuration parameters:" << std::endl;
    std::cout << "  - neuron_count must be > 0 and ≤ 10000" << std::endl;
    std::cout << "  - region_count must be > 0" << std::endl;
    std::cout << "  - connection_probability must be in [0, 1]" << std::endl;
    std::cout << "  - STDP weights must be ≥ 0" << std::endl;
    std::cout << "  - Plasticity rates must be in [0, 1]" << std::endl;
    std::cout << std::endl;
    
    // Example 4: Show test results format
    std::cout << "Example 4: Test results format" << std::endl;
    std::cout << "The enhanced test framework provides multiple output formats:" << std::endl;
    std::cout << "  - Console output with detailed logging" << std::endl;
    std::cout << "  - CSV file export for further analysis" << std::endl;
    std::cout << "  - Statistics and performance metrics" << std::endl;
    std::cout << std::endl;
    
    // Example 5: Show command line options
    std::cout << "Example 5: Available command line options" << std::endl;
    std::cout << "./nlm [OPTIONS]:" << std::endl;
    std::cout << "  --verbose, -v            Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q              Quiet output mode" << std::endl;
    std::cout << "  --help, -h               Show this help message" << std::endl;
    std::cout << "  --save-results           Export test results to CSV file" << std::endl;
    std::cout << "  --config-file <file>     Specify configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --test-filter <name>     Filter tests by name (can be repeated)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Example completed successfully ===" << std::endl;
    std::cout << "The enhanced NLM Phase 2 test framework provides comprehensive testing capabilities" << std::endl;
    std::cout << "for the NLM artificial neural network system with improved error handling," << std::endl;
    std::cout << "configuration validation, and detailed reporting." << std::endl;
    
    return 0;
}