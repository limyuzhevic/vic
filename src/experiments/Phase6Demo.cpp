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
#include <string>
#include <sstream>

using namespace nlm;

void printUsage() {
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "Phase 6 Integration Test Demo" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --neuron-count N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps N             Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region-count N          Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob P       Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --checkpoint              Enable checkpointing (default: enabled)" << std::endl;
    std::cout << "  --no-checkpoint           Disable checkpointing" << std::endl;
    std::cout << "  --replay                  Enable replay (default: enabled)" << std::endl;
    std::cout << "  --no-replay               Disable replay" << std::endl;
    std::cout << "  --development             Enable development (default: enabled)" << std::endl;
    std::cout << "  --no-development          Disable development" << std::endl;
    std::cout << "  --checkpoint-path PATH    Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --quiet                   Reduce output verbosity" << std::endl;
    std::cout << "  --verbose                 Increase output verbosity" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6: Final Integration)" << std::endl;
    std::cout << "Build type: Release" << std::endl;
}

int main(int argc, char* argv[]) {
    // Default configuration
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    std::string checkpointPath = "./checkpoint_test.bin";
    bool quietMode = false;
    bool verboseMode = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--quiet") {
            quietMode = true;
        } else if (arg == "--verbose") {
            verboseMode = true;
        } else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            size_t pos = key.find('=');
            
            if (pos != std::string::npos) {
                std::string valueStr = key.substr(pos + 1);
                key = key.substr(0, pos);
                
                if (key == "neuron-count") {
                    neuronCount = std::stoul(valueStr);
                } else if (key == "max-steps") {
                    maxSteps = std::stoul(valueStr);
                } else if (key == "region-count") {
                    regionCount = std::stoul(valueStr);
                } else if (key == "connection-prob") {
                    connectionProbability = std::stof(valueStr);
                } else if (key == "checkpoint-path") {
                    checkpointPath = valueStr;
                } else {
                    std::cerr << "ERROR: Unknown option --" << key << std::endl;
                    printUsage();
                    return 1;
                }
            } else if (key == "checkpoint") {
                enableCheckpointing = true;
            } else if (key == "no-checkpoint") {
                enableCheckpointing = false;
            } else if (key == "replay") {
                enableReplay = true;
            } else if (key == "no-replay") {
                enableReplay = false;
            } else if (key == "development") {
                enableDevelopment = true;
            } else if (key == "no-development") {
                enableDevelopment = false;
            } else {
                std::cerr << "ERROR: Unknown option --" << key << std::endl;
                printUsage();
                return 1;
            }
        } else {
            std::cerr << "ERROR: Unknown argument: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    // Validate parameters
    if (neuronCount == 0) {
        std::cerr << "ERROR: neuron-count must be greater than 0" << std::endl;
        return 1;
    }
    if (maxSteps == 0) {
        std::cerr << "ERROR: max-steps must be greater than 0" << std::endl;
        return 1;
    }
    if (regionCount == 0) {
        std::cerr << "ERROR: region-count must be greater than 0" << std::endl;
        return 1;
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        std::cerr << "ERROR: connection-prob must be between 0.0 and 1.0" << std::endl;
        return 1;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(verboseMode ? Logger::Level::Debug : 
                                  quietMode ? Logger::Level::Warning : Logger::Level::Info);
    
    // Print header
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    if (!quietMode) {
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Max steps: " << maxSteps << std::endl;
        std::cout << "  Regions: " << regionCount << std::endl;
        std::cout << "  Connection probability: " << connectionProbability << std::endl;
        std::cout << "  Checkpointing: " << (enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    if (!quietMode) {
        std::cout << "--- Integration Verification ---" << std::endl;
    }
    bool integrationOK = experiment.verifyIntegration();
    
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    if (!quietMode) {
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    if (!quietMode) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
    }
    experiment.testMemoryIntegration();
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    if (!quietMode) {
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    }
    experiment.testNeuromodulationIntegration();
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    if (!quietMode) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
    }
    experiment.testCheckpointing();
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    if (!quietMode) {
        std::cout << "--- Replay Test ---" << std::endl;
    }
    experiment.testReplay();
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    // Run full experiment with user configuration
    if (!quietMode) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        std::cout << "Configuration: " << neuronCount << " neurons, " << maxSteps << " steps" << std::endl;
    }
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.regionCount = regionCount;
    config.connectionProbability = connectionProbability;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.checkpointPath = checkpointPath;
    
    auto result = experiment.run(config);
    
    if (!quietMode) {
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
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    } else {
        // Quiet mode: just report success
        std::cout << "Demo completed successfully!" << std::endl;
        std::cout << "Results: " << result.totalReward << " reward, " << result.avgFiringRate << " avg firing rate" << std::endl;
    }
    
    return 0;
}
