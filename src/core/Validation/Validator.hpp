#pragma once

#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include "../Logger/Logger.hpp"

namespace nlm {

// Validation error types
enum class ValidationError {
    InvalidParameter,
    OutOfRange,
    NullPointer,
    EmptyCollection,
    InvalidState,
    ConfigurationError
};

// Validation result
struct ValidationResult {
    bool valid;
    ValidationError error;
    std::string message;
    
    ValidationResult() : valid(true), error(ValidationError::InvalidParameter), message() {}
    ValidationResult(bool v, ValidationError e, const std::string& msg) 
        : valid(v), error(e), message(msg) {}
};

// Parameter validation functions
class ParameterValidator {
public:
    // Validation functions for Brain methods
    static ValidationResult validateNeuronId(const NeuronId& id);
    static ValidationResult validateSynapseId(const SynapseId& id);
    static ValidationResult validateRegionId(const RegionId& id);
    static ValidationResult validatePopulationId(const PopulationId& id);
    static ValidationResult validateNeuronCount(size_t count);
    static ValidationResult validateRegionCount(size_t count);
    static ValidationResult validateConnectionProbability(float prob);
    static ValidationResult validateStep(SimulationStep step);
    static ValidationResult validateTime(Timestamp time);
    static ValidationResult validateCurrent(MembranePotential current);
    static ValidationResult validateWeight(SynapticWeight weight);
    static ValidationResult validateDelay(Delay delay);
    static ValidationResult validateTimestamp(Timestamp timestamp);
    static ValidationResult validateCapacity(size_t capacity);
    static ValidationResult validateMaxEpisodes(size_t maxEpisodes);
    static ValidationResult validatePlanningDepth(size_t depth);
    static ValidationResult validateInhibitionStrength(float strength);
    static ValidationResult validateExcitationStrength(float strength);
    static ValidationResult validateConfiguration(const std::shared_ptr<Config>& config);
    
    // Validation functions for memory systems
    static ValidationResult validateWorkingMemoryCapacity(size_t capacity);
    static ValidationResult validateEpisodicMemoryCapacity(size_t maxEpisodes);
    
    // Error recovery strategies
    static void recoverFromInvalidParameter(ValidationResult& result);
    static void recoverFromOutOfRange(ValidationResult& result);
    static void recoverFromNullPointer(ValidationResult& result);
    static void recoverFromEmptyCollection(ValidationResult& result);
    static void recoverFromInvalidState(ValidationResult& result);
    
    // Statistics and recovery tracking
    struct ValidationStats {
        size_t totalValidations = 0;
        size_t successfulValidations = 0;
        size_t validationErrors = 0;
        std::vector<std::string> recoveryActions;
        
        void recordValidation(bool success, const std::string& action = "");
        float getSuccessRate() const;
        std::string getStatsSummary() const;
    };
    
    static ValidationStats& getValidationStats();
};

// Safe parameter extraction with validation
class SafeParameterExtractor {
public:
    // Safe extraction of configuration parameters
    template<typename T>
    static std::pair<T, bool> safeGetConfig(const std::shared_ptr<Config>& config, 
                                          const std::string& key, 
                                          const T& defaultValue);
    
    // Safe extraction of configuration parameters with error recovery
    template<typename T>
    static T safeGetConfigWithRecovery(const std::shared_ptr<Config>& config,
                                      const std::string& key,
                                      const T& defaultValue,
                                      ValidationError recoveryType = ValidationError::InvalidParameter);
};

// Error logging utility class
class ErrorLogger {
public:
    static void logValidationError(const ValidationResult& result, 
                                  const std::string& operation,
                                  const std::string& component = "NLM");
    
    static void logRecoveryAction(const std::string& action,
                                 const std::string& operation,
                                 const std::string& component = "NLM");
    
    static void setValidationLoggingLevel(LogLevel level);
    static LogLevel getValidationLoggingLevel();
    
    static void enableValidationRecovery(bool enable);
    static bool isValidationRecoveryEnabled();
};