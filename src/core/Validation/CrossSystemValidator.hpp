#pragma once

/**
 * Cross-System Validator
 * 
 * Comprehensive cross-system validation for NLM brain simulation.
 * Validates cross-system communication, cross-system interaction,
 * cross-system dependency, and cross-system synchronization.
 * 
 * Features:
 * - Cross-system communication validation
 * - Cross-system interaction validation
 * - Cross-system dependency validation
 * - Cross-system synchronization validation
 * - Integration with neural subsystems
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "IntegrationMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Cross-system validation result
 */
struct CrossSystemValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t communicationChannelsValidated;
    size_t interactionPointsValidated;
    size_t dependenciesValidated;
    size_t synchronizationsValidated;
    
    CrossSystemValidationResult()
        : isValid(true), validationTime(0.0), communicationChannelsValidated(0),
          interactionPointsValidated(0), dependenciesValidated(0), synchronizationsValidated(0) {}
};

/**
 * Cross-system validator
 */
class CrossSystemValidator {
public:
    CrossSystemValidator();
    ~CrossSystemValidator();
    
    // Disable copying, enable moving
    CrossSystemValidator(const CrossSystemValidator&) = delete;
    CrossSystemValidator& operator=(const CrossSystemValidator&) = delete;
    CrossSystemValidator(CrossSystemValidator&&) noexcept = default;
    CrossSystemValidator& operator=(CrossSystemValidator&&) noexcept = default;
    
    // Core validation operations
    CrossSystemValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setCrossSystemCommunicationValidationEnabled(bool enabled) { 
        validateCrossSystemCommunication_ = enabled; 
    }
    
    void setCrossSystemInteractionValidationEnabled(bool enabled) { 
        validateCrossSystemInteraction_ = enabled; 
    }
    
    void setCrossSystemDependencyValidationEnabled(bool enabled) { 
        validateCrossSystemDependency_ = enabled; 
    }
    
    void setCrossSystemSynchronizationValidationEnabled(bool enabled) { 
        validateCrossSystemSynchronization_ = enabled; 
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
    
    bool validateCrossSystemCommunication_ = true;
    bool validateCrossSystemInteraction_ = true;
    bool validateCrossSystemDependency_ = true;
    bool validateCrossSystemSynchronization_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    CrossSystemValidationResult validateCrossSystemCommunication(const Brain& brain) const;
    CrossSystemValidationResult validateCrossSystemInteraction(const Brain& brain) const;
    CrossSystemValidationResult validateCrossSystemDependency(const Brain& brain) const;
    CrossSystemValidationResult validateCrossSystemSynchronization(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
