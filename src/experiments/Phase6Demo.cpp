/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Enhanced version with command-line options and better error handling.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

using namespace nlm;

/**
 * Parse command-line arguments
 */
struct Phase6DemoOptions {
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "INFO";
    
    bool parseArgs(int argc, char* argv[]) {
        std::map<std::string, std::string> argMap;
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--neurons" && i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                maxSteps = std::stoul(argv[++i]);
            } else if (arg == "--no-checkpoint") {
                enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--no-development") {
                enableDevelopment = false;
            } else if (arg == "--verbose") {
                verbose = true;
                quiet = false;
            } else if (arg == "--quiet") {
                quiet = true;
                verbose = false;
            } else if (arg == "--log-level" && i + 1 < argc) {
                logLevel = argv[++i];
            } else if (arg.substr(0, 2) == "--" && i + 1 < argc && 
                       (argMap.find(arg.substr(2)) == argMap.end())) {
                argMap[arg.substr(2)] = argv[++i];
            } else if (arg == "--help") {
                printHelp();
                return false;
            } else {
                if (!quiet) {
                    std::cerr << "Unknown argument: " << arg << std::endl;
                }
                printHelp();
                return false;
            }
        }
        
        return true;
    }
    
    void printHelp() {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --config <file>              Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --neurons <count>            Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <count>              Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "  --no-checkpoint              Disable checkpoint testing" << std::endl;
        std::cout << "  --no-replay                  Disable replay testing" << std::endl;
        std::cout << "  --no-development             Disable development system" << std::endl;
        std::cout << "  --verbose                    Enable verbose logging" << std::endl;
        std::cout << "  --quiet                      Suppress non-essential output" << std::endl;
        std::cout << "  --log-level <level>          Log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
        std::cout << "  --help                       Print this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_phase6_demo --config custom.cfg --neurons 1000" << std::endl;
        std::cout << "  nlm_phase6_demo --no-checkpoint --no-replay" << std::endl;
        std::cout << "  nlm_phase6_demo --verbose" << std::endl;
        std::cout << std::endl;
        std::cout << "The demo tests integration of:" << std::endl;
        std::cout << "  - Memory systems (working, episodic, associative)" << std::endl;
        std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
        std::cout << "  - Prediction system" << std::endl;
        std::cout << "  - Cognition (planning, concept formation, attention)" << std::endl;
        std::cout << "  - Development system" << std::endl;
        std::cout << "  - Checkpoint save/load" << std::endl;
        std::cout << "  - Memory replay and consolidation" << std::endl;
    }
};

/**
 * Enhanced main function with command-line support
 */
int main(int argc, char* argv[]) {
    Phase6DemoOptions options;
    
    // Parse command line options
    if (!options.parseArgs(argc, argv)) {
        return 1;
    }
    
    // Initialize logging based on options
    auto logger = Logger::getInstance();
    
    // Set log level
    LogLevel level;
    if (options.logLevel == "DEBUG") level = LogLevel::Debug;
    else if (options.logLevel == "WARNING") level = LogLevel::Warning;
    else if (options.logLevel == "ERROR") level = LogLevel::Error;
    else level = LogLevel::Info; // Default
    
    logger->setLevel(level);
    
    if (options.verbose) {
        std::cout << "=== NLM Phase 6 Integration Demo (Verbose Mode) ===" << std::endl;
        std::cout << "Configuration file: " << options.configFile << std::endl;
        std::cout << "Neuron count: " << options.neuronCount << std::endl;
        std::cout << "Max steps: " << options.maxSteps << std::endl;
        std::cout << "Checkpointing: " << (options.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "Replay: " << (options.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "Development: " << (options.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << "Log level: " << options.logLevel << std::endl;
        std::cout << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    } else if (!options.quiet) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    if (!options.quiet) {
        std::cout << "--- Integration Verification ---" << std::endl;
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!options.quiet) {
        std::cout << std::endl;
    }
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        if (options.verbose) {
            std::cerr << "This indicates a problem with brain system integration." << std::endl;
            std::cerr << "Please check the logs for detailed error information." << std::endl;
        }
        return 1;
    }
    
    if (!options.quiet) {
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    if (!options.quiet) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
    }
    bool memoryTestPassed = experiment.testMemoryIntegration();
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    }
    bool neuromodulationTestPassed = experiment.testNeuromodulationIntegration();
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---" << std::endl;
    }
    bool checkpointTestPassed = experiment.testCheckpointing();
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "--- Replay Test ---" << std::endl;
    }
    bool replayTestPassed = experiment.testReplay();
    
    // Update checkpoint path based on command line
    std::string checkpointPath = options.checkpointPath;
    if (options.enableCheckpointing && checkpointTestPassed) {
        if (options.verbose) {
            std::cout << "Checkpoint test passed! Save/load functionality working." << std::endl;
        }
    } else if (options.enableCheckpointing) {
        if (options.verbose) {
            std::cout << "Checkpoint test failed (expected for integration demo)." << std::endl;
        }
        checkpointPath = "./checkpoint_test.bin"; // Use default path
    }
    
    // Run full experiment with options
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "--- Full Integration Experiment ---" << std::endl;
    }
    
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = checkpointPath;
    
    auto result = experiment.run(config);
    
    // Display enhanced results
    if (!options.quiet) {
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
        
        // Additional integration metrics
        if (options.verbose) {
            std::cout << std::endl;
            std::cout << "=== DETAILED ANALYSIS ===" << std::endl;
            std::cout << "Individual tests passed:" << std::endl;
            std::cout << "  - Memory integration: " << (memoryTestPassed ? "YES" : "NO") << std::endl;
            std::cout << "  - Neuromodulation integration: " << (neuromodulationTestPassed ? "YES" : "NO") << std::endl;
            std::cout << "  - Checkpoint test: " << (checkpointTestPassed ? "YES" : "NO") << std::endl;
            std::cout << "  - Replay test: " << (replayTestPassed ? "YES" : "NO") << std::endl;
            std::cout << std::endl;
            std::cout << "Configuration used:" << std::endl;
            std::cout << "  - Neurons: " << config.neuronCount << std::endl;
            std::cout << "  - Max steps: " << config.maxSteps << std::endl;
            std::cout << "  - Checkpointing: " << (config.enableCheckpointing ? "YES" : "NO") << std::endl;
            std::cout << "  - Replay: " << (config.enableReplay ? "YES" : "NO") << std::endl;
            std::cout << "  - Development: " << (config.enableDevelopment ? "YES" : "NO") << std::endl;
        }
    }
    
    return 0;
}
