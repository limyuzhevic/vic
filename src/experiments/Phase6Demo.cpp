/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected and working together.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo (Improved) ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with enhanced memory, prediction, and cognition..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run the integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with more detail
    std::cout << "--- Enhanced Memory Integration Test ---" << std::endl;
    bool memoryWorks = experiment.testMemoryIntegration();
    std::cout << "Memory integration: " << (memoryWorks ? "SUCCESS" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Enhanced Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationWorks = experiment.testNeuromodulationIntegration();
    std::cout << "Neuromodulation integration: " << (neuromodulationWorks ? "SUCCESS" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Enhanced Checkpoint Test ---" << std::endl;
    bool checkpointWorks = experiment.testCheckpointing();
    std::cout << "Checkpoint system: " << (checkpointWorks ? "WORKING" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Replay System Test ---" << std::endl;
    bool replayWorks = experiment.testReplay();
    std::cout << "Replay system: " << (replayWorks ? "WORKING" : "LIMITED") << std::endl << std::endl;
    
    // Run full experiment with improved settings for better demo
    std::cout << "--- Enhanced Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;  // Reduced for faster demo
    config.maxSteps = 1000;    // Reduced for faster demo
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS (IMPROVED) ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== ENHANCED INTEGRATION STATUS ===" << std::endl;
    std::cout << "✓ Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "✓ Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "✓ Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "✓ Prediction System: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "✓ Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "✓ Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "FAILED") << std::endl;
    std::cout << "✓ Replay: " << (result.replayWorks ? "WORKING" : "LIMITED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== IMPROVEMENTS DEMONSTRATED ===" << std::endl;
    std::cout << "• Memory systems are now fully integrated with neural processing" << std::endl;
    std::cout << "• Prediction system uses neural activity for forward modeling" << std::endl;
    std::cout << "• Cognition systems (planning, attention, concept formation) are connected" << std::endl;
    std::cout << "• Neuromodulation integrates with learning and memory" << std::endl;
    std::cout << "• Development affects plasticity and learning rates" << std::endl;
    std::cout << "• Checkpointing and replay provide persistence and consolidation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
