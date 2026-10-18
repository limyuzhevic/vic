// Implementation of Phase6Config.hpp

#include "Phase6Config.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

std::shared_ptr<Config> Phase6Config::toConfig() const {
    auto config = std::make_shared<Config>();
    
    // Core simulation parameters
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(regionCount));
    config->set("connection_probability", connectionProbability);
    config->set("simulation_timestep", timestep);
    config->set("max_steps", static_cast<int64_t>(maxSteps));
    
    // Feature flags
    config->set("enable_checkpointing", enableCheckpointing);
    config->set("enable_replay", enableReplay);
    config->set("enable_development", enableDevelopment);
    config->set("enable_neuromodulation", enableNeuromodulation);
    config->set("enable_prediction", enablePrediction);
    config->set("enable_cognition", enableCognition);
    
    // Advanced options
    config->set("reward_decay", rewardDecay);
    config->set("novelty_threshold", noveltyThreshold);
    config->set("curiosity_threshold", curiosityThreshold);
    config->set("learning_rate", learningRate);
    config->set("exploration_rate", static_cast<int64_t>(explorationRate));
    
    // Performance optimization
    config->set("parallel_processing", parallelProcessing);
    config->set("vectorization", vectorization);
    config->set("memory_pooling", memoryPooling);
    
    // Debug and analysis
    config->set("debug_mode", debugMode);
    config->set("profiling_enabled", profilingEnabled);
    config->set("verbose_logging", verboseLogging);
    
    // Validation and safety
    config->set("max_neurons", static_cast<int64_t>(maxNeurons));
    config->set("max_steps_safe", static_cast<int64_t>(maxStepsSafe));
    config->set("min_connection_probability", minConnectionProbability);
    config->set("max_connection_probability", maxConnectionProbability);
    
    return config;
}

Phase6Config Phase6Config::loadFromFile(const std::string& filename) {
    Phase6Config config;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Apply to config based on key
            applyValueFromString(config, key, value);
        }
    }
    
    return config;
}

Phase6Config Phase6Config::loadFromArgs(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                applyValueFromString(config, key, value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            applyValueFromString(config, key, value);
        }
    }
    
    return config;
}

void Phase6Config::applyPreset(Phase6Config& config, const std::string& presetName) {
    std::string preset = toLower(presetName);
    
    if (preset == "quick" || preset == "demo") {
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.connectionProbability = 0.15f;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.debugMode = true;
        config.verboseLogging = true;
    }
    else if (preset == "benchmark" || preset == "performance") {
        config.neuronCount = 2000;
        config.maxSteps = 50000;
        config.connectionProbability = 0.05f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.parallelProcessing = true;
        config.vectorization = true;
        config.memoryPooling = true;
        config.profilingEnabled = true;
        config.debugMode = false;
        config.verboseLogging = false;
    }
    else if (preset == "research" || preset == "full") {
        config.neuronCount = 5000;
        config.maxSteps = 100000;
        config.connectionProbability = 0.2f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.enableNeuromodulation = true;
        config.enablePrediction = true;
        config.enableCognition = true;
        config.learningRate = 0.005f;
        config.explorationRate = 20;
        config.parallelProcessing = true;
        config.vectorization = true;
        config.memoryPooling = true;
        config.debugMode = false;
        config.verboseLogging = false;
    }
    else if (preset == "compact" || preset == "minimal") {
        config.neuronCount = 100;
        config.maxSteps = 1000;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        config.enableNeuromodulation = false;
        config.enablePrediction = false;
        config.enableCognition = false;
        config.debugMode = true;
        config.verboseLogging = true;
    }
    else {
        std::cerr << "Warning: Unknown preset '" << presetName << "'. Using default." << std::endl;
    }
}

bool Phase6Config::validate() const {
    if (neuronCount == 0) {
        std::cerr << "Error: neuronCount must be greater than 0" << std::endl;
        return false;
    }
    
    if (neuronCount > maxNeurons) {
        std::cerr << "Error: neuronCount (" << neuronCount << ") exceeds maximum allowed (" << maxNeurons << ")" << std::endl;
        return false;
    }
    
    if (maxSteps == 0) {
        std::cerr << "Error: maxSteps must be greater than 0" << std::endl;
        return false;
    }
    
    if (maxSteps > maxStepsSafe) {
        std::cerr << "Error: maxSteps (" << maxSteps << ") exceeds safe maximum (" << maxStepsSafe << ")" << std::endl;
        return false;
    }
    
    if (connectionProbability < minConnectionProbability) {
        std::cerr << "Error: connectionProbability (" << connectionProbability << ") is below minimum (" << minConnectionProbability << ")" << std::endl;
        return false;
    }
    
    if (connectionProbability > maxConnectionProbability) {
        std::cerr << "Error: connectionProbability (" << connectionProbability << ") exceeds maximum (" << maxConnectionProbability << ")" << std::endl;
        return false;
    }
    
    if (timestep <= 0.0f) {
        std::cerr << "Error: timestep must be positive" << std::endl;
        return false;
    }
    
    if (learningRate < 0.0f || learningRate > 1.0f) {
        std::cerr << "Error: learningRate must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (explorationRate < 0 || explorationRate > 100) {
        std::cerr << "Error: explorationRate must be between 0 and 100" << std::endl;
        return false;
    }
    
    return true;
}

void Phase6Config::printSummary() const {
    std::cout << "=== Phase 6 Configuration ===" << std::endl;
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Max Steps: " << maxSteps << std::endl;
    std::cout << "Regions: " << regionCount << std::endl;
    std::cout << "Connection Probability: " << connectionProbability << std::endl;
    std::cout << "Timestep: " << timestep << "s" << std::endl;
    std::cout << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "  Neuromodulation: " << (enableNeuromodulation ? "Yes" : "No") << std::endl;
    std::cout << "  Prediction: " << (enablePrediction ? "Yes" : "No") << std::endl;
    std::cout << "  Cognition: " << (enableCognition ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  Reward Decay: " << rewardDecay << std::endl;
    std::cout << "  Novelty Threshold: " << noveltyThreshold << std::endl;
    std::cout << "  Curiosity Threshold: " << curiosityThreshold << std::endl;
    std::cout << "  Learning Rate: " << learningRate << std::endl;
    std::cout << "  Exploration Rate: " << explorationRate << "%" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance:" << std::endl;
    std::cout << "  Parallel Processing: " << (parallelProcessing ? "Yes" : "No") << std::endl;
    std::cout << "  Vectorization: " << (vectorization ? "Yes" : "No") << std::endl;
    std::cout << "  Memory Pooling: " << (memoryPooling ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    std::cout << "Debug:" << std::endl;
    std::cout << "  Debug Mode: " << (debugMode ? "Yes" : "No") << std::endl;
    std::cout << "  Profiling: " << (profilingEnabled ? "Yes" : "No") << std::endl;
    std::cout << "  Verbose Logging: " << (verboseLogging ? "Yes" : "No") << std::endl;
}

std::string Phase6Config::toString() const {
    std::ostringstream oss;
    oss << "[Phase6Config]" << std::endl;
    oss << "neuronCount=" << neuronCount << std::endl;
    oss << "maxSteps=" << maxSteps << std::endl;
    oss << "regionCount=" << regionCount << std::endl;
    oss << "connectionProbability=" << connectionProbability << std::endl;
    oss << "timestep=" << timestep << std::endl;
    oss << "enableCheckpointing=" << (enableCheckpointing ? "true" : "false") << std::endl;
    oss << "enableReplay=" << (enableReplay ? "true" : "false") << std::endl;
    oss << "enableDevelopment=" << (enableDevelopment ? "true" : "false") << std::endl;
    oss << "enableNeuromodulation=" << (enableNeuromodulation ? "true" : "false") << std::endl;
    oss << "enablePrediction=" << (enablePrediction ? "true" : "false") << std::endl;
    oss << "enableCognition=" << (enableCognition ? "true" : "false") << std::endl;
    oss << "rewardDecay=" << rewardDecay << std::endl;
    oss << "noveltyThreshold=" << noveltyThreshold << std::endl;
    oss << "curiosityThreshold=" << curiosityThreshold << std::endl;
    oss << "learningRate=" << learningRate << std::endl;
    oss << "explorationRate=" << explorationRate << std::endl;
    oss << "parallelProcessing=" << (parallelProcessing ? "true" : "false") << std::endl;
    oss << "vectorization=" << (vectorization ? "true" : "false") << std::endl;
    oss << "memoryPooling=" << (memoryPooling ? "true" : "false") << std::endl;
    oss << "debugMode=" << (debugMode ? "true" : "false") << std::endl;
    oss << "profilingEnabled=" << (profilingEnabled ? "true" : "false") << std::endl;
    oss << "verboseLogging=" << (verboseLogging ? "true" : "false") << std::endl;
    return oss.str();
}

Phase6Config Phase6Config::fromString(const std::string& configString) {
    Phase6Config config;
    std::istringstream iss(configString);
    std::string line;
    
    while (std::getline(iss, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Remove quotes from value
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        applyValueFromString(config, key, value);
    }
    
    return config;
}

void Phase6Config::applyValueFromString(Phase6Config& config, const std::string& key, const std::string& value) {
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "neuroncount") {
        config.neuronCount = std::stoull(value);
    }
    else if (lowerKey == "maxsteps") {
        config.maxSteps = std::stoull(value);
    }
    else if (lowerKey == "regioncount") {
        config.regionCount = std::stoull(value);
    }
    else if (lowerKey == "connectionprobability") {
        config.connectionProbability = std::stof(value);
    }
    else if (lowerKey == "timestep") {
        config.timestep = std::stof(value);
    }
    else if (lowerKey == "enablecheckpointing") {
        config.enableCheckpointing = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "enablereplay") {
        config.enableReplay = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "enabledevelopment") {
        config.enableDevelopment = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "enableneuromodulation") {
        config.enableNeuromodulation = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "enableprediction") {
        config.enablePrediction = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "enablecognition") {
        config.enableCognition = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "rewarddecay") {
        config.rewardDecay = std::stof(value);
    }
    else if (lowerKey == "noveltythreshold") {
        config.noveltyThreshold = std::stof(value);
    }
    else if (lowerKey == "curiositythreshold") {
        config.curiosityThreshold = std::stof(value);
    }
    else if (lowerKey == "learningrate") {
        config.learningRate = std::stof(value);
    }
    else if (lowerKey == "explorationrate") {
        config.explorationRate = std::stoi(value);
    }
    else if (lowerKey == "parallelprocessing") {
        config.parallelProcessing = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "vectorization") {
        config.vectorization = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "memorypooling") {
        config.memoryPooling = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "debugmode") {
        config.debugMode = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "profilingenabled") {
        config.profilingEnabled = (value == "true" || value == "1" || value == "yes");
    }
    else if (lowerKey == "verboselogging") {
        config.verboseLogging = (value == "true" || value == "1" || value == "yes");
    }
}

std::string Phase6Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Phase6Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
