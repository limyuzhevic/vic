#pragma once

/**
 * Integration Point Validator
 * 
 * Comprehensive validation of system integration points for NLM brain simulation.
 * Validates system integration points, component integration, interface integration,
 * and data flow integration.
 * 
 * Features:
 * - System integration points validation
 * - Component integration validation
 * - Interface integration validation
 * - Data flow integration validation
 * - Integration consistency validation
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
 * Integration validation result
 */
struct IntegrationValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    double validationTime;
    size_t integrationPointsValidated;
    size_t componentsValidated;
    size_t interfacesValidated;
    size_t dataFlowsValidated;
    
    IntegrationValidationResult()
        : isValid(true), validationTime(0.0), integrationPointsValidated(0),
          componentsValidated(0), interfacesValidated(0), dataFlowsValidated(0) {}
};

/**
 * Integration point validator
 */
class IntegrationPointValidator {
public:
    IntegrationPointValidator();
    ~IntegrationPointValidator();
    
    // Disable copying, enable moving
    IntegrationPointValidator(const IntegrationPointValidator&) = delete;
    IntegrationPointValidator& operator=(const IntegrationPointValidator&) = delete;
    IntegrationPointValidator(IntegrationPointValidator&&) noexcept = default;
    IntegrationPointValidator& operator=(IntegrationPointValidator&&) noexcept = default;
    
    // Core validation operations
    IntegrationValidationResult validate(const Brain& brain) const;
    
    // Configuration
    void setIntegrationPointsValidationEnabled(bool enabled) { 
        validateIntegrationPoints_ = enabled; 
    }
    
    void setComponentIntegrationValidationEnabled(bool enabled) { 
        validateComponentIntegration_ = enabled; 
    }
    
    void setInterfaceIntegrationValidationEnabled(bool enabled) { 
        validateInterfaceIntegration_ = enabled; 
    }
    
    void setDataFlowIntegrationValidationEnabled(bool enabled) { 
        validateDataFlowIntegration_ = enabled; 
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
    
    bool validateIntegrationPoints_ = true;
    bool validateComponentIntegration_ = true;
    bool validateInterfaceIntegration_ = true;
    bool validateDataFlowIntegration_ = true;
    
    size_t totalValidations_ = 0;
    double totalValidationTime_ = 0.0;
    size_t totalErrors_ = 0;
    size_t totalWarnings_ = 0;
    
    // Internal validation helpers
    IntegrationValidationResult validateIntegrationPoints(const Brain& brain) const;
    IntegrationValidationResult validateComponentIntegration(const Brain& brain) const;
    IntegrationValidationResult validateInterfaceIntegration(const Brain& brain) const;
    IntegrationValidationResult validateDataFlowIntegration(const Brain& brain) const;
};

} // namespace validation
} // namespace nlm
