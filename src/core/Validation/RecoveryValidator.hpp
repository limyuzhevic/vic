#pragma once

/**
 * Recovery Validator
 * 
 * Recovery process validation for NLM brain simulation.
 * Validates recovery processes and error recovery mechanisms.
 * 
 * Features:
 * - Recovery process validation
 * - Error recovery mechanism validation
 * - Recovery state validation
 * - Recovery strategy validation
 * - Recovery effectiveness validation
 * - Real-time recovery validation
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
 * Recovery validation result
 */
struct RecoveryValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t recoveryProcessesValidated;
    size_t recoveryMechanismsValidated;
    size_t recoveryStatesValidated;
    size_t recoveryStrategiesValidated;
    
    RecoveryValidationResult()
        : isValid(true), validationTime(0.0), recoveryProcessesValidated(0),
          recoveryMechanismsValidated(0), recoveryStatesValidated(0),
          recoveryStrategiesValidated(0) {}
};

/**
 * Recovery validator
 */
class RecoveryValidator {
public:
    RecoveryValidator();
    ~RecoveryValidator();
    
    // Disable copying, enable moving
    RecoveryValidator(const RecoveryValidator&) = delete;
    RecoveryValidator& operator=(const RecoveryValidator&) = delete;
    RecoveryValidator(RecoveryValidator&&) noexcept = default;
    RecoveryValidator& operator=(RecoveryValidator&&) noexcept = default;
    
    // Core validation operations
    RecoveryValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setRecoveryProcessValidationEnabled(bool enabled) { 
        validateRecoveryProcess_ = enabled; 
    }
    
    void setRecoveryMechanismValidationEnabled(bool enabled) { 
        validateRecoveryMechanism_ = enabled; 
    }
    
    void setRecoveryStateValidationEnabled(bool enabled) { 
        validateRecoveryState_ = enabled; 
    }
    
    void setRecoveryStrategyValidationEnabled(bool enabled) { 
        validateRecoveryStrategy_ = enabled; 
    }
    
    void setRecoveryEffectivenessValidationEnabled(bool enabled) { 
        validateRecoveryEffectiveness_ = enabled; 
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
    
    bool validateRecoveryProcess_ = true;
    bool validateRecoveryMechanism_ = true;
    bool validateRecoveryState_ = true;
    bool validateRecoveryStrategy_ = true;
    bool validateRecoveryEffectiveness_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    RecoveryValidationResult validateRecoveryProcess(const Brain& brain) const;
    RecoveryValidationResult validateRecoveryMechanism(const Brain& brain) const;
    RecoveryValidationResult validateRecoveryState(const Brain& brain) const;
    RecoveryValidationResult validateRecoveryStrategy(const Brain& brain) const;
    RecoveryValidationResult validateRecoveryEffectiveness(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
