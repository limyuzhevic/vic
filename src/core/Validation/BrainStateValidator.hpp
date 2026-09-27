#pragma once

/**
 * Brain State Validator
 * 
 * Comprehensive validation of brain system state for NLM brain simulation.
 * Validates brain system initialization, operation, shutdown, and error states.
 * 
 * Features:
 * - Brain system initialization validation
 * - Brain system operation validation
 * - Brain system shutdown validation
 * - Brain system error states validation
 * - Integration with neural subsystems
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Brain system validation result
 */
struct BrainSystemValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t regionsValidated;
    size_t populationsValidated;
    size_t neuronsValidated;
    
    BrainSystemValidationResult()
        : isValid(true), validationTime(0.0), regionsValidated(0), 
          populationsValidated(0), neuronsValidated(0) {}
};

/**
 * Brain system state validator
 */
class BrainStateValidator {
public:
    BrainStateValidator();
    ~BrainStateValidator();
    
    // Disable copying, enable moving
    BrainStateValidator(const BrainStateValidator&) = delete;
    BrainStateValidator& operator=(const BrainStateValidator&) = delete;
    BrainStateValidator(BrainStateValidator&&) noexcept = default;
    BrainStateValidator& operator=(BrainStateValidator&&) noexcept = default;
    
    // Core validation operations
    BrainSystemValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setBrainInitializationValidationEnabled(bool enabled) { 
        validateBrainInitialization_ = enabled; 
    }
    
    void setBrainOperationValidationEnabled(bool enabled) { 
        validateBrainOperation_ = enabled; 
    }
    
    void setBrainShutdownValidationEnabled(bool enabled) { 
        validateBrainShutdown_ = enabled; 
    }
    
    void setBrainErrorValidationEnabled(bool enabled) { 
        validateBrainErrors_ = enabled; 
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
    
    bool validateBrainInitialization_ = true;
    bool validateBrainOperation_ = true;
    bool validateBrainShutdown_ = true;
    bool validateBrainErrors_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    BrainSystemValidationResult validateBrainInitialization(const Brain& brain) const;
    BrainSystemValidationResult validateBrainOperation(const Brain& brain) const;
    BrainSystemValidationResult validateBrainShutdown(const Brain& brain) const;
    BrainSystemValidationResult validateBrainErrors(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
