// Comprehensive NLM Framework Test Suite
// Tests all 10 areas of edge cases and error scenarios

#include "tests/test_config.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_config_edge.cpp"
#include "tests/test_brain_edge.cpp"
#include "tests/test_neuron_edge.cpp"
#include "tests/test_memory_edge.cpp"
#include "tests/test_error_handling.cpp"
#include "tests/test_timing_edge.cpp"
#include "tests/test_plasticity_edge.cpp"
#include "tests/test_boundary.cpp"
#include "tests/test_memory_safety.cpp"
#include "tests/test_performance.cpp"
#include "tests/test_integration.cpp"

int main() {
    std::cout << "=== NLM Framework Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing all 10 areas of edge cases and error scenarios..." << std::endl << std::endl;
    
    // Run all configuration tests
    std::cout << "1. Configuration Edge Cases" << std::endl;
    std::cout << "------------------------" << std::endl;
    test_config::runAll();
    
    // Run comprehensive configuration edge cases
    std::cout << std::endl << "1b. Advanced Configuration Edge Cases" << std::endl;
    std::cout << "------------------------------" << std::endl;
    test_config_edge::runAll();
    
    // Run all brain tests
    std::cout << std::endl << "2. Brain Initialization Edge Cases" << std::endl;
    std::cout << "------------------------------" << std::endl;
    test_brain::runAll();
    
    // Run comprehensive brain edge cases
    std::cout << std::endl << "2b. Advanced Brain Edge Cases" << std::endl;
    std::cout << "--------------------------" << std::endl;
    test_brain_edge::runAll();
    
    // Run all neuron tests
    std::cout << std::endl << "3. Neuron and Population Tests" << std::endl;
    std::cout << "------------------------" << std::endl;
    test_neuron::runAll();
    
    // Run comprehensive neuron edge cases
    std::cout << std::endl << "3b. Advanced Neuron Edge Cases" << std::endl;
    std::cout << "--------------------------" << std::endl;
    test_neuron_edge::runAll();
    
    // Run memory and resource edge cases
    std::cout << std::endl << "4. Memory and Resource Edge Cases" << std::endl;
    std::cout << "---------------------------" << std::endl;
    test_memory_edge::runAll();
    
    // Run error handling tests
    std::cout << std::endl << "5. Error Handling Tests" << std::endl;
    std::cout << "--------------------" << std::endl;
    test_error_handling::runAll();
    
    // Run timing and synchronization edge cases
    std::cout << std::endl << "6. Timing and Synchronization Edge Cases" << std::endl;
    std::cout << "------------------------------" << std::endl;
    test_timing_edge::runAll();
    
    // Run plasticity and neuromodulation edge cases
    std::cout << std::endl << "7. Plasticity and Neuromodulation Edge Cases" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    test_plasticity_edge::runAll();
    
    // Run boundary conditions tests
    std::cout << std::endl << "8. Boundary Conditions Tests" << std::endl;
    std::cout << "--------------------" << std::endl;
    test_boundary::runAll();
    
    // Run memory corruption and safety tests
    std::cout << std::endl << "9. Memory Corruption and Safety Tests" << std::endl;
    std::cout << "------------------------" << std::endl;
    test_memory_safety::runAll();
    
    // Run performance stress tests
    std::cout << std::endl << "10. Performance Stress Tests" << std::endl;
    std::cout << "--------------------" << std::endl;
    test_performance::runAll();
    
    // Run integration tests
    std::cout << std::endl << "11. Integration Tests" << std::endl;
    std::cout << "------------" << std::endl;
    test_integration::runAll();
    
    std::cout << std::endl << "=== All Tests Completed Successfully ===" << std::endl;
    std::cout << "Total test suite executed with comprehensive edge case coverage!" << std::endl;
    
    return 0;
}
