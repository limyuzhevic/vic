#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ConfigValidator() = default;
    
    // Validate configuration values against schema
    ValidationResult validate(const Config& config) const {
        ValidationResult result;
        result.isValid = true;
        
        // Validate required keys exist
        if (!config.has("random_seed")) {
            result.errors.push_back("Missing required configuration: random_seed");
            result.isValid = false;
        }
        
        if (!config.has("neuron_count")) {
            result.errors.push_back("Missing required configuration: neuron_count");
            result.isValid = false;
        }
        
        if (!config.has("region_count")) {
            result.errors.push_back("Missing required configuration: region_count");
            result.isValid = false;
        }
        
        // Validate individual values
        validateRandomSeed(config, result);
        validateNeuronCount(config, result);
        validateRegionCount(config, result);
        validateConnectionProbability(config, result);
        validateTimestep(config, result);
        validatePlasticityParameters(config, result);
        validateStructuralPlasticity(config, result);
        
        return result;
    }
    
    // Validate JSON/YAML configuration file format
    ValidationResult validateConfigFile(const std::string& filepath) const {
        ValidationResult result;
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            result.isValid = false;
            result.errors.push_back("Cannot open configuration file: " + filepath);
            return result;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = Config::trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate key=value format
            if (line.find('=') == std::string::npos) {
                result.isValid = false;
                result.errors.push_back("Invalid format on line " + std::to_string(lineNumber) + ": missing '='");
                continue;
            }
            
            // Extract key and value
            size_t pos = line.find('=');
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Validate key
            if (!isValidKey(key)) {
                result.isValid = false;
                result.errors.push_back("Invalid key on line " + std::to_string(lineNumber) + ": " + key);
            }
            
            // Validate value format
            if (!isValidValue(value)) {
                result.warnings.push_back("Potentially invalid value on line " + std::to_string(lineNumber) + ": " + value);
            }
        }
        
        return result;
    }
    
    // Load configuration with validation
    bool loadConfigWithValidation(const std::string& filepath, std::shared_ptr<Config>& config) const {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        auto validationResult = validateConfigFile(filepath);
        if (!validationResult.isValid) {
            std::cerr << "Configuration file validation failed:" << std::endl;
            for (const auto& error : validationResult.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return false;
        }
        
        if (!validationResult.warnings.empty()) {
            std::cout << "Configuration file warnings:" << std::endl;
            for (const auto& warning : validationResult.warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
        
        // Try to load configuration
        if (!config->loadFromFile(filepath)) {
            std::cerr << "Failed to load configuration from: " << filepath << std::endl;
            return false;
        }
        
        // Validate loaded configuration
        auto validationResult2 = validate(*config);
        if (!validationResult2.isValid) {
            std::cerr << "Loaded configuration validation failed:" << std::endl;
            for (const auto& error : validationResult2.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return false;
        }
        
        return true;
    }
    
    // Get validation error messages
    static std::string getValidationErrorMessages(const ValidationResult& result) {
        std::stringstream ss;
        
        if (!result.isValid) {
            ss << "Configuration validation failed:" << std::endl;
            for (const auto& error : result.errors) {
                ss << "  - " << error << std::endl;
            }
        }
        
        if (!result.warnings.empty()) {
            ss << "\nConfiguration warnings:" << std::endl;
            for (const auto& warning : result.warnings) {
                ss << "  - " << warning << std::endl;
            }
        }
        
        return ss.str();
    }
    
private:
    // Validation helper functions
    void validateRandomSeed(const Config& config, ValidationResult& result) const {
        auto seedOpt = config.get<int64_t>("random_seed");
        if (seedOpt.has_value()) {
            int64_t seed = *seedOpt;
            if (seed < 0) {
                result.errors.push_back("random_seed must be non-negative (got " + std::to_string(seed) + ")");
                result.isValid = false;
            }
        }
    }
    
    void validateNeuronCount(const Config& config, ValidationResult& result) const {
        auto countOpt = config.get<int64_t>("neuron_count");
        if (countOpt.has_value()) {
            int64_t count = *countOpt;
            if (count < 1) {
                result.errors.push_back("neuron_count must be >= 1 (got " + std::to_string(count) + ")");
                result.isValid = false;
            } else if (count > 100000) {
                result.warnings.push_back("neuron_count is very large (" + std::to_string(count) + ") - may impact performance");
            }
        }
    }
    
    void validateRegionCount(const Config& config, ValidationResult& result) const {
        auto countOpt = config.get<int64_t>("region_count");
        if (countOpt.has_value()) {
            int64_t count = *countOpt;
            if (count < 1) {
                result.errors.push_back("region_count must be >= 1 (got " + std::to_string(count) + ")");
                result.isValid = false;
            }
        }
    }
    
    void validateConnectionProbability(const Config& config, ValidationResult& result) const {
        auto probOpt = config.get<double>("connection_probability");
        if (probOpt.has_value()) {
            double prob = *probOpt;
            if (prob < 0.0 || prob > 1.0) {
                result.errors.push_back("connection_probability must be between 0.0 and 1.0 (got " + std::to_string(prob) + ")");
                result.isValid = false;
            }
        }
    }
    
    void validateTimestep(const Config& config, ValidationResult& result) const {
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt.has_value()) {
            double timestep = *timestepOpt;
            if (timestep <= 0.0) {
                result.errors.push_back("simulation_timestep must be positive (got " + std::to_string(timestep) + ")");
                result.isValid = false;
            } else if (timestep > 1.0) {
                result.warnings.push_back("simulation_timestep is large (" + std::to_string(timestep) + ") - may affect simulation accuracy");
            }
        }
    }
    
    void validatePlasticityParameters(const Config& config, ValidationResult& result) const {
        // Validate STDP parameters
        auto ltpOpt = config.get<float>("stdp_ltp_weight");
        if (ltpOpt.has_value()) {
            float ltp = *ltpOpt;
            if (ltp < 0.0 || ltp > 1.0) {
                result.warnings.push_back("stdp_ltp_weight is outside typical range [0.0, 1.0] (got " + std::to_string(ltp) + ")");
            }
        }
        
        auto ltdOpt = config.get<float>("stdp_ltd_weight");
        if (ltdOpt.has_value()) {
            float ltd = *ltdOpt;
            if (ltd < 0.0 || ltd > 1.0) {
                result.warnings.push_back("stdp_ltd_weight is outside typical range [0.0, 1.0] (got " + std::to_string(ltd) + ")");
            }
        }
        
        auto tauOpt = config.get<float>("stdp_tau");
        if (tauOpt.has_value()) {
            float tau = *tauOpt;
            if (tau < 1.0 || tau > 100.0) {
                result.warnings.push_back("stdp_tau is outside typical range [1.0, 100.0] ms (got " + std::to_string(tau) + ")");
            }
        }
    }
    
    void validateStructuralPlasticity(const Config& config, ValidationResult& result) const {
        // Validate structural plasticity parameters
        auto synaptogenesisOpt = config.get<float>("synaptogenesis_rate");
        if (synaptogenesisOpt.has_value()) {
            float rate = *synaptogenesisOpt;
            if (rate < 0.0 || rate > 0.1) {
                result.warnings.push_back("synaptogenesis_rate is outside reasonable range [0.0, 0.1] (got " + std::to_string(rate) + ")");
            }
        }
        
        auto pruningOpt = config.get<float>("pruning_rate");
        if (pruningOpt.has_value()) {
            float rate = *pruningOpt;
            if (rate < 0.0 || rate > 0.01) {
                result.warnings.push_back("pruning_rate is outside reasonable range [0.0, 0.01] (got " + std::to_string(rate) + ")");
            }
        }
    }
    
    // Key validation
    bool isValidKey(const std::string& key) const {
        // Valid keys for NLM configuration
        static const std::vector<std::string> validKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_tau",
            "synaptogenesis_rate",
            "pruning_rate",
            "replay_interval",
            "consolidation_interval",
            "checkpoint_dir"
        };
        
        return std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
    }
    
    // Value validation
    bool isValidValue(const std::string& value) const {
        // Basic value format validation
        // Check for valid number formats
        if (value.empty()) return false;
        
        // Try to parse as number
        char* endptr;
        strtod(value.c_str(), &endptr);
        
        // If it starts with digit or decimal point, it's likely a number
        if (std::isdigit(value[0]) || value[0] == '.' || value[0] == '-') {
            return endptr != value.c_str() && *endptr == '\0';
        }
        
        // Check for quoted strings
        if ((value.front() == '\"' && value.back() == '\"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            return true;
        }
        
        // Check for boolean values
        static const std::vector<std::string> boolValues = {"true", "false"};
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        if (std::find(boolValues.begin(), boolValues.end(), lowerValue) != boolValues.end()) {
            return true;
        }
        
        return false;
    }
};

} // namespace nlm
