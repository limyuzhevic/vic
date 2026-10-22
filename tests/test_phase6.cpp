// Phase 6 Integration Tests
// Tests for the integrated brain systems from Phase 6

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>

namespace test_phase6 {

void testPhase6IntegrationVerification() {
    std::cout << "  Testing Phase 6 integration verification..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Simple verification test
    bool integrationOK = experiment.verifyIntegration();
    
    assert(integrationOK);
    
    std::cout << "    testPhase6IntegrationVerification passed" << std::endl;
}

void testPhase6MemoryIntegration() {
    std::cout << "  Testing Phase 6 memory integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    bool memoryOK = experiment.testMemoryIntegration();
    
    assert(memoryOK);
    
    std::cout << "    testPhase6MemoryIntegration passed" << std::endl;
}

void testPhase6NeuromodulationIntegration() {
    std::cout << "  Testing Phase 6 neuromodulation integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    
    assert(neuromodulationOK);
    
    std::cout << "    testPhase6NeuromodulationIntegration passed" << std::endl;
}

void testPhase6Checkpointing() {
    std::cout << "  Testing Phase 6 checkpointing..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    bool checkpointingOK = experiment.testCheckpointing();
    
    assert(checkpointingOK);
    
    std::cout << "    testPhase6Checkpointing passed" << std::endl;
}

void testPhase6Replay() {
    std::cout << "  Testing Phase 6 replay system..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    bool replayOK = experiment.testReplay();
    
    assert(replayOK);
    
    std::cout << "    testPhase6Replay passed" << std::endl;
}

void testPhase6CompleteIntegration() {
    std::cout << "  Testing complete Phase 6 integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    nlm::Phase6Config config;
    
    // Use smaller settings for faster testing
    config.maxSteps = 200;
    config.neuronCount = 200;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./test_checkpoint.bin";
    
    nlm::Phase6IntegrationResult result = experiment.run(config);
    
    // Verify that systems are integrated
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.developmentIntegrated);
    assert(result.checkpointingWorks);
    assert(result.replayWorks);
    
    // Verify that metrics are reasonable
    assert(result.totalReward >= 0.0f);
    assert(result.avgFiringRate >= 0.0f);
    assert(result.memoryEpisodesStored >= 0.0f);
    
    std::cout << "    testPhase6CompleteIntegration passed" << std::endl;
    
    // Clean up test checkpoint file
    std::remove("./test_checkpoint.bin");
}

} // namespace test_phase6