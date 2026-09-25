// Main test runner for NLM
// Includes all test categories: unit tests, integration tests, and error condition tests

#include <iostream>
#include <cassert>
#include "test_main.cpp"
#include "integration_tests.cpp"
#include "error_tests.cpp"

// Forward declarations for all test namespaces
namespace test_types { void runAll(); }
namespace test_config { void runAll(); }
namespace test_random { void runAll(); }
namespace test_clock { void runAll(); }
namespace test_neuron { void runAll(); }
namespace test_synapse { void runAll(); }
namespace test_stdp { void runAll(); }
namespace test_brain { void runAll(); }

namespace integration_tests { bool runAllIntegrationTests(); }
namespace error_tests { bool runAllErrorTests(); }

int main() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Phase 6: Final Integration - Complete System Testing" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run Phase 2 Unit Tests (existing tests)
    std::cout << "=== Phase 2 Unit Tests ===" << std::endl;
    try {
        std::cout << "Running Types tests..." << std::endl;
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Config tests..." << std::endl;
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Random tests..." << std::endl;
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Clock tests..." << std::endl;
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Neuron tests..." << std::endl;
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Synapse tests..." << std::endl;
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running STDP tests..." << std::endl;
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    try {
        std::cout << "Running Brain tests..." << std::endl;
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run Phase 6 Integration Tests
    try {
        bool integrationPassed = integration_tests::runAllIntegrationTests();
        if (!integrationPassed) {
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Run Error Condition Tests
    try {
        bool errorPassed = error_tests::runAllErrorTests();
        if (!errorPassed) {
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Final summary
    std::cout << "=== Test Suite Summary ===" << std::endl;
    if (allPassed) {
        std::cout << "✓ All tests PASSED!" << std::endl;
        std::cout << "✓ The NLM Phase 6 integration is working correctly" << std::endl;
        std::cout << "✓ All systems (memory, neuromodulation, development, prediction) are integrated" << std::endl;
        std::cout << "✓ Error handling is robust and comprehensive" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some tests FAILED!" << std::endl;
        std::cout << "✗ Please review the failures above" << std::endl;
        return 1;
    }
}
