/*
 * nlm/Validation.hpp - Core validation utilities for the NLM system
 * Comprehensive validation framework for configuration, parameters, and operations
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <memory>
#include "nlm/Exceptions.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getFullErrorMessage() const {
        std::ostringstream oss;
        if (!errors.empty()) {
            oss << "Validation errors:";
            for (const auto& error : errors) {
                oss << "\n  - " << error;
            }
        }
        if (!warnings.empty()) {
            if (!errors.empty()) oss << "\n\nWarnings:";
            else oss << "Validation warnings:";
            for (const auto& warning : warnings) {
                oss << "\n  - " << warning;
            }
        }
        return oss.str();
    }
};

// Range validation utilities
class RangeValidator {
public:
    static bool isInRange(float value, float min, float max, bool inclusive = true) {
        if (inclusive) {
            return value >= min && value <= max;
        } else {
            return value > min && value < max;
        }
    }
    
    static bool isInRange(double value, double min, double max, bool inclusive = true) {
        if (inclusive) {
            return value >= min && value <= max;
        } else {
            return value > min && value < max;
        }
    }
    
    static bool isInRange(int value, int min, int max, bool inclusive = true) {
        if (inclusive) {
            return value >= min && value <= max;
        } else {
            return value > min && value < max;
        }
    }
    
    static ValidationResult validateFloatRange(float value, float min, float max, 
                                               const std::string& name,
                                               bool inclusive = true) {
        ValidationResult result;
        if (!isInRange(value, min, max, inclusive)) {
            std::ostringstream oss;
            oss << "Value for '" << name << "' (" << value << ") is outside the valid range [" 
                << min << ", " << max << "]";
            result.addError(oss.str());
        }
        return result;
    }
    
    static ValidationResult validateDoubleRange(double value, double min, double max,
                                                 const std::string& name,
                                                 bool inclusive = true) {
        ValidationResult result;
        if (!isInRange(value, min, max, inclusive)) {
            std::ostringstream oss;
            oss << "Value for '" << name << "' (" << value << ") is outside the valid range [" 
                << min << ", " << max << "]";
            result.addError(oss.str());
        }
        return result;
    }
    
    static ValidationResult validateIntRange(int value, int min, int max,
                                              const std::string& name,
                                              bool inclusive = true) {
        ValidationResult result;
        if (!isInRange(value, min, max, inclusive)) {
            std::ostringstream oss;
            oss << "Value for '" << name << "' (" << value << ") is outside the valid range [" 
                << min << ", " << max << "]";
            result.addError(oss.str());
        }
        return result;
    }
};

// Type validation utilities
class TypeValidator {
public:
    static ValidationResult validateString(const std::string& value,
                                           size_t minLength = 0,
                                           size_t maxLength = std::string::max_size,
                                           const std::string& name = "string") {
        ValidationResult result;
        if (value.length() < minLength) {
            std::ostringstream oss;
            oss << "Value for '" << name << "' is too short (minimum " << minLength << " characters)";
            result.addError(oss.str());
        }
        if (value.length() > maxLength) {
            std::ostringstream oss;
            oss << "Value for '" << name << "' is too long (maximum " << maxLength << " characters)";
            result.addError(oss.str());
        }
        return result;
    }
    
    static ValidationResult validateBool(const std::string& name = "boolean") {
        return ValidationResult(); // bool validation is always valid
    }
    
    static ValidationResult validateInt(const std::string& name = "integer") {
        return ValidationResult(); // int validation is always valid
    }
    
    static ValidationResult validateDouble(const std::string& name = "double") {
        return ValidationResult(); // double validation is always valid
    }
};

// Parameter validation utilities
class ParameterValidator {
public:
    // Neuron parameter validation
    static ValidationResult validateNeuronParameters(const std::unordered_map<std::string, double>& params) {
        ValidationResult result;
        
        // Membrane capacitance validation (typically 0.1-1.0 nF)
        auto capIt = params.find("capacitance");
        if (capIt != params.end()) {
            ValidationResult capResult = RangeValidator::validateFloatRange(
                capIt->second, 0.1f, 1.0f, "neuron.capacitance");
            result.errors.insert(result.errors.end(), capResult.errors.begin(), capResult.errors.end());
            result.warnings.insert(result.warnings.end(), capResult.warnings.begin(), capResult.warnings.end());
            result.isValid = result.isValid && capResult.isValid;
        }
        
        // Time constant validation (typically 10-30 ms)
        auto tcIt = params.find("timeConstant");
        if (tcIt != params.end()) {
            ValidationResult tcResult = RangeValidator::validateFloatRange(
                tcIt->second, 10.0f, 30.0f, "neuron.timeConstant");
            result.errors.insert(result.errors.end(), tcResult.errors.begin(), tcResult.errors.end());
            result.warnings.insert(result.warnings.end(), tcResult.warnings.begin(), tcResult.warnings.end());
            result.isValid = result.isValid && tcResult.isValid;
        }
        
        // Leak conductance validation (typically -70 to -50 nS)
        auto leakIt = params.find("leakConductance");
        if (leakIt != params.end()) {
            ValidationResult leakResult = RangeValidator::validateFloatRange(
                leakIt->second, -70.0f, -50.0f, "neuron.leakConductance");
            result.errors.insert(result.errors.end(), leakResult.errors.begin(), leakResult.errors.end());
            result.warnings.insert(result.warnings.end(), leakResult.warnings.begin(), leakResult.warnings.end());
            result.isValid = result.isValid && leakResult.isValid;
        }
        
        // Threshold validation (typically -50 to -40 mV)
        auto threshIt = params.find("threshold");
        if (threshIt != params.end()) {
            ValidationResult threshResult = RangeValidator::validateFloatRange(
                threshIt->second, -50.0f, -40.0f, "neuron.threshold");
            result.errors.insert(result.errors.end(), threshResult.errors.begin(), threshResult.errors.end());
            result.warnings.insert(result.warnings.end(), threshResult.warnings.begin(), threshResult.warnings.end());
            result.isValid = result.isValid && threshResult.isValid;
        }
        
        // Refractory period validation (typically 2-10 ms)
        auto refracIt = params.find("refractoryPeriod");
        if (refracIt != params.end()) {
            ValidationResult refracResult = RangeValidator::validateIntRange(
                refIt->second, 2, 10, "neuron.refractoryPeriod");
            result.errors.insert(result.errors.end(), refracResult.errors.begin(), refracResult.errors.end());
            result.warnings.insert(result.warnings.end(), refracResult.warnings.begin(), refracResult.warnings.end());
            result.isValid = result.isValid && refracResult.isValid;
        }
        
        return result;
    }
    
    // Synaptic parameter validation
    static ValidationResult validateSynapticParameters(const std::unordered_map<std::string, double>& params) {
        ValidationResult result;
        
        // Excitatory weight validation (typically 0.1-10.0)
        auto excIt = params.find("excitatoryWeight");
        if (excIt != params.end()) {
            ValidationResult excResult = RangeValidator::validateFloatRange(
                excIt->second, 0.1f, 10.0f, "synapse.excitatoryWeight");
            result.errors.insert(result.errors.end(), excResult.errors.begin(), excResult.errors.end());
            result.warnings.insert(result.warnings.end(), excResult.warnings.begin(), excResult.warnings.end());
            result.isValid = result.isValid && excResult.isValid;
        }
        
        // Inhibitory weight validation (typically -10.0 to -0.1)
        auto inhIt = params.find("inhibitoryWeight");
        if (inhIt != params.end()) {
            ValidationResult inhResult = RangeValidator::validateFloatRange(
                inhIt->second, -10.0f, -0.1f, "synapse.inhibitoryWeight");
            result.errors.insert(result.errors.end(), inhResult.errors.begin(), inhResult.errors.end());
            result.warnings.insert(result.warnings.end(), inhResult.warnings.begin(), inhResult.warnings.end());
            result.isValid = result.isValid && inhResult.isValid;
        }
        
        return result;
    }
    
    // Plasticity parameter validation
    static ValidationResult validatePlasticityParameters(const std::unordered_map<std::string, double>& params) {
        ValidationResult result;
        
        // STDP learning rate validation (typically 0.01-0.1)
        auto stdpIt = params.find("stdpLearningRate");
        if (stdpIt != params.end()) {
            ValidationResult stdpResult = RangeValidator::validateFloatRange(
                stdpIt->second, 0.01f, 0.1f, "plasticity.stdpLearningRate");
            result.errors.insert(result.errors.end(), stdpResult.errors.begin(), stdpResult.errors.end());
            result.warnings.insert(result.warnings.end(), stdpResult.warnings.begin(), stdpResult.warnings.end());
            result.isValid = result.isValid && stdpResult.isValid;
        }
        
        // Hebbian learning rate validation (typically 0.01-0.1)
        auto hebbIt = params.find("hebbianLearningRate");
        if (hebbIt != params.end()) {
            ValidationResult hebbResult = RangeValidator::validateFloatRange(
                hebbIt->second, 0.01f, 0.1f, "plasticity.hebbianLearningRate");
            result.errors.insert(result.errors.end(), hebbResult.errors.begin(), hebbResult.errors.end());
            result.warnings.insert(result.warnings.end(), hebbResult.warnings.begin(), hebbResult.warnings.end());
            result.isValid = result.isValid && hebbResult.isValid;
        }
        
        return result;
    }
};

// Configuration validation utilities
class ConfigValidator {
public:
    // Validate configuration key-value pairs
    static ValidationResult validateConfigEntry(const std::string& key, 
                                               const std::string& value,
                                               const std::string& source = "file") {
        ValidationResult result;
        
        // Validate key format
        if (key.empty()) {
            result.addError("Configuration key cannot be empty");
        }
        
        if (key.find('=') != std::string::npos) {
            result.addError("Configuration key cannot contain '=' character");
        }
        
        if (key.find('\n') != std::string::npos || key.find('\r') != std::string::npos) {
            result.addError("Configuration key cannot contain newline characters");
        }
        
        // Validate value based on key prefix
        std::string lowerKey = key;
        std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
        
        if (lowerKey.find("neuron") != std::string::npos) {
            // Validate numeric values for neuron parameters
            bool isNumeric = true;
            for (char c : value) {
                if (!std::isdigit(c) && c != '.' && c != '-' && c != '+') {
                    isNumeric = false;
                    break;
                }
            }
            if (!isNumeric) {
                result.addError("Neuron parameter values must be numeric");
            }
        } else if (lowerKey.find("synapse") != std::string::npos) {
            // Validate numeric values for synapse parameters
            bool isNumeric = true;
            for (char c : value) {
                if (!std::isdigit(c) && c != '.' && c != '-' && c != '+') {
                    isNumeric = false;
                    break;
                }
            }
            if (!isNumeric) {
                result.addError("Synapse parameter values must be numeric");
            }
        }
        
        return result;
    }
    
    // Validate configuration entry with type checking
    static ValidationResult validateConfigEntryWithType(const std::string& key,
                                                       const std::string& value,
                                                       const std::string& expectedType,
                                                       const std::string& source = "file") {
        ValidationResult result = validateConfigEntry(key, value, source);
        
        std::string lowerKey = key;
        std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
        
        // Type checking
        if (lowerKey.find("count") != std::string::npos || 
            lowerKey.find("period") != std::string::npos ||
            lowerKey.find("steps") != std::string::npos) {
            // Integer type
            if (expectedType == "int") {
                bool isInt = true;
                for (char c : value) {
                    if (!std::isdigit(c) && c != '-' && c != '+') {
                        isInt = false;
                        break;
                    }
                }
                if (!isInt) {
                    result.addError("Expected integer value for parameter: " + key);
                }
            }
        } else if (lowerKey.find("rate") != std::string::npos ||
                  lowerKey.find("weight") != std::string::npos ||
                  lowerKey.find("potential") != std::string::npos ||
                  lowerKey.find("conductance") != std::string::npos ||
                  lowerKey.find("current") != std::string::npos ||
                  lowerKey.find("threshold") != std::string::npos ||
                  lowerKey.find("voltage") != std::string::npos) {
            // Double type
            if (expectedType == "double") {
                bool isDouble = true;
                bool hasDecimal = false;
                for (char c : value) {
                    if (!std::isdigit(c) && c != '.' && c != '-' && c != '+') {
                        isDouble = false;
                        break;
                    }
                    if (c == '.') hasDecimal = true;
                }
                if (!isDouble || (hasDecimal && value.find('.') == value.rfind('.'))) {
                    result.addError("Expected numeric value for parameter: " + key);
                }
            }
        } else if (lowerKey.find("mode") != std::string::npos ||
                  lowerKey.find("enabled") != std::string::npos ||
                  lowerKey.find("enable") != std::string::npos) {
            // Boolean type
            if (expectedType == "bool") {
                if (value != "true" && value != "false") {
                    result.addError("Expected boolean value (true/false) for parameter: " + key);
                }
            }
        }
        
        return result;
    }
    
    // Validate complete configuration
    static ValidationResult validateCompleteConfig(const std::unordered_map<std::string, std::string>& config) {
        ValidationResult result;
        
        // Check for duplicate keys (shouldn't happen with map, but good practice)
        std::unordered_map<std::string, int> keyCount;
        for (const auto& pair : config) {
            keyCount[pair.first]++;
            if (keyCount[pair.first] > 1) {
                result.addError("Duplicate configuration key: " + pair.first);
            }
        }
        
        // Validate each entry
        for (const auto& pair : config) {
            ValidationResult entryResult = validateConfigEntry(pair.first, pair.second);
            result.errors.insert(result.errors.end(), entryResult.errors.begin(), entryResult.errors.end());
            result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
        }
        
        result.isValid = result.isValid && result.errors.empty();
        return result;
    }
};

// Main validation interface
class Validation {
public:
    // Validate configuration
    static ValidationResult validateConfig(const std::unordered_map<std::string, std::string>& config) {
        return ConfigValidator::validateCompleteConfig(config);
    }
    
    // Validate neuron parameters
    static ValidationResult validateNeuronParameters(const std::unordered_map<std::string, double>& params) {
        return ParameterValidator::validateNeuronParameters(params);
    }
    
    // Validate synaptic parameters
    static ValidationResult validateSynapticParameters(const std::unordered_map<std::string, double>& params) {
        return ParameterValidator::validateSynapticParameters(params);
    }
    
    // Validate plasticity parameters
    static ValidationResult validatePlasticityParameters(const std::unordered_map<std::string, double>& params) {
        return ParameterValidator::validatePlasticityParameters(params);
    }
    
    // Validate range
    template<typename T>
    static ValidationResult validateRange(const std::string& name, T value, T min, T max, bool inclusive = true) {
        return RangeValidator::validateFloatRange(name, value, min, max, inclusive);
    }
    
    // Validate type
    template<typename T>
    static ValidationResult validateType(const std::string& value, const std::string& name) {
        return TypeValidator::validateString(value, 0, std::string::max_size, name);
    }
};

} // namespace nlm
