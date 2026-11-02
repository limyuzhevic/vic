#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// Type definitions for configuration validation
enum class ConfigDataType {
    String,
    Int64,
    Float,
    Double,
    Boolean
};

struct ValidationRule {
    std::string key;
    ConfigDataType expectedType;
    std::string description;
    bool required;
    std::vector<std::string> allowedValues;
    
    // Validation functions
    std::function<bool(const std::string&)> customValidator;
    
    // Range validation for numeric types
    struct Range {
        double min;
        double max;
    } range;
    
    ValidationRule(std::string k, ConfigDataType t, std::string desc = "", bool req = false)
        : key(std::move(k)), expectedType(t), description(std::move(desc)), required(req) {}
    
    ValidationRule& withAllowedValues(const std::vector<std::string>& values) {
        allowedValues = values;
        return *this;
    }
    
    ValidationRule& withRange(double min, double max) {
        range.min = min;
        range.max = max;
        return *this;
    }
    
    ValidationRule& withValidator(std::function<bool(const std::string&)> validator) {
        customValidator = std::move(validator);
        return *this;
    }
};

class ConfigurationValidator {
public:
    ConfigurationValidator();
    ~ConfigurationValidator();
    
    // Disable copying
    ConfigurationValidator(const ConfigurationValidator&) = delete;
    ConfigurationValidator& operator=(const ConfigurationValidator&) = delete;
    
    // Enable moving
    ConfigurationValidator(ConfigurationValidator&&) noexcept;
    ConfigurationValidator& operator=(ConfigurationValidator&&) noexcept;
    
    // Add validation rules
    void addRule(const ValidationRule& rule);
    void addRule(std::string key, ConfigDataType type, std::string description = "", bool required = false);
    
    // Validate configuration
    bool validate(const std::shared_ptr<const Config>& config, std::string& errorMessage) const;
    
    // Get all validation rules
    const std::vector<ValidationRule>& getRules() const { return rules_; }
    
    // Check if key is valid
    bool hasRule(const std::string& key) const;
    
    // Get rule for key
    const ValidationRule* getRule(const std::string& key) const;
    
    // Clear all rules
    void clearRules();
    
    // Get default configuration validator
    static ConfigurationValidator& getDefaultValidator();
    
    // Set default validator
    static void setDefaultValidator(const ConfigurationValidator& validator);
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalValidations;
        size_t successfulValidations;
        size_t failedValidations;
        std::unordered_map<std::string, size_t> validationCounts;
    };
    
    ValidationStats getValidationStats() const { return stats_; }
    
    void resetValidationStats() {
        stats_ = ValidationStats{};
        stats_.totalValidations = 0;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ValidationRule> rules_;
    ValidationStats stats_;
    
    // Type conversion and validation functions
    bool convertAndValidate(const std::string& value, ConfigDataType type, std::string& convertedValue) const;
    bool validateRange(const std::string& value, const ValidationRule& rule) const;
    bool validateAllowedValues(const std::string& value, const ValidationRule& rule) const;
    bool validateCustom(const std::string& value, const ValidationRule& rule) const;
    bool validateRequired(const std::string& value, const ValidationRule& rule) const;
};

// Convenience functions for common validation rules
namespace ConfigValidation {
    ValidationRule RequiredString(const std::string& key, const std::string& description = "");
    ValidationRule RequiredInt(const std::string& key, const std::string& description = "");
    ValidationRule RequiredFloat(const std::string& key, const std::string& description = "");
    ValidationRule RequiredDouble(const std::string& key, const std::string& description = "");
    ValidationRule RequiredBool(const std::string& key, const std::string& description = "");
    
    ValidationRule StringWithAllowedValues(const std::string& key, 
                                          const std::vector<std::string>& allowedValues,
                                          const std::string& description = "");
    ValidationRule IntInRange(const std::string& key, int64_t min, int64_t max, 
                            const std::string& description = "");
    ValidationRule FloatInRange(const std::string& key, float min, float max, 
                              const std::string& description = "");
    ValidationRule DoubleInRange(const std::string& key, double min, double max, 
                               const std::string& description = "");
    
    ValidationRule CustomValidator(const std::string& key, 
                                   std::function<bool(const std::string&)> validator,
                                   const std::string& description = "");
}

} // namespace nlm
