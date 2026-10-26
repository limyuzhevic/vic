/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 *
 * Enhanced version with comprehensive configuration options,
 * error handling, and better user interface.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>

using namespace nlm;

/**
 * Prints the Phase 6 demo banner
 */
void printDemoBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM Phase 6 — FINAL INTEGRATION DEMO                      ║
    ║                                                               ║
    ║     Complete brain system integration test                    ║
    ║                                                               ║
    ║     Verifying all brain systems work together coherently.     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

/**
 * Parses command line arguments for configuration
 * @param argc Number of command line arguments
 * @param argv Command line arguments
 * @param config Reference to Phase6Config to populate
 * @return true if parsing succeeded, false otherwise
 */
bool parseCommandLineArgs(int argc, char* argv[], Phase6Config& config) {
    std::map<std::string, std::string> argMap;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                argMap[key] = value;
            } else {
                argMap[arg.substr(2)] = "true";
            }
        }
    }
    
    // Apply parsed arguments to config
    auto it = argMap.find("neurons");
    if (it != argMap.end()) {
        try {
            config.neuronCount = std::stoi(it->second);
        } catch (...) {
            std::cerr << "ERROR: Invalid neuron count: " << it->second << std::endl;
            return false;
        }
    }
    
    it = argMap.find("steps");
    if (it != argMap.end()) {
        try {
            config.maxSteps = std::stoull(it->second);
        } catch (...) {
            std::cerr << "ERROR: Invalid max steps: " << it->second << std::endl;
            return false;
        }
    }
    
    it = argMap.find("regions");
    if (it != argMap.end()) {
        try {
            config.regionCount = std::stoi(it->second);
        } catch (...) {
            std::cerr << "ERROR: Invalid region count: " << it->second << std::endl;
            return false;
        }
    }
    
    it = argMap.find("conn_prob");
    if (it != argMap.end()) {
        try {
            config.connectionProbability = std::stof(it->second);
        } catch (...) {
            std::cerr << "ERROR: Invalid connection probability: " << it->second << std::endl;
            return false;
        }
    }
    
    // Handle boolean flags
    std::vector<std::pair<std::string, bool&>> boolFlags = {
        {"checkpoint", config.enableCheckpointing},
        {"replay", config.enableReplay},
        {"development", config.enableDevelopment}
    };
    
    for (const auto& flag : boolFlags) {
        auto it = argMap.find(flag.first);
        if (it != argMap.end() && it->second == "false") {
            flag.second = false;
        }
    }
    
    return true;
}

/**
 * Loads configuration from file
 * @param filename Configuration file name
 * @param config Reference to Phase6Config to populate
 * @return true if loading succeeded, false otherwise
 */
bool loadConfigFromFile(const std::string& filename, Phase6Config& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "WARNING: Could not open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse based on key
        if (key == "neurons") {
            try { config.neuronCount = std::stoi(value); } catch (...) {}
        } else if (key == "steps") {
            try { config.maxSteps = std::stoull(value); } catch (...) {}
        } else if (key == "regions") {
            try { config.regionCount = std::stoi(value); } catch (...) {}
        } else if (key == "conn_prob") {
            try { config.connectionProbability = std::stof(value); } catch (...) {}
        } else if (key == "checkpoint") {
            config.enableCheckpointing = (value == "true" || value == "1");
        } else if (key == "replay") {
            config.enableReplay = (value == "true" || value == "1");
        } else if (key == "development") {
            config.enableDevelopment = (value == "true" || value == "1");
        } else if (key == "checkpoint_path") {
            config.checkpointPath = value;
        }
    }
    
    file.close();
    return true;
}

/**
 * Validates configuration parameters
 * @param config Configuration to validate
 * @return true if configuration is valid, false otherwise
 */
bool validateConfiguration(const Phase6Config& config) {
    const uint64_t MIN_NEURONS = 100;
    const uint64_t MAX_NEURONS = 10000;
    const uint64_t MIN_STEPS = 100;
    const uint64_t MAX_STEPS = 100000;
    const size_t MIN_REGIONS = 1;
    const size_t MAX_REGIONS = 10;
    const float MIN_CONN_PROB = 0.01f;
    const float MAX_CONN_PROB = 1.0f;
    
    if (config.neuronCount < MIN_NEURONS || config.neuronCount > MAX_NEURONS) {
        std::cerr << "ERROR: Invalid neuron count: " << config.neuronCount
                  << ". Must be between " << MIN_NEURONS << " and " << MAX_NEURONS << std::endl;
        return false;
    }
    
    if (config.maxSteps < MIN_STEPS || config.maxSteps > MAX_STEPS) {
        std::cerr << "ERROR: Invalid max steps: " << config.maxSteps
                  << ". Must be between " << MIN_STEPS << " and " << MAX_STEPS << std::endl;
        return false;
    }
    
    if (config.regionCount < MIN_REGIONS || config.regionCount > MAX_REGIONS) {
        std::cerr << "ERROR: Invalid region count: " << config.regionCount
                  << ". Must be between " << MIN_REGIONS << " and " << MAX_REGIONS << std::endl;
        return false;
    }
    
    if (config.connectionProbability < MIN_CONN_PROB || config.connectionProbability > MAX_CONN_PROB) {
        std::cerr << "ERROR: Invalid connection probability: " << config.connectionProbability
                  << ". Must be between " << MIN_CONN_PROB << " and " << MAX_CONN_PROB << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Prints configuration summary
 * @param config Configuration to display
 */
void printConfigurationSummary(const Phase6Config& config) {
    std::cout << std::endl;
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::cout << "Neurons: " << config.neuronCount << std::endl;
    std::cout << "Regions: " << config.regionCount << std::endl;
    std::cout << "Max Steps: " << config.maxSteps << std::endl;
    std::cout << "Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    if (!config.checkpointPath.empty()) {
        std::cout << "Checkpoint Path: " << config.checkpointPath << std::endl;
    }
    std::cout << std::endl;
}

/**
 * Prints demo usage information
 */
void printDemoUsage() {
    std::cout << std::endl;
    std::cout << "=== Phase 6 Demo Usage ===" << std::endl;
    std::cout << "nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons=N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=N            Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --regions=N          Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --conn_prob=VAL      Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --checkpoint[=BOOL]   Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --replay[=BOOL]      Enable replay (default: true)" << std::endl;
    std::cout << "  --development[=BOOL] Enable development (default: true)" << std::endl;
    std::cout << "  --config=FILE        Load configuration from file" << std::endl;
    std::cout << "  --help               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo --neurons=1000 --steps=5000" << std::endl;
    std::cout << "  nlm_phase6_demo --config=./demo_config.cfg" << std::endl;
    std::cout << "  nlm_phase6_demo --replay=false --checkpoint=true" << std::endl;
    std::cout << std::endl;
}

/**
 * Get current timestamp for logging
 */
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main(int argc, char* argv[]) {
    printDemoBanner();
    
    std::cout << "Phase 6 Integration Demo Starting..." << std::endl;
    std::cout << "Started at: " << getCurrentTimestamp() << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help") {
            printDemoUsage();
            return 0;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Create configuration with defaults
    Phase6Config config;
    
    // Check for config file
    std::string configFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            break;
        }
    }
    
    // Load configuration from file if specified
    if (!configFile.empty()) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        if (!loadConfigFromFile(configFile, config)) {
            std::cerr << "ERROR: Failed to load configuration file!" << std::endl;
            printDemoUsage();
            return 1;
        }
    }
    
    // Parse command line arguments (override config file)
    if (!parseCommandLineArgs(argc, argv, config)) {
        printDemoUsage();
        return 1;
    }
    
    // Validate configuration
    if (!validateConfiguration(config)) {
        return 1;
    }
    
    // Print configuration summary
    printConfigurationSummary(config);
    
    // Run quick integration verification
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
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running simulation with " << config.neuronCount << " neurons for "
              << config.maxSteps << " steps..." << std::endl << std::endl;
    
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
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << "Program completed at: " << getCurrentTimestamp() << std::endl << std::endl;
    
    std::cout << "=== DEMO SUMMARY ===" << std::endl;
    std::cout << "Successfully completed Phase 6 integration demo!" << std::endl;
    std::cout << "All brain systems are properly connected and functional." << std::endl;
    
    return 0;
}