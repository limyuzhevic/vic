#include "ConfigValidator.hpp"

namespace nlm {

ConfigValidator::ConfigValidator() {
    // Register common validation rules for different parameter types
    
    // Numeric range validators
    registerRangeValidator("neuron_count", 1, 1000000, "neuron_count must be between 1 and 1,000,000");
    registerRangeValidator("region_count", 1, 1000, "region_count must be between 1 and 1,000");
    registerRangeValidator("simulation_timestep", 1e-6, 1.0, "simulation_timestep must be between 1μs and 1.0s");
    registerRangeValidator("connection_probability", 0.0, 1.0, "connection_probability must be between 0.0 and 1.0");
    registerRangeValidator("random_seed", 0, std::numeric_limits<int64_t>::max(), "random_seed must be a positive integer");
    
    // STDP parameters
    registerRangeValidator("stdp_ltp_weight", 0.0, 1.0, "stdp_ltp_weight must be between 0.0 and 1.0");
    registerRangeValidator("stdp_ltd_weight", 0.0, 1.0, "stdp_ltd_weight must be between 0.0 and 1.0");
    registerRangeValidator("stdp_tau", 1.0, 1000.0, "stdp_tau must be between 1.0 and 1000.0");
    
    // Development parameters
    registerRangeValidator("synaptogenesis_rate", 0.0, 1.0, "synaptogenesis_rate must be between 0.0 and 1.0");
    registerRangeValidator("pruning_rate", 0.0, 1.0, "pruning_rate must be between 0.0 and 1.0");
    registerRangeValidator("development_stage", -100.0, 100.0, "development_stage must be a reasonable value");
    
    // Neuromodulation parameters
    registerRangeValidator("dopamine_baseline", 0.0, 1.0, "dopamine_baseline must be between 0.0 and 1.0");
    registerRangeValidator("plasticity_mod", 0.0, 1.0, "plasticity_mod must be between 0.0 and 1.0");
    registerRangeValidator("curiosity_level", 0.0, 1.0, "curiosity_level must be between 0.0 and 1.0");
    registerRangeValidator("prediction_error", -10.0, 10.0, "prediction_error must be between -10.0 and 10.0");
    
    // Memory parameters
    registerRangeValidator("working_memory_capacity", 0, 10000, "working_memory_capacity must be between 0 and 10,000");
    registerRangeValidator("consolidation_interval", 1, 10000, "consolidation_interval must be between 1 and 10,000");
    
    // Simulation parameters
    registerRangeValidator("max_simulation_steps", 1, 1000000, "max_simulation_steps must be between 1 and 1,000,000");
    registerRangeValidator("simulation_time_limit", 0.0, 86400.0, "simulation_time_limit must be between 0.0 and 86,400.0s");
    
    // Dependencies
    registerDependency("neurons_per_region", "neuron_count", "neurons_per_region requires neuron_count to be defined");
    registerDependency("neurons_per_region", "region_count", "neurons_per_region requires region_count to be defined");
    
    // Log level validation
    registerValidator("log_level", [](const ConfigValue& v) {
        if (std::holds_alternative<std::string>(v)) {
            std::string level = std::get<std::string>(v);
            return level == "DEBUG" || level == "INFO" || level == "WARNING" || level == "ERROR" || level == "FATAL";
        }
        return false;
    }, "log_level must be one of: DEBUG, INFO, WARNING, ERROR, FATAL");
    
    // Boolean validators
    registerValidator("log_to_file", [](const ConfigValue& v) {
        return std::holds_alternative<bool>(v);
    }, "log_to_file must be a boolean value");
    
    registerValidator("visualization_enabled", [](const ConfigValue& v) {
        return std::holds_alternative<bool>(v);
    }, "visualization_enabled must be a boolean value");
}

void ConfigValidator::registerValidator(const std::string& key,
                                       std::function<bool(const ConfigValue&)> validator,
                                       const std::string& errorMessage) {
    validators_[key] = {std::move(validator), errorMessage};
}

void ConfigValidator::registerDependency(const std::string& dependentKey,
                                        const std::string& requiredKey,
                                        const std::string& errorMessage) {
    dependencies_[dependentKey] = {requiredKey, errorMessage};
}

void ConfigValidator::registerRangeValidator(const std::string& key,
                                            double min, double max,
                                            const std::string& errorMessage) {
    rangeValidators_[key] = {min, max, errorMessage};
}

ConfigValidator::ValidationResult ConfigValidator::validate(const std::string& key, const ConfigValue& value, ConfigSource source) const {
    // Check individual validator
    auto validatorIt = validators_.find(key);
    if (validatorIt != validators_.end()) {
        if (!validatorIt->second.validator(value)) {
            return ValidationResult(false, validatorIt->second.errorMessage, key, source);
        }
    }
    
    // Check range validator
    auto rangeIt = rangeValidators_.find(key);
    if (rangeIt != rangeValidators_.end()) {
        double val;
        if (getValueAsDouble(value, val)) {
            if (val < rangeIt->second.min || val > rangeIt->second.max) {
                return ValidationResult(false, rangeIt->second.errorMessage, key, source);
            }
        } else {
            // Skip range check if value can't be converted to double
        }
    }
    
    return ValidationResult(true, "", key, source);
}

std::vector<ConfigValidator::ValidationResult> ConfigValidator::validateAll(const Config& config) const {
    std::vector<ValidationResult> results;
    
    // Validate all registered keys that exist in config
    for (const auto& validator : validators_) {
        if (config.has(validator.first)) {
            auto value = config.get(validator.first);
            if (value.has_value()) {
                results.push_back(validate(validator.first, *value));
            }
        }
    }
    
    return results;
}

std::vector<ConfigValidator::ValidationResult> ConfigValidator::checkConsistency(const Config& config) const {
    std::vector<ValidationResult> results;
    
    // Check dependencies
    for (const auto& dep : dependencies_) {
        if (config.has(dep.second.requiredKey)) {
            if (!config.has(dep.first)) {
                results.push_back(ValidationResult(false, dep.second.errorMessage, dep.first));
            }
        }
    }
    
    return results;
}

std::string ConfigValidator::getErrorMessage(const std::string& key) const {
    auto validatorIt = validators_.find(key);
    if (validatorIt != validators_.end()) {
        return validatorIt->second.errorMessage;
    }
    
    auto rangeIt = rangeValidators_.find(key);
    if (rangeIt != rangeValidators_.end()) {
        return rangeIt->second.errorMessage;
    }
    
    auto depIt = dependencies_.find(key);
    if (depIt != dependencies_.end()) {
        return depIt->second.errorMessage;
    }
    
    return "Unknown validation error";
}

std::vector<std::string> ConfigValidator::getRegisteredKeys() const {
    std::vector<std::string> keys;
    keys.reserve(validators_.size() + dependencies_.size() + rangeValidators_.size());
    
    for (const auto& validator : validators_) keys.push_back(validator.first);
    for (const auto& dep : dependencies_) keys.push_back(dep.first);
    for (const auto& range : rangeValidators_) keys.push_back(range.first);
    
    // Remove duplicates
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    
    return keys;
}

bool ConfigValidator::getValueAsDouble(const ConfigValue& value, double& out) const {
    if (std::holds_alternative<int>(value)) {
        out = static_cast<double>(std::get<int>(value));
        return true;
    } else if (std::holds_alternative<int64_t>(value)) {
        out = static_cast<double>(std::get<int64_t>(value));
        return true;
    } else if (std::holds_alternative<double>(value)) {
        out = std::get<double>(value);
        return true;
    } else if (std::holds_alternative<float>(value)) {
        out = static_cast<double>(std::get<float>(value));
        return true;
    }
    return false;
}

} // namespace nlm