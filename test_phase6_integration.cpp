#!/usr/bin/env cpp
// Phase 6 Integration Validation Test
// Validates that all Phase 6 integration components work correctly

#include "Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Phase 6 Integration Validation Test ===" << std::endl;
    
    // Create Phase 6 integrated experiment configuration
    Phase6Config config;
    config.neuronCount = 500;
    config.regionCount = 2;
    config.connectionProbability = 0.15f;
    config.maxSteps = 2000;
    config.enableDevelopment = true;
    config.enableCheckpointing = true;
    config.checkpointPath = "./test_checkpoint.bin";
    
    // Create and run experiment
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(config);
    
    std::cout << "\n=== Experiment Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    // Validate integration verification
    std::cout << "\n=== Integration Verification ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "YES" : "NO") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "YES" : "NO") << std::endl;
    
    // Test assertions
    bool allTestsPass = true;
    
    // Integration tests
    assert(result.memoryWorkingMemoryIntegrated && "Working memory should be integrated");
    assert(result.memoryEpisodicMemoryIntegrated && "Episodic memory should be integrated");
    assert(result.neuromodulationIntegrated && "Neuromodulation should be integrated");
    assert(result.predictionIntegrated && "Prediction should be integrated");
    assert(result.developmentIntegrated && "Development should be integrated");
    assert(result.checkpointingWorks && "Checkpointing should work");
    
    // Performance thresholds
    assert(result.totalReward > 0.5f && "Should achieve meaningful reward");
    assert(result.memoryEpisodesStored > 50 && "Should store multiple episodes");
    assert(result.avgFiringRate > 1.0f && "Should have reasonable firing rates");
    
    std::cout << "\n=== All Integration Tests PASSED ===" << std::endl;
    std::cout << "Phase 6 integration successfully completed!" << std::endl;
    
    return 0;
}
