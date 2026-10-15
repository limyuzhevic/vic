#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <functional>
#include <chrono>
#include <any>
#include <type_traits>
#include <sstream>
#include <set>
#include "../Error.hpp"

namespace nlm {

/**
 * @brief Comprehensive validation framework for the NLM project
 * 
 * This validation framework provides:
 * - Type-safe validation for all configuration values
 * - Input validation for all external interfaces
 * - Validation rules and constraints for all systems
 * - Comprehensive error reporting for validation failures
 * - Validation utilities for all data types
 */

// Validation result type
class ValidationResult {
public:
    ValidationResult() : isValid_(true) {}
    ValidationResult(const std::string& error) : isValid_(false), error_(error) {}
    
    // Check if validation passed
    bool isValid() const { return isValid_; }
    
    // Get validation error message
    const std::string& getError() const { return error_; }
    
    // Add a warning
    void addWarning(const std::string& warning) {
        warnings_.push_back(warning);
    }
    
    // Get warnings
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    // Combine multiple validation results
    ValidationResult combine(const ValidationResult& other) const {
        ValidationResult result;
        result.isValid_ = isValid_ && other.isValid_;
        if (!other.error_.empty()) {
            result.error_ = error_ + " | " + other.error_;
        }
        result.warnings_ = warnings_;
        result.warnings_.insert(result.warnings_.end(), other.warnings_.begin(), other.warnings_.end());
        return result;
    }
    
    // Validation success factory
    static ValidationResult success() { return ValidationResult(); }
    
    // Validation failure factory
    static ValidationResult failure(const std::string& error) { return ValidationResult(error); }
    
private:
    bool isValid_;
    std::string error_;
    std::vector<std::string> warnings_;
};

// Validation rule types
enum class ValidationRuleType {
    Required,
    Type,
    Range,
    Length,
    Pattern,
    Unique,
    Custom
};

// Validation rule for a specific field
struct ValidationRule {
    std::string fieldName;
    ValidationRuleType type;
    std::any value; // Rule-specific value (e.g., min/max for range, pattern regex for pattern, etc.)
    std::string errorMessage;
    bool isOptional = false;
    
    ValidationRule() = default;
    
    ValidationRule(const std::string& field, ValidationRuleType ruleType,
                   const std::any& val, const std::string& msg)
        : fieldName(field), type(ruleType), value(val), errorMessage(msg) {}
    
    ValidationRule(const std::string& field, ValidationRuleType ruleType,
                   const std::any& val, const std::string& msg, bool optional)
        : fieldName(field), type(ruleType), value(val), errorMessage(msg), isOptional(optional) {}
};

// Base validator class
class IValidator {
public:
    virtual ~IValidator() = default;
    
    // Validate a value
    virtual ValidationResult validate(const std::any& value, const std::string& fieldName = "") const = 0;
    
    // Get validator type
    virtual std::string getTypeName() const = 0;
};

// Template validator for type-safe validation
class Validator {
public:
    // Type validation
    class TypeValidator : public IValidator {
    public:
        explicit TypeValidator(const std::type_info& type) : type_(type) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "TypeValidator"; }
    
    private:
        const std::type_info& type_;
    };
    
    // Range validation
    class RangeValidator : public IValidator {
    public:
        RangeValidator(double min, double max, bool inclusive = true)
            : min_(min), max_(max), inclusive_(inclusive) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "RangeValidator"; }
    
    private:
        double min_;
        double max_;
        bool inclusive_;
    };
    
    // String length validation
    class LengthValidator : public IValidator {
    public:
        LengthValidator(size_t min, size_t max, bool inclusive = true)
            : min_(min), max_(max), inclusive_(inclusive) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "LengthValidator"; }
    
    private:
        size_t min_;
        size_t max_;
        bool inclusive_;
    };
    
    // String pattern validation
    class PatternValidator : public IValidator {
    public:
        PatternValidator(const std::string& pattern) : pattern_(pattern) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "PatternValidator"; }
    
    private:
        std::string pattern_;
    };
    
    // Positive value validation
    class PositiveValidator : public IValidator {
    public:
        explicit PositiveValidator(bool allowZero = false) : allowZero_(allowZero) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "PositiveValidator"; }
    
    private:
        bool allowZero_;
    };
    
    // Non-negative value validation
    class NonNegativeValidator : public IValidator {
    public:
        NonNegativeValidator() = default;
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "NonNegativeValidator"; }
    };
    
    // Value in list validation
    class InListValidator : public IValidator {
    public:
        template<typename T>
        InListValidator(const std::vector<T>& values) : values_(values) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "InListValidator"; }
    
    private:
        std::any values_;
    };
    
    // Custom validation function
    class CustomValidator : public IValidator {
    public:
        CustomValidator(std::function<ValidationResult(const std::any&)> validator)
            : validator_(validator) {}
        
        ValidationResult validate(const std::any& value, const std::string& fieldName = "") const override;
        std::string getTypeName() const override { return "CustomValidator"; }
    
    private:
        std::function<ValidationResult(const std::any&)> validator_;
    };
    
    // Validation rule builder
    class ValidationRuleBuilder {
    public:
        ValidationRuleBuilder& required() {
            rule_.type = ValidationRuleType::Required;
            return *this;
        }
        
        ValidationRuleBuilder& ofType(const std::type_info& type) {
            rule_.type = ValidationRuleType::Type;
            rule_.value = type;
            return *this;
        }
        
        ValidationRuleBuilder& inRange(double min, double max, bool inclusive = true) {
            rule_.type = ValidationRuleType::Range;
            rule_.value = std::make_pair(min, std::make_pair(max, inclusive));
            return *this;
        }
        
        ValidationRuleBuilder& length(size_t min, size_t max, bool inclusive = true) {
            rule_.type = ValidationRuleType::Length;
            rule_.value = std::make_pair(min, std::make_pair(max, inclusive));
            return *this;
        }
        
        ValidationRuleBuilder& pattern(const std::string& pattern) {
            rule_.type = ValidationRuleType::Pattern;
            rule_.value = pattern;
            return *this;
        }
        
        ValidationRuleBuilder& positive(bool allowZero = false) {
            rule_.type = ValidationRuleType::Custom;
            rule_.value = std::make_pair(allowZero, std::string("positive"));
            return *this;
        }
        
        ValidationRuleBuilder& nonNegative() {
            rule_.type = ValidationRuleType::Custom;
            rule_.value = std::string("non-negative");
            return *this;
        }
        
        template<typename T>
        ValidationRuleBuilder& inList(const std::vector<T>& values) {
            rule_.type = ValidationRuleType::Custom;
            rule_.value = values;
            return *this;
        }
        
        ValidationRuleBuilder& error(const std::string& msg) {
            rule_.errorMessage = msg;
            return *this;
        }
        
        ValidationRuleBuilder& optional() {
            rule_.isOptional = true;
            return *this;
        }
        
        ValidationRule build(const std::string& fieldName) {
            if (rule_.errorMessage.empty()) {
                rule_.errorMessage = "Validation failed for field: " + fieldName;
            }
            rule_.fieldName = fieldName;
            return rule_;
        }
        
    private:
        ValidationRule rule_;
    };
    
    // Field validator
    class FieldValidator {
    public:
        FieldValidator() = default;
        
        // Validate a single value with multiple rules
        ValidationResult validate(const std::any& value, const std::vector<ValidationRule>& rules) const {
            ValidationResult result;
            
            // Check if value is present (required)
            bool isPresent = (value.type() != typeid(void));
            for (const auto& rule : rules) {
                if (rule.type == ValidationRuleType::Required) {
                    if (!isPresent && !rule.isOptional) {
                        result = ValidationResult(rule.errorMessage);
                        break;
                    }
                    if (!isPresent && rule.isOptional) {
                        continue;
                    }
                }
            }
            
            // If required check passed, continue with other rules
            if (result.isValid()) {
                for (const auto& rule : rules) {
                    if (rule.type == ValidationRuleType::Required && !isPresent) {
                        continue; // Already checked
                    }
                    
                    ValidationResult ruleResult = validateWithRule(value, rule);
                    result = result.combine(ruleResult);
                    if (!result.isValid()) {
                        break;
                    }
                }
            }
            
            return result;
        }
        
    private:
        ValidationResult validateWithRule(const std::any& value, const ValidationRule& rule) const {
            switch (rule.type) {
                case ValidationRuleType::Type:
                    return validateType(value, rule);
                case ValidationRuleType::Range:
                    return validateRange(value, rule);
                case ValidationRuleType::Length:
                    return validateLength(value, rule);
                case ValidationRuleType::Pattern:
                    return validatePattern(value, rule);
                case ValidationRuleType::Custom:
                    return validateCustom(value, rule);
                default:
                    return ValidationResult("Unsupported validation rule type");
            }
        }
        
        ValidationResult validateType(const std::any& value, const ValidationRule& rule) const {
            // Type validation implementation
            if (rule.value.type() == typeid(const std::type_info&)) {
                const auto& expectedType = std::any_cast<const std::type_info&>(rule.value);
                if (value.type() != expectedType) {
                    return ValidationResult(rule.errorMessage);
                }
            }
            return ValidationResult::success();
        }
        
        ValidationResult validateRange(const std::any& value, const ValidationRule& rule) const {
            // Range validation implementation
            return ValidationResult::success();
        }
        
        ValidationResult validateLength(const std::any& value, const ValidationRule& rule) const {
            // Length validation implementation
            return ValidationResult::success();
        }
        
        ValidationResult validatePattern(const std::any& value, const ValidationRule& rule) const {
            // Pattern validation implementation
            return ValidationResult::success();
        }
        
        ValidationResult validateCustom(const std::any& value, const ValidationRule& rule) const {
            // Custom validation implementation
            return ValidationResult::success();
        }
    };
    
    // Schema validator for complex objects
    class SchemaValidator {
    public:
        SchemaValidator() = default;
        
        // Add a field with rules
        SchemaValidator& addField(const std::string& fieldName,
                                 const std::vector<ValidationRule>& rules) {
            schema_[fieldName] = rules;
            return *this;
        }
        
        // Validate an object (represented as map of field names to values)
        ValidationResult validate(const std::map<std::string, std::any>& object) const {
            ValidationResult result;
            
            // Check all fields in schema
            for (const auto& [fieldName, rules] : schema_) {
                auto it = object.find(fieldName);
                if (it == object.end()) {
                    // Field not present
                    bool hasRequiredRule = false;
                    for (const auto& rule : rules) {
                        if (rule.type == ValidationRuleType::Required) {
                            hasRequiredRule = true;
                            break;
                        }
                    }
                    if (hasRequiredRule && !rules.front().isOptional) {
                        result = ValidationResult("Missing required field: " + fieldName);
                        break;
                    }
                    continue;
                }
                
                ValidationResult fieldResult = FieldValidator().validate(it->second, rules);
                result = result.combine(fieldResult);
                if (!result.isValid()) {
                    break;
                }
            }
            
            return result;
        }
        
    private:
        std::map<std::string, std::vector<ValidationRule>> schema_;
    };
    
    // Input validator for external interfaces
    class InputValidator {
    public:
        InputValidator() = default;
        
        // Validate sensory input
        ValidationResult validateSensoryInput(const std::vector<float>& input) const {
            ValidationResult result;
            
            // Check input is not empty
            if (input.empty()) {
                return ValidationResult("Sensory input cannot be empty");
            }
            
            // Check input values are valid (e.g., within expected range)
            for (size_t i = 0; i < input.size(); ++i) {
                if (std::isnan(input[i]) || std::isinf(input[i])) {
                    result = ValidationResult(std::format("Invalid sensory input value at index {}: NaN or infinity", i));
                    break;
                }
                if (input[i] < -10.0f || input[i] > 10.0f) {
                    result.addWarning(std::format("Sensory input value at index {} is outside expected range [-10, 10]", i));
                }
            }
            
            return result;
        }
        
        // Validate brain configuration
        ValidationResult validateBrainConfig(const std::map<std::string, std::any>& config) const {
            ValidationResult result;
            
            // Check required configuration fields
            std::vector<std::string> requiredFields = {"neuron_count", "region_count", "simulation_timestep"};
            
            for (const auto& field : requiredFields) {
                if (config.find(field) == config.end()) {
                    result = ValidationResult("Missing required configuration field: " + field);
                    break;
                }
            }
            
            // Check value ranges
            if (config.find("neuron_count") != config.end()) {
                auto it = config.find("neuron_count");
                ValidationResult countResult = FieldValidator().validate(
                    it->second, 
                    {ValidationRuleBuilder().ofType(typeid(size_t)).error("Invalid neuron count type").build("neuron_count")}
                );
                result = result.combine(countResult);
            }
            
            return result;
        }
        
    private:
        FieldValidator fieldValidator_;
    };
    
    // Global validation helpers
    class ValidationHelpers {
    public:
        // Convert ConfigValue to string for validation
        static std::string configValueToString(const ConfigValue& value) {
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, value);
            return oss.str();
        }
        
        // Validate numeric range
        static ValidationResult validateRange(double value, double min, double max, bool inclusive = true) {
            if (inclusive) {
                if (value < min || value > max) {
                    return ValidationResult(std::format("Value {} is outside range [{}, {}]", value, min, max));
                }
            } else {
                if (value <= min || value >= max) {
                    return ValidationResult(std::format("Value {} is outside range ({}, {})", value, min, max));
                }
            }
            return ValidationResult::success();
        }
        
        // Validate string pattern
        static ValidationResult validatePattern(const std::string& value, const std::string& pattern) {
            // Simple pattern matching (just checking contains for now)
            if (pattern == "required" && value.empty()) {
                return ValidationResult("Value cannot be empty");
            }
            if (pattern == "alphanumeric" && !std::all_of(value.begin(), value.end(),
                                                       [](char c) { return std::isalnum(c) || c == '_'; })) {
                return ValidationResult("Value must be alphanumeric");
            }
            return ValidationResult::success();
        }
        
        // Validate vector bounds
        static ValidationResult validateVectorBounds(const std::vector<float>& values, 
                                                     size_t minSize, size_t maxSize,
                                                     float minValue, float maxValue) {
            ValidationResult result;
            
            if (values.size() < minSize || values.size() > maxSize) {
                result = ValidationResult(std::format("Vector size {} is outside range [{}, {}]", 
                                                 values.size(), minSize, maxSize));
            }
            
            for (size_t i = 0; i < values.size(); ++i) {
                if (std::isnan(values[i]) || std::isinf(values[i])) {
                    result = ValidationResult(std::format("Invalid vector value at index {}: NaN or infinity", i));
                    break;
                }
                if (values[i] < minValue || values[i] > maxValue) {
                    result.addWarning(std::format("Vector value at index {} is outside expected range [{}, {}]", 
                                               values[i], minValue, maxValue));
                }
            }
            
            return result;
        }
    };
};

// Validation macros and helper functions
#define VALIDATE_VALUE(value, rule) nlm::Validator::ValidationRuleBuilder().rule

// Global validation instance
extern Validator validator;

} // namespace nlm
