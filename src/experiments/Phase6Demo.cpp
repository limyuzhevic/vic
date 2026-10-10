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
    
    // Parse command line arguments
    Phase6Config config;
    bool quickMode = false;
    std::string outputFile;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quick") {
            quickMode = true;
            config.neuronCount = 200;
            config.maxSteps = 500;
        } else if (arg == "--large") {
            config.neuronCount = 1000;
            config.maxSteps = 5000;
        } else if (arg == "--small") {
            config.neuronCount = 100;
            config.maxSteps = 200;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Options:" << std::endl;
            std::cout << "  --quick          Run quick test (200 neurons, 500 steps)" << std::endl;
            std::cout << "  --small          Run small test (100 neurons, 200 steps)" << std::endl;
            std::cout << "  --large          Run large test (1000 neurons, 5000 steps)" << std::endl;
            std::cout << "  --output file    Save results to file" << std::endl;
            std::cout << "  --help, -h       Show this help" << std::endl;
            return 0;
        }
    }
    
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
    
    // Run full experiment with configurable settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    if (quickMode) {
        std::cout << "Running in QUICK MODE for fast testing" << std::endl;
        config.neuronCount = 200;
        config.maxSteps = 500;
    }
    
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
    
    // Save results if requested
    if (!outputFile.empty()) {
        std::cout << "Saving results to: " << outputFile << std::endl;
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "NLM Phase 6 Integration Demo Results" << std::endl;
            file << "===============================" << std::endl;
            file << "Neuron Count: " << config.neuronCount << std::endl;
            file << "Max Steps: " << config.maxSteps << std::endl;
            file << "Total Reward: " << result.totalReward << std::endl;
            file << "Avg Firing Rate: " << result.avgFiringRate << std::endl;
            file << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
            file << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
            file << std::endl;
            file << "Integration Status:" << std::endl;
            file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            file << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            file << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            file << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            file.close();
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "ERROR: Could not open output file: " << outputFile << std::endl;
        }
    }
    
    return 0;
}
