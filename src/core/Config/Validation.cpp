// Configuration Validation Implementation
// File: /workspace/app/src/core/Config/Validation.cpp
// Provides concrete implementation of configuration validation

#include "Validation.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

void DefaultConfigValidator::validateIntegerRange(
    ValidationResult& result, const std::string& key,
    const std::string& value, int64_t min, int64_t max) const {
    try {
        uint64_t intValue = std::stoull(value);
        if (intValue < min) {
            result.addError("Config " + key + " value " + value + " below minimum " + 
                          std::to_string(min));
        } else if (intValue > max) {
            result.addWarning("Config " + key + " value " + value + " above recommended maximum " + 
                            std::to_string(max));
        }
    } catch (const std::exception&) {
        result.addError("Config " + key + " has invalid integer format: " + value);
    }
}

void DefaultConfigValidator::validateFloatRange(
    ValidationResult& result, const std::string& key,
    const std::string& value, float min, float max) const {
    try {
        float floatValue = std::stof(value);
        if (floatValue < min) {
            result.addError("Config " + key + " value " + value + " below minimum " + 
                          std::to_string(min));
        } else if (floatValue > max) {
            result.addWarning("Config " + key + " value " + value + " above recommended maximum " + 
                            std::to_string(max));
        }
    } catch (const std::exception&) {
        result.addError("Config " + key + " has invalid float format: " + value);
    }
}

void DependentConfigValidator::validateRateDependencies(ValidationResult& result) const {
    auto synRateIt = tempConfig_.find("synaptogenesis_rate");
    auto pruneRateIt = tempConfig_.find("pruning_rate");
    
    if (synRateIt != tempConfig_.end() && pruneRateIt != tempConfig_.end()) {
        try {
            float synRate = std::stof(synRateIt->second);
            float pruneRate = std::stof(pruneRateIt->second);
            
            // Synaptogenesis rate should not vastly exceed pruning rate
            if (synRate > pruneRate * 1000) {
                result.addWarning("Synaptogenesis rate much higher than pruning rate - may cause uncontrolled growth");
            }
            
            // Ensure both rates are within reasonable bounds
            if (synRate > 0.0001f) {
                result.addWarning("Synaptogenesis rate above 0.01% - may impact performance");
            }
            
            if (pruneRate > 0.00001f) {
                result.addWarning("Pruning rate above 0.001% - may cause excessive neuron loss");
            }
            
        } catch (const std::exception&) {
            // Ignore if conversion fails
        }
    }
}

void DependentConfigValidator::validateDevelopmentTimeline(ValidationResult& result) const {
    auto startIt = tempConfig_.find("development_critical_period_start");
    auto endIt = tempConfig_.find("development_critical_period_end");
    
    if (startIt != tempConfig_.end() && endIt != tempConfig_.end()) {
        try {
            float start = std::stof(startIt->second);
            float end = std::stof(endIt->second);
            
            if (end <= start) {
                result.addError("Critical period end must be after start");
            }
            
            if (start < 0 || end < 0) {
                result.addError("Critical period values must be non-negative");
            }
            
        } catch (const std::exception&) {
            // Ignore if conversion fails
        }
    }
}

} // namespace nlm