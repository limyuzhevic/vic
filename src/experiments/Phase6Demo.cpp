#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with enhanced reporting..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run the quick integration verification first
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward (average per step): " << result.totalReward << std::endl;
    std::cout << "Total accumulated reward: " << result.totalReward * config.maxSteps << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Enhanced metrics reporting
    std::cout << "\n=== ENHANCED METRICS ===" << std::endl;
    if (result.totalReward > 0.0f) {
        std::cout << "✓ POSITIVE LEARNING: Agent is learning from experience (total reward > 0)" << std::endl;
    } else if (result.totalReward < -0.1f) {
        std::cout << "⚠ NEGATIVE REINFORCEMENT: Agent is experiencing net negative consequences" << std::endl;
    } else {
        std::cout << "ℹ NEUTRAL LEARNING: Agent rewards are near zero (may need more exploration)" << std::endl;
    }
    
    std::cout << "Firing rate analysis: ";
    if (result.avgFiringRate > 100.0f) {
        std::cout << "HIGH (" << result.avgFiringRate << " Hz) - high activity level" << std::endl;
    } else if (result.avgFiringRate > 30.0f) {
        std::cout << "MODERATE (" << result.avgFiringRate << " Hz) - moderate activity level" << std::endl;
    } else {
        std::cout << "LOW (" << result.avgFiringRate << " Hz) - low activity level" << std::endl;
    }
    
    std::cout << "Neuromodulation analysis: ";
    if (result.dopamineLevel > 0.1f) {
        std::cout << "STRONG (" << result.dopamineLevel << ") - high reward prediction error" << std::endl;
    } else if (result.dopamineLevel < -0.1f) {
        std::cout << "STRONG (" << result.dopamineLevel << ") - high punishment prediction error" << std::endl;
    } else {
        std::cout << "STABLE (" << result.dopamineLevel << ") - stable prediction error" << std::endl;
    }
    
    return 0;
}