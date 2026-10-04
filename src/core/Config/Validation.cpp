#include "Validation.hpp"
#include "Config.hpp"
#include <iostream>
#include <unordered_map>
#include <variant>
#include <typeindex>
#include <cmath>

namespace nlm {

struct ConfigurationSchema::Impl {
    std::unordered_map<std::string, ParameterInfo> parameters;
};

ConfigurationSchema::ConfigurationSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigurationSchema::~ConfigurationSchema() = default;

ConfigurationSchema::ConfigurationSchema(ConfigurationSchema&&) noexcept = default;
ConfigurationSchema& ConfigurationSchema::operator=(ConfigurationSchema&&) noexcept = default;

void ConfigurationSchema::addParameter(const std::string& name, Type type, const std::string& description,
                                     bool required, const std::any& defaultValue,
                                     const std::vector<ValidationRule>& validationRules) {
    ParameterInfo info;
    info.name = name;
    info.type = type;
    info.description = description;
    info.required = required;
    info.defaultValue = defaultValue;
    info.validationRules = validationRules;
    
    pImpl->parameters[name] = info;
}

const ConfigurationSchema::ParameterInfo* ConfigurationSchema::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return &it->second;
    }
    return nullptr;
}

ValidationResult ConfigurationSchema::validate(const std::string& key, const ConfigValue& value, ConfigSource source) const {
    ValidationResult result;
    
    const ParameterInfo* param = getParameter(key);
    if (!param) {
        result.addWarning("Parameter '" + key + "' is not defined in the schema but is present in configuration.");
        return result;
    }
    
    // Type validation
    if (!validateType(key, value, param->type)) {
        std::string actualType = "unknown";
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) actualType = "int";
            else if constexpr (std::is_same_v<T, int64_t>) actualType = "int64_t";
            else if constexpr (std::is_same_v<T, double>) actualType = "double";
            else if constexpr (std::is_same_v<T, float>) actualType = "float";
            else if constexpr (std::is_same_v<T, bool>) actualType = "bool";
            else if constexpr (std::is_same_v<T, std::string>) actualType = "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) actualType = "vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) actualType = "vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) actualType = "vector<string>";
        }, value);
        
        std::string expectedTypeName;
        switch (param->type) {
            case Type::INT64_T: expectedTypeName = "int64_t"; break;
            case Type::INT: expectedTypeName = "int"; break;
            case Type::DOUBLE: expectedTypeName = "double"; break;
            case Type::FLOAT: expectedTypeName = "float"; break;
            case Type::BOOL: expectedTypeName = "bool"; break;
            case Type::STRING: expectedTypeName = "string"; break;
            case Type::VECTOR_INT: expectedTypeName = "vector<int>"; break;
            case Type::VECTOR_DOUBLE: expectedTypeName = "vector<double>"; break;
            case Type::VECTOR_STRING: expectedTypeName = "vector<string>"; break;
        }
        
        result.addError("Parameter '" + key + "' has invalid type. Expected " + expectedTypeName + ", got " + actualType + ".");
    }
    
    // Convert variant to any for range validation
    std::any valueAny;
    std::visit([&](auto&& arg) {
        valueAny = arg;
    }, value);
    
    // Range validation
    for (const auto& rule : param->validationRules) {
        if (!validateRange(valueAny, rule)) {
            std::string ruleDesc = rule.description.empty() ? "Validation rule" : rule.description;
            result.addError("Parameter '" + key + "' failed validation: " + ruleDesc + ".");
        }
    }
    
    return result;
}

ValidationResult ConfigurationSchema::validateAll(const std::vector<ConfigEntry>& entries) const {
    ValidationResult result;
    
    // Check for missing required parameters
    for (const auto& param : pImpl->parameters) {
        if (param.second.required && !hasParameterInEntries(param.first, entries)) {
            result.addError("Missing required parameter: '" + param.first + "'.");
        }
    }
    
    // Validate each entry
    for (const auto& entry : entries) {
        ValidationResult entryResult = validate(entry.key, entry.value, entry.source);
        if (!entryResult.valid) {
            result.errors.insert(result.errors.end(), entryResult.errors.begin(), entryResult.errors.end());
        }
        if (!entryResult.warnings.empty()) {
            result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
        }
    }
    
    result.valid = result.errors.empty();
    return result;
}

std::string ConfigurationSchema::createValidationReport(const ValidationResult& result) {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    oss << "Overall: " << (result.valid ? "PASS" : "FAIL") << "\n";
    oss << "Total Errors: " << result.errors.size() << "\n";
    oss << "Total Warnings: " << result.warnings.size() << "\n\n";
    
    if (!result.errors.empty()) {
        oss << "Errors:";
        for (const auto& error : result.errors) {
            oss << "\n  - " << error;
        }
        oss << "\n\n";
    }
    
    if (!result.warnings.empty()) {
        oss << "Warnings:";
        for (const auto& warning : result.warnings) {
            oss << "\n  - " << warning;
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

std::vector<std::string> ConfigurationSchema::getParameterNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->parameters.size());
    for (const auto& param : pImpl->parameters) {
        names.push_back(param.first);
    }
    return names;
}

void ConfigurationSchema::clear() {
    pImpl->parameters.clear();
}

bool ConfigurationSchema::validateType(const std::string& key, const ConfigValue& value, Type expectedType) const {
    const ParameterInfo* param = getParameter(key);
    if (!param) return false;
    
    switch (param->type) {
        case Type::INT64_T:
            return std::holds_alternative<int64_t>(value);
        case Type::INT:
            return std::holds_alternative<int>(value);
        case Type::DOUBLE:
            return std::holds_alternative<double>(value);
        case Type::FLOAT:
            return std::holds_alternative<float>(value);
        case Type::BOOL:
            return std::holds_alternative<bool>(value);
        case Type::STRING:
            return std::holds_alternative<std::string>(value);
        case Type::VECTOR_INT:
            return std::holds_alternative<std::vector<int>>(value);
        case Type::VECTOR_DOUBLE:
            return std::holds_alternative<std::vector<double>>(value);
        case Type::VECTOR_STRING:
            return std::holds_alternative<std::vector<std::string>>(value);
        default:
            return false;
    }
}

bool ConfigurationSchema::validateRange(const std::any& value, const ValidationRule& rule) const {
    if (value.type() == typeid(double)) {
        double val = std::any_cast<double>(value);
        
        switch (rule.type) {
            case ValidationType::RANGE_MIN:
                if (rule.min.has_value()) return val >= std::any_cast<double>(rule.min);
                return true;
            case ValidationType::RANGE_MAX:
                if (rule.max.has_value()) return val <= std::any_cast<double>(rule.max);
                return true;
            case ValidationType::RANGE_MIN_MAX:
                if (rule.min.has_value() && rule.max.has_value()) {
                    return val >= std::any_cast<double>(rule.min) && val <= std::any_cast<double>(rule.max);
                }
                return true;
            case ValidationType::POSITIVE:
                return val > 0.0;
            case ValidationType::NON_NEGATIVE:
                return val >= 0.0;
            case ValidationType::ZERO_TO_ONE:
                return val >= 0.0 && val <= 1.0;
            case ValidationType::GREATER_THAN:
                if (rule.min.has_value()) return val > std::any_cast<double>(rule.min);
                return true;
            case ValidationType::LESS_THAN:
                if (rule.min.has_value()) return val < std::any_cast<double>(rule.min);
                return true;
            case ValidationType::EQUAL_TO:
                if (rule.exact.has_value()) return val == std::any_cast<double>(rule.exact);
                return true;
            case ValidationType::CONSTRAINT:
            case ValidationType::CUSTOM:
                if (rule.customValidator) return rule.customValidator(value);
                return true;
            default:
                return true;
        }
    } else if (value.type() == typeid<float)) {
        float val = std::any_cast<float>(value);
        
        switch (rule.type) {
            case ValidationType::RANGE_MIN:
                if (rule.min.has_value()) return val >= std::any_cast<float>(rule.min);
                return true;
            case ValidationType::RANGE_MAX:
                if (rule.max.has_value()) return val <= std::any_cast<float>(rule.max);
                return true;
            case ValidationType::RANGE_MIN_MAX:
                if (rule.min.has_value() && rule.max.has_value()) {
                    return val >= std::any_cast<float>(rule.min) && val <= std::any_cast<float>(rule.max);
                }
                return true;
            case ValidationType::POSITIVE:
                return val > 0.0f;
            case ValidationType::NON_NEGATIVE:
                return val >= 0.0f;
            case ValidationType::ZERO_TO_ONE:
                return val >= 0.0f && val <= 1.0f;
            case ValidationType::GREATER_THAN:
                if (rule.min.has_value()) return val > std::any_cast<float>(rule.min);
                return true;
            case ValidationType::LESS_THAN:
                if (rule.min.has_value()) return val < std::any_cast<float>(rule.min);
                return true;
            case ValidationType::EQUAL_TO:
                if (rule.exact.has_value()) return val == std::any_cast<float>(rule.exact);
                return true;
            case ValidationType::CONSTRAINT:
            case ValidationType::CUSTOM:
                if (rule.customValidator) return rule.customValidator(value);
                return true;
            default:
                return true;
        }
    } else if (value.type() == typeid(int64_t)) {
        int64_t val = std::any_cast<int64_t>(value);
        
        switch (rule.type) {
            case ValidationType::RANGE_MIN:
                if (rule.min.has_value()) return val >= std::any_cast<int64_t>(rule.min);
                return true;
            case ValidationType::RANGE_MAX:
                if (rule.max.has_value()) return val <= std::any_cast<int64_t>(rule.max);
                return true;
            case ValidationType::RANGE_MIN_MAX:
                if (rule.min.has_value() && rule.max.has_value()) {
                    return val >= std::any_cast<int64_t>(rule.min) && val <= std::any_cast<int64_t>(rule.max);
                }
                return true;
            case ValidationType::POSITIVE:
                return val > 0;
            case ValidationType::NON_NEGATIVE:
                return val >= 0;
            case ValidationType::GREATER_THAN:
                if (rule.min.has_value()) return val > std::any_cast<int64_t>(rule.min);
                return true;
            case ValidationType::LESS_THAN:
                if (rule.min.has_value()) return val < std::any_cast<int64_t>(rule.min);
                return true;
            case ValidationType::EQUAL_TO:
                if (rule.exact.has_value()) return val == std::any_cast<int64_t>(rule.exact);
                return true;
            case ValidationType::CONSTRAINT:
            case ValidationType::CUSTOM:
                if (rule.customValidator) return rule.customValidator(value);
                return true;
            default:
                return true;
        }
    } else if (value.type() == typeid(int)) {
        int val = std::any_cast<int>(value);
        
        switch (rule.type) {
            case ValidationType::RANGE_MIN:
                if (rule.min.has_value()) return val >= std::any_cast<int>(rule.min);
                return true;
            case ValidationType::RANGE_MAX:
                if (rule.max.has_value()) return val <= std::any_cast<int>(rule.max);
                return true;
            case ValidationType::RANGE_MIN_MAX:
                if (rule.min.has_value() && rule.max.has_value()) {
                    return val >= std::any_cast<int>(rule.min) && val <= std::any_cast<int>(rule.max);
                }
                return true;
            case ValidationType::POSITIVE:
                return val > 0;
            case ValidationType::NON_NEGATIVE:
                return val >= 0;
            case ValidationType::GREATER_THAN:
                if (rule.min.has_value()) return val > std::any_cast<int>(rule.min);
                return true;
            case ValidationType::LESS_THAN:
                if (rule.min.has_value()) return val < std::any_cast<int>(rule.min);
                return true;
            case ValidationType::EQUAL_TO:
                if (rule.exact.has_value()) return val == std::any_cast<int>(rule.exact);
                return true;
            case ValidationType::CONSTRAINT:
            case ValidationType::CUSTOM:
                if (rule.customValidator) return rule.customValidator(value);
                return true;
            default:
                return true;
        }
    }
    
    return true; // No validation for other types
}

bool ConfigurationSchema::hasParameterInEntries(const std::string& key, const std::vector<ConfigEntry>& entries) const {
    return std::any_of(entries.begin(), entries.end(), [&key](const ConfigEntry& e) {
        return e.key == key;
    });
}

} // namespace nlm
