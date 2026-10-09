/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Advanced demo options
    std::string savePath = "";
    std::string loadPath = "";
    std::string debugMode = "none";
    int debugSteps = 0;
    bool enableAdvancedFeatures = false;
    bool enableDebugOutput = false;
    
    // Parse advanced command line options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--save" && i + 1 < argc) {
            savePath = argv[++i];
            enableAdvancedFeatures = true;
        } else if (arg == "--load" && i + 1 < argc) {
            loadPath = argv[++i];
            enableAdvancedFeatures = true;
        } else if (arg == "--debug" && i + 1 < argc) {
            debugMode = argv[++i];
            enableDebugOutput = true;
            enableAdvancedFeatures = true;
        } else if (arg == "--debug-steps" && i + 1 < argc) {
            debugSteps = std::atoi(argv[++i]);
            enableDebugOutput = true;
            enableAdvancedFeatures = true;
        } else if (arg == "--advanced") {
            enableAdvancedFeatures = true;
        } else if (arg == "--debug-output") {
            enableDebugOutput = true;
        }
    }
    
    // Run integration verification with advanced options
    std::cout << "--- Integration Verification (Advanced Mode) ---" << std::endl;
    
    if (enableAdvancedFeatures) {
        std::cout << "Advanced features enabled: " << std::endl;
        if (!savePath.empty()) std::cout << "  Save path: " << savePath << std::endl;
        if (!loadPath.empty()) std::cout << "  Load path: " << loadPath << std::endl;
        if (debugMode != "none") std::cout << "  Debug mode: " << debugMode << std::endl;
        if (debugSteps > 0) std::cout << "  Debug steps: " << debugSteps << std::endl;
        std::cout << std::endl;
    }
    
    // For now, just verify integration
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