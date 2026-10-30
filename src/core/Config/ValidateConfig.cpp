#include "ValidateConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <map>
#include <chrono>
#include "Logger.hpp"

namespace nlm {

struct ConfigValidator::Impl {
    std::set<std::string> requiredKeys;
    std::map<std::string, std::string> validationRules;
    std::unordered_map<std::string, std::vector<std::string>> allowedValues;
    std::unordered_map<std::string, std::pair<double, double>> valueRanges;
    
    Impl() {
        // Initialize with default validation rules for NLM configuration
        initializeDefaultValidationRules();
    }
    
    void initializeDefaultValidationRules() {
        // Required keys for basic NLM operation
        requiredKeys.insert("random_seed");
        requiredKeys.insert("simulation_timestep");
        requiredKeys.insert("neuron_count");
        requiredKeys.insert("region_count");
        
        // Value validation rules
        validationRules["random_seed"] = "int >= 0";
        validationRules["simulation_timestep"] = "double > 0.0";
        validationRules["neuron_count"] = "int > 0";
        validationRules["region_count"] = "int > 0";
        
        // Value ranges
        valueRanges["random_seed"] = {0, 2147483647};
        valueRanges["simulation_timestep"] = {0.0001, 1.0};
        valueRanges["neuron_count"] = {1, 100000};
        valueRanges["region_count"] = {1, 100};
        valueRanges["connection_probability"] = {0.0, 1.0};
        valueRanges["stdp_ltp_weight"] = {0.0, 1.0};
        valueRanges["stdp_ltd_weight"] = {0.0, 1.0};
        valueRanges["stdp_tau"] = {1.0, 1000.0};
        valueRanges["synaptogenesis_rate"] = {0.0, 1.0};
        valueRanges["pruning_rate"] = {0.0, 1.0};
        
        // Allowed values for categorical settings
        allowedValues["environment_name"] = {"GridWorld", "MazeWorld", "ContinuousWorld"};
        allowedValues["log_level"] = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
    }
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

ValidationResult ConfigValidator::validateFile(const std::string& filepath) const {
    ValidationResult result;
    
    // Validate file path first
    auto pathResult = validateFilePath(filepath);
    if (!pathResult.isValid) {
        result.errors.insert(result.errors.end(), pathResult.errors.begin(), pathResult.errors.end());
        result.warnings.insert(result.warnings.end(), pathResult.warnings.begin(), pathResult.warnings.end());
    }
    
    // Check for duplicates
    auto duplicateResult = checkDuplicates(filepath);
    if (!duplicateResult.isValid) {
        result.errors.insert(result.errors.end(), duplicateResult.errors.begin(), duplicateResult.errors.end());
        result.warnings.insert(result.warnings.end(), duplicateResult.warnings.begin(), duplicateResult.warnings.end());
    }
    
    // Validate content
    auto contentResult = validateFileContent(filepath);
    if (!contentResult.isValid) {
        result.errors.insert(result.errors.end(), contentResult.errors.begin(), contentResult.errors.end());
        result.warnings.insert(result.warnings.end(), contentResult.warnings.begin(), contentResult.warnings.end());
    }
    
    return result;
}

ValidationResult ConfigValidator::validateConfig(const Config& config) const {
    ValidationResult result;
    
    // Check for duplicate keys
    std::unordered_set<std::string> seenKeys;
    for (const auto& entry : config.getKeys()) {
        if (seenKeys.find(entry) != seenKeys.end()) {
            result.addError("Duplicate key found: " + entry);
        } else {
            seenKeys.insert(entry);
        }
    }
    
    // Validate required keys
    for (const auto& requiredKey : pImpl->requiredKeys) {
        if (!config.has(requiredKey)) {
            result.addError("Missing required key: " + requiredKey);
        }
    }
    
    // Validate individual values
    for (const auto& key : config.getKeys()) {
        auto valueRange = pImpl->valueRanges.find(key);
        if (valueRange != pImpl->valueRanges.end()) {
            // Check value range
            auto value = config.get<std::string>(key);
            if (value) {
                try {
                    double val = std::stod(*value);
                    if (val < valueRange->second.first || val > valueRange->second.second) {
                        result.addError("Key " + key + " value " + *value + " is outside valid range [" +
                                      std::to_string(valueRange->second.first) + ", " +
                                      std::to_string(valueRange->second.second) + "]");
                    }
                } catch (...) {
                    // If we can't parse as double, check if it's an integer
                    try {
                        long long intVal = std::stoll(*value);
                        if (intVal < valueRange->second.first || intVal > valueRange->second.second) {
                            result.addError("Key " + key + " value " + *value + " is outside valid range [" +
                                          std::to_string(static_cast<long long>(valueRange->second.first)) + ", " +
                                          std::to_string(static_cast<long long>(valueRange->second.second)) + "]");
                        }
                    } catch (...) {
                        // Value is neither integer nor double
                        result.addError("Key " + key + " has invalid value: " + *value);
                    }
                }
            }
        }
        
        // Check allowed values
        auto allowedValues = pImpl->allowedValues.find(key);
        if (allowedValues != pImpl->allowedValues.end()) {
            auto value = config.get<std::string>(key);
            if (value) {
                if (std::find(allowedValues->second.begin(), allowedValues->second.end(), *value) == allowedValues->second.end()) {
                    result.addError("Key " + key + " has invalid value: " + *value + ". Allowed values: ");
                    for (size_t i = 0; i < allowedValues->second.size(); ++i) {
                        if (i > 0) result.errors.back() += ", ";
                        result.errors.back() += allowedValues->second[i];
                    }
                }
            }
        }
    }
    
    // Validate value relationships
    result.addWarning("Value relationship validation not yet implemented");
    
    return result;
}

void ConfigValidator::setRequiredKeys(const std::vector<std::string>& keys) {
    pImpl->requiredKeys.clear();
    pImpl->requiredKeys.insert(keys.begin(), keys.end());
}

void ConfigValidator::addValidationRule(const std::string& key, const std::string& rule) {
    pImpl->validationRules[key] = rule;
}

ValidationResult ConfigValidator::validateFilePath(const std::string& filepath) const {
    ValidationResult result;
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        result.addError("Configuration file does not exist: " + filepath);
    }
    
    // Check if it's a file (not a directory)
    if (std::filesystem::exists(filepath) && !std::filesystem::is_regular_file(filepath)) {
        result.addError("Path is not a file: " + filepath);
    }
    
    // Check if file is readable
    if (std::filesystem::exists(filepath) && !std::filesystem::is_readable(filepath)) {
        result.addError("Configuration file is not readable: " + filepath);
    }
    
    return result;
}

ValidationResult ConfigValidator::checkDuplicates(const std::string& filepath) const {
    ValidationResult result;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        result.addError("Cannot open configuration file for duplicate check: " + filepath);
        return result;
    }
    
    std::unordered_set<std::string> seenKeys;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            
            if (seenKeys.find(key) != seenKeys.end()) {
                result.addError("Duplicate key '" + key + "' found at line " + std::to_string(lineNumber));
            } else {
                seenKeys.insert(key);
            }
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateValueRanges(const std::string& filepath) const {
    ValidationResult result;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        result.addError("Cannot open configuration file for validation: " + filepath);
        return result;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate against rules
            auto ruleIt = pImpl->validationRules.find(key);
            if (ruleIt != pImpl->validationRules.end()) {
                // Parse and validate rule (simplified for now)
                result.addWarning("Validation rule for key '" + key + "' not yet implemented: " + ruleIt->second);
            }
            
            // Check value range if defined
            auto rangeIt = pImpl->valueRanges.find(key);
            if (rangeIt != pImpl->valueRanges.end()) {
                try {
                    double val = std::stod(value);
                    if (val < rangeIt->second.first || val > rangeIt->second.second) {
                        result.addError("Key '" + key + "' value '" + value + "' is outside valid range [" +
                                      std::to_string(rangeIt->second.first) + ", " +
                                      std::to_string(rangeIt->second.second) + "] at line " + std::to_string(lineNumber));
                    }
                } catch (...) {
                    // Check if integer
                    try {
                        long long intVal = std::stoll(value);
                        if (intVal < static_cast<long long>(rangeIt->second.first) || 
                            intVal > static_cast<long long>(rangeIt->second.second)) {
                            result.addError("Key '" + key + "' value '" + value + "' is outside valid range [" +
                                          std::to_string(static_cast<long long>(rangeIt->second.first)) + ", " +
                                          std::to_string(static_cast<long long>(rangeIt->second.second)) + "] at line " + std::to_string(lineNumber));
                        }
                    } catch (...) {
                        // Value is neither integer nor double
                        result.addError("Key '" + key + "' has invalid value '" + value + "' at line " + std::to_string(lineNumber));
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<std::string> ConfigValidator::getRequiredKeys() const {
    return std::vector<std::string>(pImpl->requiredKeys.begin(), pImpl->requiredKeys.end());
}

} // namespace nlm
