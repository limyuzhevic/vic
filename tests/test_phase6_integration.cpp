// Phase 6 Integration Tests
// Tests for the integrated brain systems

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_phase6 {

void testPhase6IntegrationVerification() {
    std::cout << "    Testing Phase 6 integration verification..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    bool integrationOK = experiment.verifyIntegration();
    assert(integrationOK && "Phase 6 integration verification failed");
    
    std::cout << "    Phase 6 integration verification passed!" << std::endl;
}

void testPhase6MemoryIntegration() {
    std::cout << "    Testing Phase 6 memory integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test memory system integration
    bool memoryIntegrated = experiment.testMemoryIntegration();
    assert(memoryIntegrated && "Memory integration test failed");
    
    std::cout << "    Phase 6 memory integration passed!" << std::endl;
}

void testPhase6NeuromodulationIntegration() {
    std::cout << "    Testing Phase 6 neuromodulation integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test neuromodulation system integration
    bool neuromodulationIntegrated = experiment.testNeuromodulationIntegration();
    assert(neuromodulationIntegrated && "Neuromodulation integration test failed");
    
    std::cout << "    Phase 6 neuromodulation integration passed!" << std::endl;
}

void testPhase6Checkpointing() {
    std::cout << "    Testing Phase 6 checkpointing..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test checkpoint system
    bool checkpointingWorks = experiment.testCheckpointing();
    assert(checkpointingWorks && "Checkpointing test failed");
    
    std::cout << "    Phase 6 checkpointing passed!" << std::endl;
}

void testPhase6Replay() {
    std::cout << "    Testing Phase 6 replay..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Test replay system
    bool replayWorks = experiment.testReplay();
    assert(replayWorks && "Replay test failed");
    
    std::cout << "    Phase 6 replay passed!" << std::endl;
}

void testPhase6DevelopmentIntegration() {
    std::cout << "    Testing Phase 6 development integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    
    // Verify development affects plasticity rates
    nlm::Phase6Config config;
    config.neuronCount = 50;
    config.maxSteps = 500;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    assert(result.developmentIntegrated && "Development not integrated");
    
    std::cout << "    Phase 6 development integration passed!" << std::endl;
}

void testPhase6FullIntegration() {
    std::cout << "    Testing Phase 6 full integration..." << std::endl;
    
    nlm::Phase6IntegratedExperiment experiment;
    nlm::Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 1000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    // Verify all systems are properly integrated
    assert(result.memoryWorkingMemoryIntegrated && "Working memory not integrated");
    assert(result.memoryEpisodicMemoryIntegrated && "Episodic memory not integrated");
    assert(result.neuromodulationIntegrated && "Neuromodulation not integrated");
    assert(result.predictionIntegrated && "Prediction not integrated");
    assert(result.developmentIntegrated && "Development not integrated");
    assert(result.checkpointingWorks && "Checkpointing not working");
    assert(result.replayWorks && "Replay not working");
    
    // Verify reasonable performance
    assert(result.totalReward >= 0.0f && "Negative reward unexpected");
    assert(result.memoryEpisodesStored >= 0.0f && "Negative episodes stored");
    
    std::cout << "    Phase 6 full integration passed!" << std::endl;
    std::cout << "    Total reward: " << result.totalReward << std::endl;
    std::cout << "    Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
}

} // namespace test_phase6
