#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "ConfigSchema.hpp"

namespace nlm {

// Configuration validation options
struct ValidationOptions {
    bool validateTypes = true;           // Validate parameter types
    bool validateRanges = true;          // Validate parameter ranges
    bool validateDependencies = true;    // Validate parameter dependencies
    bool validateConsistency = true;     // Validate configuration consistency
    bool failFast = false;               // Stop on first error
    bool includeWarnings = true;         // Include warnings in results
    bool logErrors = true;               // Log validation errors
    
    ValidationOptions() = default;
};

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept = default;
    ConfigValidator& operator=(ConfigValidator&&) noexcept = default;
    
    // Set validation options
    void setOptions(const ValidationOptions& options);
    
    // Get validation options
    ValidationOptions getOptions() const;
    
    // Validate a single parameter
    ValidationResult validateParameter(
        const std::string& name,
        const ConfigValueSchema& value,
        const std::string& source = "" 
    ) const;
    
    // Validate a collection of parameters
    std::vector<ValidationResult> validateParameters(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Validate configuration from key-value pairs
    std::vector<ValidationResult> validateConfig(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Check for configuration consistency
    std::vector<ValidationResult> checkConsistency(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Validate parameter dependencies
    std::vector<ValidationResult> validateDependencies(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Validate parameter ranges
    std::vector<ValidationResult> validateRanges(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Validate parameter types
    std::vector<ValidationResult> validateTypes(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Get parameter info by name
    const ParameterInfo* getParameterInfo(const std::string& name) const;
    
    // Get all parameter names
    std::vector<std::string> getAllParameterNames() const;
    
    // Check if parameter has constraints
    bool hasConstraints(const std::string& name) const;
    
    // Get parameter constraints
    std::vector<Constraint> getParameterConstraints(const std::string& name) const;
    
    // Validate against specific constraint type
    bool validateAgainstConstraint(
        const std::string& name,
        const ConfigValueSchema& value,
        const Constraint& constraint
    ) const;
    
    // Get constraint error message
    std::string getConstraintErrorMessage(
        const Constraint& constraint,
        const ConfigValueSchema& value
    ) const;
    
    // Check if parameter is dependent on another
    bool isDependentParameter(const std::string& name, const std::string& dependsOn) const;
    
    // Get all dependent parameters for a key
    std::vector<std::string> getDependentParameters(const std::string& name) const;
    
    // Validate configuration completeness
    std::vector<ValidationResult> validateCompleteness(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Check for required parameters
    std::vector<ValidationResult> checkRequiredParameters(
        const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
    ) const;
    
    // Validate specific parameter value against schema
    bool validateAgainstSchema(
        const std::string& name,
        const ConfigValueSchema& value,
        bool checkConstraints = true
    ) const;
    
    // Get parameter type name
    std::string getParameterTypeName(const std::string& name) const;
    
    // Check if parameter type matches expected type
    bool isTypeMatch(const std::string& name, const ConfigValueSchema& value) const;
    
    // Get type error message
    std::string getTypeErrorMessage(const std::string& name, const ConfigValueSchema& value) const;
    
    // Validate parameter value range
    bool validateValueRange(const std::string& name, const ConfigValueSchema& value) const;
    
    // Get range error message
    std::string getRangeErrorMessage(const std::string& name, const ConfigValueSchema& value) const;
    
    // Validate configuration has no duplicate keys
    bool validateNoDuplicateKeys(const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters) const;
    
    // Get duplicate key error message
    std::string getDuplicateKeyErrorMessage(const std::string& key) const;
    
    // Validate parameter format (e.g., string patterns, vector consistency)
    bool validateParameterFormat(const std::string& name, const ConfigValueSchema& value) const;
    
    // Get format error message
    std::string getFormatErrorMessage(const std::string& name, const ConfigValueSchema& value) const;
    
    // Clear validation cache
    void clearCache();
    
    // Get validation statistics
    struct ValidationStatistics {
        int totalChecks = 0;
        int passedChecks = 0;
        int failedChecks = 0;
        int warnings = 0;
        std::unordered_map<std::string, int> parameterFailures;
        
        double getSuccessRate() const {
            return totalChecks > 0 ? (static_cast<double>(passedChecks) / totalChecks) * 100.0 : 100.0;
        }
    };
    
    ValidationStatistics getStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    bool matchesPattern(const std::string& value, const std::string& pattern) const;
    std::string typeToString(const ConfigValueSchema& value) const;
    bool isValueInEnum(const ConfigValueSchema& value, const std::string& enumValues) const;
    bool isValueInRange(const ConfigValueSchema& value, const std::string& range) const;
};

} // namespace nlm
