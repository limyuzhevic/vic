/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Command-line options:
 *   --steps <num>            Maximum simulation steps (default: 2000)
 *   --neurons <num>          Number of neurons (default: 500)
 *   --regions <num>          Number of brain regions (default: 1)
 *   --conn <float>           Connection probability (default: 0.15)
 *   --no-checkpointing       Disable checkpointing
 *   --no-replay              Disable replay
 *   --no-development         Disable development
 *   --checkpoint <path>      Checkpoint file path (default: ./checkpoint_test.bin)
 *   --help                   Show this help message
 *   --verbose                Enable verbose logging
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace nlm;

// Parse command line arguments into a Phase6Config
Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl << std::endl;
            std::cout << "Command-line options:" << std::endl;
            std::cout << "  --steps <num>            Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --neurons <num>          Number of neurons (default: 500)" << std::endl;
            std::cout << "  --regions <num>          Number of brain regions (default: 1)" << std::endl;
            std::cout << "  --conn <float>           Connection probability (default: 0.15)" << std::endl;
            std::cout << "  --no-checkpointing       Disable checkpointing" << std::endl;
            std::cout << "  --no-replay              Disable replay" << std::endl;
            std::cout << "  --no-development         Disable development" << std::endl;
            std::cout << "  --checkpoint <path>      Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
            std::cout << "  --help                   Show this help message" << std::endl;
            std::cout << "  --verbose                Enable verbose logging" << std::endl;
            exit(0);
        }
        
        if (arg == "--verbose" || arg == "-v") {
            Logger::getGlobal().setLevel(LogLevel::Debug);
            continue;
        }
        
        if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
            continue;
        }
        
        if (arg == "--no-replay") {
            config.enableReplay = false;
            continue;
        }
        
        if (arg == "--no-development") {
            config.enableDevelopment = false;
            continue;
        }
        
        // Handle --key=value format
        if (arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            if (key == "--steps") {
                config.maxSteps = std::stoull(value);
            } else if (key == "--neurons") {
                config.neuronCount = std::stoul(value);
            } else if (key == "--regions") {
                config.regionCount = std::stoul(value);
            } else if (key == "--conn") {
                config.connectionProbability = std::stof(value);
            } else if (key == "--checkpoint") {
                config.checkpointPath = value;
            }
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
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
    
    // Use parsed configuration for the full experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Using configuration:" << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    
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