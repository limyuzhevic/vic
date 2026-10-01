#include "tests/IntegrationTestRunner.hpp"
#include "tests/TestConfig.hpp"
#include "tests/TestPlasticityRules.hpp"
#include "tests/TestMemorySystems.hpp"
#include "tests/TestDebugInterface.hpp"

void runAllIntegrationTests() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    std::cout << "Running comprehensive tests for all improved systems..." << std::endl;
    std::cout << std::endl;
    
    // Initialize test results
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    
    try {
        // Test 1: Config System
        std::cout << "1. Testing Config System (JSON/YAML support, enhanced API)..." << std::endl;
        totalTests += runConfigTests();
        passedTests += getConfigPassedCount();
        failedTests += getConfigFailedCount();
        std::cout << "   Config tests completed: " << getConfigPassedCount() << "/" << totalTests << " passed" << std::endl;
        std::cout << std::endl;
        
        // Test 2: Plasticity Rules
        std::cout << "2. Testing Plasticity Rules (Hebbian, STDP, R-STDP, TripletSTDP, BCM)..." << std::endl;
        totalTests += runPlasticityTests();
        passedTests += getPlasticityPassedCount();
        failedTests += getPlasticityFailedCount();
        std::cout << "   Plasticity tests completed: " << getPlasticityPassedCount() << "/" << totalTests << " passed" << std::endl;
        std::cout << std::endl;
        
        // Test 3: Memory Systems
        std::cout << "3. Testing Memory Systems (Working Memory, Episodic Memory, Associative Memory)..." << std::endl;
        totalTests += runMemoryTests();
        passedTests += getMemoryPassedCount();
        failedTests += getMemoryFailedCount();
        std::cout << "   Memory tests completed: " << getMemoryPassedCount() << "/" << totalTests << " passed" << std::endl;
        std::cout << std::endl;
        
        // Test 4: Debug Interface
        std::cout << "4. Testing Debug Interface (system status, connectivity reports, activity monitoring)..." << std::endl;
        totalTests += runDebugTests();
        passedTests += getDebugPassedCount();
        failedTests += getDebugFailedCount();
        std::cout << "   Debug tests completed: " << getDebugPassedCount() << "/" << totalTests << " passed" << std::endl;
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR during integration tests: " << e.what() << std::endl;
        failedTests++;
    }
    
    // Final results
    std::cout << "=== Integration Test Results ===" << std::endl;
    std::cout << "Total tests: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    std::cout << std::endl;
    
    if (failedTests == 0 && totalTests > 0) {
        std::cout << "✓ ALL INTEGRATION TESTS PASSED!" << std::endl;
        std::cout << "The NLM improvements are working correctly." << std::endl;
    } else {
        std::cout << "✗ SOME INTEGRATION TESTS FAILED" << std::endl;
        std::cout << "Please review the failed tests above." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Summary of Improvements ===" << std::endl;
    std::cout << "1. Enhanced Config System: JSON/YAML support, automatic format detection" << std::endl;
    std::cout << "2. Cleaned Plasticity System: unified STDP, Hebbian, and added R-STDP, TripletSTDP, BCM" << std::endl;
    std::cout << "3. Implemented NeuralAssociativeMemory: pattern-based memory associations" << std::endl;
    std::cout << "4. Created PlasticityFactory: rule registration and management system" << std::endl;
    std::cout << "5. Added DebugInterface: comprehensive monitoring and debugging tools" << std::endl;
    std::cout << "6. Improved redundancy: removed duplicate plasticity rule implementations" << std::endl;
    std::cout << "7. Enhanced error handling: better exception management and validation" << std::endl;
    std::cout << "8. Added extensive test coverage: integration tests for all new features" << std::endl;
}

int main() {
    std::cout << "NLM Integration Test Suite" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << std::endl;
    
    runAllIntegrationTests();
    
    return (getConfigFailedCount() + getPlasticityFailedCount() + 
            getMemoryFailedCount() + getDebugFailedCount()) > 0 ? 1 : 0;
}