#include "ConfigValidator.hpp"
#include <stdexcept>
#include <cctype>
#include <regex>
#include <sstream>

namespace nlm {

// ConfigRangeValidator implementation
ConfigRangeValidator::ConfigRangeValidator(const std::any& minValue, const std::any& maxValue,
                                         bool inclusiveMin, bool inclusiveMax)
    : minValue(minValue), maxValue(maxValue), inclusiveMin(inclusiveMin), inclusiveMax(inclusiveMax) {}

ValidationResult ConfigRangeValidator::validate(const std::string& fieldName, const ConfigValue& value) const {
    try {
        if (const int* intVal = std::get_if<int>(&value)) {
            if (!validateRange(*intVal)) {
                return ValidationResult(false, "Value out of range", fieldName);
            }
        } else if (const int64_t* int64Val = std::get_if<int64_t>(&value)) {
            if (!validateRange(*int64Val)) {
                return ValidationResult(false, "Value out of range", fieldName);
            }
        } else if (const double* doubleVal = std::get_if<double>(&value)) {
            if (!validateRange(*doubleVal)) {
                return ValidationResult(false, "Value out of range", fieldName);
            }
        } else {
            return ValidationResult(false, "Value type mismatch for range validation", fieldName);
        }
    } catch (const std::bad_variant_access&) {
        return ValidationResult(false, "Cannot access value for range validation", fieldName);
    }
    return ValidationResult();
}

// Template implementation for range validation
template<typename T>
bool ConfigRangeValidator::validateRange(const T& value) const {
    bool minOk = true, maxOk = true;
    
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        const auto* minInt = std::get_if<int64_t>(&minValue);
        const auto* maxInt = std::get_if<int64_t>(&maxValue);
        
        if (minInt) {
            if (inclusiveMin) minOk = value >= *minInt;
            else minOk = value > *minInt;
        }
        
        if (maxInt) {
            if (inclusiveMax) maxOk = value <= *maxInt;
            else maxOk = value < *maxInt;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        const auto* minDouble = std::get_if<double>(&minValue);
        const auto* maxDouble = std::get_if<double>(&maxValue);
        
        if (minDouble) {
            if (inclusiveMin) minOk = value >= *minDouble;
            else minOk = value > *minDouble;
        }
        
        if (maxDouble) {
            if (inclusiveMax) maxOk = value <= *maxDouble;
            else maxOk = value < *maxDouble;
        }
    }
    
    return minOk && maxOk;
}

// ConfigFormatValidator implementation
ConfigFormatValidator::ConfigFormatValidator(const std::string& pattern, const std::string& errorMessage)
    : pattern(pattern), errorMessage(errorMessage) {}

ValidationResult ConfigFormatValidator::validate(const std::string& fieldName, const ConfigValue& value) const {
    const std::string* strVal = std::get_if<std::string>(&value);
    if (!strVal) {
        return ValidationResult(false, "Value is not a string", fieldName);
    }
    
    // Basic pattern matching
    if (pattern.find('*') != std::string::npos) {
        // Simple wildcard matching
        std::string regexPattern = std::regex_escape(pattern);
        regexPattern = std::regex_replace(regexPattern, std::regex(R"(\*)\), "[^"]*");
        std::regex regex(regexPattern);
        if (!std::regex_match(*strVal, regex)) {
            return ValidationResult(false, 
                errorMessage.empty() ? "Value does not match pattern " + pattern : errorMessage,
                fieldName);
        }
    } else if (pattern.find('@') != std::string::npos && pattern.find('.') != std::string::npos) {
        // Simple email validation
        static const std::regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})$");
        if (!std::regex_match(*strVal, emailRegex)) {
            return ValidationResult(false,
                errorMessage.empty() ? "Invalid email format" : errorMessage,
                fieldName);
        }
    } else {
        // Exact match
        if (*strVal != pattern) {
            return ValidationResult(false,
                errorMessage.empty() ? "Value does not match expected value " + pattern : errorMessage,
                fieldName);
        }
    }
    
    return ValidationResult();
}

// ConfigDependencyValidator implementation
ConfigDependencyValidator::ConfigDependencyValidator(const std::vector<std::string>& requiredFields,
                                                     const std::string& errorMessage)
    : requiredFields(requiredFields), errorMessage(errorMessage) {}

ValidationResult ConfigDependencyValidator::validate(const std::string& fieldName, const ConfigValue& value) const {
    // This validator doesn't validate the value itself, but checks if dependencies exist
    // It should be used in conjunction with other validators
    
    // For demonstration, we'll just check that the field itself exists in the configuration
    // In a real implementation, this would check if required fields are present in the config
    if (fieldName.empty()) {
        return ValidationResult(false, "Field name is empty", fieldName);
    }
    
    return ValidationResult();
}

// ConfigValidationCallback implementation
ConfigValidationCallback::ConfigValidationCallback(const std::string& fieldName,
                                                 std::function<ValidationResult(const ConfigValue&)> callback,
                                                 const std::string& errorMessage)
    : fieldName(fieldName), callback(callback), errorMessage(errorMessage) {}

ValidationResult ConfigValidationCallback::validate(const std::string& fieldName, const ConfigValue& value) const {
    if (this->fieldName != fieldName) {
        return ValidationResult(); // Not our responsibility
    }
    
    ValidationResult result = callback(value);
    if (!result.errorMessage.empty() && result.errorMessage != errorMessage) {
        // Use the provided error message
        result.errorMessage = errorMessage.empty() ? result.errorMessage : errorMessage;
    }
    
    return result;
}

} // namespace nlm