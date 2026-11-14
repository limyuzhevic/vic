#ifndef NLM_CONFIG_UTILS_HPP
#define NLM_CONFIG_UTILS_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <map>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration validation utilities
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid = true;
        std::string errorMessage;
        std::vector<std::string> warnings;
    };
    
    static ValidationResult validateConfig(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        
        // Check for required parameters
        if (!config->has("random_seed")) {
            result.warnings.push_back("random_seed not set, using default value");
        }
        
        if (!config->has("neuron_count")) {
            result.errorMessage = "neuron_count is required";
            result.isValid = false;
        }
        
        if (!config->has("simulation_timestep")) {
            result.warnings.push_back("simulation_timestep not set, using default 0.001");
        }
        
        // Validate parameter ranges
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
        if (neuronCount < 1 || neuronCount > 100000) {
            result.errorMessage = "neuron_count must be between 1 and 100000";
            result.isValid = false;
        }
        
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0 || timestep > 1.0) {
            result.errorMessage = "simulation_timestep must be between 0 and 1.0 seconds";
            result.isValid = false;
        }
        
        double connectionProb = config->getOr<float>("connection_probability", 0.1f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            result.errorMessage = "connection_probability must be between 0.0 and 1.0";
            result.isValid = false;
        }
        
        // Check for development parameters
        double synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.001f);
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
            result.errorMessage = "synaptogenesis_rate must be between 0.0 and 1.0";
            result.isValid = false;
        }
        
        double pruningRate = config->getOr<float>("pruning_rate", 0.0001f);
        if (pruningRate < 0.0f || pruningRate > 1.0f) {
            result.errorMessage = "pruning_rate must be between 0.0 and 1.0";
            result.isValid = false;
        }
        
        // Check for plasticity parameters
        double stdpLtpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        if (stdpLtpWeight < 0.0f || stdpLtpWeight > 1.0f) {
            result.errorMessage = "stdp_ltp_weight must be between 0.0 and 1.0";
            result.isValid = false;
        }
        
        double stdpLtdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        if (stdpLtdWeight < 0.0f || stdpLtdWeight > 1.0f) {
            result.errorMessage = "stdp_ltd_weight must be between 0.0 and 1.0";
            result.isValid = false;
        }
        
        // Check for neuromodulation parameters
        float dopamineBaseline = config->getOr<float>("dopamine_baseline", 0.1f);
        if (dopamineBaseline < 0.0f || dopamineBaseline > 10.0f) {
            result.errorMessage = "dopamine_baseline must be between 0.0 and 10.0";
            result.isValid = false;
        }
        
        // Warn about potentially problematic configurations
        if (neuronCount > 10000) {
            result.warnings.push_back("Large neuron count (" + std::to_string(neuronCount) + ") may impact performance");
        }
        
        if (connectionProb < 0.01f) {
            result.warnings.push_back("Low connection probability may result in disconnected network");
        }
        
        if (synaptogenesisRate > 0.01f) {
            result.warnings.push_back("High synaptogenesis rate may cause excessive growth");
        }
        
        if (pruningRate > 0.01f) {
            result.warnings.push_back("High pruning rate may cause excessive synapse loss");
        }
        
        return result;
    }
    
    // Apply safe defaults for missing parameters
    static void applySafeDefaults(std::shared_ptr<Config>& config) {
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        
        if (!config->has("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        }
        
        if (!config->has("simulation_timestep")) {
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
        }
        
        if (!config->has("region_count")) {
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        }
        
        if (!config->has("connection_probability")) {
            config->set("connection_probability", 0.1f, ConfigSource::Default);
        }
        
        if (!config->has("stdp_ltp_weight")) {
            config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        }
        
        if (!config->has("stdp_ltd_weight")) {
            config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        }
        
        if (!config->has("stdp_time_constant")) {
            config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
        }
        
        if (!config->has("synaptogenesis_rate")) {
            config->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
        }
        
        if (!config->has("pruning_rate")) {
            config->set("pruning_rate", 0.0001f, ConfigSource::Default);
        }
        
        if (!config->has("dopamine_baseline")) {
            config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
        }
        
        if (!config->has("reward_discount_factor")) {
            config->set("reward_discount_factor", 0.99, ConfigSource::Default);
        }
        
        if (!config->has("environment_width")) {
            config->set("environment_width", 10, ConfigSource::Default);
        }
        
        if (!config->has("environment_height")) {
            config->set("environment_height", 10, ConfigSource::Default);
        }
        
        if (!config->has("vision_width")) {
            config->set("vision_width", 16, ConfigSource::Default);
        }
        
        if (!config->has("vision_height")) {
            config->set("vision_height", 16, ConfigSource::Default);
        }
        
        if (!config->has("log_level")) {
            config->set("log_level", "INFO", ConfigSource::Default);
        }
    }
};

// Error handling utilities
class ErrorHandler {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_PARAMETER = 1,
        CONFIGURATION_ERROR = 2,
        MEMORY_ERROR = 3,
        SIMULATION_ERROR = 4,
        UNKNOWN_ERROR = 5
    };
    
    struct ErrorInfo {
        ErrorCode code;
        std::string message;
        std::string context;
        std::string suggestedFix;
    };
    
    static void handleError(const ErrorInfo& error) {
        std::cerr << "NLM Error [" << static_cast<int>(error.code) << "]: " << error.message << std::endl;
        if (!error.context.empty()) {
            std::cerr << "Context: " << error.context << std::endl;
        }
        if (!error.suggestedFix.empty()) {
            std::cerr << "Suggestion: " << error.suggestedFix << std::endl;
        }
        
        // Exit with error code
        std::exit(static_cast<int>(error.code));
    }
    
    static void printError(const std::string& message) {
        std::cerr << "NLM Error: " << message << std::endl;
    }
};

// Configuration file parser with validation
class ConfigFileParser {
public:
    static bool parseConfigFile(const std::string& filepath, std::shared_ptr<Config>& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        bool inComment = false;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key=value pairs
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                // Invalid line format
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Remove whitespace from key and value
            key.erase(key.find_last_not_of(" \t") + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            
            value.erase(value.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            
            // Convert value based on key type
            if (key == "random_seed") {
                try {
                    long seed = std::stol(value);
                    config->set("random_seed", seed, ConfigSource::File);
                } catch (const std::exception& e) {
                    ErrorHandler::printError("Failed to parse random_seed value: " + value + ". Using default.");
                }
            } else if (key == "neuron_count" || key == "region_count") {
                try {
                    long count = std::stol(value);
                    config->set(key, count, ConfigSource::File);
                } catch (const std::exception& e) {
                    ErrorHandler::printError("Failed to parse " + key + " value: " + value + ". Using default.");
                }
            } else if (key == "simulation_timestep" || key == "reward_discount_factor" || 
                      key == "environment_width" || key == "environment_height" ||
                      key == "vision_width" || key == "vision_height" ||
                      key == "stdp_time_constant" || key == "dopamine_baseline") {
                try {
                    double val = std::stod(value);
                    config->set(key, val, ConfigSource::File);
                } catch (const std::exception& e) {
                    ErrorHandler::printError("Failed to parse " + key + " value: " + value + ". Using default.");
                }
            } else if (key == "connection_probability" || key == "plasticity_learning_rate" ||
                      key == "stdp_ltp_weight" || key == "stdp_ltd_weight" ||
                      key == "synaptogenesis_rate" || key == "pruning_rate") {
                try {
                    float val = std::stof(value);
                    config->set(key, val, ConfigSource::File);
                } catch (const std::exception& e) {
                    ErrorHandler::printError("Failed to parse " + key + " value: " + value + ". Using default.");
                }
            } else if (key == "log_level") {
                config->set(key, value, ConfigSource::File);
            } else {
                ErrorHandler::printError("Unknown configuration key: " + key);
            }
        }
        
        file.close();
        return true;
    }
};

// Performance optimization utilities
class PerformanceOptimizer {
public:
    struct PerformanceProfile {
        double memoryUsageMB = 0.0;
        double cpuUsagePercent = 0.0;
        double initializationTimeMs = 0.0;
        double simulationSpeed = 0.0;
        size_t totalOperations = 0;
    };
    
    static PerformanceProfile measurePerformance(std::shared_ptr<Brain> brain) {
        PerformanceProfile profile;
        
        // Measure initialization time
        auto startTime = std::chrono::high_resolution_clock::now();
        if (brain && !brain->initialize()) {
            return profile;
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        profile.initializationTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        if (!brain) return profile;
        
        // Measure simulation performance
        startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            profile.totalOperations += brain->getFiringNeuronCount();
        }
        
        endTime = std::chrono::high_resolution_clock::now();
        double simulationTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        if (simulationTimeMs > 0) {
            profile.simulationSpeed = 100.0 / simulationTimeMs * 1000.0;  // steps per second
        }
        
        // Estimate memory usage (rough approximation)
        profile.memoryUsageMB = (brain->getTotalNeuronCount() * sizeof(float) + 
                               brain->getTotalSynapseCount() * sizeof(float)) / (1024.0 * 1024.0);
        
        return profile;
    }
    
    // Optimize configuration based on available resources
    static void optimizeForResources(std::shared_ptr<Config>& config) {
        // Read available memory (simplified - in real implementation would check system)
        long availableMemory = config->getOr<long>("system_memory_limit_mb", 2048);
        
        int64_t currentNeuronCount = config->getOr<int64_t>("neuron_count", 1000);
        
        // Adjust neuron count based on available memory
        if (currentNeuronCount > 10000 && availableMemory < 1024) {
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            ErrorHandler::printError("Reducing neuron count to 5000 due to limited memory");
        }
        
        // Adjust connection probability for performance
        double connectionProb = config->getOr<float>("connection_probability", 0.1f);
        if (connectionProb > 0.2f && availableMemory < 1024) {
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            ErrorHandler::printError("Reducing connection probability to 0.1 for better performance");
        }
        
        // Optimize simulation timestep for faster simulation
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep < 0.0005) {
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            ErrorHandler::printError("Increasing simulation timestep to 0.001 for better performance");
        }
    }
};

} // namespace nlm

#endif // NLM_CONFIG_UTILS_HPP
