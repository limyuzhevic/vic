ConfigValidator.cpp - Updated implementation of configuration validation with enhanced error handling

#include "ConfigValidator.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

// Global validation rules initialization
std::vector<ValidationRule> ConfigValidator::validationRules_ = {
    // Basic configuration validation rules
    ValidationRule("random_seed", "range", "0, 2147483647", "random_seed must be between 0 and 2147483647"),
    ValidationRule("simulation_timestep", "range", "0.0001, 1.0", "simulation_timestep must be between 0.0001s and 1.0s"),
    ValidationRule("neuron_count", "range", "1, 1000000", "neuron_count must be between 1 and 1000000"),
    ValidationRule("region_count", "range", "1, 100", "region_count must be between 1 and 100"),
    ValidationRule("connection_probability", "range", "0.0, 1.0", "connection_probability must be between 0.0 and 1.0"),
    ValidationRule("working_memory_capacity", "range", "0, 10000", "working_memory_capacity must be between 0 and 10000"),
    ValidationRule("stdp_ltp_weight", "range", "0.0, 1.0", "stdp_ltp_weight must be between 0.0 and 1.0"),
    ValidationRule("stdp_ltd_weight", "range", "0.0, 1.0", "stdp_ltd_weight must be between 0.0 and 1.0"),
    ValidationRule("stdp_tau", "range", "1, 1000", "stdp_tau must be between 1 and 1000"),
    ValidationRule("synaptogenesis_rate", "range", "0.0, 0.1", "synaptogenesis_rate must be between 0.0 and 0.1"),
    ValidationRule("pruning_rate", "range", "0.0, 0.01", "pruning_rate must be between 0.0 and 0.01"),
    
    // String pattern validation
    ValidationRule("checkpoint_dir", "pattern", ".+", "checkpoint_dir cannot be empty"),
    ValidationRule("config_file", "pattern", ".+", "config_file cannot be empty"),
};

// Global error handling
static std::vector<ConfigError::Error> recentErrors;

ConfigError::Error lastError;

ConfigValidationResult ConfigValidator::validate(const Config& config) {
    ConfigValidationResult result;
    
    for (const auto& rule : validationRules_) {
        if (config.has(rule.key)) {
            auto valueOpt = config.get<std::string>(rule.key);
            if (valueOpt) {
                // Convert to appropriate type based on rule
                ConfigValue value;
                if (rule.ruleType == "range" || rule.ruleType == "enum") {
                    // Try to convert to appropriate type based on typical usage
                    if (rule.key.find("seed") != std::string::npos) {
                        int64_t intVal = std::stoll(*valueOpt);
                        value = intVal;
                    } else if (rule.key.find("rate") != std::string::npos) {
                        float floatVal = std::stof(*valueOpt);
                        value = floatVal;
                    } else if (rule.key.find("count") != std::string::npos) {
                        int intVal = std::stoi(*valueOpt);
                        value = intVal;
                    } else if (rule.key.find("timestep") != std::string::npos) {
                        double doubleVal = std::stod(*valueOpt);
                        value = doubleVal;
                    } else {
                        // Default to string for pattern validation
                        value = *valueOpt;
                    }
                    
                    if (!validateValue(value, rule)) {
                        result.valid = false;
                        result.error = rule.errorMessage;
                        break;
                    }
                }
            }
        }
    }
    
    return result;
}

ConfigValidationResult ConfigValidator::validateKey(const Config& config, const std::string& key) {
    for (const auto& rule : validationRules_) {
        if (rule.key == key && config.has(key)) {
            auto valueOpt = config.get<std::string>(key);
            if (valueOpt) {
                ConfigValue value;
                if (rule.ruleType == "range") {
                    // Parse range params (min,max)
                    size_t commaPos = rule.ruleParams.find(',');
                    if (commaPos != std::string::npos) {
                        std::string minStr = rule.ruleParams.substr(0, commaPos);
                        std::string maxStr = rule.ruleParams.substr(commaPos + 1);
                        
                        double minVal = std::stod(minStr);
                        double maxVal = std::stod(maxStr);
                        
                        double val = std::stod(*valueOpt);
                        if (val < minVal || val > maxVal) {
                            return ConfigValidationResult(false, rule.errorMessage);
                        }
                    }
                } else if (rule.ruleType == "pattern") {
                    // Simple pattern validation for non-empty
                    if (valueOpt->empty()) {
                        return ConfigValidationResult(false, rule.errorMessage);
                    }
                }
            }
        }
    }
    
    return ConfigValidationResult(true);
}

void ConfigValidator::addValidationRule(const std::string& key, const std::string& type, 
                                       const std::string& params, const std::string& error) {
    validationRules_.emplace_back(key, type, params, error);
}

void ConfigValidator::clearValidationRules() {
    validationRules_.clear();
}

bool ConfigValidator::validateValue(const ConfigValue& value, const ValidationRule& rule) {
    if (rule.ruleType == "range") {
        return validateRange(value, rule.ruleParams);
    } else if (rule.ruleType == "pattern") {
        return validatePattern(value, rule.ruleParams);
    } else if (rule.ruleType == "positive") {
        return validatePositive(value);
    } else if (rule.ruleType == "nonnegative") {
        return validateNonNegative(value);
    } else if (rule.ruleType == "string_length") {
        return validateStringLength(value, rule.ruleParams);
    }
    return true;
}

bool ConfigValidator::validateRange(const ConfigValue& value, const std::string& params) {
    try {
        double val = std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), int>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), int64_t>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), double>) return arg;
            else if constexpr (std::is_same_v<decltype(arg), float>) return static_cast<double>(arg);
            else return 0.0;
        }, value);
        
        size_t commaPos = params.find(',');
        if (commaPos == std::string::npos) return true;
        
        std::string minStr = params.substr(0, commaPos);
        std::string maxStr = params.substr(commaPos + 1);
        
        double minVal = std::stod(minStr);
        double maxVal = std::stod(maxStr);
        
        return val >= minVal && val <= maxVal;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigValidator::validatePattern(const ConfigValue& value, const std::string& params) {
    try {
        std::string val = std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) return arg;
            else return std::to_string(arg);
        }, value);
        
        return !val.empty();
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigValidator::validatePositive(const ConfigValue& value) {
    try {
        double val = std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), int>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), int64_t>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), double>) return arg;
            else if constexpr (std::is_same_v<decltype(arg), float>) return static_cast<double>(arg);
            else return 0.0;
        }, value);
        
        return val > 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigValidator::validateNonNegative(const ConfigValue& value) {
    try {
        double val = std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), int>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), int64_t>) return static_cast<double>(arg);
            else if constexpr (std::is_same_v<decltype(arg), double>) return arg;
            else if constexpr (std::is_same_v<decltype(arg), float>) return static_cast<double>(arg);
            else return 0.0;
        }, value);
        
        return val >= 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigValidator::validateStringLength(const ConfigValue& value, const std::string& params) {
    try {
        std::string val = std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) return arg;
            else return std::to_string(arg);
        }, value);
        
        size_t maxLen = std::stoul(params);
        return val.length() <= maxLen;
    } catch (const std::exception&) {
        return false;
    }
}

// Error handling functions
std::string ConfigError::toString(ErrorType type) {
    switch (type) {
        case ErrorType::FileNotFound: return "File not found";
        case ErrorType::ParseError: return "Parse error";
        case ErrorType::InvalidValue: return "Invalid value";
        case ErrorType::MissingRequired: return "Missing required parameter";
        case ErrorType::ValidationError: return "Configuration validation error";
        case ErrorType::FileWriteError: return "File write error";
        case ErrorType::MemoryError: return "Memory error";
        default: return "Unknown error";
    }
}

std::string ConfigError::formatError(const Error& error) {
    std::string result = toString(error.type);
    if (!error.message.empty()) {
        result += ": " + error.message;
    }
    if (!error.key.empty()) {
        result += " (key: " + error.key + ")";
    }
    if (!error.context.empty()) {
        result += " (context: " + error.context + ")";
    }
    return result;
}
