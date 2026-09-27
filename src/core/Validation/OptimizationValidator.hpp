#pragma once

/**
 * Optimization Validator
 * 
 * Performance optimization validation for NLM brain simulation.
 * Validates performance optimization, optimization effectiveness,
 * and optimization strategies.
 * 
 * Features:
 * - Performance optimization validation
 * - Optimization effectiveness validation
 * - Optimization strategy validation
 * - Memory optimization validation
 * - Computational optimization validation
 * - Real-time optimization validation
 * - Thread-safe validation operations
 */

#include "Brain.hpp"
#include "Neuron.hpp"
#include "PerformanceProfiler.hpp"
#include "PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <mutex>
#include <vector>
#include <string>

namespace nlm {
namespace validation {

/**
 * Optimization validation result
 */
struct OptimizationValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t performanceOptimizationsValidated;
    size_t optimizationEffectivenessValidated;
    size_t optimizationStrategiesValidated;
    size_t memoryOptimizationsValidated;
    size_t computationalOptimizationsValidated;
    
    OptimizationValidationResult()
        : isValid(true), validationTime(0.0), performanceOptimizationsValidated(0),
          optimizationEffectivenessValidated(0), optimizationStrategiesValidated(0),
          memoryOptimizationsValidated(0), computationalOptimizationsValidated(0) {}
};

/**
 * Optimization validator
 */
class OptimizationValidator {
public:
    OptimizationValidator();
    ~OptimizationValidator();
    
    // Disable copying, enable moving
    OptimizationValidator(const OptimizationValidator&) = delete;
    OptimizationValidator& operator=(const OptimizationValidator&) = delete;
    OptimizationValidator(OptimizationValidator&&) noexcept = default;
    OptimizationValidator& operator=(OptimizationValidator&&) noexcept = default;
    
    // Core validation operations
    OptimizationValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setPerformanceOptimizationValidationEnabled(bool enabled) { 
        validatePerformanceOptimization_ = enabled; 
    }
    
    void setOptimizationEffectivenessValidationEnabled(bool enabled) { 
        validateOptimizationEffectiveness_ = enabled; 
    }
    
    void setOptimizationStrategyValidationEnabled(bool enabled) { 
        validateOptimizationStrategy_ = enabled; 
    }
    
    void setMemoryOptimizationValidationEnabled(bool enabled) { 
        validateMemoryOptimization_ = enabled; 
    }
    
    void setComputationalOptimizationValidationEnabled(bool enabled) { 
        validateComputationalOptimization_ = enabled; 
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
    
    bool validatePerformanceOptimization_ = true;
    bool validateOptimizationEffectiveness_ = true;
    bool validateOptimizationStrategy_ = true;
    bool validateMemoryOptimization_ = true;
    bool validateComputationalOptimization_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    OptimizationValidationResult validatePerformanceOptimization(const Brain& brain) const;
    OptimizationValidationResult validateOptimizationEffectiveness(const Brain& brain) const;
    OptimizationValidationResult validateOptimizationStrategy(const Brain& brain) const;
    OptimizationValidationResult validateMemoryOptimization(const Brain& brain) const;
    OptimizationValidationResult validateComputationalOptimization(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
