#pragma once

/**
 * Consistency Validator
 * 
 * Overall consistency validation for NLM brain simulation.
 * Provides comprehensive consistency validation across all neural systems.
 * 
 * Features:
 * - Neural state consistency validation
 * - Cross-system consistency validation
 * - Integration point consistency validation
 * - Memory system consistency validation
 * - Neuromodulator system consistency validation
 * - Real-time consistency validation
 * - Thread-safe validation operations
 */

#include "NeuralStateValidator.hpp"
#include "BrainStateValidator.hpp"
#include "MemoryStateValidator.hpp"
#include "NeuromodulatorStateValidator.hpp"
#include "CrossSystemValidator.hpp"
#include "IntegrationPointValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Consistency validation result
 */
struct ConsistencyValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t neuralStatesValidated;
    size_t brainSystemsValidated;
    size_t memorySystemsValidated;
    size_t neuromodulatorSystemsValidated;
    size_t crossSystemsValidated;
    size_t integrationPointsValidated;
    
    ConsistencyValidationResult()
        : isValid(true), validationTime(0.0), neuralStatesValidated(0),
          brainSystemsValidated(0), memorySystemsValidated(0),
          neuromodulatorSystemsValidated(0), crossSystemsValidated(0),
          integrationPointsValidated(0) {}
};

/**
 * Consistency validator
 */
class ConsistencyValidator {
public:
    ConsistencyValidator();
    ~ConsistencyValidator();
    
    // Disable copying, enable moving
    ConsistencyValidator(const ConsistencyValidator&) = delete;
    ConsistencyValidator& operator=(const ConsistencyValidator&) = delete;
    ConsistencyValidator(ConsistencyValidator&&) noexcept = default;
    ConsistencyValidator& operator=(ConsistencyValidator&&) noexcept = default;
    
    // Core validation operations
    ConsistencyValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setNeuralStateConsistencyValidationEnabled(bool enabled) { 
        validateNeuralStateConsistency_ = enabled; 
    }
    
    void setBrainSystemConsistencyValidationEnabled(bool enabled) { 
        validateBrainSystemConsistency_ = enabled; 
    }
    
    void setMemorySystemConsistencyValidationEnabled(bool enabled) { 
        validateMemorySystemConsistency_ = enabled; 
    }
    
    void setNeuromodulatorSystemConsistencyValidationEnabled(bool enabled) { 
        validateNeuromodulatorSystemConsistency_ = enabled; 
    }
    
    void setCrossSystemConsistencyValidationEnabled(bool enabled) { 
        validateCrossSystemConsistency_ = enabled; 
    }
    
    void setIntegrationPointConsistencyValidationEnabled(bool enabled) { 
        validateIntegrationPointConsistency_ = enabled; 
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
    
    bool validateNeuralStateConsistency_ = true;
    bool validateBrainSystemConsistency_ = true;
    bool validateMemorySystemConsistency_ = true;
    bool validateNeuromodulatorSystemConsistency_ = true;
    bool validateCrossSystemConsistency_ = true;
    bool validateIntegrationPointConsistency_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    ConsistencyValidationResult validateNeuralStateConsistency(const Brain& brain) const;
    ConsistencyValidationResult validateBrainSystemConsistency(const Brain& brain) const;
    ConsistencyValidationResult validateMemorySystemConsistency(const Brain& brain) const;
    ConsistencyValidationResult validateNeuromodulatorSystemConsistency(const Brain& brain) const;
    ConsistencyValidationResult validateCrossSystemConsistency(const Brain& brain) const;
    ConsistencyValidationResult validateIntegrationPointConsistency(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
