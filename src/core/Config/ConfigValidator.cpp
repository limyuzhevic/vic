#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "int64_t", "double", "bool", "string", "float", "vector"
    std::string description;
    bool required;
    ConfigValue defaultValue;
    
    // Validation rules
    std::optional<std::pair<double, double>> intRange;
    std::optional<std::pair<double, double>> doubleRange;
    std::optional<size_t> minLength;
    std::optional<size_t> maxLength;
    
    // Dependencies
    std::vector<std::string> dependsOn;
};

// Global configuration schema
class GlobalConfigSchema {
public:
    static GlobalConfigSchema& getInstance() {
        static GlobalConfigSchema instance;
        return instance;
    }
    
    void addSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema(const std::string& key) const;
    bool validateAll(const Config& config) const;
    std::vector<std::string> getRequiredKeys() const;
    
private:
    GlobalConfigSchema() = default;
    std::unordered_map<std::string, ConfigSchema> schemas_;
};

// Configuration validation implementation
bool ConfigValidator::validate(const std::string& key, const ConfigValue& value) {
    const auto* schema = GlobalConfigSchema::getInstance().getSchema(key);
    if (!schema) return true;  // No schema means no validation required
    
    return validateValue(key, value, *schema);
}

std::string ConfigValidator::getValidationError(const std::string& key, const ConfigValue& value) {
    const auto* schema = GlobalConfigSchema::getInstance().getSchema(key);
    if (!schema) return "";
    
    if (!validateValue(key, value, *schema)) {
        return "Validation failed for key '" + key + "' with value '" + getValueString(value) + "'";
    }
    
    return "";
}

bool ConfigValidator::validateFileFormat(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension == "json" || extension == "yaml" || extension == "yml";
}

bool ConfigValidator::validateValueRange(const std::string& key, const ConfigValue& value) {
    const auto* schema = GlobalConfigSchema::getInstance().getSchema(key);
    if (!schema) return true;
    
    return validateValueRange(value, *schema);
}

bool ConfigValidator::validateDependencies(const std::string& key, const ConfigValue& value, const Config& config) {
    const auto* schema = GlobalConfigSchema::getInstance().getSchema(key);
    if (!schema || schema->dependsOn.empty()) return true;
    
    for (const auto& depKey : schema->dependsOn) {
        if (!config.has(depKey)) {
            return false;  // Required dependency is missing
        }
    }
    
    return true;
}

bool ConfigValidator::validateValue(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) {
    if (schema.required && !hasValue(value)) {
        return false;
    }
    
    if (!hasValue(value)) return true;  // Empty value is okay for optional fields
    
    // Type validation
    bool typeValid = false;
    if (schema.type == "int" && std::holds_alternative<int>(value)) typeValid = true;
    else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(value)) typeValid = true;
    else if (schema.type == "double" && std::holds_alternative<double>(value)) typeValid = true;
    else if (schema.type == "bool" && std::holds_alternative<bool>(value)) typeValid = true;
    else if (schema.type == "string" && std::holds_alternative<std::string>(value)) typeValid = true;
    else if (schema.type == "float" && std::holds_alternative<double>(value)) typeValid = true;  // Use double for float
    else if (schema.type == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) typeValid = true;
    else if (schema.type == "vector<double>" && std::holds_alternative<std::vector<double>>(value)) typeValid = true;
    else if (schema.type == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) typeValid = true;
    
    if (!typeValid) return false;
    
    // Range validation
    if (!validateValueRange(value, schema)) return false;
    
    // Length validation
    if (!validateValueLength(value, schema)) return false;
    
    return true;
}

std::string ConfigValidator::getValueString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool ConfigValidator::hasValue(const ConfigValue& value) {
    return !std::holds_alternative<std::monostate>(value);
}

bool ConfigValidator::validateValueRange(const ConfigValue& value, const ConfigSchema& schema) {
    if (schema.type == "int") {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (schema.intRange) {
                return val >= schema.intRange->first && val <= schema.intRange->second;
            }
        }
    } else if (schema.type == "int64_t") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (schema.intRange) {
                return val >= schema.intRange->first && val <= schema.intRange->second;
            }
        }
    } else if (schema.type == "double" || schema.type == "float") {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (schema.doubleRange) {
                return val >= schema.doubleRange->first && val <= schema.doubleRange->second;
            }
        }
    }
    
    return true;  // No range specified or different type
}

bool ConfigValidator::validateValueLength(const ConfigValue& value, const ConfigSchema& schema) {
    if (schema.minLength && schema.maxLength) {
        size_t length = getValueLength(value);
        return length >= *schema.minLength && length <= *schema.maxLength;
    } else if (schema.minLength) {
        size_t length = getValueLength(value);
        return length >= *schema.minLength;
    } else if (schema.maxLength) {
        size_t length = getValueLength(value);
        return length <= *schema.maxLength;
    }
    
    return true;
}

size_t ConfigValidator::getValueLength(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value).size();
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value).size();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value).size();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value).size();
    }
    
    return 0;
}

} // namespace nlm
