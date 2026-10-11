#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

namespace nlm {

// JSON/YAML parsing exceptions
class ConfigParseError : public std::exception {
public:
    ConfigParseError(const std::string& message, const std::string& path = "")
        : message_("ConfigParseError: " + message + (path.empty() ? "" : " (at " + path + ")")) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

class ConfigValidationError : public std::exception {
public:
    ConfigValidationError(const std::string& field, const std::string& expected, const std::string& actual)
        : message_("ConfigValidationError: Field '" + field + "': expected '" + expected + "', got '" + actual + "'") {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

// Configuration schema definitions
class ConfigSchema {
public:
    struct FieldInfo {
        std::string name;
        std::string description;
        std::string type;  // "int", "double", "string", "bool", "array", "object"
        std::any defaultValue;
        bool required = false;
        std::string validationPattern;
        std::vector<std::string> allowedValues;
        double minValue = std::numeric_limits<double>::lowest();
        double maxValue = std::numeric_limits<double>::max();
        size_t minSize = 0;
        size_t maxSize = std::numeric_limits<size_t>::max();
    };
    
    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    void addField(const FieldInfo& field) {
        fields_[field.name] = field;
    }
    
    const FieldInfo* getField(const std::string& name) const {
        auto it = fields_.find(name);
        return it != fields_.end() ? &it->second : nullptr;
    }
    
    bool validate(const std::string& key, const ConfigValue& value) const {
        const FieldInfo* field = getField(key);
        if (!field) {
            return !strictMode_;  // Allow extra fields unless strict mode is on
        }
        
        // Type validation
        if (field->type == "int") {
            if (!std::holds_alternative<int>(value) && 
                !std::holds_alternative<int64_t>(value)) {
                return false;
            }
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                return val >= field->minValue && val <= field->maxValue;
            } else {
                int64_t val = std::get<int64_t>(value);
                return static_cast<double>(val) >= field->minValue && 
                       static_cast<double>(val) <= field->maxValue;
            }
        } else if (field->type == "double") {
            if (!std::holds_alternative<double>(value)) {
                return false;
            }
            double val = std::get<double>(value);
            return val >= field->minValue && val <= field->maxValue;
        } else if (field->type == "string") {
            if (!std::holds_alternative<std::string>(value)) {
                return false;
            }
            std::string val = std::get<std::string>(value);
            if (!field->validationPattern.empty()) {
                // Simple regex check - in production would use proper regex library
                if (field->validationPattern == "^[a-zA-Z0-9_]+$") {
                    for (char c : val) {
                        if (!isalnum(c) && c != '_') return false;
                    }
                }
            }
            if (!field->allowedValues.empty()) {
                return std::find(field->allowedValues.begin(), field->allowedValues.end(), val) != field->allowedValues.end();
            }
            return true;
        } else if (field->type == "bool") {
            if (!std::holds_alternative<bool>(value)) {
                return false;
            }
            return true;
        } else if (field->type == "array") {
            if (!std::holds_alternative<std::vector<int>>(value) &&
                !std::holds_alternative<std::vector<double>>(value) &&
                !std::holds_alternative<std::vector<std::string>>(value)) {
                return false;
            }
            size_t size = 0;
            if (std::holds_alternative<std::vector<int>>(value)) {
                size = std::get<std::vector<int>>(value).size();
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                size = std::get<std::vector<double>>(value).size();
            } else {
                size = std::get<std::vector<std::string>>(value).size();
            }
            return size >= field->minSize && size <= field->maxSize;
        }
        return true;  // Unknown type, accept any
    }
    
    std::string validateAll(const std::unordered_map<std::string, ConfigValue>& config) const {
        std::string errors;
        for (const auto& [key, value] : config) {
            if (!validate(key, value)) {
                const FieldInfo* field = getField(key);
                if (field) {
                    errors += "Invalid value for field '" + key + "'\n";
                } else if (strictMode_) {
                    errors += "Unexpected field '" + key + "'\n";
                }
            }
        }
        return errors;
    }
    
    void setStrictMode(bool strict) {
        strictMode_ = strict;
    }
    
private:
    std::unordered_map<std::string, FieldInfo> fields_;
    bool strictMode_ = true;
};

class ConfigSchemaBuilder {
public:
    static ConfigSchema::FieldInfo intField(const std::string& name, const std::string& description, 
                                           int defaultValue, bool required = false,
                                           double min = std::numeric_limits<double>::lowest(),
                                           double max = std::numeric_limits<double>::max()) {
        ConfigSchema::FieldInfo field;
        field.name = name;
        field.description = description;
        field.type = "int";
        field.defaultValue = defaultValue;
        field.required = required;
        field.minValue = min;
        field.maxValue = max;
        return field;
    }
    
    static ConfigSchema::FieldInfo doubleField(const std::string& name, const std::string& description,
                                               double defaultValue, bool required = false,
                                               double min = std::numeric_limits<double>::lowest(),
                                               double max = std::numeric_limits<double>::max()) {
        ConfigSchema::FieldInfo field;
        field.name = name;
        field.description = description;
        field.type = "double";
        field.defaultValue = defaultValue;
        field.required = required;
        field.minValue = min;
        field.maxValue = max;
        return field;
    }
    
    static ConfigSchema::FieldInfo stringField(const std::string& name, const std::string& description,
                                               const std::string& defaultValue, bool required = false,
                                               const std::string& pattern = "",
                                               const std::vector<std::string>& allowed = {}) {
        ConfigSchema::FieldInfo field;
        field.name = name;
        field.description = description;
        field.type = "string";
        field.defaultValue = defaultValue;
        field.required = required;
        field.validationPattern = pattern;
        field.allowedValues = allowed;
        return field;
    }
    
    static ConfigSchema::FieldInfo boolField(const std::string& name, const std::string& description,
                                            bool defaultValue, bool required = false) {
        ConfigSchema::FieldInfo field;
        field.name = name;
        field.description = description;
        field.type = "bool";
        field.defaultValue = defaultValue;
        field.required = required;
        return field;
    }
    
    static ConfigSchema::FieldInfo arrayField(const std::string& name, const std::string& description,
                                              const std::vector<int>& defaultValue,
                                              size_t minSize = 0, size_t maxSize = 1000) {
        ConfigSchema::FieldInfo field;
        field.name = name;
        field.description = description;
        field.type = "array";
        field.defaultValue = defaultValue;
        field.minSize = minSize;
        field.maxSize = maxSize;
        return field;
    }
};

} // namespace nlm
