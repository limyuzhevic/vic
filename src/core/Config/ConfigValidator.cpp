// **Configuration Validation Implementation**
// 
// Implementation of configuration validation functionality
// 
#include "ConfigValidator.hpp"
#include <algorithm>
#include <numeric>
#include <regex>
#include <filesystem>
#include <iostream>

namespace nlm {

    // ConfigValidator implementation
    
    ValidationResult ConfigValidator::validate(const ConfigEntry& entry) const {
        ValidationResult result;
        
        // Check if field is defined in schema
        if (!isKnownField(entry.key)) {
            if (strictMode) {
                result.addError(std::make_shared<ValidationException>(
                    "Unknown configuration field: " + entry.key
                ));
            } else {
                result.addWarning("Unknown configuration field: " + entry.key);
            }
            return result;
        }
        
        // Validate against constraints
        validateConstraints(entry, result);
        
        return result;
    }
    
    ValidationResult ConfigValidator::validateAll(const std::vector<ConfigEntry>& entries) const {
        ValidationResult result;
        
        for (const auto& entry : entries) {
            ValidationResult entryResult = validate(entry);
            // Merge results
            if (!entryResult.isValid) {
                result.isValid = false;
                result.errors.insert(result.errors.end(), 
                                   entryResult.errors.begin(), entryResult.errors.end());
            }
            result.warnings.insert(result.warnings.end(),
                                  entryResult.warnings.begin(), entryResult.warnings.end());
        }
        
        return result;
    }
    
    // NLMConfigValidator implementation
    
    // All schema definitions are in the header file
    
    // ConfigValidator implementation
    
    bool RangeConstraint::validate(const ConfigValue& value, ValidationResult& result) const {
        if (!value.has_value<double>()) return true;  // Not a numeric value, skip
        
        double val = value.value<double>();
        if (val < minValue || val > maxValue) {
            result.addError(std::make_shared<RangeValidationError>(
                "Unknown field", constraintDescription, val
            ));
            return false;
        }
        return true;
    }
    
    bool EnumConstraint::validate(const ConfigValue& value, ValidationResult& result) const {
        if (!value.has_value<std::string>()) return true;  // Not a string value, skip
        
        std::string val = value.value<std::string>();
        bool isValid = std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
        
        if (!isValid) {
            std::ostringstream oss;
            oss << "Allowed: ";
            for (size_t i = 0; i < allowedValues.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << allowedValues[i];
            }
            result.addError(std::make_shared<EnumValidationError>(
                "Unknown field", val, oss.str()
            ));
            return false;
        }
        return true;
    }
    
    bool FormatConstraint::validate(const ConfigValue& value, ValidationResult& result) const {
        if (!value.has_value<std::string>()) return true;  // Not a string value, skip
        
        std::string val = value.value<std::string>();
        std::regex regex(pattern);
        if (!std::regex_match(val, regex)) {
            result.addError(std::make_shared<FormatValidationError>(
                "Unknown field", pattern, val
            ));
            return false;
        }
        return true;
    }
    
    bool RequiredConstraint::validate(const ConfigValue& value, ValidationResult& result) const {
        if (optional) return true;  // Optional field, no validation
        if (!hasValue(value)) {
            result.addError(std::make_shared<RequiredFieldError>("Unknown field"));
            return false;
        }
        return true;
    }
    
    bool RequiredConstraint::hasValue(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) return false;
            if constexpr (std::is_same_v<T, std::string>) return !arg.empty();
            if constexpr (std::is_same_v<T, int64_t>) return arg != 0;
            if constexpr (std::is_same_v<T, double>) return arg != 0.0;
            if constexpr (std::is_same_v<T, bool>) return arg;
            return true;
        }, value);
    }
}
