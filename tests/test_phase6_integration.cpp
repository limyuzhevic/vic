// Phase 6 Integration Tests
// Comprehensive tests for Phase 6 system integration

#include "tests/Phase6IntegrationTests.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

namespace test_phase6_integration {

void testPhase6IntegrationVerification() {
    std::cout << "    Testing Phase 6 integration verification..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK);
    
    std::cout << "    testPhase6IntegrationVerification passed" << std::endl;
}

void testPhase6MemoryIntegration() {
    std::cout << "    Testing Phase 6 memory integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    experiment.testMemoryIntegration();
    
    std::cout << "    testPhase6MemoryIntegration passed" << std::endl;
}

void testPhase6NeuromodulationIntegration() {
    std::cout << "    Testing Phase 6 neuromodulation integration..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    experiment.testNeuromodulationIntegration();
    
    std::cout << "    testPhase6NeuromodulationIntegration passed" << std::endl;
}

void testPhase6Checkpointing() {
    std::cout << "    Testing Phase 6 checkpointing..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    experiment.testCheckpointing();
    
    std::cout << "    testPhase6Checkpointing passed" << std::endl;
}

void testPhase6Replay() {
    std::cout << "    Testing Phase 6 replay..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    experiment.testReplay();
    
    std::cout << "    testPhase6Replay passed" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testPhase6IntegrationVerification();
        testPhase6MemoryIntegration();
        testPhase6NeuromodulationIntegration();
        testPhase6Checkpointing();
        testPhase6Replay();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Phase 6 Integration Tests FAILED ===" << std::endl;
    }
}