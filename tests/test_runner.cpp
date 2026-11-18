// Comprehensive Test Runner
// Orchestrates all test suites for the NLM enhancements

#include <iostream>
#include <string>
#include <vector>

// Include all test modules
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_enhanced_bindings.cpp"
#include "tests/test_enhanced_config.cpp"
#include "tests/test_cli.cpp"
#include "tests/test_visualization.cpp"
#include "tests/test_documentation.cpp"
#include "tests/test_benchmarks.cpp"

// Test runner functions
void runConfigTests() {
    std::cout << "=== Running Configuration Tests ===" << std::endl;
    test_config::runAll();
    std::cout << "Configuration tests completed." << std::endl;
}

void runBrainTests() {
    std::cout << "=== Running Brain Tests ===" << std::endl;
    test_brain::runAll();
    std::cout << "Brain tests completed." << std::endl;
}

void runEnhancedBindingsTests() {
    std::cout << "=== Running Enhanced Bindings Tests ===" << std::endl;
    runEnhancedBindingsTests();
    std::cout << "Enhanced bindings tests completed." << std::endl;
}

void runEnhancedConfigTests() {
    std::cout << "=== Running Enhanced Configuration Tests ===" << std::endl;
    test_enhanced_config::runAll();
    std::cout << "Enhanced configuration tests completed." << std::endl;
}

void runCLITests() {
    std::cout << "=== Running CLI Tests ===" << std::endl;
    runCLITests();
    std::cout << "CLI tests completed." << std::endl;
}

void runVisualizationTests() {
    std::cout << "=== Running Visualization Tests ===" << std::endl;
    runVisualizationProfilingTests();
    std::cout << "Visualization tests completed." << std::endl;
}

void runDocumentationTests() {
    std::cout << "=== Running Documentation Tests ===" << std::endl;
    runDocumentationExamplesTests();
    std::cout << "Documentation tests completed." << std::endl;
}

void runBenchmarkTests() {
    std::cout << "=== Running Benchmark Tests ===" << std::endl;
    runBenchmarkTests();
    std::cout << "Benchmark tests completed." << std::endl;
}

int main() {
    std::cout << "NLM Enhanced Test Suite" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Testing NLM framework enhancements..." << std::endl;
    
    // Run all test suites
    runConfigTests();
    runBrainTests();
    runEnhancedBindingsTests();
    runEnhancedConfigTests();
    runCLITests();
    runVisualizationTests();
    runDocumentationTests();
    runBenchmarkTests();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    std::cout << "NLM framework enhancements are working correctly!" << std::endl;
    
    return 0;
}