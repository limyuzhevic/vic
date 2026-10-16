// src/core/Config/ConfigurationValidator.cpp
#include "ConfigurationValidator.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

ConfigurationValidator::ConfigurationValidator() : autoFixEnabled(false) {}

ConfigurationValidator::ConfigurationValidator(std::unique_ptr<ConfigurationSchema> s)
    : schema(std::move(s)), autoFixEnabled(false) {}

ValidationResult ConfigurationValidator::validate(const nlohmann::json& config) const {
    ValidationResult result;
    
    if (!hasSchema()) {
        return result; // No schema to validate against
    }
    
    // Validate against schema
    std::string schemaError;
    if (!schema->validate(config, schemaError)) {
        result.success = false;
        result.errorMessage = "Schema validation failed: " + schemaError;
        return result;
    }
    
    // Validate each parameter with custom validators
    for (const auto& [paramName, validator] : customValidators) {
        auto paramResult = validateParameter(paramName, config[paramName]);
        if (!paramResult.success) {
            result.success = false;
            result.errorMessage += "Parameter validation failed for '" + paramName + "': " + paramResult.errorMessage + "\n";
        }
        for (const auto& warning : paramResult.warnings) {
            result.addWarning(warning);
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateParameter(const std::string& paramName, const nlohmann::json& value) const {
    ValidationResult result;
    
    if (!value.is_null()) { // Skip null values (optional parameters)
        const auto* param = schema->getParameter(paramName);
        if (param) {
            // Check if value is array
            if (param->type.isArray) {
                if (!value.is_array()) {
                    result.success = false;
                    result.errorMessage = "Parameter '" + paramName + "' must be an array";
                    return result;
                }
                
                // Validate array size if specified
                if (param->defaultValue.has_value()) {
                    // For simplicity, just validate element types in the existing validate method
                }
            }
        }
    }
    
    return result;
}

nlohmann::json ConfigurationValidator::applyDefaults(const nlohmann::json& config) const {
    nlohmann::json result = config;
    
    if (!hasSchema()) {
        return result;
    }
    
    // Apply default values for missing parameters
    for (const auto& paramName : schema->getParameterNames()) {
        const auto* param = schema->getParameter(paramName);
        if (!param || param->defaultValue.has_value()) {
            continue; // Already has value or no default
        }
        
        if (result.find(paramName) == result.end()) {
            // Apply default based on type
            if (param->type.name == "int" || param->type.name == "int64") {
                if (param->defaultValue.type() == typeid(int)) {
                    result[paramName] = std::any_cast<int>(param->defaultValue);
                } else if (param->defaultValue.type() == typeid(int64_t)) {
                    result[paramName] = std::any_cast<int64_t>(param->defaultValue);
                }
            } else if (param->type.name == "double") {
                result[paramName] = std::any_cast<double>(param->defaultValue);
            } else if (param->type.name == "bool") {
                result[paramName] = std::any_cast<bool>(param->defaultValue);
            } else if (param->type.name == "string") {
                result[paramName] = std::any_cast<std::string>(param->defaultValue);
            }
        }
    }
    
    return result;
}

nlohmann::json ConfigurationValidator::autoFix(const nlohmann::json& config) const {
    nlohmann::json result = config;
    
    if (!autoFixEnabled || !hasSchema()) {
        return result;
    }
    
    // Simple auto-fix: ensure values are within reasonable bounds
    for (auto& [key, value] : result.items()) {
        if (value.is_number()) {
            double val = value.get<double>();
            
            // Fix negative values for non-negative parameters
            if (val < 0) {
                // Find if there's a non-negative parameter with this name
                for (const auto& section : schema->getSections()) {
                    for (const auto& param : section.parameters) {
                        if (param.name == key) {
                            if (param.validator && std::string(param.validator.target_type().name()) == "FZ4") {
                                // Can't determine auto-fix logic from validator directly
                                // Use default behavior for common parameter names
                                if (key == "random_seed" || key == "region_count" || key == "neuron_count") {
                                    if (val < 0) {
                                        result[key] = std::max(0.0, val);
                                    }
                                } else if (key.find("weight") != std::string::npos || key.find("rate") != std::string::npos) {
                                    if (val < 0) {
                                        result[key] = std::max(0.0, val);
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

std::string ConfigurationValidator::getValidationReport(const nlohmann::json& config) const {
    std::ostringstream oss;
    oss << "Configuration Validation Report\n";
    oss << "==============================\n\n";
    
    if (hasSchema()) {
        oss << "Schema: " << schema->getDescription() << "\n\n";
    }
    
    // Validate and report
    auto result = validate(config);
    
    if (result.success) {
        oss << "✓ Configuration is valid\n";
        if (!result.warnings.empty()) {
            oss << "\nWarnings:\n";
            for (const auto& warning : result.warnings) {
                oss << "  - " << warning << "\n";
            }
        }
    } else {
        oss << "✗ Configuration is invalid:\n";
        oss << "  " << result.errorMessage << "\n";
        if (!result.warnings.empty()) {
            oss << "\nWarnings:\n";
            for (const auto& warning : result.warnings) {
                oss << "  - " << warning << "\n";
            }
        }
    }
    
    return oss.str();
}

// Validation function implementations
namespace Validation {
    bool validatePositive(const nlohmann::json& value, std::string& error) {
        if (!value.is_number()) {
            error = "Value must be a number";
            return false;
        }
        if (value.get<double>() <= 0) {
            error = "Value must be positive";
            return false;
        }
        return true;
    }
    
    bool validateNonNegative(const nlohmann::json& value, std::string& error) {
        if (!value.is_number()) {
            error = "Value must be a number";
            return false;
        }
        if (value.get<double>() < 0) {
            error = "Value must be non-negative";
            return false;
        }
        return true;
    }
    
    bool validateInteger(const nlohmann::json& value, std::string& error) {
        if (!value.is_number_integer()) {
            error = "Value must be an integer";
            return false;
        }
        return true;
    }
    
    bool validateRange(const nlohmann::json& value, std::string& error, double min, double max) {
        if (!value.is_number()) {
            error = "Value must be a number";
            return false;
        }
        double val = value.get<double>();
        if (val < min || val > max) {
            error = "Value must be between " + std::to_string(min) + " and " + std::to_string(max);
            return false;
        }
        return true;
    }
    
    bool validateArraySize(const nlohmann::json& value, std::string& error, size_t minSize, size_t maxSize) {
        if (!value.is_array()) {
            error = "Value must be an array";
            return false;
        }
        size_t size = value.size();
        if (size < minSize || size > maxSize) {
            error = "Array size must be between " + std::to_string(minSize) + " and " + std::to_string(maxSize);
            return false;
        }
        return true;
    }
    
    bool validateStringPattern(const nlohmann::json& value, std::string& error, const std::string& pattern) {
        if (!value.is_string()) {
            error = "Value must be a string";
            return false;
        }
        // Simple pattern matching for demonstration
        if (pattern == "^[a-zA-Z0-9_]+$") {
            std::regex regex(pattern);
            if (!std::regex_match(value.get<std::string>(), regex)) {
                error = "String must match pattern: " + pattern;
                return false;
            }
        }
        return true;
    }
    
    std::string jsonToString(const nlohmann::json& value) {
        return value.is_null() ? "" : value.get<std::string>();
    }
    
    double jsonToDouble(const nlohmann::json& value) {
        return value.is_null() ? 0.0 : value.get<double>();
    }
    
    int64_t jsonToInt64(const nlohmann::json& value) {
        return value.is_null() ? 0 : value.get<int64_t>();
    }
    
    bool jsonToBool(const nlohmann::json& value) {
        return value.is_null() ? false : value.get<bool>();
    }
    
    void mergeJson(nlohmann::json& target, const nlohmann::json& source) {
        for (auto it = source.begin(); it != source.end(); ++it) {
            target[it.key()] = it.value();
        }
    }
    
    std::unique_ptr<ConfigurationSchema> createDefaultNLMConfigSchema() {
        auto schema = std::make_unique<ConfigurationSchema>();
        
        // Create core section
        ConfigSection coreSection("Core", "Basic simulation parameters");
        coreSection.addParameter(ConfigParameter("random_seed", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                                42LL, "Random seed for reproducible results"));
        coreSection.addParameter(ConfigParameter("simulation_timestep", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                0.001, "Simulation timestep in seconds"));
        coreSection.addParameter(ConfigParameter("neuron_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                                500, "Number of neurons in the simulation"));
        coreSection.addParameter(ConfigParameter("region_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                                1, "Number of neural regions"));
        coreSection.addParameter(ConfigParameter("connection_probability", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                0.15, "Probability of synaptic connections"));
        
        // Create STDP section
        ConfigSection stdpSection("STDP", "Spike-timing dependent plasticity parameters");
        stdpSection.addParameter(ConfigParameter("stdp_ltp_weight", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                0.02, "STDP LTP weight"));
        stdpSection.addParameter(ConfigParameter("stdp_ltd_weight", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                0.015, "STDP LTD weight"));
        stdpSection.addParameter(ConfigParameter("stdp_tau", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                20.0, "STDP time constant"));
        
        // Create plasticity section
        ConfigSection plasticitySection("Plasticity", "Structural plasticity parameters");
        plasticitySection.addParameter(ConfigParameter("synaptogenesis_rate", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                      0.0001, "Rate of new synapse formation"));
        plasticitySection.addParameter(ConfigParameter("pruning_rate", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                     0.00001, "Rate of synapse elimination"));
        
        schema->addSection(coreSection);
        schema->addSection(stdpSection);
        schema->addSection(plasticitySection);
        
        return schema;
    }
}
}