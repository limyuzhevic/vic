#pragma once

/**
 * Configuration Validator
 * 
 * Configuration validation system with comprehensive validation rules, error reporting,
 * and validation strategies for the NLM brain simulation project.
 * 
 * Features:
 * - Comprehensive validation rules
 * - Multi-level validation
 * - Validation strategies
 * - Error reporting and recovery
 * - Validation history
 * - Validation performance tracking
 * - Contextual validation
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace core {

class ConfigurationManager;
class ConfigurationSchema;

/**
 * Validation strategy
 */
enum class ValidationStrategy {
    STRICT,        // Fail on first error
    LENIENT,       // Collect all errors
    ADAPTIVE,      // Use different strategies based on context
    CUSTOM        // Use custom validation logic
};

/**
 * Validation context
 */
struct ValidationContext {
    std::string sessionId;
    std::string componentName;
    std::map<std::string, std::string> environmentVariables;
    double timestamp;
    std::string validationSource;
    
    ValidationContext() : timestamp(0.0) {}
};

/**
 * Validation result
 */
struct ValidationResult {
    bool valid;
    std::vector<ConfigError> errors;
    std::vector<ConfigError> warnings;
    double validationTime;
    size_t itemsValidated;
    ValidationStrategy strategy;
    
    ValidationResult() : valid(true), validationTime(0.0), itemsValidated(0), strategy(ValidationStrategy::STRICT) {}
};

/**
 * Custom validator function type
 */
using CustomValidator = std::function<ValidationResult(const std::map<std::string, std::string>& configData,
                                                     const ConfigurationSchema& schema,
                                                     const ValidationContext& context)>;

/**
 * Configuration validator
 */
class ConfigurationValidator {
public:
    ConfigurationValidator();
    ~ConfigurationValidator();
    
    // Disable copying, enable moving
    ConfigurationValidator(const ConfigurationValidator&) = delete;
    ConfigurationValidator& operator=(const ConfigurationValidator&) = delete;
    ConfigurationValidator(ConfigurationValidator&&) noexcept = default;
    ConfigurationValidator& operator=(ConfigurationValidator&&) noexcept = default;
    
    // Core validation operations
    ValidationResult validate(const std::map<std::string, std::string>& configData) const;
    ValidationResult validate(const ConfigurationManager& configManager) const;
    
    // Schema-based validation
    ValidationResult validateAgainstSchema(const std::map<std::string, std::string>& configData,
                                          const ConfigurationSchema& schema) const;
    
    // Value validation
    ValidationResult validateValue(const std::string& key, const std::string& value,
                                  ConfigDataType dataType) const;
    
    // Constraint validation
    ValidationResult validateConstraints(const std::map<std::string, std::string>& configData,
                                        const ConfigurationSchema& schema) const;
    
    // Custom validation
    void addCustomValidator(const std::string& name, const CustomValidator& validator);
    bool removeCustomValidator(const std::string& name);
    ValidationResult validateWithCustom(const std::string& validatorName,
                                       const std::map<std::string, std::string>& configData,
                                       const ConfigurationSchema& schema,
                                       const ValidationContext& context = ValidationContext()) const;
    
    // Validation rules
    void addValidationRule(const std::string& ruleName, const SchemaRule& rule);
    bool removeValidationRule(const std::string& ruleName);
    std::vector<SchemaRule> getValidationRules() const { return validation_rules_; }
    
    // Validation context
    void setValidationContext(const ValidationContext& context) { current_context_ = context; }
    ValidationContext getValidationContext() const { return current_context_; }
    
    // Validation strategy
    void setValidationStrategy(ValidationStrategy strategy) { current_strategy_ = strategy; }
    ValidationStrategy getValidationStrategy() const { return current_strategy_; }
    
    // Validation result processing
    bool isResultValid(const ValidationResult& result) const { return result.valid; }
    std::vector<ConfigError> getAllErrors(const ValidationResult& result) const;
    std::vector<ConfigError> getAllWarnings(const ValidationResult& result) const;
    
    // Validation history
    void addValidationResult(const ValidationResult& result);
    std::vector<ValidationResult> getValidationHistory(size_t limit = 100) const;
    void clearValidationHistory();
    
    // Validation statistics
    size_t getTotalValidations() const { return total_validations_; }
    double getAverageValidationTime() const { return average_validation_time_; }
    double getSuccessRate() const { return success_rate_; }
    
    // Validation configuration
    void enablePerformanceTracking(bool enable) { track_performance_ = enable; }
    bool isPerformanceTrackingEnabled() const { return track_performance_; }
    
    // Validation error recovery
    std::map<std::string, std::string> fixErrors(const std::map<std::string, std::string>& configData,
                                                const ValidationResult& result) const;
    bool canFixError(const ConfigError& error) const;
    
    // Validation reporting
    std::string formatValidationReport(const ValidationResult& result) const;
    std::string formatValidationSummary(const ValidationResult& result) const;
    
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
    ValidationStrategy current_strategy_ = ValidationStrategy::LENIENT;
    ValidationContext current_context_;
    
    std::vector<SchemaRule> validation_rules_;
    std::vector<CustomValidator> custom_validators_;
    std::vector<std::string> custom_validator_names_;
    
    size_t total_validations_ = 0;
    double total_validation_time_ = 0.0;
    double success_rate_ = 1.0;
    bool track_performance_ = true;
    
    // Internal helpers
    ValidationResult validateWithStrategy(const std::map<std::string, std::string>& configData,
                                         const ConfigurationSchema& schema) const;
    ValidationResult validateField(const std::string& fieldName, const std::string& value,
                                  const SchemaField& field) const;
    std::vector<ConfigError> collectFieldErrors(const std::string& fieldName, const std::string& value,
                                               const SchemaField& field) const;
    double calculateSuccessRate() const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
