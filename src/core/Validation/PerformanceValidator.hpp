#pragma once

/**
 * Performance Validator
 * 
 * Performance consistency validation for NLM brain simulation.
 * Validates performance consistency, correctness, completeness, and compatibility.
 * 
 * Features:
 * - Performance consistency validation
 * - Performance correctness validation
 * - Performance completeness validation
 * - Performance compatibility validation
 * - Integration with performance monitoring
 * - Real-time validation during simulation
 * - Thread-safe validation operations
 */

#include "PerformanceProfiler.hpp"
#include "PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Performance validation result
 */
struct PerformanceValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t performanceMetricsValidated;
    size_t thresholdChecksValidated;
    size_t integrationPointsValidated;
    
    PerformanceValidationResult()
        : isValid(true), validationTime(0.0), performanceMetricsValidated(0),
          thresholdChecksValidated(0), integrationPointsValidated(0) {}
};

/**
 * Performance validator
 */
class PerformanceValidator {
public:
    PerformanceValidator();
    ~PerformanceValidator();
    
    // Disable copying, enable moving
    PerformanceValidator(const PerformanceValidator&) = delete;
    PerformanceValidator& operator=(const PerformanceValidator&) = delete;
    PerformanceValidator(PerformanceValidator&&) noexcept = default;
    PerformanceValidator& operator=(PerformanceValidator&&) noexcept = default;
    
    // Core validation operations
    PerformanceValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setPerformanceConsistencyValidationEnabled(bool enabled) { 
        validatePerformanceConsistency_ = enabled; 
    }
    
    void setPerformanceCorrectnessValidationEnabled(bool enabled) { 
        validatePerformanceCorrectness_ = enabled; 
    }
    
    void setPerformanceCompletenessValidationEnabled(bool enabled) { 
        validatePerformanceCompleteness_ = enabled; 
    }
    
    void setPerformanceCompatibilityValidationEnabled(bool enabled) { 
        validatePerformanceCompatibility_ = enabled; 
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
    
    bool validatePerformanceConsistency_ = true;
    bool validatePerformanceCorrectness_ = true;
    bool validatePerformanceCompleteness_ = true;
    bool validatePerformanceCompatibility_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    PerformanceValidationResult validatePerformanceConsistency(const Brain& brain) const;
    PerformanceValidationResult validatePerformanceCorrectness(const Brain& brain) const;
    PerformanceValidationResult validatePerformanceCompleteness(const Brain& brain) const;
    PerformanceValidationResult validatePerformanceCompatibility(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
