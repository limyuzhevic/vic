// Comprehensive test runner for all enhanced tests
// This file includes all test categories to ensure comprehensive coverage

// Basic tests
#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_random.cpp"
#include "tests/test_types.cpp"
#include "tests/test_main.cpp"

// New test categories
#include "tests/test_config_validation.cpp"
#include "tests/test_error_handling.cpp"
#include "tests/test_integration.cpp"
#include "tests/test_performance.cpp"

// Main test runner
int main() {
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "COMPREHENSIVE TEST SUITE FOR IMPROVED NEURAL NETWORKS" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    
    std::cout << "Running ALL test categories..." << std::endl;
    std::cout << std::endl;
    
    // Run basic tests
    std::cout << "--- BASIC TESTS ---" << std::endl;
    test_config::runAll();
    test_brain::runAll();
    test_neuron::runAll();
    test_synapse::runAll();
    test_stdp::runAll();
    test_clock::runAll();
    test_random::runAll();
    test_types::runAll();
    test_main::runAll();
    std::cout << std::endl;
    
    // Run new test categories
    std::cout << "--- CONFIGURATION VALIDATION TESTS ---" << std::endl;
    test_config_validation::runAll();
    std::cout << std::endl;
    
    std::cout << "--- ERROR HANDLING TESTS ---" << std::endl;
    test_error_handling::runAll();
    std::cout << std::endl;
    
    std::cout << "--- INTEGRATION TESTS ---" << std::endl;
    test_integration::runAll();
    std::cout << std::endl;
    
    std::cout << "--- PERFORMANCE TESTS ---" << std::endl;
    test_performance::runAll();
    std::cout << std::endl;
    
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED SUCCESSFULLY!" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    return 0;
}