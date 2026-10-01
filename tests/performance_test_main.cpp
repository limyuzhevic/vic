// Performance test runner
// Runs performance and stress tests for NLM connected systems

#include "performance/test_performance.cpp"
#include <iostream>

int main() {
    std::cout << "NLM Performance and Stress Test Suite" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    bool allTestsPassed = true;
    
    try {
        test_performance::runAll();
    } catch (const std::exception& e) {
        std::cerr << "Performance tests failed: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    if (allTestsPassed) {
        std::cout << std::endl << "=== ALL PERFORMANCE TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "=== SOME PERFORMANCE TESTS FAILED ===" << std::endl;
        return 1;
    }
}