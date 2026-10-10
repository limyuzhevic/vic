// Enhanced configuration schema and validation system for NLM
#pragma once

#include <string>
#include <variant>
#include <optional>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

// Configuration error types
enum class ConfigErrorType {
    None,
    InvalidType,
    OutOfRange,
    MissingDependency,
    ConflictingValues,
    InvalidValue,
    FileNotFound,
    ParseError,
    SchemaError
};

// Structured configuration error
class ConfigError {
public:
    ConfigError(ConfigErrorType type, const std::string& message, 
                const std::string& key = "", const std::string& suggestion = "")
        : type(type), message(message), key(key), suggestion(suggestion) {}
    
    ConfigErrorType type;
    std::string message;
    std::string key;
    std::string suggestion;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Error[" << static_cast<int>(type) << "]";
        if (!key.empty()) ss << " key:'" << key << "'";
        ss << ": " << message;
        if (!suggestion.empty()) ss << " (Suggestion: " << suggestion << ")";
        return ss.str();
    }
};

// Configuration value types with enhanced type safety
enum class ConfigType {
    None,
    Integer,
    Integer64,
    Float,
    Boolean,
    String,
    StringList,
    IntList,
    FloatList
};

// Configuration parameter definition
struct ConfigParameter {
    std::string key;
    ConfigType type;
    std::string description;
    std::string category;
    
    // Default values
    std::optional<int> intDefault;
    std::optional<int64_t> int64Default;
    std::optional<double> floatDefault;
    std::optional<bool> boolDefault;
    std::optional<std::string> stringDefault;
    std::optional<std::vector<std::string>> stringListDefault;
    std::optional<std::vector<int>> intListDefault;
    std::optional<std::vector<double>> floatListDefault;
    
    // Validation ranges
    std::optional<std::pair<double, double>> floatRange;
    std::optional<std::pair<int64_t, int64_t>> int64Range;
    std::optional<std::pair<int, int>> intRange;
    
    // Validation flags
    bool required = false;
    bool positive = false;
    bool nonNegative = false;
    bool inRange = false;
    
    // Dependency validation
    std::vector<std::string> dependencies;
    std::vector<std::string> exclusiveKeys;
    
    // Suggestions for invalid values
    std::vector<std::string> validValues;
    
    // Profile-based defaults
    std::map<std::string, std::variant<int, double, bool, std::string>> profileDefaults;
    
    ConfigParameter(const std::string& k, ConfigType t, const std::string& desc, const std::string& cat = "General")
        : key(k), type(t), description(desc), category(cat) {}
    
    template<typename T>
    ConfigParameter& defaultValue(const T& val) {
        switch (type) {
            case ConfigType::Integer: intDefault = val; break;
            case ConfigType::Integer64: int64Default = val; break;
            case ConfigType::Float: floatDefault = val; break;
            case ConfigType::Boolean: boolDefault = val; break;
            case ConfigType::String: stringDefault = val; break;
            case ConfigType::StringList: stringListDefault = val; break;
            case ConfigType::IntList: intListDefault = val; break;
            case ConfigType::FloatList: floatListDefault = val; break;
            default: break;
        }
        return *this;
    }
    
    ConfigParameter& range(double min, double max) {
        floatRange = std::make_pair(min, max);
        inRange = true;
        return *this;
    }
    
    ConfigParameter& range(int64_t min, int64_t max) {
        int64Range = std::make_pair(min, max);
        inRange = true;
        return *this;
    }
    
    ConfigParameter& range(int min, int max) {
        intRange = std::make_pair(min, max);
        inRange = true;
        return *this;
    }
    
    ConfigParameter& required(bool req = true) {
        required = req;
        return *this;
    }
    
    ConfigParameter& positive(bool pos = true) {
        positive = pos;
        return *this;
    }
    
    ConfigParameter& nonNegative(bool nn = true) {
        nonNegative = nn;
        return *this;
    }
    
    ConfigParameter& addDependency(const std::string& dep) {
        dependencies.push_back(dep);
        return *this;
    }
    
    ConfigParameter& addExclusiveKey(const std::string& excl) {
        exclusiveKeys.push_back(excl);
        return *this;
    }
    
    ConfigParameter& addValidValue(const std::string& val) {
        validValues.push_back(val);
        return *this;
    }
    
    ConfigParameter& setProfileDefault(const std::string& profile, const std::variant<int, double, bool, std::string>& val) {
        profileDefaults[profile] = val;
        return *this;
    }
};

// Enhanced configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    ConfigSchema& addParameter(const ConfigParameter& param) {
        parameters[param.key] = param;
        return *this;
    }
    
    bool hasParameter(const std::string& key) const {
        return parameters.find(key) != parameters.end();
    }
    
    const ConfigParameter& getParameter(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) return it->second;
        static const ConfigParameter empty("", ConfigType::None, "");
        return empty;
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : parameters) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::vector<std::string> getKeysByCategory(const std::string& category) const {
        std::vector<std::string> keys;
        for (const auto& pair : parameters) {
            if (pair.second.category == category) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }
    
    std::string getCategory(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) return it->second.category;
        return "General";
    }
    
    std::string getDescription(const std::string& key) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) return it->second.description;
        return "";
    }
    
    bool validateParameter(const std::string& key, const ConfigValue& value) const {
        auto it = parameters.find(key);
        if (it == parameters.end()) return false;
        
        const auto& param = it->second;
        return validateParameter(param, value);
    }
    
    std::vector<ConfigError> validateParameter(const ConfigParameter& param, const ConfigValue& value) const {
        std::vector<ConfigError> errors;
        
        if (!valueValid(param, value)) {
            errors.emplace_back(ConfigErrorType::InvalidType, 
                             "Value type does not match parameter type", 
                             param.key, "Try using a value of the correct type");
        }
        
        if (valueInvalid(param, value)) {
            errors.emplace_back(ConfigErrorType::OutOfRange,
                             "Value is out of valid range",
                             param.key, "Try a value within the specified range");
        }
        
        return errors;
    }
    
    std::vector<ConfigError> validateDependencies(const std::map<std::string, ConfigValue>& values) const {
        std::vector<ConfigError> errors;
        
        for (const auto& pair : parameters) {
            const auto& param = pair.second;
            
            // Check dependencies
            for (const auto& dep : param.dependencies) {
                if (values.find(dep) == values.end()) {
                    errors.emplace_back(ConfigErrorType::MissingDependency,
                                     "Required dependency '" + dep + "' is missing",
                                     param.key, "Set dependency '" + dep + "' first");
                }
            }
            
            // Check exclusive keys
            for (const auto& excl : param.exclusiveKeys) {
                if (values.find(excl) != values.end()) {
                    errors.emplace_back(ConfigErrorType::ConflictingValues,
                                     "Key '" + param.key + "' cannot be used with '" + excl + "'",
                                     param.key, "Remove one of the conflicting keys");
                }
            }
        }
        
        return errors;
    }
    
    std::vector<ConfigError> validateAll(const std::map<std::string, ConfigValue>& values) const {
        std::vector<ConfigError> errors;
        
        // Validate each provided parameter
        for (const auto& pair : values) {
            const auto& param = parameters.find(pair.first);
            if (param != parameters.end()) {
                auto paramErrors = validateParameter(param->second, pair.second);
                errors.insert(errors.end(), paramErrors.begin(), paramErrors.end());
            }
        }
        
        // Validate dependencies
        auto depErrors = validateDependencies(values);
        errors.insert(errors.end(), depErrors.begin(), depErrors.end());
        
        return errors;
    }
    
    std::string getDefaultValueString(const std::string& key) const {
        auto it = parameters.find(key);
        if (it == parameters.end()) return "";
        
        const auto& param = it->second;
        std::stringstream ss;
        
        switch (param.type) {
            case ConfigType::Integer:
                if (param.intDefault.has_value()) ss << param.intDefault.value();
                break;
            case ConfigType::Integer64:
                if (param.int64Default.has_value()) ss << param.int64Default.value();
                break;
            case ConfigType::Float:
                if (param.floatDefault.has_value()) ss << param.floatDefault.value();
                break;
            case ConfigType::Boolean:
                if (param.boolDefault.has_value()) ss << (param.boolDefault.value() ? "true" : "false");
                break;
            case ConfigType::String:
                if (param.stringDefault.has_value()) ss << '"' << param.stringDefault.value() << '"';
                break;
            case ConfigType::StringList:
                if (param.stringListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.stringListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << '"' << param.stringListDefault.value()[i] << '"';
                    }
                    ss << "]";
                }
                break;
            case ConfigType::IntList:
                if (param.intListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.intListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << param.intListDefault.value()[i];
                    }
                    ss << "]";
                }
                break;
            case ConfigType::FloatList:
                if (param.floatListDefault.has_value()) {
                    ss << "[";
                    for (size_t i = 0; i < param.floatListDefault.value().size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << param.floatListDefault.value()[i];
                    }
                    ss << "]";
                }
                break;
            default:
                break;
        }
        
        return ss.str();
    }
    
    std::string getValidationHint(const std::string& key) const {
        auto it = parameters.find(key);
        if (it == parameters.end()) return "";
        
        const auto& param = it->second;
        std::stringstream ss;
        ss << "Type: ";
        
        switch (param.type) {
            case ConfigType::Integer: ss << "integer"; break;
            case ConfigType::Integer64: ss << "64-bit integer"; break;
            case ConfigType::Float: ss << "floating point"; break;
            case ConfigType::Boolean: ss << "boolean"; break;
            case ConfigType::String: ss << "string"; break;
            case ConfigType::StringList: ss << "string list"; break;
            case ConfigType::IntList: ss << "integer list"; break;
            case ConfigType::FloatList: ss << "float list"; break;
            default: ss << "unknown"; break;
        }
        
        if (param.inRange) {
            ss << ", Range: ";
            if (param.floatRange.has_value()) {
                ss << param.floatRange->first << " - " << param.floatRange->second;
            } else if (param.int64Range.has_value()) {
                ss << param.int64Range->first << " - " << param.int64Range->second;
            } else if (param.intRange.has_value()) {
                ss << param.intRange->first << " - " << param.intRange->second;
            }
        }
        
        if (param.positive) ss << ", Must be positive";
        if (param.nonNegative) ss << ", Must be non-negative";
        if (param.required) ss << ", Required";
        
        if (!param.validValues.empty()) {
            ss << ", Valid values: ";
            for (size_t i = 0; i < param.validValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << param.validValues[i];
            }
        }
        
        return ss.str();
    }
    
    std::string getProfileOptions() const {
        std::set<std::string> profiles;
        for (const auto& pair : parameters) {
            for (const auto& profilePair : pair.second.profileDefaults) {
                profiles.insert(profilePair.first);
            }
        }
        
        if (profiles.empty()) return "No profile-specific defaults defined";
        
        std::stringstream ss;
        ss << "Available profiles: ";
        for (size_t i = 0; i < profiles.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << '"' << *profiles.begin() << '"';
            profiles.erase(profiles.begin());
        }
        
        return ss.str();
    }
    
private:
    bool valueValid(const ConfigParameter& param, const ConfigValue& value) const {
        try {
            switch (param.type) {
                case ConfigType::Integer:
                    return std::holds_alternative<int>(value);
                case ConfigType::Integer64:
                    return std::holds_alternative<int64_t>(value);
                case ConfigType::Float:
                    return std::holds_alternative<double>(value);
                case ConfigType::Boolean:
                    return std::holds_alternative<bool>(value);
                case ConfigType::String:
                    return std::holds_alternative<std::string>(value);
                case ConfigType::StringList:
                    return std::holds_alternative<std::vector<std::string>>(value);
                case ConfigType::IntList:
                    return std::holds_alternative<std::vector<int>>(value);
                case ConfigType::FloatList:
                    return std::holds_alternative<std::vector<double>>(value);
                default:
                    return false;
            }
        } catch (...) {
            return false;
        }
    }
    
    bool valueInvalid(const ConfigParameter& param, const ConfigValue& value) const {
        if (!valueValid(param, value)) return true;
        
        try {
            switch (param.type) {
                case ConfigType::Float:
                    if (param.inRange && std::holds_alternative<double>(value)) {
                        double val = std::get<double>(value);
                        if (param.floatRange.has_value()) {
                            if (val < param.floatRange->first || val > param.floatRange->second) return true;
                        }
                    }
                    break;
                case ConfigType::Integer64:
                    if (param.inRange && std::holds_alternative<int64_t>(value)) {
                        int64_t val = std::get<int64_t>(value);
                        if (param.int64Range.has_value()) {
                            if (val < param.int64Range->first || val > param.int64Range->second) return true;
                        }
                    }
                    break;
                case ConfigType::Integer:
                    if (param.inRange && std::holds_alternative<int>(value)) {
                        int val = std::get<int>(value);
                        if (param.intRange.has_value()) {
                            if (val < param.intRange->first || val > param.intRange->second) return true;
                        }
                    }
                    break;
                case ConfigType::Boolean:
                    if (param.positive && std::holds_alternative<bool>(value)) {
                        return false; // Booleans are always valid for positivity check
                    }
                    break;
                default:
                    break;
            }
            
            if (param.positive && valueValid(param, value)) {
                if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) return true;
                if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < 0) return true;
                if (std::holds_alternative<double>(value) && std::get<double>(value) < 0) return true;
            }
            
            if (param.nonNegative && valueValid(param, value)) {
                if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) return true;
                if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < 0) return true;
                if (std::holds_alternative<double>(value) && std::get<double>(value) < 0) return true;
            }
            
        } catch (...) {
            return true;
        }
        
        return false;
    }
    
    std::map<std::string, ConfigParameter> parameters;
};

// Configuration profiles
enum class ConfigProfile {
    None,
    Research,
    Simulation,
    Production,
    Development,
    Benchmark
};

// Enhanced configuration help system
class ConfigHelp {
public:
    static std::string getParameterHelp(const std::string& key, const ConfigSchema& schema) {
        const auto& param = schema.getParameter(key);
        if (param.key.empty()) return "Unknown parameter: " + key;
        
        std::stringstream ss;
        ss << "Parameter: " << key << "\n";
        ss << "Type: ";
        
        switch (param.type) {
            case ConfigType::Integer: ss << "integer"; break;
            case ConfigType::Integer64: ss << "64-bit integer"; break;
            case ConfigType::Float: ss << "floating point"; break;
            case ConfigType::Boolean: ss << "boolean"; break;
            case ConfigType::String: ss << "string"; break;
            case ConfigType::StringList: ss << "string list"; break;
            case ConfigType::IntList: ss << "integer list"; break;
            case ConfigType::FloatList: ss << "float list"; break;
            default: ss << "unknown"; break;
        }
        
        ss << "\nDescription: " << param.description << "\n";
        ss << "Category: " << param.category << "\n";
        ss << "Default: " << schema.getDefaultValueString(key) << "\n";
        ss << "Validation: " << schema.getValidationHint(key) << "\n";
        
        if (!param.dependencies.empty()) {
            ss << "Dependencies: ";
            for (size_t i = 0; i < param.dependencies.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << param.dependencies[i];
            }
            ss << "\n";
        }
        
        if (!param.exclusiveKeys.empty()) {
            ss << "Exclusive with: ";
            for (size_t i = 0; i < param.exclusiveKeys.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << param.exclusiveKeys[i];
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    static std::string getUsageExample(const std::string& key, const ConfigSchema& schema) {
        const auto& param = schema.getParameter(key);
        std::stringstream ss;
        ss << "# Example usage of " << key << "\n";
        
        switch (param.type) {
            case ConfigType::Integer:
                ss << key << " = 42";
                break;
            case ConfigType::Float:
                ss << key << " = 3.14";
                break;
            case ConfigType::Boolean:
                ss << key << " = true";
                break;
            case ConfigType::String:
                ss << key << " = \"example\"";
                break;
            case ConfigType::StringList:
                ss << key << " = [\"item1\", \"item2\", \"item3\"]";
                break;
            case ConfigType::IntList:
                ss << key << " = [1, 2, 3, 4, 5]";
                break;
            case ConfigType::FloatList:
                ss << key << " = [1.5, 2.5, 3.5]";
                break;
            default:
                ss << "# Configure " << key << " here";
        }
        
        return ss.str();
    }
    
    static std::string getCategoryHelp(const std::string& category, const ConfigSchema& schema) {
        std::stringstream ss;
        ss << "=== " << category << " Parameters ===\n\n";
        
        auto keys = schema.getKeysByCategory(category);
        for (const auto& key : keys) {
            ss << "  " << key << ": " << schema.getParameter(key).description << "\n";
        }
        
        return ss.str();
    }
    
    static std::string getAllHelp(const ConfigSchema& schema) {
        std::stringstream ss;
        ss << "NLM Configuration Help\n";
        ss << "=======================\n\n";
        
        auto keys = schema.getKeys();
        std::map<std::string, std::vector<std::string>> categories;
        
        for (const auto& key : keys) {
            const auto& param = schema.getParameter(key);
            categories[param.category].push_back(key);
        }
        
        for (const auto& pair : categories) {
            ss << ConfigHelp::getCategoryHelp(pair.first, schema);
            ss << "\n";
        }
        
        ss << "\nExample config file:\n";
        ss << "# Save this as config.cfg and load with Config::loadFromFile()\n";
        for (const auto& key : keys) {
            ss << key << " = " << schema.getDefaultValueString(key) << "\n";
        }
        
        return ss.str();
    }
};

} // namespace nlm