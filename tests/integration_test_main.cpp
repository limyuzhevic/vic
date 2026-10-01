// Main integration test runner
// Runs all comprehensive integration tests for NLM connected systems

#include "memory/test_memory_integration.cpp"
#include "prediction/test_prediction_integration.cpp"
#include "cognition/test_cognition_integration.cpp"
#include "neuromodulation/test_neuromodulation_integration.cpp"
#include "system/test_system_integration.cpp"
#include "performance/test_performance.cpp"
#include <iostream>

void runAllMemoryTests() {
    std::cout << "=== Memory Integration Tests ===" << std::endl;
    test_memory_integration::runAll();
}

void runAllPredictionTests() {
    std::cout << "=== Prediction Integration Tests ===" << std::endl;
    test_prediction_integration::runAll();
}

void runAllCognitionTests() {
    std::cout << "=== Cognition Integration Tests ===" << std::endl;
    test_cognition_integration::runAll();
}

void runAllNeuromodulationTests() {
    std::cout << "=== Neuromodulation Integration Tests ===" << std::endl;
    test_neuromodulation_integration::runAll();
}

void runAllSystemTests() {
    std::cout << "=== System Integration Tests ===" << std::endl;
    test_system_integration::runAll();
}

void runAllPerformanceTests() {
    std::cout << "=== Performance and Stress Tests ===" << std::endl;
    test_performance::runAll();
}

int main() {
    std::cout << "NLM Comprehensive Integration Test Suite" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    bool allTestsPassed = true;
    
    try {
        runAllMemoryTests();
    } catch (const std::exception& e) {
        std::cerr << "Memory integration tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    try {
        runAllPredictionTests();
    } catch (const std::exception& e) {
        std::cerr << "Prediction integration tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    try {
        runAllCognitionTests();
    } catch (const std::exception& e) {
        std::cerr << "Cognition integration tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    try {
        runAllNeuromodulationTests();
    } catch (const std::exception& e) {
        std::cerr << "Neuromodulation integration tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    try {
        runAllSystemTests();
    } catch (const std::exception& e) {
        std::cerr << "System integration tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    try {
        runAllPerformanceTests();
    } catch (const std::exception& e) {
        std::cerr << "Performance tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    if (allTestsPassed) {
        std::cout << std::endl << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}