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
    Logger::getInstance().setLevel(Logger::Level::Info);
    
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
    
    // Initialize additional Phase 6 configuration parameters
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.workingMemoryCapacity = 50;
    config.episodicMemoryMaxEpisodes = 100;
    config.associativeMemoryPatternCount = 200;
    config.memoryReplayInterval = 100;
    config.memoryConsolidationInterval = 1000;
    config.predictionHorizon = 5;
    config.predictionErrorThreshold = 0.1f;
    config.predictionConfidenceThreshold = 0.8f;
    config.plannerPlanningDepth = 3;
    config.conceptFormationThreshold = 0.7f;
    config.attentionInhibitionStrength = 0.5f;
    config.attentionExcitationStrength = 1.5f;
    config.attentionSelectionThreshold = 0.6f;
    config.developmentalStage = 0;
    config.plasticityRateInitial = 1.0f;
    config.plasticityRateCriticalPeriod = 0.8f;
    config.plasticityRateMaturation = 0.5f;
    config.plasticityRateAdult = 0.2f;
    config.developmentUpdateInterval = 1000;
    config.dopamineBaselineLevel = 0.1f;
    config.dopaminePlasticityFactor = 1.0f;
    config.curiosityExplorationRate = 0.3f;
    config.noveltyThreshold = 0.5f;
    config.predictionErrorGain = 1.0f;
    config.sensoryInputScale = 10.0f;
    config.motorOutputStrength = 1.0f;
    config.environmentWidth = 16;
    config.environmentHeight = 16;
    config.agentExplorationRate = 0.3f;
    config.agentCuriosityEnabled = true;
    config.agentRewardModulationEnabled = true;
    
    // Apply Phase 6 integration experiment
    auto result = experiment.run(config);
    
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
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
