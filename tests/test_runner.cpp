// Main Test Runner
// Coordinates all unit tests for the NLM system

#include "tests/test_brain.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_plasticity.cpp"
#include "tests/test_memory.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_config.cpp"
#include "tests/test_agent_brain.cpp"
#include "tests/test_development.cpp"
#include "tests/test_neuromodulation.cpp"
#include "tests/test_integration.cpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== NLM Unit Test Suite ===" << std::endl;
    std::cout << "Testing NLM Phase 6 Integrated Artificial Brain" << std::endl;
    std::cout << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    
    try {
        // Run all test suites
        std::cout << "1. Running Brain tests..." << std::endl;
        test_brain::runAll();
        totalTests += 7;
        passedTests += 7;
        
        std::cout << "2. Running Synapse tests..." << std::endl;
        test_synapse::runAll();
        totalTests += 11;
        passedTests += 11;
        
        std::cout << "3. Running Plasticity tests..." << std::endl;
        test_plasticity::runAll();
        totalTests += 5;
        passedTests += 5;
        
        std::cout << "4. Running Memory tests..." << std::endl;
        test_memory::runAll();
        totalTests += 4;
        passedTests += 4;
        
        std::cout << "5. Running Neuron tests..." << std::endl;
        test_neuron::runAll();
        totalTests += 8;
        passedTests += 8;
        
        std::cout << "6. Running Config tests..." << std::endl;
        test_config::runAll();
        totalTests += 7;
        passedTests += 7;
        
        std::cout << "7. Running AgentBrain tests..." << std::endl;
        test_agent_brain::runAll();
        totalTests += 6;
        passedTests += 6;
        
        std::cout << "8. Running Development tests..." << std::endl;
        test_development::runAll();
        totalTests += 3;
        passedTests += 3;
        
        std::cout << "9. Running Neuromodulation tests..." << std::endl;
        test_neuromodulation::runAll();
        totalTests += 6;
        passedTests += 6;
        
        std::cout << "10. Running Integration tests..." << std::endl;
        test_integration::runAll();
        totalTests += 5;
        passedTests += 5;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR during testing: " << e.what() << std::endl;
        failedTests++;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception during testing" << std::endl;
        failedTests++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    // Print summary
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total test cases: " << totalTests << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    std::cout << "Total time: " << duration.count() << " seconds" << std::endl;
    std::cout << std::endl;
    
    if (failedTests == 0) {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ " << failedTests << " test(s) failed!" << std::endl;
        return 1;
    }
}
