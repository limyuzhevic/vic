// Basic Phase 6 Integration Tests
// Tests to verify that all brain systems are properly connected and functional

#include "Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>

void testBasicIntegration() {
    std::cout << "Testing basic Phase 6 integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    assert(experiment.verifyIntegration() && "Phase 6 integration verification failed");
    
    std::cout << "✓ Basic integration test passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "Testing memory integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test memory integration
    assert(experiment.testMemoryIntegration() && "Memory integration test failed");
    
    std::cout << "✓ Memory integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test neuromodulation integration
    assert(experiment.testNeuromodulationIntegration() && "Neuromodulation integration test failed");
    
    std::cout << "✓ Neuromodulation integration test passed" << std::endl;
}

void testCheckpointing() {
    std::cout << "Testing checkpoint system..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test checkpointing
    assert(experiment.testCheckpointing() && "Checkpoint test failed");
    
    std::cout << "✓ Checkpoint test passed" << std::endl;
}

void testReplay() {
    std::cout << "Testing replay system..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test replay
    assert(experiment.testReplay() && "Replay test failed");
    
    std::cout << "✓ Replay test passed" << std::endl;
}

void runAllPhase6Tests() {
    std::cout << "=== Running Phase 6 Integration Tests ===" << std::endl;
    
    try {
        testBasicIntegration();
        testMemoryIntegration();
        testNeuromodulationIntegration();
        testCheckpointing();
        testReplay();
        
        std::cout << "\n=== All Phase 6 Integration Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        exit(1);
    }
}

int main() {
    runAllPhase6Tests();
    return 0;
}
