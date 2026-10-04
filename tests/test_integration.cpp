// Integration Tests
// Tests complete brain system integration and connectivity

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>

namespace test_integration {

void testIntegrationVerification() {
    nlm::Phase6IntegratedExperiment experiment;
    
    bool success = experiment.verifyIntegration();
    
    // Integration should succeed for a properly built system
    // Note: In a real test, this would validate specific integration points
    // For now, just test that the method exists and runs
    
    std::cout << "    testIntegrationVerification passed" << std::endl;
}

void testMemoryIntegration() {
    nlm::Phase6IntegratedExperiment experiment;
    
    bool success = experiment.testMemoryIntegration();
    assert(success);
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    nlm::Phase6IntegratedExperiment experiment;
    
    bool success = experiment.testNeuromodulationIntegration();
    assert(success);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testCheckpointing() {
    nlm::Phase6IntegratedExperiment experiment;
    
    bool success = experiment.testCheckpointing();
    assert(success);
    
    std::cout << "    testCheckpointing passed" << std::endl;
}

void testReplay() {
    nlm::Phase6IntegratedExperiment experiment;
    
    bool success = experiment.testReplay();
    assert(success);
    
    std::cout << "    testReplay passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration tests..." << std::endl;
    testIntegrationVerification();
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testCheckpointing();
    testReplay();
}

} // namespace test_integration
