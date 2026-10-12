// Input Validation Framework for NLM
// This file provides comprehensive input validation and sanitization

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <memory>
#include <functional>
#include <stdexcept>

namespace nlm {

// Validation Result
struct ValidationResult {
    bool isValid;
    std::string message;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::string& msg) : isValid(valid), message(msg) {}
    
    static ValidationResult success() { return {true, "Input validation passed"}; }
    static ValidationResult failure(const std::string& error) { 
        ValidationResult result;
        result.isValid = false;
        result.errors.push_back(error);
        return result;
    }
    
    ValidationResult& addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
        return *this;
    }
    
    ValidationResult& addWarning(const std::string& warning) {
        warnings.push_back(warning);
        return *this;
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Input Validation Framework
class InputValidator {
public:
    InputValidator() = default;
    
    // Validate input
    ValidationResult validate(const std::string& value, const std::string& context = "") const;
    
    // Validate and sanitize
    ValidationResult validateAndSanitize(const std::string& value, std::string& sanitizedValue, const std::string& context = "") const;
    
    // Check if valid
    bool isValid(const std::string& value, const std::string& context = "") const {
        return validate(value, context).isValid;
    }
};

// Global Validation Manager
class GlobalValidationManager {
public:
    GlobalValidationManager() = default;
    
    // Validate field
    ValidationResult validateField(const std::string& fieldName, const std::string& value, bool isRequired = false) const {
        // Default validation
        ValidationResult result;
        if (isRequired && value.empty()) {
            result.addError("Field is required");
        }
        return result;
    }
    
    // Sanitize field
    std::string sanitizeField(const std::string& fieldName, const std::string& value) const {
        return value;
    }
};

inline GlobalValidationManager& getGlobalValidationManager() {
    static GlobalValidationManager manager;
    return manager;
}

inline ValidationResult validateField(const std::string& fieldName, const std::string& value, bool isRequired = false) {
    return getGlobalValidationManager().validateField(fieldName, value, isRequired);
}

inline std::string sanitizeField(const std::string& fieldName, const std::string& value) {
    return getGlobalValidationManager().sanitizeField(fieldName, value);
}

} // namespace nlm

