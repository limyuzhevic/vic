/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

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
    
    // Enhanced config with reward learning optimization
    config.enableRewardLearning = true;  // Enable explicit reward learning
    config.rewardDecayRate = 0.5f;       // Balance of recent vs older rewards
    config.minLearningSteps = 100;      // Minimum steps to ensure meaningful learning
    config.maxNoProgressSteps = 200;    // Max steps without progress before reporting
    
    auto result = experiment.run(config);
    
    // Validate and enhance the result reporting
    validateIntegrationResults(result, experiment);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    // Enhanced integration validation and reporting
    validateIntegrationResults(result, experiment);
    
    std::cout << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}

// Enhanced integration result validation and reporting
void validateIntegrationResults(const Phase6IntegrationResult& result, Phase6IntegratedExperiment& experiment) {
    std::cout << "=== ENHANCED INTEGRATION VALIDATION ===" << std::endl;
    
    // Validate minimum reward threshold for meaningful learning
    bool hasMeaningfulLearning = result.totalReward > 0.0f;
    bool hasStablePerformance = result.avgFiringRate > 0.0f && result.avgFiringRate < 100.0f;
    bool hasMemoryStorage = result.memoryEpisodesStored > 0;
    bool hasNeuromodulation = result.dopamineLevel > 0.0f;
    
    std::cout << "=== LEARNING VALIDATION ===" << std::endl;
    std::cout << "Meaningful learning achieved: " << (hasMeaningfulLearning ? "YES" : "NO") << std::endl;
    std::cout << "(Reward > 0 indicates successful adaptation and learning)" << std::endl;
    
    if (!hasMeaningfulLearning) {
        std::cout << "WARNING: Total reward is 0 - the brain may not be learning effectively" << std::endl;
        std::cout << "This could indicate:" << std::endl;
        std::cout << "  - Reward signals not properly integrated" << std::endl;
        std::cout << "  - Neural connections not properly established" << std::endl;
        std::cout << "  - Task may not be appropriately challenging" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Validate performance metrics
    std::cout << "=== PERFORMANCE VALIDATION ===" << std::endl;
    std::cout << "Stable firing rate (0-100 Hz): " << (hasStablePerformance ? "YES" : "NO") << std::endl;
    std::cout << "  Current rate: " << result.avgFiringRate << " Hz" << std::endl;
    
    if (result.avgFiringRate >= 100.0f) {
        std::cout << "WARNING: High firing rate may indicate instability or excessive excitation" << std::endl;
    } else if (result.avgFiringRate <= 0.0f) {
        std::cout << "WARNING: Low firing rate may indicate insufficient neural activity" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Validate memory integration
    std::cout << "=== MEMORY VALIDATION ===" << std::endl;
    std::cout << "Memory storage functional: " << (hasMemoryStorage ? "YES" : "NO") << std::endl;
    std::cout << "(Episodes stored: " << result.memoryEpisodesStored << ")" << std::endl;
    
    if (hasMemoryStorage && result.memoryEpisodesStored < 10) {
        std::cout << "CAUTION: Low episode count may indicate insufficient learning or memory consolidation" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Validate neuromodulation
    std::cout << "=== NEUROMODULATION VALIDATION ===" << std::endl;
    std::cout << "Dopamine signaling present: " << (hasNeuromodulation ? "YES" : "NO") << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    if (hasNeuromodulation && result.dopamineLevel < 0.1f) {
        std::cout << "CAUTION: Low dopamine level may indicate weak reward signals" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Overall integration score
    std::cout << "=== INTEGRATION SCORE ===" << std::endl;
    int score = 0;
    if (hasMeaningfulLearning) score++;
    if (hasStablePerformance) score++;
    if (hasMemoryStorage) score++;
    if (hasNeuromodulation) score++;
    
    std::cout << "Integration Quality Score: " << score << "/4" << std::endl;
    
    if (score >= 4) {
        std::cout << "EXCELLENT: All major systems integrated and functional" << std::endl;
    } else if (score >= 3) {
        std::cout << "GOOD: Most systems integrated, minor issues present" << std::endl;
    } else if (score >= 2) {
        std::cout << "FAIR: Basic integration achieved, significant improvements needed" << std::endl;
    } else {
        std::cout << "POOR: Major integration issues - extensive debugging required" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Detailed error reporting
    std::cout << "=== DETAILED DIAGNOSTICS ===" << std::endl;
    std::cout << "System health summary:" << std::endl;
    std::cout << "  - Reward Learning: " << (result.totalReward == 0 ? "DISABLED" : "ENABLED" ) << ". Reward: " << result.totalReward << std::endl;
    std::cout << "  - Neural Activity: Avg firing rate of " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  - Memory System: " << (result.memoryEpisodesStored > 0 ? "STORING EPISODES" : "NOT ACTIVE" ) << ". Count: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  - Neuromodulation: " << (result.dopamineLevel > 0 ? "SIGNALING" : "QUIESCENT" ) << ". Level: " << result.dopamineLevel << std::endl;
}
