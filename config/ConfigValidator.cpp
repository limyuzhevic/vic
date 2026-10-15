// ConfigValidator.cpp implementation
#include "config/ConfigValidator.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <regex>

namespace nlm {

struct ConfigValidator::Impl {
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&, const Config&)>>> parameterValidators;
    std::map<std::string, std::pair<double, double>> rangeValidators;
    std::map<std::string, std::set<std::string>> enumValidators;
    std::map<std::string, std::string> dependencyValidators;
    bool envVarSubEnabled = false;
    bool fileInclusionEnabled = false;
    bool macroSupportEnabled = false;
    bool conditionalEnabled = false;
    bool hotReloadEnabled = false;
    std::map<std::string, std::string> envVarMap;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.name] = schema;
}

void ConfigValidator::setParameterValidator(const std::string& parameter,
                                           std::function<bool(const ConfigValue&, const Config&)> validator) {
    pImpl->parameterValidators[parameter].push_back(validator);
}

void ConfigValidator::setRangeValidator(const std::string& parameter, double min, double max) {
    pImpl->rangeValidators[parameter] = std::make_pair(min, max);
}

void ConfigValidator::setEnumValidator(const std::string& parameter, const std::set<std::string>& values) {
    pImpl->enumValidators[parameter] = values;
}

void ConfigValidator::setDependencyValidator(const std::string& parameter, const std::string& dependsOn) {
    pImpl->dependencyValidators[parameter] = dependsOn;
}

ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    
    // Validate against all registered schemas
    for (const auto& [schemaName, schema] : pImpl->schemas) {
        ValidationResult schemaResult = validateAgainstSchema(config, schema);
        if (!schemaResult.valid) {
            result.errors.insert(result.errors.end(), schemaResult.errors.begin(), schemaResult.errors.end());
            result.parameterErrors.insert(schemaResult.parameterErrors.begin(), schemaResult.parameterErrors.end());
        }
        result.warnings.insert(result.warnings.end(), schemaResult.warnings.begin(), schemaResult.warnings.end());
    }
    
    // Validate all parameters
    ValidationResult paramResult = validateAllParameters(config);
    if (!paramResult.valid) {
        result.errors.insert(result.errors.end(), paramResult.errors.begin(), paramResult.errors.end());
        result.parameterErrors.insert(paramResult.parameterErrors.begin(), paramResult.parameterErrors.end());
    }
    result.warnings.insert(result.warnings.end(), paramResult.warnings.begin(), paramResult.warnings.end());
    
    collectValidationErrors(result);
    return result;
}

bool ConfigValidator::isValid(const Config& config) const {
    return validate(config).valid;
}

ValidationResult ConfigValidator::validateAgainstSchema(const Config& config, const ConfigSchema& schema) const {
    ValidationResult result;
    
    // Check required parameters
    for (const auto& [param, value] : schema.defaultValues) {
        if (schema.required && !config.has(param)) {
            result.addParameterError(param, "Required parameter is missing");
        }
    }
    
    // Validate each parameter
    for (const auto& entry : config.pImpl->entries) {
        const std::string& param = entry.key;
        const ConfigValue& value = entry.value;
        
        ValidationResult paramResult = validateParameter(param, value, config);
        if (!paramResult.valid) {
            result.errors.insert(result.errors.end(), paramResult.errors.begin(), paramResult.errors.end());
            result.parameterErrors.insert(paramResult.parameterErrors.begin(), paramResult.parameterErrors.end());
        }
        result.warnings.insert(result.warnings.end(), paramResult.warnings.begin(), paramResult.warnings.end());
    }
    
    return result;
}

ValidationResult ConfigValidator::validateParameter(const std::string& parameter, const ConfigValue& value,
                                                   const Config& config) const {
    ValidationResult result;
    
    // Type validation
    if (!validateParameterType(value, "unknown")) {
        result.addParameterError(parameter, "Type validation failed");
    }
    
    // Range validation
    auto rangeIt = pImpl->rangeValidators.find(parameter);
    if (rangeIt != pImpl->rangeValidators.end()) {
        double min = rangeIt->second.first;
        double max = rangeIt->second.second;
        if (!validateParameterRange(value, min, max)) {
            result.addParameterError(parameter, "Value out of range: " + std::to_string(min) + " to " + std::to_string(max));
        }
    }
    
    // Enum validation
    auto enumIt = pImpl->enumValidators.find(parameter);
    if (enumIt != pImpl->enumValidators.end()) {
        if (!validateParameterEnum(value, enumIt->second)) {
            result.addParameterError(parameter, "Value not in allowed set");
        }
    }
    
    // Custom parameter validators
    auto validatorsIt = pImpl->parameterValidators.find(parameter);
    if (validatorsIt != pImpl->parameterValidators.end()) {
        for (const auto& validator : validatorsIt->second) {
            if (!validator(value, config)) {
                result.addParameterError(parameter, "Custom validation failed");
            }
        }
    }
    
    // Dependency validation
    auto dependencyIt = pImpl->dependencyValidators.find(parameter);
    if (dependencyIt != pImpl->dependencyValidators.end()) {
        if (!validateParameterDependency(parameter, value, config)) {
            result.addParameterError(parameter, "Dependency validation failed: requires " + dependencyIt->second);
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateAllParameters(const Config& config) const {
    ValidationResult result;
    
    // Brain parameters validation
    ValidationResult brainResult = validateBrainParameters(config);
    if (!brainResult.valid) {
        result.errors.insert(result.errors.end(), brainResult.errors.begin(), brainResult.errors.end());
        result.parameterErrors.insert(brainResult.parameterErrors.begin(), brainResult.parameterErrors.end());
    }
    result.warnings.insert(result.warnings.end(), brainResult.warnings.begin(), brainResult.warnings.end());
    
    // Plasticity parameters validation
    ValidationResult plasticityResult = validatePlasticityParameters(config);
    if (!plasticityResult.valid) {
        result.errors.insert(result.errors.end(), plasticityResult.errors.begin(), plasticityResult.errors.end());
        result.parameterErrors.insert(plasticityResult.parameterErrors.begin(), plasticityResult.parameterErrors.end());
    }
    result.warnings.insert(result.warnings.end(), plasticityResult.warnings.begin(), plasticityResult.warnings.end());
    
    // Neuromodulation parameters validation
    ValidationResult neuromodResult = validateNeuromodulationParameters(config);
    if (!neuromodResult.valid) {
        result.errors.insert(result.errors.end(), neuromodResult.errors.begin(), neuromodResult.errors.end());
        result.parameterErrors.insert(neuromodResult.parameterErrors.begin(), neuromodResult.parameterErrors.end());
    }
    result.warnings.insert(result.warnings.end(), neuromodResult.warnings.begin(), neuromodResult.warnings.end());
    
    return result;
}

ValidationResult ConfigValidator::validateBrainParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate neuron_count
    if (config.has("neuron_count")) {
        auto val = config.get<int64_t>("neuron_count");
        if (val && (*val < 10 || *val > 100000)) {
            result.addParameterError("neuron_count", "Neuron count must be between 10 and 100,000");
        }
    }
    
    // Validate region_count
    if (config.has("region_count")) {
        auto val = config.get<int64_t>("region_count");
        if (val && (*val < 1 || *val > 100)) {
            result.addParameterError("region_count", "Region count must be between 1 and 100");
        }
    }
    
    // Validate connection_probability
    if (config.has("connection_probability")) {
        auto val = config.get<double>("connection_probability");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("connection_probability", "Connection probability must be between 0.0 and 1.0");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validatePlasticityParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate STDP parameters
    if (config.has("stdp_ltp_weight")) {
        auto val = config.get<double>("stdp_ltp_weight");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("stdp_ltp_weight", "STDP LTP weight must be between 0.0 and 1.0");
        }
    }
    
    if (config.has("stdp_ltd_weight")) {
        auto val = config.get<double>("stdp_ltd_weight");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("stdp_ltd_weight", "STDP LTD weight must be between 0.0 and 1.0");
        }
    }
    
    if (config.has("stdp_time_constant")) {
        auto val = config.get<double>("stdp_time_constant");
        if (val && (*val <= 0.0)) {
            result.addParameterError("stdp_time_constant", "STDP time constant must be positive");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateNeuromodulationParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate dopamine parameters
    if (config.has("dopamine_scale")) {
        auto val = config.get<double>("dopamine_scale");
        if (val && (*val < 0.0)) {
            result.addParameterError("dopamine_scale", "Dopamine scale must be non-negative");
        }
    }
    
    if (config.has("dopamine_baseline")) {
        auto val = config.get<double>("dopamine_baseline");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("dopamine_baseline", "Dopamine baseline must be between 0.0 and 1.0");
        }
    }
    
    if (config.has("curiosity_threshold")) {
        auto val = config.get<double>("curiosity_threshold");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("curiosity_threshold", "Curiosity threshold must be between 0.0 and 1.0");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateDevelopmentParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate developmental stages
    if (config.has("developmental_stages")) {
        auto val = config.get<int64_t>("developmental_stages");
        if (val && (*val < 1 || *val > 100)) {
            result.addParameterError("developmental_stages", "Developmental stages must be between 1 and 100");
        }
    }
    
    // Validate plasticity rates
    if (config.has("plasticity_rate")) {
        auto val = config.get<double>("plasticity_rate");
        if (val && (*val < 0.0 || *val > 1.0)) {
            result.addParameterError("plasticity_rate", "Plasticity rate must be between 0.0 and 1.0");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validateMemoryParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate memory capacities
    if (config.has("working_memory_capacity")) {
        auto val = config.get<int64_t>("working_memory_capacity");
        if (val && (*val < 0)) {
            result.addParameterError("working_memory_capacity", "Working memory capacity must be non-negative");
        }
    }
    
    if (config.has("episodic_memory_size")) {
        auto val = config.get<int64_t>("episodic_memory_size");
        if (val && (*val < 0)) {
            result.addParameterError("episodic_memory_size", "Episodic memory size must be non-negative");
        }
    }
    
    return result;
}

ValidationResult ConfigValidator::validatePredictionParameters(const Config& config) const {
    ValidationResult result;
    
    // Validate prediction parameters
    if (config.has("prediction_horizon")) {
        auto val = config.get<int64_t>("prediction_horizon");
        if (val && (*val < 0)) {
            result.addParameterError("prediction_horizon", "Prediction horizon must be non-negative");
        }
    }
    
    if (config.has("prediction_error_threshold")) {
        auto val = config.get<double>("prediction_error_threshold");
        if (val && (*val < 0.0)) {
            result.addParameterError("prediction_error_threshold", "Prediction error threshold must be non-negative");
        }
    }
    
    return result;
}

bool ConfigValidator::compareConfigurations(const Config& config1, const Config& config2) const {
    if (config1.pImpl->entries.size() != config2.pImpl->entries.size()) {
        return false;
    }
    
    std::map<std::string, ConfigValue> values1, values2;
    for (const auto& entry : config1.pImpl->entries) {
        values1[entry.key] = entry.value;
    }
    for (const auto& entry : config2.pImpl->entries) {
        values2[entry.key] = entry.value;
    }
    
    return values1 == values2;
}

std::map<std::string, std::pair<std::string, std::string>> ConfigValidator::getDifferences(
    const Config& config1, const Config& config2) const {
    std::map<std::string, std::pair<std::string, std::string>> differences;
    
    std::map<std::string, ConfigValue> values1, values2;
    for (const auto& entry : config1.pImpl->entries) {
        values1[entry.key] = entry.value;
    }
    for (const auto& entry : config2.pImpl->entries) {
        values2[entry.key] = entry.value;
    }
    
    for (const auto& [key, value2] : values2) {
        auto it1 = values1.find(key);
        if (it1 == values1.end()) {
            differences[key] = std::make_pair("missing", std::to_string(value2));
        } else if (it1->second != value2) {
            differences[key] = std::make_pair(std::to_string(it1->second), std::to_string(value2));
        }
    }
    
    return differences;
}

std::shared_ptr<Config> ConfigValidator::normalizeConfig(const Config& config) const {
    auto normalized = std::make_shared<Config>();
    
    // Apply schema defaults for missing parameters
    for (const auto& [schemaName, schema] : pImpl->schemas) {
        for (const auto& [param, defaultValue] : schema.defaultValues) {
            if (!config.has(param)) {
                // Convert defaultValue to ConfigValue
                if (std::holds_alternative<int>(defaultValue)) {
                    normalized->set(param, std::get<int>(defaultValue));
                } else if (std::holds_alternative<int64_t>(defaultValue)) {
                    normalized->set(param, std::get<int64_t>(defaultValue));
                } else if (std::holds_alternative<double>(defaultValue)) {
                    normalized->set(param, std::get<double>(defaultValue));
                } else if (std::holds_alternative<bool>(defaultValue)) {
                    normalized->set(param, std::get<bool>(defaultValue));
                } else if (std::holds_alternative<std::string>(defaultValue)) {
                    normalized->set(param, std::get<std::string>(defaultValue));
                }
            }
        }
    }
    
    // Copy existing parameters
    for (const auto& entry : config.pImpl->entries) {
        normalized->set(entry.key, entry.value);
    }
    
    return normalized;
}

std::shared_ptr<Config> ConfigValidator::applyTemplate(const Config& config, const ConfigTemplate& template) const {
    auto result = std::make_shared<Config>(config); // Copy constructor
    
    // Apply template defaults for missing parameters
    for (const auto& param : template.getParameters()) {
        if (!result->has(param)) {
            auto defaults = template.getDefaultValues();
            auto it = defaults.find(param);
            if (it != defaults.end()) {
                // We need to extract the value from ConfigValue
                // This is a simplified approach
                result->set(param, it->second);
            }
        }
    }
    
    return result;
}

void ConfigValidator::collectValidationErrors(const ValidationResult& result) {
    currentErrors.clear();
    currentErrors.insert(currentErrors.end(), result.errors.begin(), result.errors.end());
    
    for (const auto& [param, errors] : result.parameterErrors) {
        for (const auto& error : errors) {
            currentErrors.push_back(param + ": " + error);
        }
    }
}

bool ConfigValidator::validateParameterType(const ConfigValue& value, const std::string& expectedType) const {
    // Basic type validation
    return !std::holds_alternative<std::monostate>(value);
}

bool ConfigValidator::validateParameterRange(const ConfigValue& value, double min, double max) const {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value) >= min && std::get<int>(value) <= max;
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value)) >= min && static_cast<double>(std::get<int64_t>(value)) <= max;
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) >= min && std::get<double>(value) <= max;
    }
    return true; // Cannot validate string or container types
}

bool ConfigValidator::validateParameterEnum(const ConfigValue& value, const std::set<std::string>& allowed) const {
    if (std::holds_alternative<std::string>(value)) {
        return allowed.find(std::get<std::string>(value)) != allowed.end();
    }
    return false;
}

bool ConfigValidator::validateParameterDependency(const std::string& parameter, const ConfigValue& value,
                                                 const Config& config) const {
    auto dependencyIt = pImpl->dependencyValidators.find(parameter);
    if (dependencyIt == pImpl->dependencyValidators.end()) {
        return true; // No dependency
    }
    
    const std::string& dependsOn = dependencyIt->second;
    if (!config.has(dependsOn)) {
        return false; // Dependency missing
    }
    
    // For now, just check that dependency exists
    // Could implement more sophisticated dependency logic
    return true;
}

} // namespace nlm
