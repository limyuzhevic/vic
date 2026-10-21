/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 *
 * Command-line usage:
 *   ./nlm_phase6_demo                    # Default settings
 *   ./nlm_phase6_demo --config path/to/config.cfg
 *   ./nlm_phase6_demo --neurons N        # Custom neuron count
 *   ./nlm_phase6_demo --steps S          # Custom simulation steps
 *   ./nlm_phase6_demo --no-checkpoint    # Disable checkpointing
 *   ./nlm_phase6_demo --no-replay        # Disable replay
 *   ./nlm_phase6_demo --no-development   # Disable development
 *   ./nlm_phase6_demo --help             # Show help
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace nlm;

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo Help ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config FILE             Load configuration from file" << std::endl;
    std::cout << "  --neurons N              Set neuron count (default: 500)" << std::endl;
    std::cout << "  --steps S                Set simulation steps (default: 2000)" << std::endl;
    std::cout << "  --regions R              Set region count (default: 1)" << std::endl;
    std::cout << "  --connection C            Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --no-checkpoint          Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              Disable replay functionality" << std::endl;
    std::cout << "  --no-development         Disable development system" << std::endl;
    std::cout << "  --verbose                Enable verbose logging" << std::endl;
    std::cout << "  --help                   Show this help message" << std::endl << std::endl;
}

Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                // --key=value format
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            } else {
                // --key value format
                if (i + 1 < argc) {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    args[key] = value;
                }
            }
        }
    }
    
    // Parse arguments
    if (args.find("neurons") != args.end()) {
        config.neuronCount = std::stoi(args["neurons"]);
        std::cout << "Using neuron count: " << config.neuronCount << std::endl;
    }
    
    if (args.find("steps") != args.end()) {
        config.maxSteps = std::stoull(args["steps"]);
        std::cout << "Using simulation steps: " << config.maxSteps << std::endl;
    }
    
    if (args.find("regions") != args.end()) {
        config.regionCount = std::stoi(args["regions"]);
        std::cout << "Using region count: " << config.regionCount << std::endl;
    }
    
    if (args.find("connection") != args.end()) {
        config.connectionProbability = std::stof(args["connection"]);
        std::cout << "Using connection probability: " << config.connectionProbability << std::endl;
    }
    
    // Boolean flags
    if (args.find("no-checkpoint") != args.end()) {
        config.enableCheckpointing = false;
        std::cout << "Checkpointing disabled" << std::endl;
    }
    
    if (args.find("no-replay") != args.end()) {
        config.enableReplay = false;
        std::cout << "Replay disabled" << std::endl;
    }
    
    if (args.find("no-development") != args.end()) {
        config.enableDevelopment = false;
        std::cout << "Development disabled" << std::endl;
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
    // Handle help
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help") {
            printHelp();
            return 0;
        }
    }
    
    // Load configuration file if specified
    if (config.connectionProbability == 0.1f && 
        (argc > 1 && (std::string(argv[1]) == "--config" || 
                     std::string(argv[1]).find("--config=") == 0))) {
        // Config file specified via --config option
        std::string configFile = "configs/default.cfg";
        size_t pos = std::string(argv[1]).find('=');
        if (pos != std::string::npos) {
            configFile = std::string(argv[1]).substr(pos + 1);
        } else if (argc > 2) {
            configFile = argv[2];
        }
        
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
            // Override with command line args
            cfg->loadFromArgs(argc, argv);
            
            // Apply configuration values to our config
            if (cfg->has("neuron_count")) {
                config.neuronCount = cfg->getOr<size_t>("neuron_count", config.neuronCount);
            }
            if (cfg->has("region_count")) {
                config.regionCount = cfg->getOr<size_t>("region_count", config.regionCount);
            }
            if (cfg->has("connection_probability")) {
                config.connectionProbability = cfg->getOr<float>("connection_probability", config.connectionProbability);
            }
            if (cfg->has("max_simulation_steps")) {
                config.maxSteps = cfg->getOr<uint64_t>("max_simulation_steps", config.maxSteps);
            }
            
            // Set checkpoint path if specified
            if (cfg->has("checkpoint_dir")) {
                std::string checkpointDir = cfg->getOr<std::string>("checkpoint_dir", "./checkpoints");
                if (!checkpointDir.empty()) {
                    config.checkpointPath = checkpointDir + "/checkpoint.bin";
                }
            }
        } else {
            std::cout << "Using default configuration (could not load: " << configFile << ")" << std::endl;
        }
    }
    
    // Initialize logging level (can be configured via --verbose)
    bool verbose = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--verbose") {
            verbose = true;
            Logger::getInstance().setLevel(Logger::Level::Debug);
            std::cout << "Verbose logging enabled" << std::endl;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated." << std::endl;
        std::cerr << "This suggests that Phase 6 integration is incomplete." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with optional verbose output
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    if (verbose) std::cout << "(Working memory active: " << experiment.getBrain()->getWorkingMemory() << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    if (verbose) std::cout << "(Dopamine level: " << experiment.getBrain()->getDopamine() << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Prediction error: " << result.predictionError << std::endl;
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
    
    // Summary
    std::cout << std::endl;
    std::cout << "=== INTEGRATION SUMMARY ===" << std::endl;
    int connectedCount = (result.memoryWorkingMemoryIntegrated ? 1 : 0) +
                         (result.memoryEpisodicMemoryIntegrated ? 1 : 0) +
                         (result.neuromodulationIntegrated ? 1 : 0) +
                         (result.predictionIntegrated ? 1 : 0) +
                         (result.developmentIntegrated ? 1 : 0);
    std::cout << "Systems connected: " << connectedCount << "/5" << std::endl;
    std::cout << "Status: " << (connectedCount >= 4 ? "GOOD" : (connectedCount >= 3 ? "NEEDS ATTENTION" : "CRITICAL ISSUES")) << std::endl;
    
    return 0;
}
