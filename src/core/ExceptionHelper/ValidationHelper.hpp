#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "Exceptions.hpp"

namespace nlm {

class ValidationHelper {
public:
    using ValidationFunction = std::function<ValidationResult(const std::string&, const ConfigValue&)>;
    using ValidationMap = std::unordered_map<std::string, ValidationFunction>;
    
    static ValidationMap& getValidationRules() {
        static ValidationMap rules;
        return rules;
    }
    
    static void registerValidation(const std::string& key, ValidationFunction func) {
        getValidationRules()[key] = std::move(func);
    }
    
    static ValidationResult validate(const std::string& key, const ConfigValue& value) {
        auto it = getValidationRules().find(key);
        if (it != getValidationRules().end()) {
            return it->second(key, value);
        }
        return ValidationResult::success(); // No validation rule means valid
    }
    
    static ValidationResult validateAll(const std::unordered_map<std::string, ConfigValue>& values) {
        for (const auto& pair : values) {
            ValidationResult result = validate(pair.first, pair.second);
            if (!result.isValid) {
                return result;
            }
        }
        return ValidationResult::success();
    }
};

// Common validation functions
ValidationResult validateIntRange(const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val < -100 || val > 100) {
            return ValidationResult::error("Integer value out of range [-100, 100]", "INT_RANGE");
        }
    }
    return ValidationResult::success();
}

ValidationResult validateDoubleRange(const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < -1000.0 || val > 1000.0) {
            return ValidationResult::error("Double value out of range [-1000.0, 1000.0]", "DOUBLE_RANGE");
        }
    }
    return ValidationResult::success();
}

ValidationResult validateStringLength(const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        const std::string& val = std::get<std::string>(value);
        if (val.length() > 1024) {
            return ValidationResult::error("String value exceeds maximum length of 1024", "STRING_LENGTH");
        }
    }
    return ValidationResult::success();
}

ValidationResult validateBoolean(const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return ValidationResult::success();
    }
    return ValidationResult::error("Expected boolean value", "TYPE_MISMATCH");
}

ValidationResult validateVectorSize(const std::string& key, const ConfigValue& value) {
    if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& val = std::get<std::vector<int>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("Integer vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& val = std::get<std::vector<double>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("Double vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& val = std::get<std::vector<std::string>>(value);
        if (val.size() > 100) {
            return ValidationResult::error("String vector exceeds maximum size of 100", "VECTOR_SIZE");
        }
    }
    return ValidationResult::success();
}

} // namespace nlm
