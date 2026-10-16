// src/core/Config/ConfigurationSchema.cpp
#include "ConfigurationSchema.hpp"
#include <sstream>
#include <iomanip>
#include <regex>

namespace nlm {

bool ConfigTypeInfo::matchesType(const std::type_index& other) const {
    return type == other;
}

bool ConfigTypeInfo::isNumeric() const {
    return type == typeid(int) || type == typeid(int64_t) || type == typeid(double);
}

// Validation function implementations
namespace {
    ValidationFunction createNotEmptyValidator() {
        return [](const nlohmann::json& value, std::string& error) -> bool {
            if (value.empty()) {
                error = "Value cannot be empty";
                return false;
            }
            return true;
        };
    }
    
    ValidationFunction createRangeValidator(double min, double max) {
        return [min, max](const nlohmann::json& value, std::string& error) -> bool {
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
        };
    }
    
    ValidationFunction createPositiveValidator() {
        return [](const nlohmann::json& value, std::string& error) -> bool {
            if (!value.is_number()) {
                error = "Value must be a number";
                return false;
            }
            if (value.get<double>() <= 0) {
                error = "Value must be positive";
                return false;
            }
            return true;
        };
    }
    
    ValidationFunction createNonNegativeValidator() {
        return [](const nlohmann::json& value, std::string& error) -> bool {
            if (!value.is_number()) {
                error = "Value must be a number";
                return false;
            }
            if (value.get<double>() < 0) {
                error = "Value must be non-negative";
                return false;
            }
            return true;
        };
    }
    
    ValidationFunction createIntegerValidator() {
        return [](const nlohmann::json& value, std::string& error) -> bool {
            if (!value.is_number_integer()) {
                error = "Value must be an integer";
                return false;
            }
            return true;
        };
    }
    
    ValidationFunction createPatternValidator(const std::string& pattern) {
        std::regex regex(pattern);
        return [regex](const nlohmann::json& value, std::string& error) -> bool {
            if (!value.is_string()) {
                error = "Value must be a string";
                return false;
            }
            if (!std::regex_match(value.get<std::string>(), regex)) {
                error = "Value must match pattern: " + pattern;
                return false;
            }
            return true;
        };
    }
}

ValidationFunction Schema::isNotEmpty() {
    return createNotEmptyValidator();
}

ValidationFunction Schema::isWithinRange(double min, double max) {
    return createRangeValidator(min, max);
}

ValidationFunction Schema::isPositive() {
    return createPositiveValidator();
}

ValidationFunction Schema::isNonNegative() {
    return createNonNegativeValidator();
}

ValidationFunction Schema::isInteger() {
    return createIntegerValidator();
}

ValidationFunction Schema::matchesPattern(const std::string& pattern) {
    return createPatternValidator(pattern);
}

bool ConfigurationSchema::validate(const nlohmann::json& config, std::string& error) const {
    // Check required parameters
    for (const auto& param : parameterMap) {
        const auto& paramInfo = param.second;
        
        if (config.find(paramInfo.name) == config.end()) {
            if (paramInfo.required == ValidationLevel::Required) {
                error = "Missing required parameter: " + paramInfo.name;
                return false;
            }
            continue; // Optional parameter, skip if missing
        }
        
        const auto& value = config[paramInfo.name];
        
        // Validate type
        bool valid = true;
        std::string typeError;
        
        // Check if type matches
        if (paramInfo.type.isArray) {
            if (!value.is_array()) {
                error = "Parameter '" + paramInfo.name + "' must be an array";
                return false;
            }
            
            // Validate array elements
            if (paramInfo.type.elementType) {
                for (size_t i = 0; i < value.size(); ++i) {
                    const auto& elem = value[i];
                    if (paramInfo.type.elementType->isNumeric()) {
                        if (!elem.is_number()) {
                            error = "Array element '" + paramInfo.name + "' at index " + std::to_string(i) + " must be numeric";
                            return false;
                        }
                    } else if (paramInfo.type.elementType->name == "bool") {
                        if (!elem.is_boolean()) {
                            error = "Array element '" + paramInfo.name + "' at index " + std::to_string(i) + " must be boolean";
                            return false;
                        }
                    } else if (paramInfo.type.elementType->name == "string") {
                        if (!elem.is_string()) {
                            error = "Array element '" + paramInfo.name + "' at index " + std::to_string(i) + " must be string";
                            return false;
                        }
                    }
                }
            }
        } else {
            // Validate scalar type
            if (paramInfo.type.name == "int" && !value.is_number_integer()) {
                error = "Parameter '" + paramInfo.name + "' must be an integer";
                return false;
            } else if (paramInfo.type.name == "double" && !value.is_number()) {
                error = "Parameter '" + paramInfo.name + "' must be a number";
                return false;
            } else if (paramInfo.type.name == "bool" && !value.is_boolean()) {
                error = "Parameter '" + paramInfo.name + "' must be a boolean";
                return false;
            } else if (paramInfo.type.name == "string" && !value.is_string()) {
                error = "Parameter '" + paramInfo.name + "' must be a string";
                return false;
            }
        }
        
        // Validate with custom validator if provided
        if (paramInfo.validator && !paramInfo.validator(value, typeError)) {
            error = "Parameter '" + paramInfo.name + "': " + typeError;
            return false;
        }
    }
    
    return true;
}

ConfigurationSchema Schema::createDefaultSchema() {
    ConfigurationSchema schema;
    
    // Core parameters section
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
    
    // STDP parameters section
    ConfigSection stdpSection("STDP", "Spike-timing dependent plasticity parameters");
    stdpSection.addParameter(ConfigParameter("stdp_ltp_weight", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            0.02, "STDP LTP weight"));
    stdpSection.addParameter(ConfigParameter("stdp_ltd_weight", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            0.015, "STDP LTD weight"));
    stdpSection.addParameter(ConfigParameter("stdp_tau", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            20.0, "STDP time constant"));
    
    // Structural plasticity section
    ConfigSection plasticitySection("Plasticity", "Structural plasticity parameters");
    plasticitySection.addParameter(ConfigParameter("synaptogenesis_rate", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                  0.0001, "Rate of new synapse formation"));
    plasticitySection.addParameter(ConfigParameter("pruning_rate", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                                 0.00001, "Rate of synapse elimination"));
    
    schema.addSection(coreSection);
    schema.addSection(stdpSection);
    schema.addSection(plasticitySection);
    
    return schema;
}

ConfigurationSchema Schema::createExperimentalSchema() {
    ConfigurationSchema schema;
    
    // Core parameters with experimental ranges
    ConfigSection coreSection("Core", "Experimental simulation parameters");
    coreSection.addParameter(ConfigParameter("random_seed", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            42LL, "Random seed for reproducible results"));
    coreSection.addParameter(ConfigParameter("simulation_timestep", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            0.001, "Simulation timestep in seconds"));
    coreSection.addParameter(ConfigParameter("neuron_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            1000, "Number of neurons in the simulation"));
    coreSection.addParameter(ConfigParameter("region_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            3, "Number of neural regions"));
    coreSection.addParameter(ConfigParameter("connection_probability", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            0.2, "Probability of synaptic connections"));
    
    schema.addSection(coreSection);
    return schema;
}

ConfigurationSchema Schema::createPerformanceSchema() {
    ConfigurationSchema schema;
    
    // Performance-optimized core parameters
    ConfigSection coreSection("Core", "Performance-optimized simulation parameters");
    coreSection.addParameter(ConfigParameter("random_seed", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            42LL, "Random seed for reproducible results"));
    coreSection.addParameter(ConfigParameter("simulation_timestep", ConfigTypeInfo::ofDouble(), ValidationLevel::Optional,
                                            0.001, "Simulation timestep in seconds"));
    coreSection.addParameter(ConfigParameter("neuron_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            100, "Number of neurons in the simulation"));
    coreSection.addParameter(ConfigParameter("region_count", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            1, "Number of neural regions"));
    
    // Minimal required parameters
    coreSection.addParameter(ConfigParameter("min_neurons_per_region", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            10, "Minimum neurons per region"));
    coreSection.addParameter(ConfigParameter("max_connections_per_neuron", ConfigTypeInfo::ofInt64(), ValidationLevel::Optional,
                                            100, "Maximum connections per neuron"));
    
    schema.addSection(coreSection);
    return schema;
}