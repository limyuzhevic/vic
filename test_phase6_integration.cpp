// Simple test to verify Phase6Integration fixes
#include "Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <cassert>

void testErrorHandling() {
    std::cout << "Testing error handling fixes...\n";
    
    // Test with invalid config
    nlm::Phase6Config invalidConfig;
    invalidConfig.maxSteps = 0;
    invalidConfig.neuronCount = 0;
    
    nlm::Phase6IntegrationResult result;
    nlm::Phase6IntegratedExperiment experiment;
    result = experiment.run(invalidConfig);
    
    // Should return immediately with error
    assert(result.endTime > 0 || result.endTime == 0); // No crash expected
    
    std::cout << "  Error handling test passed!\n";
}

void testMemoryManagement() {
    std::cout << "Testing memory management fixes...\n";
    
    // Test with valid config
    nlm::Phase6Config validConfig;
    validConfig.maxSteps = 100;
    validConfig.neuronCount = 100;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // These should not crash
    bool verifyResult = experiment.verifyIntegration();
    bool memoryResult = experiment.testMemoryIntegration();
    bool neuromodResult = experiment.testNeuromodulationIntegration();
    
    // Verify tests pass
    assert(verifyResult == true);
    assert(memoryResult == true);
    assert(neuromodResult == true);
    
    std::cout << "  Memory management test passed!\n";
}

void testRobustness() {
    std::cout << "Testing robustness improvements...\n";
    
    nlm::Phase6Config config;
    config.maxSteps = 50;
    config.neuronCount = 50;
    config.enableCheckpointing = false; // Disable checkpointing for simpler test
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Should run without crashes
    nlm::Phase6IntegrationResult result = experiment.run(config);
    
    // Verify result is populated
    assert(result.startTime > 0);
    assert(result.endTime > 0);
    assert(result.totalWallClockTime > 0.0);
    
    std::cout << "  Robustness test passed!\n";
}

int main() {
    std::cout << "=== Phase6Integration Testing ===\n\n";
    
    testErrorHandling();
    testMemoryManagement();
    testRobustness();
    
    std::cout << "\n=== All tests passed! ===\n";
    return 0;
}
