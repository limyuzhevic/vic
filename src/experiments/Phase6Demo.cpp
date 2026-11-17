/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
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
    std::cout << "=== DETAILED REWARD ANALYSIS ===" << std::endl;
    std::cout << "Total reward average: " << result.totalReward << " per step" << std::endl;
    std::cout << "Max reward per step: " << result.maxRewardPerStep << std::endl;
    std::cout << "Min reward per step: " << result.minRewardPerStep << std::endl;
    std::cout << "Reward standard deviation: " << result.rewardStdDev << std::endl;
    std::cout << "Steps with positive reward: " << result.stepsWithPositiveReward << " (" 
              << (result.totalStepsExecuted > 0 ? result.stepsWithPositiveReward / result.totalStepsExecuted * 100 : 0) << "%)" << std::endl;
    std::cout << "Steps with negative reward: " << result.stepsWithNegativeReward << " (" 
              << (result.totalStepsExecuted > 0 ? result.stepsWithNegativeReward / result.totalStepsExecuted * 100 : 0) << "%)" << std::endl;
    std::cout << "Steps with zero reward: " << result.stepsWithZeroReward << " (" 
              << (result.totalStepsExecuted > 0 ? result.stepsWithZeroReward / result.totalStepsExecuted * 100 : 0) << "%)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== BEHAVIORAL METRICS ===" << std::endl;
    std::cout << "Steps executed: " << result.totalStepsExecuted << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== LEARNING PERFORMANCE ===" << std::endl;
    // Calculate learning score based on reward variance and exploration
    float learningScore = 0.0f;
    if (result.stepsWithPositiveReward > 0 && result.stepsWithNegativeReward > 0) {
        learningScore += 50.0f; // Good balance of rewards
    }
    if (result.curiosityLevel > 0.2f) learningScore += 20.0f; // Active exploration
    if (result.dopamineLevel > 0.0f) learningScore += 15.0f; // Active learning
    if (result.stepsWithZeroReward < result.totalStepsExecuted * 0.8f) learningScore += 15.0f; // Not stuck on zero
    
    std::cout << "Learning score: " << learningScore << "/100" << std::endl;
    
    if (learningScore > 50) {
        std::cout << "✓ Good learning behavior detected" << std::endl;
    } else if (learningScore > 25) {
        std::cout << "⚠ Limited learning behavior" << std::endl;
    } else {
        std::cout << "✗ Poor learning behavior - reward system may be broken" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=== SYSTEM INTEGRATION ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
