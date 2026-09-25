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
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace nlm;

void printAdvancedUsage() {
    std::cout << "=== NLM Phase 6 Integration Demo - Advanced Options ===" << std::endl;
    std::cout << "Basic Usage:" << std::endl;
    std::cout << "  ./nlm_phase6_demo" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick  # Skip full integration test" << std::endl;
    std::cout << "  ./nlm_phase6_demo --config=my_config.cfg" << std::endl;
    std::cout << "  ./nlm_phase6_demo --output=results.json" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons=2000 --steps=5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --save=" << std::endl;
    std::cout << "  ./nlm_phase6_demo --pause  # Pause after initialization" << std::endl;
    std::cout << "  ./nlm_phase6_demo --verbose  # Detailed logging" << std::endl;
    std::cout << "  ./nlm_phase6_demo --replay  # Focus on replay system" << std::endl;
    std::cout << "  ./nlm_phase6_demo --dev    # Enable development system" << std::endl;
    std::cout << "  ./nlm_phase6_demo --checkpoint=" << std::endl;
    std::cout << "  ./nlm_phase6_demo --benchmark  # Run performance benchmark" << std::endl;
    std::cout << "" << std::endl;
}

Phase6Config parseAdvancedArgs(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = false;
            config.enableReplay = false;
            config.enableDevelopment = false;
        } else if (arg.substr(0, 10) == "--neurons=") {
            config.neuronCount = std::stoul(arg.substr(10));
        } else if (arg.substr(0, 9) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(9));
        } else if (arg.substr(0, 10) == "--config=") {
            std::string configFile = arg.substr(10);
            auto cfg = std::make_shared<Config>();
            if (cfg->loadFromFile(configFile)) {
                std::cout << "Loaded custom config from: " + configFile << std::endl;
            }
        } else if (arg.substr(0, 11) == "--output=") {
            std::string outputFile = arg.substr(11);
            std::cout << "Results will be saved to: " + outputFile << std::endl;
        } else if (arg.substr(0, 8) == "--save=") {
            std::string savePath = arg.substr(8);
            config.checkpointPath = savePath + ".bin";
            config.enableCheckpointing = true;
        } else if (arg.substr(0, 13) == "--checkpoint=") {
            std::string checkpointPath = arg.substr(13);
            config.checkpointPath = checkpointPath;
            config.enableCheckpointing = true;
        } else if (arg == "--pause") {
            std::cout << "Press Enter to continue..." << std::endl;
            std::cin.get();
        } else if (arg == "--verbose") {
            Logger::getInstance().setLevel(Logger::Level::Debug);
        } else if (arg == "--replay") {
            config.enableReplay = true;
            config.enableCheckpointing = false;
        } else if (arg == "--dev") {
            config.enableDevelopment = true;
        } else if (arg == "--benchmark") {
            std::cout << "Running benchmark mode..." << std::endl;
            config.neuronCount = 5000;
            config.maxSteps = 10000;
            config.enableCheckpointing = false;
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printAdvancedUsage();
            return 0;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse advanced command line arguments
    Phase6Config config = parseAdvancedArgs(argc, argv);
    
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
    
    // Run full experiment with provided config
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, "
              << config.maxSteps << " steps" << std::endl;
    
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
