#include "ConfigValidator.hpp"

namespace nlm {

bool ConfigValidator::validateValue(const ConfigSchema& schema, const std::any& value) {
    if (!value.has_value()) {
        return !schema.required;
    }

    if (!schema.validators.empty()) {
        for (const auto& validator : schema.validators) {
            try {
                validator->validate(value);
            } catch (const ConfigError& e) {
                return false;
            }
        }
    }

    // Check type compatibility
    ConfigSchema::Type actualType = ConfigSchema::getTypeFromAny(value);
    if (actualType != schema.type) {
        return false;
    }

    // Check range constraints
    if (schema.minValue.has_value() && schema.maxValue.has_value()) {
        if (value.type() == typeid(int)) {
            int val = std::any_cast<int>(value);
            int min = std::any_cast<int>(schema.minValue);
            int max = std::any_cast<int>(schema.maxValue);
            if (val < min || val > max) return false;
        } else if (value.type() == typeid(double)) {
            double val = std::any_cast<double>(value);
            double min = std::any_cast<double>(schema.minValue);
            double max = std::any_cast<double>(schema.maxValue);
            if (val < min || val > max) return false;
        } else if (value.type() == typeid(int64_t)) {
            int64_t val = std::any_cast<int64_t>(value);
            int64_t min = std::any_cast<int64_t>(schema.minValue);
            int64_t max = std::any_cast<int64_t>(schema.maxValue);
            if (val < min || val > max) return false;
        }
    }

    // Check length constraints for strings
    if (schema.minLength.has_value() && schema.maxLength.has_value() &&
        value.type() == typeid(std::string)) {
        size_t val = std::any_cast<const std::string&>(value).size();
        size_t min = std::any_cast<size_t>(schema.minLength);
        size_t max = std::any_cast<size_t>(schema.maxLength);
        if (val < min || val > max) return false;
    }

    return true;
}

bool ConfigValidator::validateAll(const std::unordered_map<std::string, ConfigSchema>& schema, 
                                  const std::unordered_map<std::string, std::any>& values) {
    for (const auto& pair : schema) {
        const auto& key = pair.first;
        const auto& schemaEntry = pair.second;

        auto it = values.find(key);
        if (it == values.end()) {
            if (schemaEntry.required) return false;
            continue;
        }

        if (!validateValue(schemaEntry, it->second)) {
            return false;
        }
    }
    return true;
}

std::string ConfigValidator::getValidationErrorMessage(const ConfigSchema& schema, 
                                                       const std::any& value) {
    std::stringstream ss;
    ss << "Validation failed for key '" << schema.name << "': ";

    if (!value.has_value()) {
        if (schema.required) {
            ss << "Required field is missing";
        } else {
            ss << "Invalid value type";
        }
    } else {
        ConfigSchema::Type actualType = ConfigSchema::getTypeFromAny(value);
        if (actualType != schema.type) {
            ss << "Expected type " << static_cast<int>(schema.type) 
               << " but got " << static_cast<int>(actualType);
        } else {
            ss << "Value validation failed";
        }
    }

    return ss.str();
}

} // namespace nlm