#pragma once

/**
 * Error Validator
 * 
 * Error state and fault validation for NLM brain simulation.
 * Validates error states, faults, and error conditions.
 * 
 * Features:
 * - Error state validation
 * - Fault detection and validation
 * - Error condition validation
 * - Error recovery validation
 * - Error handling validation
 * - Real-time error validation
 * - Thread-safe validation operations
 */

#include "Brain.hpp"
#include "Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Error validation result
 */
struct ErrorValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t errorStatesValidated;
    size_t faultConditionsValidated;
    size_t recoveryConditionsValidated;
    
    ErrorValidationResult()
        : isValid(true), validationTime(0.0), errorStatesValidated(0),
          faultConditionsValidated(0), recoveryConditionsValidated(0) {}
};

/**
 * Error validator
 */
class ErrorValidator {
public:
    ErrorValidator();
    ~ErrorValidator();
    
    // Disable copying, enable moving
    ErrorValidator(const ErrorValidator&) = delete;
    ErrorValidator& operator=(const ErrorValidator&) = delete;
    ErrorValidator(ErrorValidator&&) noexcept = default;
    ErrorValidator& operator=(ErrorValidator&&) noexcept = default;
    
    // Core validation operations
    ErrorValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setErrorStateValidationEnabled(bool enabled) { 
        validateErrorState_ = enabled; 
    }
    
    void setFaultValidationEnabled(bool enabled) { 
        validateFault_ = enabled; 
    }
    
    void setErrorConditionValidationEnabled(bool enabled) { 
        validateErrorCondition_ = enabled; 
    }
    
    void setErrorRecoveryValidationEnabled(bool enabled) { 
        validateErrorRecovery_ = enabled; 
    }
    
    // Validation statistics
    size_t getTotalValidations() const { return totalValidations_; }
    double getAverageValidationTime() const { return totalValidationTime_ / totalValidations_; }
    size_t getErrorCount() const { return totalErrors_; }
    size_t getWarningCount() const { return totalWarnings_; }
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    bool validateErrorState_ = true;
    bool validateFault_ = true;
    bool validateErrorCondition_ = true;
    bool validateErrorRecovery_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    ErrorValidationResult validateErrorState(const Brain& brain) const;
    ErrorValidationResult validateFault(const Brain& brain) const;
    ErrorValidationResult validateErrorCondition(const Brain& brain) const;
    ErrorValidationResult validateErrorRecovery(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
