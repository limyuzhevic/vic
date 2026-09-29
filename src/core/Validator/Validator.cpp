#include "Validator.hpp"
#include "../Config/Config.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace nlm {

void ConfigValidator::addParameter(const std::string& key, const ParameterConstraints& constraints) {
    parameters_[key] = constraints;
}

std::vector<ValidationError> ConfigValidator::validate(const class Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& [key, param] : parameters_) {
        // Check if required
        if (param.required && !config.has(key)) {
            errors.emplace_back(ValidationErrorType::MissingRequired, key,
                              "Required configuration parameter is missing", "");
            continue;
        }
        
        if (config.has(key)) {
            // Get value and validate
            std::optional<ConfigValue> valueOpt;
            
            // Try to get value based on type
            if (param.type == "int") {
                valueOpt = config.get<int>(key);
            } else if (param.type == "int64") {
                valueOpt = config.get<int64_t>(key);
            } else if (param.type == "double") {
                valueOpt = config.get<double>(key);
            } else if (param.type == "bool") {
                valueOpt = config.get<bool>(key);
            } else if (param.type == "string") {
                valueOpt = config.get<std::string>(key);
            } else {
                // Try to get value as string for validation
                std::optional<std::string> strOpt = config.get<std::string>(key);
                if (strOpt) {
                    valueOpt = ConfigValue(*strOpt);
                }
            }
            
            if (valueOpt) {
                std::string valueStr = typeToString(*valueOpt);
                auto validationErrors = validateValue(param, valueStr);
                errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
            }
        }
    }
    
    return errors;
}

bool ConfigValidator::hasKey(const std::string& key) const {
    return parameters_.find(key) != parameters_.end();
}

const ConfigValidator::ParameterConstraints* ConfigValidator::getParameter(const std::string& key) const {
    auto it = parameters_.find(key);
    return it != parameters_.end() ? &it->second : nullptr;
}

std::vector<std::string> ConfigValidator::getParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters_.size());
    for (const auto& pair : parameters_) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigValidator::clear() {
    parameters_.clear();
}

std::string ConfigValidator::getSummary() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary (" << parameters_.size() << " parameters):\n";
    
    for (const auto& [key, param] : parameters_) {
        oss << "  " << key << ":\n";
        oss << "    Type: " << param.type << "\n";
        oss << "    Required: " << (param.required ? "yes" : "no") << "\n";
        if (!param.description.empty()) {
            oss << "    Description: " << param.description << "\n";
        }
        if (!param.defaultValue.empty()) {
            oss << "    Default: " << param.defaultValue << "\n";
        }
        if (!param.allowedValues.empty()) {
            oss << "    Allowed values: ";
            for (size_t i = 0; i < param.allowedValues.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << param.allowedValues[i].first << " (" << param.allowedValues[i].second << ")";
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

void ConfigValidator::addBrainParameters() {
    // Brain size parameters
    ParameterConstraints neuronCount;\n    neuronCount.name = "Neuron Count";
    neuronCount.description = "Total number of neurons in the brain";
    neuronCount.required = true;
    neuronCount.type = "int64";
    neuronCount.defaultValue = "1000";
    neuronCount.minValue = "100";
    neuronCount.maxValue = "100000";
    addParameter("neuron_count", neuronCount);
    
    ParameterConstraints regionCount;
    regionCount.name = "Region Count";
    regionCount.description = "Number of brain regions";
    regionCount.required = false;
    regionCount.type = "int64";
    regionCount.defaultValue = "1";
    regionCount.minValue = "1";
    regionCount.maxValue = "100";
    addParameter("region_count", regionCount);
    
    ParameterConstraints connectionProbability;
    connectionProbability.name = "Connection Probability";
    connectionProbability.description = "Probability of connection between neurons";
    connectionProbability.required = false;
    connectionProbability.type = "double";
    connectionProbability.defaultValue = "0.1";
    connectionProbability.minValue = "0.0";
    connectionProbability.maxValue = "1.0";
    addParameter("connection_probability", connectionProbability);
}

void ConfigValidator::addSensoryParameters() {
    ParameterConstraints visionWidth;
    visionWidth.name = "Vision Width";
    visionWidth.description = "Width of visual field in pixels";
    visionWidth.required = false;
    visionWidth.type = "int64";
    visionWidth.defaultValue = "8";
    visionWidth.minValue = "1";
    visionWidth.maxValue = "1000";
    addParameter("sensory.vision.width", visionWidth);
    
    ParameterConstraints visionHeight;
    visionHeight.name = "Vision Height";
    visionHeight.description = "Height of visual field in pixels";
    visionHeight.required = false;
    visionHeight.type = "int64";
    visionHeight.defaultValue = "8";
    visionHeight.minValue = "1";
    visionHeight.maxValue = "1000";
    addParameter("sensory.vision.height", visionHeight);
}

void ConfigValidator::addMotorParameters() {
    ParameterConstraints maxSpeed;
    maxSpeed.name = "Max Speed";
    maxSpeed.description = "Maximum speed for motor actions";
    maxSpeed.required = false;
    maxSpeed.type = "double";
    maxSpeed.defaultValue = "1.0";
    maxSpeed.minValue = "0.0";
    maxSpeed.maxValue = "100.0";
    addParameter("motor.max_speed", maxSpeed);
    
    ParameterConstraints acceleration;
    acceleration.name = "Acceleration";
    acceleration.description = "Acceleration for motor actions";
    acceleration.required = false;
    acceleration.type = "double";
    acceleration.defaultValue = "0.1";
    acceleration.minValue = "0.0";
    acceleration.maxValue = "10.0";
    addParameter("motor.acceleration", acceleration);
}

void ConfigValidator::addPlasticityParameters() {
    ParameterConstraints stdpLearningRate;
    stdpLearningRate.name = "STDP Learning Rate";
    stdpLearningRate.description = "Learning rate for Spike-Timing Dependent Plasticity";
    stdpLearningRate.required = false;
    stdpLearningRate.type = "double";
    stdpLearningRate.defaultValue = "0.001";
    stdpLearningRate.minValue = "0.0";
    stdpLearningRate.maxValue = "1.0";
    addParameter("plasticity.stdp.learning_rate", stdpLearningRate);
    
    ParameterConstraints hebbianLearningRate;
    hebbianLearningRate.name = "Hebbian Learning Rate";
    hebbianLearningRate.description = "Learning rate for Hebbian plasticity";
    hebbianLearningRate.required = false;
    hebbianLearningRate.type = "double";
    hebbianLearningRate.defaultValue = "0.01";
    hebbianLearningRate.minValue = "0.0";
    hebbianLearningRate.maxValue = "1.0";
    addParameter("plasticity.hebbian.learning_rate", hebbianLearningRate);
}

void ConfigValidator::addNeuromodulationParameters() {
    ParameterConstraints dopamineScale;
    dopamineScale.name = "Dopamine Scale";
    dopamineScale.description = "Scale factor for dopamine modulation";
    dopamineScale.required = false;
    dopamineScale.type = "double";
    dopamineScale.defaultValue = "1.0";
    dopamineScale.minValue = "0.0";
    dopamineScale.maxValue = "10.0";
    addParameter("neuromodulation.dopamine.scale", dopamineScale);
    
    ParameterConstraints curiosityEnabled;
    curiosityEnabled.name = "Curiosity Enabled";
    curiosityEnabled.description = "Whether curiosity-driven exploration is enabled";
    curiosityEnabled.required = false;
    curiosityEnabled.type = "bool";
    curiosityEnabled.defaultValue = "true";
    addParameter("neuromodulation.curiosity.enable", curiosityEnabled);
}

void ConfigValidator::addDevelopmentParameters() {
    ParameterConstraints developmentRate;
    developmentRate.name = "Development Rate";
    developmentRate.description = "Rate of developmental changes";
    developmentRate.required = false;
    developmentRate.type = "double";
    developmentRate.defaultValue = "0.001";
    developmentRate.minValue = "0.0";
    developmentRate.maxValue = "0.1";
    addParameter("development.rate", developmentRate);
    
    ParameterConstraints criticalPeriod;
    criticalPeriod.name = "Critical Period";
    criticalPeriod.description = "Duration of critical development period";
    criticalPeriod.required = false;
    criticalPeriod.type = "double";
    criticalPeriod.defaultValue = "100.0";
    criticalPeriod.minValue = "0.0";
    criticalPeriod.maxValue = "1000.0";
    addParameter("development.critical_period", criticalPeriod);
}

std::vector<ValidationError> ConfigValidator::validateValue(const ParameterConstraints& param, 
                                                           const std::string& value) const {
    std::vector<ValidationError> errors;
    
    // Check allowed values
    if (!param.allowedValues.empty()) {
        bool found = false;
        for (const auto& allowed : param.allowedValues) {
            if (allowed.first == value) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::string allowedList;
            for (size_t i = 0; i < param.allowedValues.size(); ++i) {
                if (i > 0) allowedList += ", ";
                allowedList += param.allowedValues[i].first;
            }
            errors.emplace_back(ValidationErrorType::InvalidValue, param.name,
                              "Value not in allowed list: " + value + ". Allowed: " + allowedList,
                              value);
        }
    }
    
    // Check range
    if (!isValueInRange(param, value)) {
        errors.emplace_back(ValidationErrorType::OutOfRange, param.name,
                          "Value is out of valid range", value);
    }
    
    // Check pattern for strings
    if (param.type == "string" && !param.pattern.empty()) {
        if (!validateStringPattern(value, param.pattern)) {
            errors.emplace_back(ValidationErrorType::InvalidValue, param.name,
                              "Value does not match pattern: " + param.pattern,
                              value);
        }
    }
    
    // Run custom validator if provided
    if (param.customValidator && !param.customValidatorName.empty()) {
        try {
            if (!param.customValidator(param.name, value)) {
                errors.emplace_back(ValidationErrorType::Custom, param.name,
                                  "Custom validation failed for: " + param.customValidatorName,
                                  value);
            }
        } catch (const std::exception& e) {
            errors.emplace_back(ValidationErrorType::Custom, param.name,
                              std::string("Custom validator exception: ") + e.what(),
                              value);
        }
    }
    
    return errors;
}

bool ConfigValidator::isValueInRange(const ParameterConstraints& param, const std::string& value) const {
    // Simple range check based on type
    if (param.type == "int64" || param.type == "int") {
        try {
            int64_t intVal = std::stoll(value);
            if (!param.minValue.empty()) {
                int64_t minVal = std::stoll(param.minValue);
                if (intVal < minVal) return false;
            }
            if (!param.maxValue.empty()) {
                int64_t maxVal = std::stoll(param.maxValue);
                if (intVal > maxVal) return false;
            }
        } catch (...) {
            return false;
        }
    } else if (param.type == "double") {
        try {
            double doubleVal = std::stod(value);
            if (!param.minValue.empty()) {
                double minVal = std::stod(param.minValue);
                if (doubleVal < minVal) return false;
            }
            if (!param.maxValue.empty()) {
                double maxVal = std::stod(param.maxValue);
                if (doubleVal > maxVal) return false;
            }
        } catch (...) {
            return false;
        }
    } else if (param.type == "bool") {
        // Booleans are valid if value is "true" or "false"
        return value == "true" || value == "false";
    }
    
    return true;
}

bool ConfigValidator::validateStringPattern(const std::string& value, const std::string& pattern) const {
    // Simple pattern matching (could be enhanced with regex)
    if (pattern.empty()) return true;
    
    // Check for wildcard patterns
    if (pattern.find('*') != std::string::npos) {
        std::string regexPattern = "^";
        size_t pos = 0;
        while ((pos = pattern.find('*', pos)) != std::string::npos) {
            size_t prevPos = pos;
            regexPattern += ".*";
            pos++;
            
            if (pos < pattern.size() && pattern[pos] != '*') {
                regexPattern += pattern.substr(pos, 1);
                pos++;
            }
        }
        
        if (pos < pattern.size()) {
            regexPattern += pattern.substr(pos);
        }
        regexPattern += "$";
        
        std::regex regex(regexPattern);
        return std::regex_match(value, regex);
    }
    
    // Exact match
    return value == pattern;
}

std::string ConfigValidator::typeToString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
            return oss.str();
        } else {
            return "";
        }
    }, value);
}

ConfigValidator& getConfigValidator() {
    static ConfigValidator validator;
    return validator;
}

} // namespace nlm
