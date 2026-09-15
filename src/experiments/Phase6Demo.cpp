#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments for Phase 6 mode
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    // Parse config file if provided
    if (argc > 1 && std::string(argv[1]) == "--config") {
        // In a real implementation, we would load from a config file
        std::cout << "Config file loading not yet implemented in demo" << std::endl;
    }
    
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
    bool memoryOK = experiment.testMemoryIntegration();
    if (!memoryOK) {
        std::cerr << "ERROR: Memory integration test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    if (!neuromodulationOK) {
        std::cerr << "ERROR: Neuromodulation integration test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    if (!checkpointOK) {
        std::cerr << "ERROR: Checkpoint test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    if (!replayOK) {
        std::cerr << "ERROR: Replay test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    // Run full experiment with settings from config
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    
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
