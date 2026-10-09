// test_main.cpp - Main test runner for all Phase 6 tests

#include "test_memory_integration.cpp"
#include "test_prediction_system.cpp"
#include "test_cognition_system.cpp"
#include "test_neuromodulation_system.cpp"
#include "test_development_system.cpp"
#include "test_performance_infrastructure.cpp"
#include "test_error_handling.cpp"
#include "test_python_bindings.cpp"
#include "test_checkpoint_reset.cpp"
#include "test_phase6_integration.cpp"

#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Running Memory Integration Tests..." << std::endl;
    test_memory_integration::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Prediction System Tests..." << std::endl;
    test_prediction_system::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Cognition System Tests..." << std::endl;
    test_cognition_system::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Neuromodulation System Tests..." << std::endl;
    test_neuromodulation_system::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Development System Tests..." << std::endl;
    test_development_system::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Performance Infrastructure Tests..." << std::endl;
    test_performance_infrastructure::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Error Handling Tests..." << std::endl;
    test_error_handling::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Python Bindings Tests..." << std::endl;
    test_python_bindings::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Checkpoint/Reset Tests..." << std::endl;
    test_checkpoint_reset::runAll();
    std::cout << std::endl;
    
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    test_phase6_integration::runAll();
    std::cout << std::endl;
    
    std::cout << "=== All Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
