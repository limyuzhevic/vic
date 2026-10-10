#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

ConfigValidator::ConfigValidator() {}

void ConfigValidator::addRule(const std::string& key,
                             std::function<bool(const ConfigValue&)> validator,
                             const std::string& errorMessage,
                             const std::string& warningMessage,
                             bool required) {
    rules.push_back({key, validator, errorMessage, warningMessage, required});
}

void ConfigValidator::addTypeRule(const std::string& key,
                                 std::function<bool(const ConfigValue&)> validator,
                                 const std::string& errorMessage,
                                 const std::string& warningMessage,
                                 bool required) {
    addRule(key, validator, errorMessage, warningMessage, required);
}

void ConfigValidator::addRangeRule(const std::string& key,
                                  std::function<bool(const ConfigValue&)> validator,
                                  const std::string& errorMessage,
                                  const std::string& warningMessage,
                                  bool required) {
    addRule(key, validator, errorMessage, warningMessage, required);
}

ConfigValidator::ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    
    // Check all validation rules
    for (const auto& rule : rules) {
        if (!config.has(rule.key)) {
            if (rule.required) {
                result.addError("Missing required configuration key: " + rule.key);
            }
            continue;
        }
        
        auto value = config.get(rule.key);
        if (!value.has_value()) {
            result.addError("Failed to get configuration value for key: " + rule.key);
            continue;
        }
        
        if (!rule.validator(*value)) {
            std::string error = "Validation failed for key '" + rule.key + "': ";
            if (!rule.errorMessage.empty()) {
                error += rule.errorMessage;
            } else {
                error += "Invalid value for key '" + rule.key + "'";
            }
            result.addError(error);
        }
    }
    
    return result;
}

ConfigValidator::ValidationResult ConfigValidator::validate(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    for (const auto& rule : rules) {
        if (rule.key != key) continue;
        
        if (!rule.validator(value)) {
            std::string error = "Validation failed for key '" + rule.key + "': ";
            if (!rule.errorMessage.empty()) {
                error += rule.errorMessage;
            } else {
                error += "Invalid value for key '" + rule.key + "'";
            }
            result.addError(error);
        }
    }
    
    return result;
}

void ConfigValidator::clearRules() {
    rules.clear();
}

std::vector<ConfigValidator::ValidationRule> ConfigValidator::getRulesForKey(const std::string& key) const {
    std::vector<ValidationRule> keyRules;
    for (const auto& rule : rules) {
        if (rule.key == key) {
            keyRules.push_back(rule);
        }
    }
    return keyRules;
}

ConfigValidator ConfigValidator::createDefaultValidator() {
    ConfigValidator validator;
    
    // Add common validation rules
    validator.addRule("neuron_count", 
                     [](const ConfigValue& v) { return isInt(v) && std::get<int>(v) > 0; },
                     "neuron_count must be a positive integer",
                     "neuron_count should be reasonable (50-10000)",
                     true);
    
    validator.addRule("simulation_timestep",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 1.0; },
                     "simulation_timestep must be positive and <= 1.0",
                     "simulation_timestep should be small for stability (0.001-0.1)",
                     true);
    
    validator.addRule("region_count",
                     [](const ConfigValue& v) { return isInt64(v) && std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 10; },
                     "region_count must be between 1 and 10",
                     "region_count should be appropriate for system size",
                     true);
    
    validator.addRule("connection_probability",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) >= 0.01 && std::get<double>(v) <= 1.0; },
                     "connection_probability must be between 0.01 and 1.0",
                     "connection_probability should be reasonable for connectivity",
                     true);
    
    validator.addRule("random_seed",
                     [](const ConfigValue& v) { return isInt64(v) && std::get<int64_t>(v) >= 0; },
                     "random_seed must be a non-negative integer",
                     "random_seed should be 0 for random initialization",
                     true);
    
    validator.addRule("stdp_ltp_weight",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                     "stdp_ltp_weight must be between 0.0 and 1.0",
                     "stdp_ltp_weight should be reasonable (0.01-0.1)",
                     true);
    
    validator.addRule("stdp_ltd_weight",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                     "stdp_ltd_weight must be between 0.0 and 1.0",
                     "stdp_ltd_weight should be reasonable (0.015-0.02)",
                     true);
    
    validator.addRule("stdp_tau",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) > 0.0; },
                     "stdp_tau must be positive",
                     "stdp_tau should be reasonable (10-50)",
                     true);
    
    validator.addRule("synaptogenesis_rate",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                     "synaptogenesis_rate must be between 0.0 and 1.0",
                     "synaptogenesis_rate should be small (0.0001-0.001)",
                     true);
    
    validator.addRule("pruning_rate",
                     [](const ConfigValue& v) { return isDouble(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
                     "pruning_rate must be between 0.0 and 1.0",
                     "pruning_rate should be small (0.00001-0.0001)",
                     true);
    
    return validator;
}

// Helper function implementations
bool ConfigValidator::isInt(const ConfigValue& value) {
    return std::holds_alternative<int>(value);
}

bool ConfigValidator::isInt64(const ConfigValue& value) {
    return std::holds_alternative<int64_t>(value);
}

bool ConfigValidator::isDouble(const ConfigValue& value) {
    return std::holds_alternative<double>(value);
}

bool ConfigValidator::isBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool ConfigValidator::isString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool ConfigValidator::isIntVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value);
}

bool ConfigValidator::isDoubleVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<double>>(value);
}

bool ConfigValidator::isStringVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<std::string>>(value);
}

} // namespace nlm