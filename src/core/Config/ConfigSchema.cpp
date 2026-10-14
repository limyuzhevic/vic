#include "ConfigSchema.hpp"
#include <sstream>
#include <algorithm>
#include <typeinfo>

namespace nlm {

ConfigSchema::ConfigSchema() {}

void ConfigSchema::registerParameter(const ConfigParameter& param) {
    parameters_[param.name] = param;
}

const ConfigParameter* ConfigSchema::getParameter(const std::string& name) const {
    auto it = parameters_.find(name);
    if (it != parameters_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<ValidationError> ConfigSchema::validate(
    const std::string& key,
    const std::any& value
) const {
    std::vector<ValidationError> errors;
    
    auto paramIt = parameters_.find(key);
    if (paramIt == parameters_.end()) {
        errors.emplace_back(key, "Unknown configuration parameter", "error");
        return errors;
    }
    
    const ConfigParameter& param = paramIt->second;
    
    // Type validation
    if (!validateType(param, value)) {
        errors.emplace_back(
            key,
            "Invalid type. Expected " + getTypeName(param.defaultValue) + ", got " + getTypeName(value),
            "error"
        );
    }
    
    // Range validation for numeric types
    if (param.numericRange.has_value() && isNumericType(param.defaultValue)) {
        double val = getNumericValue(value);
        double min = param.numericRange->first;
        double max = param.numericRange->second;
        
        if (val < min || val > max) {
            errors.emplace_back(
                key,
                "Value " + std::to_string(val) + " is outside valid range [" + 
                std::to_string(min) + ", " + std::to_string(max) + "]",
                "error"
            );
        }
    }
    
    // Custom validator if provided
    if (param.validator && !param.validator(value)) {
        errors.emplace_back(
            key,
            "Value failed custom validation",
            "error"
        );
    }
    
    return errors;
}

std::vector<std::string> ConfigSchema::getParameterNames() const {
    std::vector<std::string> names;
    names.reserve(parameters_.size());
    for (const auto& pair : parameters_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<const ConfigParameter*> ConfigSchema::getParametersByCategory(
    const std::string& category
) const {
    std::vector<const ConfigParameter*> result;
    for (const auto& pair : parameters_) {
        if (pair.second.category == category) {
            result.push_back(&pair.second);
        }
    }
    return result;
}

std::string ConfigSchema::getSchemaDescription() const {
    std::ostringstream oss;
    oss << "NLM Configuration Schema\n";
    oss << "Total parameters: " << parameters_.size() << "\n";
    oss << "\n";
    
    // Group parameters by category
    std::unordered_map<std::string, std::vector<const ConfigParameter*>> byCategory;
    for (const auto& pair : parameters_) {
        byCategory[pair.second.category].push_back(&pair.second);
    }
    
    for (const auto& categoryPair : byCategory) {
        oss << categoryPair.first << ":\n";
        for (const ConfigParameter* param : categoryPair.second) {
            oss << "  " << param->name << ": " << param->description << "\n";
            oss << "    Default: " << getValueString(param->defaultValue) << "\n";
            if (param->numericRange.has_value()) {
                oss << "    Range: [" << param->numericRange->first << ", " << 
                    param->numericRange->second << "]\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ConfigSchema::getValidationSummary() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:\n";
    oss << "Total registered parameters: " << parameters_.size() << "\n";
    oss << "Validation errors found: " << validationErrors_.size() << "\n";
    
    if (!validationErrors_.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : validationErrors_) {
            oss << "  " << error.severity << ": " << error.parameter << " - " << error.message << "\n";
        }
    }
    
    return oss.str();
}

bool ConfigSchema::hasParameter(const std::string& name) const {
    return parameters_.find(name) != parameters_.end();
}

// Helper functions

std::string ConfigSchema::getTypeName(const std::variant<int, int64_t, double, bool, std::string>& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    return "unknown";
}

std::string ConfigSchema::getTypeName(const std::any& value) const {
    if (value.type() == typeid(int)) return "int";
    if (value.type() == typeid(int64_t)) return "int64_t";
    if (value.type() == typeid(double)) return "double";
    if (value.type() == typeid(bool)) return "bool";
    if (value.type() == typeid(std::string)) return "string";
    return "unknown";
}

bool ConfigSchema::validateType(
    const ConfigParameter& param,
    const std::any& value
) const {
    if (param.defaultValue.index() != value.index()) {
        return false;
    }
    
    if (value.type() == typeid(std::string)) {
        return std::holds_alternative<std::string>(param.defaultValue);
    } else if (value.type() == typeid(int)) {
        return std::holds_alternative<int>(param.defaultValue);
    } else if (value.type() == typeid(int64_t)) {
        return std::holds_alternative<int64_t>(param.defaultValue);
    } else if (value.type() == typeid(double)) {
        return std::holds_alternative<double>(param.defaultValue);
    } else if (value.type() == typeid(bool)) {
        return std::holds_alternative<bool>(param.defaultValue);
    }
    
    return true;
}

bool ConfigSchema::isNumericType(const std::variant<int, int64_t, double, bool, std::string>& value) const {
    return std::holds_alternative<int>(value) || 
           std::holds_alternative<int64_t>(value) || 
           std::holds_alternative<double>(value);
}

double ConfigSchema::getNumericValue(const std::any& value) const {
    if (value.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        return static_cast<double>(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    }
    return 0.0;
}

std::string ConfigSchema::getValueString(const std::variant<int, int64_t, double, bool, std::string>& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::any_cast<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::any_cast<std::string>(value);
    }
    return "";
}

} // namespace nlm