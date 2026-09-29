#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

namespace validation {

// Custom exception for validation errors
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& context = "")
        : std::runtime_error(formatMessage(message, context)) {}
    
    static std::string formatMessage(const std::string& message, const std::string& context) {
        if (context.empty()) {
            return "Validation error: " + message;
        } else {
            return "Validation error in " + context + ": " + message;
        }
    }
};

// Basic validation functions
inline bool isValidFloat(float value, float min = -std::numeric_limits<float>::max(), 
                         float max = std::numeric_limits<float>::max()) {
    return !std::isnan(value) && !std::isinf(value) && value >= min && value <= max;
}

inline bool isValidDouble(double value, double min = -std::numeric_limits<double>::max(), 
                          double max = std::numeric_limits<double>::max()) {
    return !std::isnan(value) && !std::isinf(value) && value >= min && value <= max;
}

inline bool isValidInt(int64_t value, int64_t min = std::numeric_limits<int64_t>::min(), 
                       int64_t max = std::numeric_limits<int64_t>::max()) {
    return value >= min && value <= max;
}

inline bool isValidSize(size_t value, size_t min = 0, size_t max = std::numeric_limits<size_t>::max()) {
    return value >= min && value <= max;
}

// Range validation templates
template<typename T>
inline bool inRange(const T& value, const T& min, const T& max) {
    return value >= min && value <= max;
}

// Array validation
template<typename T>
inline bool isValidArray(const std::vector<T>& array, size_t maxSize = std::numeric_limits<size_t>::max()) {
    if (array.size() > maxSize) {
        return false;
    }
    
    for (const auto& value : array) {
        if (!isValidValue(value)) {
            return false;
        }
    }
    
    return true;
}

// Neuron state validation
struct NeuronState {
    float membranePotential;
    float restingPotential;
    float threshold;
    float refractoryPeriod;
    bool isRefractory;
};

inline bool isValidNeuronState(const NeuronState& state, float vMin = -100.0f, 
                               float vMax = 100.0f, float threshMin = -100.0f, 
                               float threshMax = 0.0f) {
    if (!isValidFloat(state.membranePotential, vMin, vMax)) return false;
    if (!isValidFloat(state.restingPotential, vMin, vMax)) return false;
    if (!isValidFloat(state.threshold, threshMin, threshMax)) return false;
    if (!isValidFloat(state.refractoryPeriod, 0.0f, 1000.0f)) return false;
    return true;
}

// Synaptic weight validation
inline bool isValidSynapticWeight(float weight, float minWeight = 0.0f, float maxWeight = 10.0f) {
    return isValidFloat(weight, minWeight, maxWeight);
}

// Configuration validation
inline bool isValidSimulationTimestep(double timestep, double minStep = 0.0001, 
                                       double maxStep = 0.1) {
    return isValidDouble(timestep, minStep, maxStep);
}

inline bool isValidNeuronCount(size_t count, size_t minCount = 1, size_t maxCount = 1000000) {
    return isValidSize(count, minCount, maxCount);
}

inline bool isValidConnectionProbability(float prob, float minProb = 0.0f, float maxProb = 1.0f) {
    return isValidFloat(prob, minProb, maxProb);
}

// Validation result with detailed error messages
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string context;
    
    ValidationResult(bool v = true, const std::string& msg = "", const std::string& ctx = "")
        : valid(v), errorMessage(msg), context(ctx) {}
    
    static ValidationResult success() {
        return ValidationResult(true);
    }
    
    static ValidationResult error(const std::string& message, const std::string& context = "") {
        return ValidationResult(false, message, context);
    }
};

// Validation utilities
class Validator {
public:
    // Generic validation with detailed error messages
    template<typename T>
    static ValidationResult validate(const std::string& name, const T& value, 
                                     const T& min, const T& max, const std::string& context = "") {
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << name << " must be between " << min << " and " << max 
                << ", got " << value;
            return ValidationResult::error(oss.str(), context);
        }
        return ValidationResult::success();
    }
    
    // Float validation with special handling for NaN and infinity
    static ValidationResult validateFloat(const std::string& name, float value, 
                                          float min = -std::numeric_limits<float>::max(), 
                                          float max = std::numeric_limits<float>::max(),
                                          const std::string& context = "") {
        if (std::isnan(value) || std::isinf(value)) {
            return ValidationResult::error(name + " is not a finite number", context);
        }
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << name << " must be between " << min << " and " << max 
                << ", got " << value;
            return ValidationResult::error(oss.str(), context);
        }
        return ValidationResult::success();
    }
    
    // Array validation
    template<typename T>
    static ValidationResult validateArray(const std::string& name, const std::vector<T>& array,
                                          size_t maxSize = std::numeric_limits<size_t>::max(),
                                          const std::string& context = "") {
        if (array.size() > maxSize) {
            std::ostringstream oss;
            oss << name << " size " << array.size() << " exceeds maximum " << maxSize;
            return ValidationResult::error(oss.str(), context);
        }
        
        for (size_t i = 0; i < array.size(); ++i) {
            auto result = validateArrayElement(name, array[i], i, context);
            if (!result.valid) {
                return result;
            }
        }
        
        return ValidationResult::success();
    }
    
private:
    template<typename T>
    static ValidationResult validateArrayElement(const std::string& name, const T& value,
                                                  size_t index, const std::string& context) {
        if (!isValidValue(value)) {
            std::ostringstream oss;
            oss << name << "[" << index << "] is invalid";
            return ValidationResult::error(oss.str(), context);
        }
        return ValidationResult::success();
    }
    
    template<typename T>
    static bool isValidValue(const T& value) {
        if constexpr (std::is_integral_v<T>) {
            return true;  // Integers are always valid
        } else if constexpr (std::is_floating_point_v<T>) {
            return isValidFloat(static_cast<float>(value));
        } else {
            return true;  // Default case
        }
    }
};

// Configuration validation
class ConfigValidator {
public:
    static ValidationResult validateConfig(const class Config& config);
    
    static ValidationResult validateSimulationParameters(const class Config& config);
    static ValidationResult validatePlasticityParameters(const class Config& config);
    static ValidationResult validateNeuromodulationParameters(const class Config& config);
    
    static ValidationResult validateBrainParameters(const class Config& config);
    static ValidationResult validateWorldParameters(const class Config& config);
    
    // Get all validation errors as a single string
    static std::string collectValidationErrors(const class Config& config);
    
private:
    static std::vector<ValidationResult> validationResults_;
};

// Neural computation validation
class NeuralComputationValidator {
public:
    static ValidationResult validateNeuronState(const NeuronState& state);
    static ValidationResult validateSynapticWeight(float weight, const std::string& context = "");
    static ValidationResult validateCurrent(float current, const std::string& context = "");
    static ValidationResult validateTime(double time, const std::string& context = "");
    
    // Integration validation
    static ValidationResult validateIntegrationResult(float result, float expectedMin, float expectedMax);
    static ValidationResult validateNormalization(float value, float min, float max);
    
    // STDP validation
    static ValidationResult validateSTDPPotentiation(float weightChange, float maxLTP = 0.1f);
    static ValidationResult validateSTDPDepression(float weightChange, float maxLTD = 0.1f);
};

// Global validation manager
class ValidationManager {
public:
    static void enableValidation(bool enabled = true) { validationEnabled_ = enabled; }
    static bool isValidationEnabled() { return validationEnabled_; }
    
    static ValidationResult validateAll(const class Config& config);
    static std::string getAllValidationErrors();
    
    // Reset validation state
    static void reset() { validationResults_.clear(); }
    
private:
    static bool validationEnabled_;
    static std::vector<ValidationResult> validationResults_;
};

} // namespace validation

} // namespace nlm
