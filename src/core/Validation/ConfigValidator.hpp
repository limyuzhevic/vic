// Configuration validation framework

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <functional>
#include <chrono>
#include <sstream>
#include "NLMExceptions.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool valid;
    std::string message;
    std::map<std::string, std::string> details;
    std::vector<std::string> suggestions;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationResult() : valid(true), timestamp(std::chrono::system_clock::now()) {}
    
    ValidationResult(bool isValid, const std::string& msg)
        : valid(isValid), message(msg), timestamp(std::chrono::system_clock::now()) {}
    
    void addDetail(const std::string& key, const std::string& value) {
        details[key] = value;
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        if (!valid) {
            oss << "Validation failed: " << message << std::endl;
            if (!details.empty()) {
                oss << "Details:" << std::endl;
                for (const auto& pair : details) {
                    oss << "  " << pair.first << ": " << pair.second << std::endl;
                }
            }
            if (!suggestions.empty()) {
                oss << "Suggestions:" << std::endl;
                for (const auto& suggestion : suggestions) {
                    oss << "  - " << suggestion << std::endl;
                }
            }
        } else {
            oss << "Validation passed: " << message;
        }
        return oss.str();
    }
};

// Base validator interface
class BaseValidator {
public:
    virtual ~BaseValidator() = default;
    virtual ValidationResult validate(const std::string& value) const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

// Numeric validator
class NumericValidator : public BaseValidator {
public:
    NumericValidator(double min = -std::numeric_limits<double>::max(),
                    double max = std::numeric_limits<double>::max(),
                    const std::string& description = "Numeric value")
        : min(min), max(max), description(description) {}
    
    ValidationResult validate(const std::string& value) const override {
        try {
            double val = std::stod(value);
            ValidationResult result;
            if (min != -std::numeric_limits<double>::max() && val < min) {
                result.valid = false;
                result.message = "Value is below minimum: " + std::to_string(val) + " < " + std::to_string(min);
                result.addSuggestion("Increase value to at least " + std::to_string(min));
            } else if (max != std::numeric_limits<double>::max() && val > max) {
                result.valid = false;
                result.message = "Value exceeds maximum: " + std::to_string(val) + " > " + std::to_string(max);
                result.addSuggestion("Decrease value to at most " + std::to_string(max));
            }
            if (result.valid) {
                result.message = "Numeric value " + value + " is valid";
            }
            return result;
        } catch (const std::exception&) {
            ValidationResult result;
            result.valid = false;
            result.message = "Invalid numeric format: " + value;
            result.addSuggestion("Use a valid numeric format (e.g., 42, 3.14, -5)");
            return result;
        }
    }
    
    std::string getName() const override { return "NumericValidator"; }
    std::string getDescription() const override { return description; }
    
private:
    double min;
    double max;
    std::string description;
};

// String validator
class StringValidator : public BaseValidator {
public:
    StringValidator(size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                   const std::string& pattern = "",
                   const std::string& description = "String value")
        : minLength(minLength), maxLength(maxLength), pattern(pattern), description(description) {}
    
    ValidationResult validate(const std::string& value) const override {
        ValidationResult result;
        
        if (value.length() < minLength) {
            result.valid = false;
            result.message = "String is too short: " + std::to_string(value.length()) + " < " + std::to_string(minLength);
            result.addSuggestion("Increase string length to at least " + std::to_string(minLength));
        } else if (value.length() > maxLength) {
            result.valid = false;
            result.message = "String is too long: " + std::to_string(value.length()) + " > " + std::to_string(maxLength);
            result.addSuggestion("Decrease string length to at most " + std::to_string(maxLength));
        } else if (!pattern.empty()) {
            // Simple pattern matching for basic cases
            if (pattern == "^[a-zA-Z0-9_]+$") {
                // Alphanumeric only
                if (!std::all_of(value.begin(), value.end(), 
                               [](char c) { return std::isalnum(c) || c == '_'; })) {
                    result.valid = false;
                    result.message = "String contains invalid characters: " + value;
                    result.addSuggestion("Use only alphanumeric characters and underscores");
                }
            }
            // Add more pattern checks as needed
        }
        
        if (result.valid) {
            result.message = "String value is valid";
        }
        return result;
    }
    
    std::string getName() const override { return "StringValidator"; }
    std::string getDescription() const override { return description; }
    
private:
    size_t minLength;
    size_t maxLength;
    std::string pattern;
    std::string description;
};

// Configuration validator
class ConfigValidator {
public:
    struct ValidationConfig {
        std::string parameterName;
        std::string expectedType;
        std::optional<double> minValue;
        std::optional<double> maxValue;
        std::optional<size_t> minLength;
        std::optional<size_t> maxLength;
        std::string validationRule;
        std::string description;
        std::vector<std::string> allowedValues;
        std::function<bool(const std::string&)> customValidator;
    };
    
    static ValidationResult validateParameter(const ConfigValidationConfig& config, const std::string& value) {
        ValidationResult result;
        
        try {
            // Type validation
            if (config.expectedType == "integer" || config.expectedType == "int64") {
                size_t pos;
                long long intVal = std::stoll(value, &pos);
                if (pos != value.length()) {
                    result.valid = false;
                    result.message = "Expected integer, got: " + value;
                    result.addSuggestion("Use a valid integer format");
                } else {
                    result = validateNumericValue(intVal, config);
                }
            } else if (config.expectedType == "float" || config.expectedType == "double") {
                size_t pos;
                double floatVal = std::stod(value, &pos);
                if (pos != value.length()) {
                    result.valid = false;
                    result.message = "Expected float, got: " + value;
                    result.addSuggestion("Use a valid float format");
                } else {
                    result = validateNumericValue(floatVal, config);
                }
            } else if (config.expectedType == "boolean") {
                if (value != "true" && value != "false") {
                    result.valid = false;
                    result.message = "Expected boolean (true/false), got: " + value;
                    result.addSuggestion("Use 'true' or 'false'");
                }
            } else if (config.expectedType == "string") {
                result = validateStringValue(value, config);
            } else if (config.expectedType == "string_list" || config.expectedType == "array") {
                result = validateListValue(value, config);
            }
            
            // Cross-parameter validation
            if (result.valid && !config.validationRule.empty()) {
                // Parse validation rule and apply cross-parameter checks
                // This would need the full configuration context
                result = validateCrossParameter(value, config);
            }
            
            // Custom validator
            if (result.valid && config.customValidator) {
                if (!config.customValidator(value)) {
                    result.valid = false;
                    result.message = "Custom validation failed for: " + value;
                }
            }
            
            // Allowed values check
            if (result.valid && !config.allowedValues.empty()) {
                bool valueAllowed = false;
                for (const auto& allowed : config.allowedValues) {
                    if (value == allowed) {
                        valueAllowed = true;
                        break;
                    }
                }
                if (!valueAllowed) {
                    result.valid = false;
                    result.message = "Value not in allowed list: " + value;
                    result.addSuggestion("Allowed values: " + formatAllowedValues(config.allowedValues));
                }
            }
            
        } catch (const std::exception& e) {
            result.valid = false;
            result.message = "Validation error: " + std::string(e.what());
        }
        
        return result;
    }
    
private:
    template<typename T>
    ValidationResult validateNumericValue(T value, const ConfigValidationConfig& config) {
        ValidationResult result;
        
        if (config.minValue.has_value() && static_cast<double>(value) < config.minValue.value()) {
            result.valid = false;
            result.message = "Value below minimum: " + std::to_string(value) + " < " + std::to_string(config.minValue.value());
            result.addSuggestion("Increase value to at least " + std::to_string(config.minValue.value()));
        } else if (config.maxValue.has_value() && static_cast<double>(value) > config.maxValue.value()) {
            result.valid = false;
            result.message = "Value exceeds maximum: " + std::to_string(value) + " > " + std::to_string(config.maxValue.value());
            result.addSuggestion("Decrease value to at most " + std::to_string(config.maxValue.value()));
        }
        
        if (result.valid) {
            result.message = "Value " + std::to_string(value) + " is valid";
        }
        return result;
    }
    
    ValidationResult validateStringValue(const std::string& value, const ConfigValidationConfig& config) {
        ValidationResult result;
        
        if (config.minLength.has_value() && value.length() < config.minLength.value()) {
            result.valid = false;
            result.message = "String too short: " + std::to_string(value.length()) + " < " + std::to_string(config.minLength.value());
            result.addSuggestion("Increase string length to at least " + std::to_string(config.minLength.value()));
        } else if (config.maxLength.has_value() && value.length() > config.maxLength.value()) {
            result.valid = false;
            result.message = "String too long: " + std::to_string(value.length()) + " > " + std::to_string(config.maxLength.value());
            result.addSuggestion("Decrease string length to at most " + std::to_string(config.maxLength.value()));
        }
        
        if (result.valid) {
            result.message = "String value is valid";
        }
        return result;
    }
    
    ValidationResult validateListValue(const std::string& value, const ConfigValidationConfig& config) {
        ValidationResult result;
        
        // Simple list parsing (e.g., "1,2,3" or "a,b,c")
        std::vector<std::string> items;
        std::stringstream ss(value);
        std::string item;
        
        while (std::getline(ss, item, ',')) {
            items.push_back(item);
        }
        
        if (config.minLength.has_value() && items.size() < config.minLength.value()) {
            result.valid = false;
            result.message = "List too short: " + std::to_string(items.size()) + " < " + std::to_string(config.minLength.value());
            result.addSuggestion("Add more items to reach at least " + std::to_string(config.minLength.value()));
        } else if (config.maxLength.has_value() && items.size() > config.maxLength.value()) {
            result.valid = false;
            result.message = "List too long: " + std::to_string(items.size()) + " > " + std::to_string(config.maxLength.value());
            result.addSuggestion("Remove items to reduce to at most " + std::to_string(config.maxLength.value()));
        }
        
        if (result.valid) {
            result.message = "List value is valid";
        }
        return result;
    }
    
    ValidationResult validateCrossParameter(const std::string& value, const ConfigValidationConfig& config) {
        // This would need access to other configuration parameters
        // For now, return a placeholder
        ValidationResult result;
        result.message = "Cross-parameter validation not implemented for rule: " + config.validationRule;
        return result;
    }
    
    std::string formatAllowedValues(const std::vector<std::string>& values) {
        std::ostringstream oss;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << values[i] << "\"";
        }
        return oss.str();
    }
};

// Simulation parameter validator
class SimulationValidator {
public:
    struct SimulationParameters {
        double timestep;
        int64_t maxSteps;
        double timeLimit;
        int64_t neuronCount;
        int64_t regionCount;
        double connectionProbability;
    };
    
    static ValidationResult validateSimulationParameters(const SimulationParameters& params) {
        ValidationResult result;
        std::map<std::string, std::string> errors;
        
        // Basic parameter validation
        if (params.timestep <= 0.0) {
            errors["timestep"] = "Must be positive, got " + std::to_string(params.timestep);
            result.addSuggestion("Set timestep to a small positive value (e.g., 0.001)");
        }
        
        if (params.maxSteps < 0) {
            errors["max_steps"] = "Must be non-negative, got " + std::to_string(params.maxSteps);
            result.addSuggestion("Set max_steps to a non-negative value");
        }
        
        if (params.timeLimit < 0.0) {
            errors["time_limit"] = "Must be non-negative, got " + std::to_string(params.timeLimit);
            result.addSuggestion("Set time_limit to a non-negative value");
        }
        
        if (params.neuronCount <= 0) {
            errors["neuron_count"] = "Must be positive, got " + std::to_string(params.neuronCount);
            result.addSuggestion("Set neuron_count to a positive value");
        }
        
        if (params.regionCount <= 0) {
            errors["region_count"] = "Must be positive, got " + std::to_string(params.regionCount);
            result.addSuggestion("Set region_count to a positive value");
        }
        
        if (params.connectionProbability < 0.0 || params.connectionProbability > 1.0) {
            errors["connection_probability"] = 
                "Must be between 0 and 1, got " + std::to_string(params.connectionProbability);
            result.addSuggestion("Set connection_probability to a value between 0 and 1");
        }
        
        // Cross-parameter validation
        if (params.neuronCount < params.regionCount * 10) {
            errors["neuron_region_ratio"] = 
                "neuron_count (" + std::to_string(params.neuronCount) + ") "
                "should be at least 10 times region_count (" + std::to_string(params.regionCount) + ")";
            result.addSuggestion("Increase neuron_count or decrease region_count");
        }
        
        if (!errors.empty()) {
            result.valid = false;
            result.message = "Simulation parameter validation failed";
            for (const auto& pair : errors) {
                result.addDetail(pair.first, pair.second);
            }
        } else {
            result.message = "All simulation parameters are valid";
        }
        
        return result;
    }
};

// Neural component validator
class NeuronValidator {
public:
    struct NeuronParameters {
        NeuronId id;
        NeuronType type;
        MembranePotential threshold;
        MembranePotential restingPotential;
        MembranePotential resetPotential;
        uint32_t refractoryPeriod;
        float leakConductance;
        float adaptationVariable;
    };
    
    static ValidationResult validateNeuronParameters(const NeuronParameters& params) {
        ValidationResult result;
        std::map<std::string, std::string> errors;
        
        // Physical parameter validation
        if (params.threshold <= params.restingPotential) {
            errors["threshold_resting"] = 
                "Threshold (" + std::to_string(params.threshold) + ") "
                "must be greater than resting potential (" + std::to_string(params.restingPotential) + ")";
            result.addSuggestion("Set threshold to a value higher than resting potential");
        }
        
        if (params.resetPotential <= params.restingPotential) {
            errors["reset_resting"] = 
                "Reset potential (" + std::to_string(params.resetPotential) + ") "
                "must be greater than resting potential (" + std::to_string(params.restingPotential) + ")";
            result.addSuggestion("Set reset potential to a value higher than resting potential");
        }
        
        if (params.refractoryPeriod == 0) {
            errors["refractory_period"] = 
                "Refractory period (0) must be greater than 0";
            result.addSuggestion("Set refractory period to a positive value (e.g., 2)");
        }
        
        if (params.leakConductance < 0.0 || params.leakConductance > 1.0) {
            errors["leak_conductance"] = 
                "Leak conductance (" + std::to_string(params.leakConductance) + ") "
                "must be between 0 and 1";
            result.addSuggestion("Set leak conductance to a value between 0 and 1");
        }
        
        if (params.adaptationVariable < 0.0) {
            errors["adaptation_variable"] = 
                "Adaptation variable (" + std::to_string(params.adaptationVariable) + ") "
                "must be non-negative";
            result.addSuggestion("Set adaptation variable to a non-negative value");
        }
        
        if (!errors.empty()) {
            result.valid = false;
            result.message = "Neuron parameter validation failed";
            for (const auto& pair : errors) {
                result.addDetail(pair.first, pair.second);
            }
        } else {
            result.message = "All neuron parameters are valid";
        }
        
        return result;
    }
};

// Synapse validator
class SynapseValidator {
public:
    struct SynapseParameters {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId destinationNeuron;
        SynapticWeight weight;
        Delay delay;
        SynapseType type;
        float efficacy;
        float shortTermDepression;
        float shortTermFacilitation;
    };
    
    static ValidationResult validateSynapseParameters(const SynapseParameters& params) {
        ValidationResult result;
        std::map<std::string, std::string> errors;
        
        // Connectivity validation
        if (params.sourceNeuron == params.destinationNeuron) {
            errors["self_connection"] = 
                "Synapse connects neuron to itself (ID " + 
                std::to_string(static_cast<long long>(params.sourceNeuron)) + ")";
            result.addSuggestion("Remove self-connection or create a different synapse");
        }
        
        // Weight validation
        if (params.weight < -1.0f || params.weight > 1.0f) {
            errors["weight_bounds"] = 
                "Weight (" + std::to_string(params.weight) + ") "
                "must be between -1.0 and 1.0";
            result.addSuggestion("Adjust weight to be within [-1.0, 1.0]");
        }
        
        // Delay validation
        if (params.delay <= 0) {
            errors["delay"] = 
                "Delay (" + std::to_string(params.delay) + ") "
                "must be positive";
            result.addSuggestion("Set delay to a positive value");
        } else if (params.delay > 100) {
            errors["delay_too_large"] = 
                "Delay (" + std::to_string(params.delay) + ") "
                "should be <= 100 steps (practical limit)";
            result.addSuggestion("Reduce delay to at most 100 steps");
        }
        
        // Efficacy validation
        if (params.efficacy < 0.0f || params.efficacy > 2.0f) {
            errors["efficacy_bounds"] = 
                "Efficacy (" + std::to_string(params.efficacy) + ") "
                "must be between 0.0 and 2.0";
            result.addSuggestion("Adjust efficacy to be within [0.0, 2.0]");
        }
        
        // Short-term plasticity state validation
        if (params.shortTermDepression < 0.0f || params.shortTermDepression > 1.0f) {
            errors["depression_bounds"] = 
                "Short-term depression (" + std::to_string(params.shortTermDepression) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust short-term depression to be within [0.0, 1.0]");
        }
        
        if (params.shortTermFacilitation < 0.0f || params.shortTermFacilitation > 1.0f) {
            errors["facilitation_bounds"] = 
                "Short-term facilitation (" + std::to_string(params.shortTermFacilitation) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust short-term facilitation to be within [0.0, 1.0]");
        }
        
        if (!errors.empty()) {
            result.valid = false;
            result.message = "Synapse parameter validation failed";
            for (const auto& pair : errors) {
                result.addDetail(pair.first, pair.second);
            }
        } else {
            result.message = "All synapse parameters are valid";
        }
        
        return result;
    }
};

// Development system validator
class DevelopmentValidator {
public:
    struct DevelopmentParameters {
        DevelopmentStage currentStage;
        float maturity;
        float complexity;
        size_t neuronCount;
        size_t synapseCount;
    };
    
    static ValidationResult validateDevelopmentParameters(const DevelopmentParameters& params) {
        ValidationResult result;
        std::map<std::string, std::string> errors;
        
        // Maturity validation
        if (params.maturity < 0.0f || params.maturity > 1.0f) {
            errors["maturity_bounds"] = 
                "Maturity (" + std::to_string(params.maturity) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust maturity to be within [0.0, 1.0]");
        }
        
        // Complexity validation
        if (params.complexity < 0.0f || params.complexity > 1.0f) {
            errors["complexity_bounds"] = 
                "Complexity (" + std::to_string(params.complexity) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust complexity to be within [0.0, 1.0]");
        }
        
        // Growth validation
        if (params.maturity < params.complexity) {
            errors["maturity_complexity"] = 
                "Maturity (" + std::to_string(params.maturity) + ") "
                "should be >= complexity (" + std::to_string(params.complexity) + ")";
            result.addSuggestion("Increase maturity or decrease complexity");
        }
        
        // Resource validation
        if (params.neuronCount == 0) {
            errors["neuron_count"] = "Neuron count cannot be zero";
            result.addSuggestion("Add at least one neuron");
        }
        
        if (params.synapseCount < params.neuronCount) {
            errors["synapse_neuron_ratio"] = 
                "Synapse count (" + std::to_string(params.synapseCount) + ") "
                "should be at least neuron count (" + std::to_string(params.neuronCount) + ")";
            result.addSuggestion("Increase synapse count or decrease neuron count");
        }
        
        if (!errors.empty()) {
            result.valid = false;
            result.message = "Development parameter validation failed";
            for (const auto& pair : errors) {
                result.addDetail(pair.first, pair.second);
            }
        } else {
            result.message = "All development parameters are valid";
        }
        
        return result;
    }
};

// Memory system validator
class MemoryValidator {
public:
    struct MemoryParameters {
        size_t capacity;
        float consistency;
        float utilization;
        size_t allocated;
        size_t free;
    };
    
    static ValidationResult validateMemoryParameters(const MemoryParameters& params) {
        ValidationResult result;
        std::map<std::string, std::string> errors;
        
        // Capacity validation
        if (params.capacity == 0) {
            errors["capacity"] = "Capacity cannot be zero";
            result.addSuggestion("Set capacity to a positive value");
        }
        
        if (params.allocated > params.capacity) {
            errors["allocated_capacity"] = 
                "Allocated memory (" + std::to_string(params.allocated) + ") "
                "exceeds capacity (" + std::to_string(params.capacity) + ")";
            result.addSuggestion("Free up some memory or increase capacity");
        }
        
        // Consistency validation
        if (params.consistency < 0.0f || params.consistency > 1.0f) {
            errors["consistency_bounds"] = 
                "Consistency (" + std::to_string(params.consistency) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust consistency to be within [0.0, 1.0]");
        }
        
        // Utilization validation
        if (params.utilization < 0.0f || params.utilization > 1.0f) {
            errors["utilization_bounds"] = 
                "Utilization (" + std::to_string(params.utilization) + ") "
                "must be between 0.0 and 1.0";
            result.addSuggestion("Adjust utilization to be within [0.0, 1.0]");
        }
        
        // Free memory validation
        if (params.free > params.capacity) {
            errors["free_capacity"] = 
                "Free memory (" + std::to_string(params.free) + ") "
                "exceeds capacity (" + std::to_string(params.capacity) + ")";
            result.addSuggestion("Adjust free memory calculation");
        }
        
        // Utilization consistency check
        if (params.capacity > 0) {
            float calculatedUtilization = 
                static_cast<float>(params.allocated) / params.capacity;
            if (std::abs(calculatedUtilization - params.utilization) > 0.01f) {
                errors["utilization_inconsistency"] = 
                    "Utilization mismatch: calculated " + std::to_string(calculatedUtilization) + 
                    " but stored " + std::to_string(params.utilization);
                result.addSuggestion("Update utilization to match actual allocation");
            }
        }
        
        if (!errors.empty()) {
            result.valid = false;
            result.message = "Memory parameter validation failed";
            for (const auto& pair : errors) {
                result.addDetail(pair.first, pair.second);
            }
        } else {
            result.message = "All memory parameters are valid";
        }
        
        return result;
    }
};

} // namespace nlm