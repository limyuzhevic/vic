#include "ConfigValidator.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct ConfigValidator::Impl {
    std::vector<ConfigValidationRule> rules;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addRule(const ConfigValidationRule& rule) {
    pImpl->rules.push_back(rule);
}

void ConfigValidator::addRule(const std::string& key, const std::string& type, bool required, const std::string& errorMessage) {
    ConfigValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.required = required;
    rule.errorMessage = errorMessage;
    
    // Set up default validator based on type
    if (type == "int" || type == "int64") {
        rule.validator = [](const ConfigValue& v) { 
            return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v); 
        };
    } else if (type == "double") {
        rule.validator = [](const ConfigValue& v) { 
            return std::holds_alternative<double>(v); 
        };
    } else if (type == "bool") {
        rule.validator = [](const ConfigValue& v) { 
            return std::holds_alternative<bool>(v); 
        };
    } else if (type == "string") {
        rule.validator = [](const ConfigValue& v) { 
            return std::holds_alternative<std::string>(v); 
        };
    } else if (type == "float") {
        rule.validator = [](const ConfigValue& v) { 
            return std::holds_alternative<double>(v); // Use double for float
        };
    } else {
        rule.validator = [](const ConfigValue& v) { 
            return true;  // No validation for unknown types
        };
    }
    
    pImpl->rules.push_back(rule);
}

void ConfigValidator::addCustomRule(const std::string& key, std::function<bool(const ConfigValue&)> validator, const std::string& errorMessage) {
    ConfigValidationRule rule;
    rule.key = key;
    rule.validator = validator;
    rule.required = true;
    rule.errorMessage = errorMessage;
    pImpl->rules.push_back(rule);
}

bool ConfigValidator::validate(const Config& config, std::vector<std::string>& errors) const {
    errors.clear();
    
    for (const auto& rule : pImpl->rules) {
        bool hasValue = config.has(rule.key);
        
        // Check if required
        if (rule.required && !hasValue) {
            errors.push_back("Required configuration key missing: '" + rule.key + "'");
            continue;
        }
        
        // If not required and no value, skip
        if (!hasValue) {
            continue;
        }
        
        // Validate type
        auto valueOpt = config.get(rule.key);
        if (!valueOpt.has_value()) {
            errors.push_back("Configuration key '" + rule.key + "' exists but cannot be retrieved");
            continue;
        }
        
        if (rule.validator && !rule.validator(valueOpt.value())) {
            errors.push_back(rule.errorMessage.empty() ? 
                ("Invalid type for configuration key '" + rule.key + "'") : 
                rule.errorMessage);
        }
    }
    
    return errors.empty();
}

bool ConfigValidator::isValid(const Config& config) const {
    std::vector<std::string> errors;
    return validate(config, errors);
}

std::vector<std::string> ConfigValidator::getErrorsForKey(const Config& config, const std::string& key) const {
    std::vector<std::string> errors;
    
    for (const auto& rule : pImpl->rules) {
        if (rule.key == key) {
            bool hasValue = config.has(rule.key);
            
            if (rule.required && !hasValue) {
                errors.push_back("Required configuration key missing: '" + rule.key + "'");
                continue;
            }
            
            if (!hasValue) {
                continue;
            }
            
            auto valueOpt = config.get(rule.key);
            if (!valueOpt.has_value()) {
                errors.push_back("Configuration key '" + rule.key + "' exists but cannot be retrieved");
                continue;
            }
            
            if (rule.validator && !rule.validator(valueOpt.value())) {
                errors.push_back(rule.errorMessage.empty() ? 
                    ("Invalid type for configuration key '" + rule.key + "'") : 
                    rule.errorMessage);
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::getAllErrors(const Config& config) const {
    std::vector<std::string> errors;
    validate(config, errors);
    return errors;
}

size_t ConfigValidator::getRuleCount() const {
    return pImpl->rules.size();
}

void ConfigValidator::clearRules() {
    pImpl->rules.clear();
}

std::vector<ConfigValidationRule> ConfigValidator::getDefaultPhase2Rules() {
    std::vector<ConfigValidationRule> rules;
    
    // Core Phase 2 settings
    rules.emplace_back("random_seed", "int64", true, "Random seed must be an integer");
    rules.emplace_back("simulation_timestep", "double", true, "Simulation timestep must be a positive number");
    rules.emplace_back("neuron_count", "int64", true, "Neuron count must be a positive integer");
    rules.emplace_back("region_count", "int64", true, "Region count must be a positive integer");
    rules.emplace_back("connection_probability", "float", true, "Connection probability must be a valid number between 0.0 and 1.0");
    
    // Additional validation rules
    rules.emplace_back("stdp_ltp_weight", "float", false, "STDP LTP weight must be a valid number");
    rules.emplace_back("stdp_ltd_weight", "float", false, "STDP LTD weight must be a valid number");
    rules.emplace_back("stdp_tau", "float", false, "STDP time constant must be a valid number");
    rules.emplace_back("synaptogenesis_rate", "float", false, "Synaptogenesis rate must be a valid number");
    rules.emplace_back("pruning_rate", "float", false, "Pruning rate must be a valid number");
    
    return rules;
}

std::vector<ConfigValidationRule> ConfigValidator::getDefaultFullPhaseRules() {
    std::vector<ConfigValidationRule> rules = getDefaultPhase2Rules();
    
    // Add Phase 3+ specific rules
    rules.emplace_back("plasticity_learning_rate", "float", false, "Plasticity learning rate must be a valid number");
    rules.emplace_back("development_synaptogenesis_rate", "float", false, "Development synaptogenesis rate must be a valid number");
    rules.emplace_back("development_pruning_rate", "float", false, "Development pruning rate must be a valid number");
    rules.emplace_back("neuromod_dopamine_scale", "float", false, "Neuromodulation dopamine scale must be a valid number");
    
    // Memory system settings
    rules.emplace_back("memory_working_memory_capacity", "int64", false, "Working memory capacity must be a positive integer");
    rules.emplace_back("memory_episodic_max_episodes", "int64", false, "Episodic memory max episodes must be a positive integer");
    
    // Performance settings
    rules.emplace_back("performance_thread_count", "int64", false, "Thread count must be a positive integer");
    
    return rules;
}

void ConfigValidator::printValidationReport(const Config& config, const std::vector<std::string>& errors) {
    if (errors.empty()) {
        NLM_LOG_INFO("Configuration validation passed - all settings are valid");
        return;
    }
    
    NLM_LOG_ERROR("Configuration validation failed with the following errors:");
    for (const auto& error : errors) {
        NLM_LOG_ERROR("  - " + error);
    }
}

} // namespace nlm