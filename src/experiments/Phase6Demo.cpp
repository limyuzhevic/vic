/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Features:
 * - Command-line argument parsing for flexible configuration
 * - Improved error handling with detailed error messages
 * - Verbosity control through log levels
 * - Integration verification with scoring
 * - Graceful error recovery and warnings
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace nlm;

// Helper function to parse command-line arguments
struct DemoArgs {
    bool help = false;
    std::string logLevel = "INFO";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string configPath = "";
};

DemoArgs parseArgs(int argc, char* argv[]) {
    DemoArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) args.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) args.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--log-level" || arg == "-l") {
            if (i + 1 < argc) args.logLevel = argv[++i];
        } else if (arg == "--checkpoint" || arg == "-c") {
            if (i + 1 < argc) args.checkpointPath = argv[++i];
        } else if (arg == "--config" || arg == "-C") {
            if (i + 1 < argc) args.configPath = argv[++i];
        } else if (arg == "--no-checkpoint") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            args.enableReplay = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            args.help = true;
        }
    }
    
    return args;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -n, --neurons N         Number of neurons (default: 500)" << std::endl;
    std::cout << "  -s, --steps S           Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  -l, --log-level L       Log level: DEBUG, INFO, WARNING, ERROR (default: INFO)" << std::endl;
    std::cout << "  -c, --checkpoint PATH   Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  -C, --config PATH       Configuration file path" << std::endl;
    std::cout << "      --no-checkpoint      Disable checkpointing" << std::endl;
    std::cout << "      --no-replay          Disable replay" << std::endl;
    std::cout << "      --no-development     Disable development" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  " << programName << " --log-level DEBUG --no-checkpoint" << std::endl;
}

LogLevel stringToLogLevel(const std::string& levelStr) {
    std::string upperLevel = levelStr;
    std::transform(upperLevel.begin(), upperLevel.end(), upperLevel.begin(), ::toupper);
    
    if (upperLevel == "DEBUG") return LogLevel::Debug;
    if (upperLevel == "INFO") return LogLevel::Info;
    if (upperLevel == "WARNING") return LogLevel::Warning;
    if (upperLevel == "ERROR") return LogLevel::Error;
    
    // Default to Info
    return LogLevel::Info;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    DemoArgs args = parseArgs(argc, argv);
    
    // Show help if requested
    if (args.help) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Initialize logging
    Logger::getGlobal().setLevel(stringToLogLevel(args.logLevel));
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Arguments: " << args.neuronCount << " neurons, " << args.maxSteps << " steps" << std::endl;
    std::cout << "Log level: " << args.logLevel << std::endl;
    std::cout << std::endl;
    
    try {
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
        if (!experiment.testMemoryIntegration()) {
            std::cerr << "WARNING: Memory integration test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        if (!experiment.testNeuromodulationIntegration()) {
            std::cerr << "WARNING: Neuromodulation integration test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        if (!experiment.testCheckpointing()) {
            std::cerr << "WARNING: Checkpoint test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        if (!experiment.testReplay()) {
            std::cerr << "WARNING: Replay test failed!" << std::endl;
        }
        std::cout << std::endl;
        
        // Run full experiment with provided settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = args.neuronCount;
        config.maxSteps = args.maxSteps;
        config.enableCheckpointing = args.enableCheckpointing;
        config.enableReplay = args.enableReplay;
        config.enableDevelopment = args.enableDevelopment;
        config.checkpointPath = args.checkpointPath;
        
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
        
        // Summary
        std::cout << "=== DEMO SUMMARY ===" << std::endl;
        int integrationScore = 0;
        integrationScore += result.memoryWorkingMemoryIntegrated ? 1 : 0;
        integrationScore += result.memoryEpisodicMemoryIntegrated ? 1 : 0;
        integrationScore += result.neuromodulationIntegrated ? 1 : 0;
        integrationScore += result.predictionIntegrated ? 1 : 0;
        integrationScore += result.developmentIntegrated ? 1 : 0;
        integrationScore += result.checkpointingWorks ? 1 : 0;
        
        std::cout << "Integration Score: " << integrationScore << "/6" << std::endl;
        if (integrationScore == 6) {
            std::cout << "SUCCESS: All systems integrated and working correctly!" << std::endl;
        } else {
            std::cout << "PARTIAL SUCCESS: Some systems may need attention." << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
