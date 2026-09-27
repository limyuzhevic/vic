#pragma once

/**
 * Neuromodulator State Validator
 * 
 * Comprehensive validation of neuromodulator state for NLM brain simulation.
 * Validates neuromodulator level, effect, interaction, and feedback.
 * 
 * Features:
 * - Neuromodulator level validation
 * - Neuromodulator effect validation
 * - Neuromodulator interaction validation
 * - Neuromodulator feedback validation
 * - Integration with neural subsystems
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Neuromodulator system validation result
 */
struct NeuromodulatorSystemValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t neuromodulatorsValidated;
    
    NeuromodulatorSystemValidationResult()
        : isValid(true), validationTime(0.0), neuromodulatorsValidated(0) {}
};

/**
 * Neuromodulator system state validator
 */
class NeuromodulatorStateValidator {
public:
    NeuromodulatorStateValidator();
    ~NeuromodulatorStateValidator();
    
    // Disable copying, enable moving
    NeuromodulatorStateValidator(const NeuromodulatorStateValidator&) = delete;
    NeuromodulatorStateValidator& operator=(const NeuromodulatorStateValidator&) = delete;
    NeuromodulatorStateValidator(NeuromodulatorStateValidator&&) noexcept = default;
    NeuromodulatorStateValidator& operator=(NeuromodulatorStateValidator&&) noexcept = default;
    
    // Core validation operations
    NeuromodulatorSystemValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setNeuromodulatorLevelValidationEnabled(bool enabled) { 
        validateNeuromodulatorLevel_ = enabled; 
    }
    
    void setNeuromodulatorEffectValidationEnabled(bool enabled) { 
        validateNeuromodulatorEffect_ = enabled; 
    }
    
    void setNeuromodulatorInteractionValidationEnabled(bool enabled) { 
        validateNeuromodulatorInteraction_ = enabled; 
    }
    
    void setNeuromodulatorFeedbackValidationEnabled(bool enabled) { 
        validateNeuromodulatorFeedback_ = enabled; 
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
    
    bool validateNeuromodulatorLevel_ = true;
    bool validateNeuromodulatorEffect_ = true;
    bool validateNeuromodulatorInteraction_ = true;
    bool validateNeuromodulatorFeedback_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    NeuromodulatorSystemValidationResult validateNeuromodulatorLevel(const Brain& brain) const;
    NeuromodulatorSystemValidationResult validateNeuromodulatorEffect(const Brain& brain) const;
    NeuromodulatorSystemValidationResult validateNeuromodulatorInteraction(const Brain& brain) const;
    NeuromodulatorSystemValidationResult validateNeuromodulatorFeedback(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
