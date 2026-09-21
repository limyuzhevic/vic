/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace nlm;

// Helper functions
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse command line arguments
void parseArgs(int argc, char* argv[], std::map<std::string, std::string>& args) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args[key] = value;
            } else {
                args[arg.substr(2)] = "";
            }
        }
    }
}

// Load configuration from file
bool loadConfigFile(const std::string& filename, std::map<std::string, std::string>& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) continue;
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            config[key] = value;
        }
    }
    return true;
}

// Convert string to number
template<typename T>
T stringToNumber(const std::string& str) {
    std::stringstream ss(str);
    T result;
    ss >> result;
    return result;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    std::map<std::string, std::string> args;
    parseArgs(argc, argv, args);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Load config file if specified
    std::string configFile = "demo_config.cfg";
    if (args.find("config") != args.end()) {
        configFile = args["config"];
    } else if (args.find("-c") != args.end()) {
        configFile = args["-c"];
    }
    
    std::map<std::string, std::string> fileConfig;
    if (loadConfigFile(configFile, fileConfig)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    }
    
    // Override with command line args
    for (const auto& arg : args) {
        if (arg.first == "config" || arg.first == "c") continue;
        fileConfig[arg.first] = arg.second;
    }
    
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
    
    // Test individual systems with enhanced logging
    std::cout << "--- Memory Integration Test ---" << std::endl;
    if (experiment.testMemoryIntegration()) {
        std::cout << "Memory integration test passed." << std::endl;
    } else {
        std::cout << "Memory integration test failed." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    if (experiment.testNeuromodulationIntegration()) {
        std::cout << "Neuromodulation integration test passed." << std::endl;
    } else {
        std::cout << "Neuromodulation integration test failed." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    if (experiment.testCheckpointing()) {
        std::cout << "Checkpoint test passed." << std::endl;
    } else {
        std::cout << "Checkpoint test failed." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    if (experiment.testReplay()) {
        std::cout << "Replay test passed." << std::endl;
    } else {
        std::cout << "Replay test failed." << std::endl;
    }
    std::cout << std::endl;
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Parse file config if available
    if (!fileConfig.empty()) {
        for (const auto& item : fileConfig) {
            std::string key = toLower(item.first);
            std::string value = item.second;
            
            if (key == "neuroncount") {
                config.neuronCount = stringToNumber<size_t>(value);
            } else if (key == "maxsteps") {
                config.maxSteps = stringToNumber<uint64_t>(value);
            } else if (key == "enablecheckpointing" && (value == "true" || value == "1")) {
                config.enableCheckpointing = true;
            } else if (key == "enablereplay" && (value == "true" || value == "1")) {
                config.enableReplay = true;
            } else if (key == "enabledevelopment" && (value == "true" || value == "1")) {
                config.enableDevelopment = true;
            }
        }
    }
    
    // Parse command line config
    if (args.find("neurons") != args.end()) {
        config.neuronCount = stringToNumber<size_t>(args["neurons"]);
    }
    if (args.find("steps") != args.end()) {
        config.maxSteps = stringToNumber<uint64_t>(args["steps"]);
    }
    if (args.find("checkpoints") != std::string::npos && (args["checkpoints"] == "false" || args["checkpoints"] == "0")) {
        config.enableCheckpointing = false;
    }
    if (args.find("replay") != std::string::npos && (args["replay"] == "false" || args["replay"] == "0")) {
        config.enableReplay = false;
    }
    if (args.find("development") != std::string::npos && (args["development"] == "false" || args["development"] == "0")) {
        config.enableDevelopment = false;
    }
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
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
    std::cout << std::endl;
    std::cout << "=== DEMONSTRATION SUMMARY ===" << std::endl;
    
    int integrationCount = 0;
    if (result.memoryWorkingMemoryIntegrated) integrationCount++;
    if (result.memoryEpisodicMemoryIntegrated) integrationCount++;
    if (result.neuromodulationIntegrated) integrationCount++;
    if (result.predictionIntegrated) integrationCount++;
    if (result.developmentIntegrated) integrationCount++;
    if (result.checkpointingWorks) integrationCount++;
    
    std::cout << "Systems successfully integrated: " << integrationCount << "/6" << std::endl;
    
    if (integrationCount >= 5) {
        std::cout << "✓ DEMONSTRATION SUCCESSFUL: NLM brain systems are properly integrated!" << std::endl;
    } else {
        std::cout << "⚠ DEMONSTRATION LIMITED: Some systems may need attention." << std::endl;
    }
    
    return 0;
}