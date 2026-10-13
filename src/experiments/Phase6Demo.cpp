/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   ./nlm_phase6_demo [options]
 * 
 * Options:
 *   --help, -h              Show this help message
 *   --neurons <N>           Number of neurons (default: 500)
 *   --steps <N>             Maximum simulation steps (default: 2000)
 *   --no-checkpoint         Disable checkpointing
 *   --no-replay              Disable replay
 *   --no-development         Disable development system
 *   --checkpoint-path <P>    Path for checkpoint file (default: ./checkpoint.bin)
 *   --verbose, -v           Enable verbose logging
 *   --quiet, -q             Enable quiet mode (minimal output)
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace nlm;

/**
 * Parse command line arguments
 */
Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    bool helpRequested = false;
    std::map<std::string, std::string> args;
    
    // Simple argument parsing (not using advanced libraries for portability)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args["verbose"] = "true";
        } else if (arg == "--quiet" || arg == "-q") {
            args["quiet"] = "true";
        } else if (arg.find("--neurons=") == 0) {
            args["neurons"] = arg.substr(10);
        } else if (arg.find("--steps=") == 0) {
            args["steps"] = arg.substr(8);
        } else if (arg.find("--checkpoint-path=") == 0) {
            args["checkpoint_path"] = arg.substr(18);
        } else if (arg == "--no-checkpoint") {
            args["checkpoint"] = "false";
        } else if (arg == "--no-replay") {
            args["replay"] = "false";
        } else if (arg == "--no-development") {
            args["development"] = "false";
        }
    }
    
    if (helpRequested) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --neurons <N>           Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <N>             Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
        std::cout << "  --no-replay              Disable replay" << std::endl;
        std::cout << "  --no-development         Disable development system" << std::endl;
        std::cout << "  --checkpoint-path <P>    Path for checkpoint file (default: ./checkpoint.bin)" << std::endl;
        std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
        std::cout << "  --quiet, -q             Enable quiet mode (minimal output)" << std::endl;
        std::exit(0);
    }
    
    // Apply parsed arguments
    if (args.find("neurons") != args.end()) {
        try {
            config.neuronCount = std::stoi(args["neurons"]);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid neuron count, using default (500)" << std::endl;
        }
    }
    
    if (args.find("steps") != args.end()) {
        try {
            config.maxSteps = std::stoull(args["steps"]);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid step count, using default (2000)" << std::endl;
        }
    }
    
    if (args.find("checkpoint_path") != args.end()) {
        config.checkpointPath = args["checkpoint_path"];
    }
    
    if (args.find("checkpoint") != args.end() && args["checkpoint"] == "false") {
        config.enableCheckpointing = false;
    }
    
    if (args.find("replay") != args.end() && args["replay"] == "false") {
        config.enableReplay = false;
    }
    
    if (args.find("development") != args.end() && args["development"] == "false") {
        config.enableDevelopment = false;
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
    bool quietMode = false;
    bool verboseMode = false;
    std::map<std::string, std::string> args;
    
    // Check for quiet/verbose flags (re-parse for simplicity)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quiet" || arg == "-q") {
            quietMode = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
        }
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    if (quietMode) {
        Logger::getInstance().setLevel(Logger::Level::Error);
    } else if (verboseMode) {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Print configuration
    if (!quietMode) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Max steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
        std::cout << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    }
    
    // First, run the quick integration verification
    if (!quietMode) {
        std::cout << "--- Integration Verification ---" << std::endl;
    }
    bool integrationOK = experiment.verifyIntegration();
    
    if (!quietMode) {
        std::cout << std::endl;
    }
    
    if (!integrationOK) {
        if (!quietMode) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
        }
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
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    }
    experiment.testNeuromodulationIntegration();
    
    if (!quietMode) {
        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---" << std::endl;
    }
    experiment.testCheckpointing();
    
    if (!quietMode) {
        std::cout << std::endl;
        std::cout << "--- Replay Test ---" << std::endl;
    }
    experiment.testReplay();
    
    if (!quietMode) {
        std::cout << std::endl;
        std::cout << "--- Full Integration Experiment ---" << std::endl;
    }
    
    // Run full experiment with user-specified settings
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
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    return 0;
}
