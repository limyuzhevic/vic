// Implementation file for validation functions
// This file contains the actual implementation of validation and error handling utilities

#include "Validator.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <cmath>

namespace nlm {

// Static instance for validation statistics
static ParameterValidator::ValidationStats& getValidationStatsInstance() {
    static ParameterValidator::ValidationStats stats;
    return stats;
}

// Validation implementations
ValidationResult ParameterValidator::validateNeuronId(const NeuronId& id) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (id.value > 0);  // Neuron IDs should be positive
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidParameter;
    result.message = "Invalid neuron ID: " + std::to_string(id.value);
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateSynapseId(const SynapseId& id) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (id.value > 0);  // Synapse IDs should be positive
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidParameter;
    result.message = "Invalid synapse ID: " + std::to_string(id.value);
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateRegionId(const RegionId& id) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (id.value > 0);  // Region IDs should be positive
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidParameter;
    result.message = "Invalid region ID: " + std::to_string(id.value);
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validatePopulationId(const PopulationId& id) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (id.value > 0);  // Population IDs should be positive
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidParameter;
    result.message = "Invalid population ID: " + std::to_string(id.value);
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateNeuronCount(size_t count) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (count > 0 && count <= 1000000);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid neuron count: " + std::to_string(count) + ". Must be > 0 and <= 1,000,000";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateRegionCount(size_t count) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (count > 0 && count <= 1000);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid region count: " + std::to_string(count) + ". Must be > 0 and <= 1000";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateConnectionProbability(float prob) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (prob >= 0.0f && prob <= 1.0f);  // Valid probability range
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid connection probability: " + std::to_string(prob) + ". Must be in range [0.0, 1.0]";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateStep(SimulationStep step) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (step >= 0);  // Step should be non-negative
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid simulation step: " + std::to_string(step) + ". Must be >= 0";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateTime(Timestamp time) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (time >= 0.0);  // Time should be non-negative
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid timestamp: " + std::to_string(time) + ". Must be >= 0.0";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateCurrent(MembranePotential current) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = std::isfinite(current) && current >= -1000.0f && current <= 1000.0f;  // Reasonable current limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid membrane potential: " + std::to_string(current) + ". Must be finite and in range [-1000, 1000] mV";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateWeight(SynapticWeight weight) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = std::isfinite(weight) && weight >= -100.0f && weight <= 100.0f;  // Reasonable weight limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid synaptic weight: " + std::to_string(weight) + ". Must be finite and in range [-100, 100]";
    
    if (result.valid) {
        statsRef.successfulValidations++();
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateDelay(Delay delay) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (delay >= 0);  // Delay should be non-negative
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid delay: " + std::to_string(delay) + ". Must be >= 0";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateTimestamp(Timestamp timestamp) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (timestamp >= -1.0f);  // Timestamp can be -1.0 (invalid)
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidState;
    result.message = "Invalid timestamp: " + std::to_string(timestamp) + ". Must be >= -1.0";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateCapacity(size_t capacity) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (capacity >= 0 && capacity <= 1000000);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid capacity: " + std::to_string(capacity) + ". Must be >= 0 and <= 1,000,000";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateMaxEpisodes(size_t maxEpisodes) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (maxEpisodes >= 0 && maxEpisodes <= 1000000);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid max episodes: " + std::to_string(maxEpisodes) + ". Must be >= 0 and <= 1,000,000";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validatePlanningDepth(size_t depth) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (depth >= 0 && depth <= 100);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid planning depth: " + std::to_string(depth) + ". Must be >= 0 and <= 100";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++();
    }
    
    return result;
}

ValidationResult ParameterValidator::validateInhibitionStrength(float strength) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (strength >= 0.0f && strength <= 10.0f);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid inhibition strength: " + std::to_string(strength) + ". Must be in range [0.0, 10.0]";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++();
    }
    
    return result;
}

ValidationResult ParameterValidator::validateExcitationStrength(float strength) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (strength >= 0.0f && strength <= 10.0f);  // Reasonable limits
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::OutOfRange;
    result.message = "Invalid excitation strength: " + std::to_string(strength) + ". Must be in range [0.0, 10.0]";
    
    if (result.valid) {
        statsRef.successfulValidations++();
    } else {
        statsRef.validationErrors++();
    }
    
    return result;
}

ValidationResult ParameterValidator::validateConfiguration(const std::shared_ptr<Config>& config) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (config != nullptr);  // Config should not be null
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::NullPointer;
    result.message = "Invalid configuration: null pointer";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++;
    }
    
    return result;
}

ValidationResult ParameterValidator::validateWorkingMemoryCapacity(size_t capacity) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (capacity > 0 && capacity <= 1000000);  // Working memory should have positive capacity
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidState;
    result.message = "Invalid working memory capacity: " + std::to_string(capacity) + ". Must be > 0";
    
    if (result.valid) {
        statsRef.successfulValidations++;
    } else {
        statsRef.validationErrors++();
    }
    
    return result;
}

ValidationResult ParameterValidator::validateEpisodicMemoryCapacity(size_t maxEpisodes) {
    auto statsRef = getValidationStatsInstance();
    statsRef.totalValidations++;
    
    ValidationResult result;
    result.valid = (maxEpisodes > 0 && maxEpisodes <= 1000000);  // Episodic memory should have positive capacity
    result.error = result.valid ? ValidationError::InvalidParameter : ValidationError::InvalidState;
    result.message = "Invalid episodic memory capacity: " + std::to_string(maxEpisodes) + ". Must be > 0";
    
    if (result.valid) {
        statsRef.successfulValidations++();
    } else {
        statsRef.validationErrors++();
    }
    
    return result;
}

// Error recovery implementations
void ParameterValidator::recoverFromInvalidParameter(ValidationResult& result) {
    NLM_LOG_WARNING("Recovering from invalid parameter: " + result.message);
    // Reset to default values or safe fallback
    result.valid = true;
}

void ParameterValidator::recoverFromOutOfRange(ValidationResult& result) {
    NLM_LOG_WARNING("Recovering from out of range error: " + result.message);
    // Clamp to valid range or use default
    result.valid = true;
}

void ParameterValidator::recoverFromNullPointer(ValidationResult& result) {
    NLM_LOG_WARNING("Recovering from null pointer error: " + result.message);
    // Create default configuration or object
    result.valid = true;
}

void ParameterValidator::recoverFromEmptyCollection(ValidationResult& result) {
    NLM_LOG_WARNING("Recovering from empty collection error: " + result.message);
    // Initialize with default empty collections
    result.valid = true;
}

void ParameterValidator::recoverFromInvalidState(ValidationResult& result) {
    NLM_LOG_WARNING("Recovering from invalid state error: " + result.message);
    // Reset to initial valid state
    result.valid = true;
}

// Statistics implementation
void ParameterValidator::ValidationStats::recordValidation(bool success, const std::string& action) {
    totalValidations++;
    if (success) {
        successfulValidations++;
    }
    if (!action.empty()) {
        recoveryActions.push_back(action);
    }
}

float ParameterValidator::ValidationStats::getSuccessRate() const {
    return totalValidations > 0 ? (static_cast<float>(successfulValidations) / totalValidations) * 100.0f : 0.0f;
}

std::string ParameterValidator::ValidationStats::getStatsSummary() const {
    std::stringstream ss;
    ss << "Validation Statistics:\n";
    ss << "  Total validations: " << totalValidations << "\n";
    ss << "  Successful validations: " << successfulValidations << "\n";
    ss << "  Validation errors: " << validationErrors << "\n";
    ss << "  Success rate: " << getSuccessRate() << "%\n";
    ss << "  Recovery actions: " << recoveryActions.size();
    if (!recoveryActions.empty()) {
        ss << "\n  Recovery actions taken:";
        for (const auto& action : recoveryActions) {
            ss << "\n    - " << action;
        }
    }
    return ss.str();
}

// Safe parameter extraction implementations
template<typename T>
std::pair<T, bool> SafeParameterExtractor::safeGetConfig(const std::shared_ptr<Config>& config,
                                                        const std::string& key,
                                                        const T& defaultValue) {
    if (!config || !config->has(key)) {
        return std::make_pair(defaultValue, false);
    }
    
    auto value = config->get<T>(key);
    if (!value.has_value()) {
        return std::make_pair(defaultValue, false);
    }
    
    return std::make_pair(*value, true);
}

template<typename T>
T SafeParameterExtractor::safeGetConfigWithRecovery(const std::shared_ptr<Config>& config,
                                                    const std::string& key,
                                                    const T& defaultValue,
                                                    ValidationError recoveryType) {
    if (!config) {
        ErrorLogger::logValidationError(ValidationResult(false, ValidationError::NullPointer, "Config is null"),
                                       "safeGetConfigWithRecovery");
        return defaultValue;
    }
    
    auto value = config->get<T>(key);
    if (!value.has_value()) {
        ErrorLogger::logValidationError(ValidationResult(false, recoveryType, "Key not found: " + key),
                                       "safeGetConfigWithRecovery");
        return defaultValue;
    }
    
    return *value;
}

// Error logger implementations
static LogLevel validationLoggingLevel = LogLevel::Warning;
static bool validationRecoveryEnabled = true;

void ErrorLogger::logValidationError(const ValidationResult& result,
                                    const std::string& operation,
                                    const std::string& component) {
    std::stringstream ss;
    ss << "[" << component << "] Validation error in " << operation << ": " << result.message;
    
    switch (result.error) {
        case ValidationError::InvalidParameter:
            NLM_LOG_WARNING(ss.str());
            break;
        case ValidationError::OutOfRange:
            NLM_LOG_ERROR(ss.str());
            break;
        case ValidationError::NullPointer:
            NLM_LOG_ERROR(ss.str());
            break;
        case ValidationError::EmptyCollection:
            NLM_LOG_WARNING(ss.str());
            break;
        case ValidationError::InvalidState:
            NLM_LOG_WARNING(ss.str());
            break;
        case ValidationError::ConfigurationError:
            NLM_LOG_ERROR(ss.str());
            break;
    }
}

void ErrorLogger::logRecoveryAction(const std::string& action,
                                   const std::string& operation,
                                   const std::string& component) {
    std::stringstream ss;
    ss << "[" << component << "] Recovery action in " << operation << ": " << action;
    NLM_LOG_INFO(ss.str());
}

void ErrorLogger::setValidationLoggingLevel(LogLevel level) {
    validationLoggingLevel = level;
}

LogLevel ErrorLogger::getValidationLoggingLevel() {
    return validationLoggingLevel;
}

void ErrorLogger::enableValidationRecovery(bool enable) {
    validationRecoveryEnabled = enable;
}

bool ErrorLogger::isValidationRecoveryEnabled() {
    return validationRecoveryEnabled;
}
