#include "Validation.hpp"
#include <stdexcept>
#include <limits>
#include <algorithm>

namespace nlm {

ValidationResult ConfigurationValidator::validateParameter(const std::string& key, const ConfigValue& value,
                                                          const ParameterMeta& meta) {
    ValidationResult result;
    
    // Check if parameter has a custom validation rule
    if (meta.validation_rule) {
        ValidationResult custom_result = meta.validation_rule(key, value);
        if (custom_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), custom_result.errors.begin(), custom_result.errors.end());
        }
    }
    
    // Type validation
    ValidationResult type_result = validateType(key, value, meta);
    if (type_result.hasErrors()) {
        result.is_valid = false;
        result.errors.insert(result.errors.end(), type_result.errors.begin(), type_result.errors.end());
    }
    
    // Constraint validation
    ValidationResult constraint_result = validateConstraints(key, value, meta);
    if (constraint_result.hasErrors()) {
        result.is_valid = false;
        result.errors.insert(result.errors.end(), constraint_result.errors.begin(), constraint_result.errors.end());
    }
    
    // Custom rule validation (if not already done)
    if (!meta.validation_rule) {
        ValidationResult rule_result = validateCustomRule(key, value, meta);
        if (rule_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), rule_result.errors.begin(), rule_result.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateType(const std::string& key, const ConfigValue& value,
                                                     const ParameterMeta& meta) {
    switch (meta.type) {
        case ParameterType::INT:
            return validateInt(key, value, meta);
        case ParameterType::INT64:
            return validateInt64(key, value, meta);
        case ParameterType::DOUBLE:
            return validateDouble(key, value, meta);
        case ParameterType::BOOL:
            return validateBool(key, value, meta);
        case ParameterType::STRING:
            return validateString(key, value, meta);
        case ParameterType::VECTOR_INT:
            return validateVectorInt(key, value, meta);
        case ParameterType::VECTOR_DOUBLE:
            return validateVectorDouble(key, value, meta);
        case ParameterType::VECTOR_STRING:
            return validateVectorString(key, value, meta);
        case ParameterType::OBJECT:
        case ParameterType::ARRAY:
            return ValidationResult(); // Simplified: treat as valid
        default:
            return ValidationResult(false, {"Unknown parameter type: " + std::to_string(static_cast<int>(meta.type))});
    }
}

ValidationResult ConfigurationValidator::validateConstraints(const std::string& key, const ConfigValue& value,
                                                             const ParameterMeta& meta) {
    ValidationResult result;
    
    switch (meta.type) {
        case ParameterType::INT:
        case ParameterType::INT64:
            result = validateRange(key, value, meta);
            break;
        case ParameterType::DOUBLE:
            result = validateRange(key, value, meta);
            break;
        case ParameterType::STRING:
            result = validateLength(key, value, meta);
            break;
        case ParameterType::VECTOR_INT:
        case ParameterType::VECTOR_DOUBLE:
            result = validateSize(key, value, meta);
            break;
        case ParameterType::VECTOR_STRING:
            result = validateLength(key, value, meta);
            break;
        case ParameterType::OBJECT:
        case ParameterType::ARRAY:
            result = validateSize(key, value, meta);
            break;
        default:
            break;
    }
    
    // Validate allowed values
    if (!meta.allowed_values.empty()) {
        ValidationResult allowed_result = validateAllowedValues(key, value, meta);
        if (allowed_result.hasErrors()) {
            result.is_valid = false;
            result.errors.insert(result.errors.end(), allowed_result.errors.begin(), allowed_result.errors.end());
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateCustomRule(const std::string& key, const ConfigValue& value,
                                                           const ParameterMeta& meta) {
    // This is handled in the main validation method
    return ValidationResult();
}

std::string ConfigurationValidator::typeToString(ParameterType type) {
    switch (type) {
        case ParameterType::INT: return "int";
        case ParameterType::INT64: return "int64";
        case ParameterType::DOUBLE: return "double";
        case ParameterType::BOOL: return "bool";
        case ParameterType::STRING: return "string";
        case ParameterType::VECTOR_INT: return "vector<int>";
        case ParameterType::VECTOR_DOUBLE: return "vector<double>";
        case ParameterType::VECTOR_STRING: return "vector<string>";
        case ParameterType::OBJECT: return "object";
        case ParameterType::ARRAY: return "array";
        default: return "unknown";
    }
}

bool ConfigurationValidator::typeMatches(const ConfigValue& value, ParameterType type) {
    switch (type) {
        case ParameterType::INT:
            return std::holds_alternative<int>(value);
        case ParameterType::INT64:
            return std::holds_alternative<int64_t>(value);
        case ParameterType::DOUBLE:
            return std::holds_alternative<double>(value);
        case ParameterType::BOOL:
            return std::holds_alternative<bool>(value);
        case ParameterType::STRING:
            return std::holds_alternative<std::string>(value);
        case ParameterType::VECTOR_INT:
            return std::holds_alternative<std::vector<int>>(value);
        case ParameterType::VECTOR_DOUBLE:
            return std::holds_alternative<std::vector<double>>(value);
        case ParameterType::VECTOR_STRING:
            return std::holds_alternative<std::vector<std::string>>(value);
        case ParameterType::OBJECT:
        case ParameterType::ARRAY:
            // Simplified: treat any vector as valid
            return std::holds_alternative<std::vector<int>>(value) ||
                   std::holds_alternative<std::vector<double>>(value) ||
                   std::holds_alternative<std::vector<std::string>>(value);
        default:
            return false;
    }
}

ValidationResult ConfigurationValidator::validateInt(const std::string& key, const ConfigValue& value,
                                                    const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::INT)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be an integer, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result;
    if (meta.min_int.has_value()) {
        int val = std::get<int>(value);
        if (val < meta.min_int.value()) {
            result.addError("Parameter '" + key + "' must be >= " + std::to_string(meta.min_int.value()));
        }
    }
    if (meta.max_int.has_value()) {
        int val = std::get<int>(value);
        if (val > meta.max_int.value()) {
            result.addError("Parameter '" + key + "' must be <= " + std::to_string(meta.max_int.value()));
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateInt64(const std::string& key, const ConfigValue& value,
                                                       const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::INT64)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be an int64, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result;
    if (meta.min_int64.has_value()) {
        int64_t val = std::get<int64_t>(value);
        if (val < meta.min_int64.value()) {
            result.addError("Parameter '" + key + "' must be >= " + std::to_string(meta.min_int64.value()));
        }
    }
    if (meta.max_int64.has_value()) {
        int64_t val = std::get<int64_t>(value);
        if (val > meta.max_int64.value()) {
            result.addError("Parameter '" + key + "' must be <= " + std::to_string(meta.max_int64.value()));
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateDouble(const std::string& key, const ConfigValue& value,
                                                       const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::DOUBLE)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a double, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result;
    if (meta.min_double.has_value()) {
        double val = std::get<double>(value);
        if (val < meta.min_double.value()) {
            result.addError("Parameter '" + key + "' must be >= " + std::to_string(meta.min_double.value()));
        }
    }
    if (meta.max_double.has_value()) {
        double val = std::get<double>(value);
        if (val > meta.max_double.value()) {
            result.addError("Parameter '" + key + "' must be <= " + std::to_string(meta.max_double.value()));
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateBool(const std::string& key, const ConfigValue& value,
                                                     const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::BOOL)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a boolean, got " + typeToString(getValueType(value))});
    }
    return ValidationResult();
}

ValidationResult ConfigurationValidator::validateString(const std::string& key, const ConfigValue& value,
                                                       const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::STRING)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a string, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result = validateLength(key, value, meta);
    return result;
}

ValidationResult ConfigurationValidator::validateVectorInt(const std::string& key, const ConfigValue& value,
                                                          const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::VECTOR_INT)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a vector<int>, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result = validateSize(key, value, meta);
    return result;
}

ValidationResult ConfigurationValidator::validateVectorDouble(const std::string& key, const ConfigValue& value,
                                                             const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::VECTOR_DOUBLE)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a vector<double>, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result = validateSize(key, value, meta);
    return result;
}

ValidationResult ConfigurationValidator::validateVectorString(const std::string& key, const ConfigValue& value,
                                                              const ParameterMeta& meta) {
    if (!typeMatches(value, ParameterType::VECTOR_STRING)) {
        return ValidationResult(false, {"Parameter '" + key + "' must be a vector<string>, got " + typeToString(getValueType(value))});
    }
    
    ValidationResult result = validateLength(key, value, meta);
    return result;
}

ValidationResult ConfigurationValidator::validateRange(const std::string& key, const ConfigValue& value,
                                                      const ParameterMeta& meta) {
    ValidationResult result;
    
    double val = 0.0;
    bool is_valid_type = false;
    
    if (meta.type == ParameterType::INT) {
        val = static_cast<double>(std::get<int>(value));
        is_valid_type = true;
    } else if (meta.type == ParameterType::INT64) {
        val = static_cast<double>(std::get<int64_t>(value));
        is_valid_type = true;
    } else if (meta.type == ParameterType::DOUBLE) {
        val = std::get<double>(value);
        is_valid_type = true;
    }
    
    if (!is_valid_type) {
        return result;
    }
    
    if (meta.min_double.has_value() && val < meta.min_double.value()) {
        result.addError("Parameter '" + key + "' must be >= " + std::to_string(meta.min_double.value()));
    }
    if (meta.max_double.has_value() && val > meta.max_double.value()) {
        result.addError("Parameter '" + key + "' must be <= " + std::to_string(meta.max_double.value()));
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateLength(const std::string& key, const ConfigValue& value,
                                                       const ParameterMeta& meta) {
    ValidationResult result;
    
    if (!typeMatches(value, ParameterType::STRING) && !typeMatches(value, ParameterType::VECTOR_STRING)) {
        return result;
    }
    
    size_t length = 0;
    if (meta.type == ParameterType::STRING) {
        length = std::get<std::string>(value).length();
    } else if (meta.type == ParameterType::VECTOR_STRING) {
        length = std::get<std::vector<std::string>>(value).size();
    }
    
    if (meta.min_length.has_value() && length < meta.min_length.value()) {
        result.addError("Parameter '" + key + "' must have at least " + std::to_string(meta.min_length.value()) + " elements");
    }
    if (meta.max_length.has_value() && length > meta.max_length.value()) {
        result.addError("Parameter '" + key + "' must have at most " + std::to_string(meta.max_length.value()) + " elements");
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateSize(const std::string& key, const ConfigValue& value,
                                                     const ParameterMeta& meta) {
    ValidationResult result;
    
    size_t size = 0;
    bool is_valid_type = false;
    
    if (meta.type == ParameterType::VECTOR_INT) {
        size = std::get<std::vector<int>>(value).size();
        is_valid_type = true;
    } else if (meta.type == ParameterType::VECTOR_DOUBLE) {
        size = std::get<std::vector<double>>(value).size();
        is_valid_type = true;
    } else if (meta.type == ParameterType::VECTOR_STRING) {
        size = std::get<std::vector<std::string>>(value).size();
        is_valid_type = true;
    }
    
    if (!is_valid_type) {
        return result;
    }
    
    if (meta.min_size.has_value() && size < meta.min_size.value()) {
        result.addError("Parameter '" + key + "' must have at least " + std::to_string(meta.min_size.value()) + " elements");
    }
    if (meta.max_size.has_value() && size > meta.max_size.value()) {
        result.addError("Parameter '" + key + "' must have at most " + std::to_string(meta.max_size.value()) + " elements");
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateAllowedValues(const std::string& key, const ConfigValue& value,
                                                              const ParameterMeta& meta) {
    ValidationResult result;
    
    if (!typeMatches(value, meta.type)) {
        return result;
    }
    
    std::string str_value;
    if (meta.type == ParameterType::STRING) {
        str_value = std::get<std::string>(value);
    } else if (meta.type == ParameterType::INT) {
        str_value = std::to_string(std::get<int>(value));
    } else if (meta.type == ParameterType::INT64) {
        str_value = std::to_string(std::get<int64_t>(value));
    } else if (meta.type == ParameterType::DOUBLE) {
        str_value = std::to_string(std::get<double>(value));
    } else {
        return result;
    }
    
    bool found = false;
    for (const auto& allowed : meta.allowed_values) {
        if (str_value == allowed) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        result.addError("Parameter '" + key + "' must be one of: ");
        for (size_t i = 0; i < meta.allowed_values.size(); ++i) {
            if (i > 0) result.errors.back() += ", ";
            result.errors.back() += "\"" + meta.allowed_values[i] + "\"";
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validatePattern(const std::string& key, const ConfigValue& value,
                                                       const std::string& pattern) {
    ValidationResult result;
    
    if (meta.type == ParameterType::STRING) {
        std::string str_value = std::get<std::string>(value);
        std::regex regex(pattern);
        if (!std::regex_match(str_value, regex)) {
            result.addError("Parameter '" + key + "' does not match pattern: " + pattern);
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateCustom(const std::string& key, const ConfigValue& value,
                                                       const std::function<ValidationResult(const std::string&, const ConfigValue&)>& rule) {
    return rule(key, value);
}

std::string ConfigurationValidator::getValueType(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

} // namespace nlm
