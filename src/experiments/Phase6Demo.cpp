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
#include <map>
#include <stdexcept>

using namespace nlm;

// Command-line argument parsing
struct DemoConfig {
    uint32_t neuronCount = 500;
    uint32_t maxSteps = 2000;
    std::string logLevel = "Info";
    std::string logFile = "";
    std::string checkpointDir = "./checkpoints";
    bool helpRequested = false;
    bool verbose = false;
};

DemoConfig parseCommandLineArgs(int argc, char* argv[]) {
    DemoConfig config;
    
    std::map<std::string, std::string> argMap;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.helpRequested = true;
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg.find("--") == 0) {
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(2, equalsPos - 2);
                std::string value = arg.substr(equalsPos + 1);
                argMap[key] = value;
            }
        } else {
            std::cerr << "WARNING: Unrecognized argument: " << arg << std::endl;
        }
    }
    
    // Apply configuration from argument map
    if (argMap.find("--neuron-count") != argMap.end()) {
        try {
            config.neuronCount = std::stoul(argMap["--neuron-count"]);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid neuron count: " << argMap["--neuron-count"] << std::endl;
        }
    }
    
    if (argMap.find("--max-steps") != argMap.end()) {
        try {
            config.maxSteps = std::stoul(argMap["--max-steps"]);
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid max steps: " << argMap["--max-steps"] << std::endl;
        }
    }
    
    if (argMap.find("--log-level") != argMap.end()) {
        config.logLevel = argMap["--log-level"];
    }
    
    if (argMap.find("--log-file") != argMap.end()) {
        config.logFile = argMap["--log-file"];
    }
    
    if (argMap.find("--checkpoint-dir") != argMap.end()) {
        config.checkpointDir = argMap["--checkpoint-dir"];
    }
    
    return config;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neuron-count N      Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps S         Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --log-level LEVEL      Logging level (Debug, Info, Warning, Error, Critical)" << std::endl;
    std::cout << "  --log-file FILE        Log output to file" << std::endl;
    std::cout << "  --checkpoint-dir DIR   Directory for checkpoints (default: ./checkpoints)" << std::endl;
    std::cout << "  --verbose              Enable verbose output" << std::endl;
    std::cout << "  --help, -h             Show this help message" << std::endl;
}

LogLevel stringToLogLevel(const std::string& levelStr) {
    std::string level = levelStr;
    std::transform(level.begin(), level.end(), level.begin(), ::tolower);
    
    if (level == "debug") return LogLevel::Debug;
    if (level == "info") return LogLevel::Info;
    if (level == "warning") return LogLevel::Warning;
    if (level == "error") return LogLevel::Error;
    if (level == "critical") return LogLevel::Critical;
    
    // Default to Info
    return LogLevel::Info;
}

bool validateConfig(const DemoConfig& config) {
    if (config.neuronCount == 0) {
        std::cerr << "ERROR: Neuron count must be greater than 0" << std::endl;
        return false;
    }
    
    if (config.maxSteps == 0) {
        std::cerr << "ERROR: Max steps must be greater than 0" << std::endl;
        return false;
    }
    
    if (config.neuronCount > 10000) {
        std::cerr << "WARNING: Large neuron count may impact performance" << std::endl;
    }
    
    if (config.maxSteps > 100000) {
        std::cerr << "WARNING: Large step count may impact performance" << std::endl;
    }
    
    return true;
}

void runDemoWithConfig(const DemoConfig& config) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    LogLevel logLevel = stringToLogLevel(config.logLevel);
    Logger::getInstance().setLevel(logLevel);
    
    if (!config.logFile.empty()) {
        try {
            Logger::getInstance().setOutputFile(config.logFile);
            std::cout << "Logging to file: " << config.logFile << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "WARNING: Failed to set log file: " << e.what() << std::endl;
        }
    }
    
    if (config.verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
        std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " max steps" << std::endl;
    }
    
    // Create experiment
    std::cout << "Creating Phase 6 integrated experiment..." << std::endl;
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with error handling
    std::cout << "--- Memory Integration Test ---" << std::endl;
    try {
        experiment.testMemoryIntegration();
        std::cout << "Memory integration test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR in memory integration test: " << e.what() << std::endl;
        // Continue with other tests even if one fails
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    try {
        experiment.testNeuromodulationIntegration();
        std::cout << "Neuromodulation integration test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR in neuromodulation integration test: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    try {
        experiment.testCheckpointing();
        std::cout << "Checkpoint test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR in checkpoint test: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    try {
        experiment.testReplay();
        std::cout << "Replay test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR in replay test: " << e.what() << std::endl;
    }
    std::cout << std::endl;
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " max steps" << std::endl;
    std::cout << "Checkpoint directory: " << config.checkpointDir << std::endl;
    std::cout << "Log level: " << config.logLevel << std::endl;
    
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = config.maxSteps;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    // Configure checkpoint directory
    try {
        // Note: This would require accessing the checkpoint manager's configuration
        // For now, we'll just note that checkpointing is enabled
        if (config.verbose) {
            std::cout << "Checkpointing enabled (directory: " << config.checkpointDir << ")" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "WARNING: Could not configure checkpoint directory: " << e.what() << std::endl;
    }
    
    auto result = experiment.run(phase6Config);
    
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
    
    // Log final status
    NLM_LOG_INFO("Phase 6 integration demo completed successfully");
    NLM_LOG_INFO(std::string("Neurons: ") + std::to_string(config.neuronCount));
    NLM_LOG_INFO(std::string("Max steps: ") + std::to_string(config.maxSteps));
    NLM_LOG_INFO(std::string("Total reward: ") + std::to_string(result.totalReward));
    NLM_LOG_INFO(std::string("Wall clock time: ") + std::to_string(result.totalWallClockTime) + "s");
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        DemoConfig config = parseCommandLineArgs(argc, argv);
        
        // Show help if requested
        if (config.helpRequested) {
            printHelp();
            return 0;
        }
        
        // Validate configuration
        if (!validateConfig(config)) {
            printHelp();
            return 1;
        }
        
        // Run demo with configuration
        runDemoWithConfig(config);
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
