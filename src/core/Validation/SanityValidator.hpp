#pragma once

/**
 * Sanity Validator
 * 
 * Sanity checks and basic validation for NLM brain simulation.
 * Provides basic sanity validation and sanity checks.
 * 
 * Features:
 * - Basic sanity checks
 * - System state validation
 * - Parameter validation
 * - Boundary condition validation
 * - Initial state validation
 * - Real-time sanity validation
 * - Thread-safe validation operations
 */

#include "Brain.hpp"
#include "Neuron.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Sanity validation result
 */
struct SanityValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t sanityChecksPassed;
    size_t sanityChecksFailed;
    
    SanityValidationResult()
        : isValid(true), validationTime(0.0), sanityChecksPassed(0), 
          sanityChecksFailed(0) {}
};

/**
 * Sanity validator
 */
class SanityValidator {
public:
    SanityValidator();
    ~SanityValidator();
    
    // Disable copying, enable moving
    SanityValidator(const SanityValidator&) = delete;
    SanityValidator& operator=(const SanityValidator&) = delete;
    SanityValidator(SanityValidator&&) noexcept = default;
    SanityValidator& operator=(SanityValidator&&) noexcept = default;
    
    // Core validation operations
    SanityValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setBasicSanityChecksValidationEnabled(bool enabled) { 
        validateBasicSanityChecks_ = enabled; 
    }
    
    void setSystemStateValidationEnabled(bool enabled) { 
        validateSystemState_ = enabled; 
    }
    
    void setParameterValidationEnabled(bool enabled) { 
        validateParameters_ = enabled; 
    }
    
    void setBoundaryValidationEnabled(bool enabled) { 
        validateBoundaries_ = enabled; 
    }
    
    void setInitialStateValidationEnabled(bool enabled) { 
        validateInitialState_ = enabled; 
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
    
    bool validateBasicSanityChecks_ = true;
    bool validateSystemState_ = true;
    bool validateParameters_ = true;
    bool validateBoundaries_ = true;
    bool validateInitialState_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    SanityValidationResult validateBasicSanityChecks(const Brain& brain) const;
    SanityValidationResult validateSystemState(const Brain& brain) const;
    SanityValidationResult validateParameters(const Brain& brain) const;
    SanityValidationResult validateBoundaries(const Brain& brain) const;
    SanityValidationResult validateInitialState(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
