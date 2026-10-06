#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <regex>
#include <algorithm>
#include <type_traits>
#include <any>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset
};

// Configuration exception hierarchy
class ConfigurationException : public std::runtime_error {
public:
    explicit ConfigurationException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigurationException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigurationException(message) {}
};

class ConfigSchemaException : public ConfigurationException {
public:
    explicit ConfigSchemaException(const std::string& message)
        : ConfigurationException(message) {}
};

class ConfigLoadException : public ConfigurationException {
public:
    explicit ConfigLoadException(const std::string& message)
        : ConfigurationException(message) {}
};

// Configuration parameter metadata
struct ConfigParameterInfo {
    std::string key;
    std::string type;
    std::string description;
    std::any defaultValue;
    bool required = false;
    bool readonly = false;
    
    // Constraints
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<std::string> pattern;
    std::optional<std::vector<std::any>> allowedValues;
    
    // Metadata
    std::string category;
    std::string environment; // dev, test, prod
    int order = 0;
    
    ConfigParameterInfo() = default;
    ConfigParameterInfo(const std::string& k, const std::string& t, 
                       const std::string& desc, const std::any& def,
                       bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), 
          required(req), readonly(false), order(0) {}
};

// Configuration validation rule
struct ValidationRule {
    enum class Type {
        Range,
        Pattern,
        AllowedValues,
        Custom
    };
    
    Type type;
    std::string message;
    std::any value1; // min/max for range, pattern string, etc.
    std::any value2; // max for range
    std::function<bool(const ConfigValue&)> customValidator;
    
    ValidationRule(Type t, const std::string& msg, const std::any& v1,
                   const std::any& v2 = std::any()) 
        : type(t), message(msg), value1(v1), value2(v2), customValidator(nullptr) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationRule> validationRules;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::vector<ValidationRule>& rules, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRules(rules) {}
};

// Configuration parameter definition
class ConfigParameter {
public:
    enum class Type {
        Int,
        Int64,
        Double,
        Bool,
        String,
        IntVector,
        DoubleVector,
        StringVector
    };
    
private:
    std::string key_;
    Type type_;
    ConfigParameterInfo info_;
    ConfigSource source_ = ConfigSource::Default;
    
public:
    ConfigParameter(const std::string& key, Type type, const std::string& description,
                   const std::any& defaultValue = std::any(), bool required = false)
        : key_(key), type_(type) {
        info_.key = key;
        info_.type = typeToString(type);
        info_.description = description;
        info_.defaultValue = defaultValue;
        info_.required = required;
    }
    
    // Type conversion helpers
    template<typename T>
    static Type getType() {
        if constexpr (std::is_same_v<T, int>) return Type::Int;
        else if constexpr (std::is_same_v<T, int64_t>) return Type::Int64;
        else if constexpr (std::is_same_v<T, double>) return Type::Double;
        else if constexpr (std::is_same_v<T, bool>) return Type::Bool;
        else if constexpr (std::is_same_v<T, std::string>) return Type::String;
        else if constexpr (std::is_same_v<T, std::vector<int>>) return Type::IntVector;
        else if constexpr (std::is_same_v<T, std::vector<double>>) return Type::DoubleVector;
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return Type::StringVector;
        else throw std::runtime_error("Unsupported type");
    }
    
    static std::string typeToString(Type type) {
        switch (type) {
            case Type::Int: return "int";
            case Type::Int64: return "int64";
            case Type::Double: return "double";
            case Type::Bool: return "bool";
            case Type::String: return "string";
            case Type::IntVector: return "vector<int>";
            case Type::DoubleVector: return "vector<double>";
            case Type::StringVector: return "vector<string>";
            default: return "unknown";
        }
    }
    
    const std::string& getKey() const { return key_; }
    Type getType() const { return type_; }
    const ConfigParameterInfo& getInfo() const { return info_; }
    ConfigSource getSource() const { return source_; }
    
    void setSource(ConfigSource source) { source_ = source; }
    
    // Validation
    bool validate(const ConfigValue& value) const {
        // Basic type checking
        if (!isValidType(value)) return false;
        
        // Check constraints
        if (info_.minValue.has_value() && !checkMin(value)) return false;
        if (info_.maxValue.has_value() && !checkMax(value)) return false;
        if (info_.pattern.has_value() && !checkPattern(value)) return false;
        if (info_.allowedValues.has_value() && !checkAllowedValues(value)) return false;
        
        return true;
    }
    
    const std::vector<ValidationRule>& getValidationRules() const {
        return info_.validationRules;
    }
    
    void addValidationRule(const ValidationRule& rule) {
        info_.validationRules.push_back(rule);
    }
    
    void setConstraint(const std::string& constraint, const std::any& value) {
        if (constraint == "min") info_.minValue = std::any_cast<double>(value);
        else if (constraint == "max") info_.maxValue = std::any_cast<double>(value);
        else if (constraint == "pattern") {
            if (value.type() == typeid(std::string)) {
                info_.pattern = std::any_cast<std::string>(value);
            }
        }
    }
    
    bool isRequired() const { return info_.required; }
    bool isReadonly() const { return info_.readonly; }
    
    std::any getDefaultValue() const { return info_.defaultValue; }
    
    template<typename T>
    T getDefault() const {
        if constexpr (std::is_same_v<T, int>) {
            return std::any_cast<int>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::any_cast<int64_t>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::any_cast<double>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::any_cast<bool>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::any_cast<std::string>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return std::any_cast<std::vector<int>>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::any_cast<std::vector<double>>(info_.defaultValue);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::any_cast<std::vector<std::string>>(info_.defaultValue);
        }
    }
    
private:
    bool isValidType(const ConfigValue& value) const {
        switch (type_) {
            case Type::Int: return std::holds_alternative<int>(value);
            case Type::Int64: return std::holds_alternative<int64_t>(value);
            case Type::Double: return std::holds_alternative<double>(value);
            case Type::Bool: return std::holds_alternative<bool>(value);
            case Type::String: return std::holds_alternative<std::string>(value);
            case Type::IntVector: return std::holds_alternative<std::vector<int>>(value);
            case Type::DoubleVector: return std::holds_alternative<std::vector<double>>(value);
            case Type::StringVector: return std::holds_alternative<std::vector<std::string>>(value);
            default: return false;
        }
    }
    
    bool checkMin(const ConfigValue& value) const {
        double val = getAsDouble(value);
        return val >= info_.minValue.value();
    }
    
    bool checkMax(const ConfigValue& value) const {
        double val = getAsDouble(value);
        return val <= info_.maxValue.value();
    }
    
    bool checkPattern(const ConfigValue& value) const {
        if (!std::holds_alternative<std::string>(value)) return false;
        const std::string& str = std::get<std::string>(value);
        return std::regex_match(str, std::regex(info_.pattern.value()));
    }
    
    bool checkAllowedValues(const ConfigValue& value) const {
        return std::find(info_.allowedValues.value().begin(), 
                        info_.allowedValues.value().end(), value) != 
               info_.allowedValues.value().end();
    }
    
    double getAsDouble(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return std::get<int>(value);
        if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value);
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        throw std::runtime_error("Cannot convert to double");
    }
};

// Configuration validator
class ConfigValidator {
public:
    static bool validateValue(const ConfigValue& value, const ConfigParameterInfo& paramInfo) {
        // Type validation
        if (!isValidType(value, paramInfo.type)) return false;
        
        // Range validation
        if (paramInfo.minValue.has_value() && !checkRange(value, paramInfo.minValue.value(), 
                                                           paramInfo.maxValue.value_or(0.0), "min")) return false;
        if (paramInfo.maxValue.has_value() && !checkRange(value, paramInfo.minValue.value_or(0.0), 
                                                           paramInfo.maxValue.value(), "max")) return false;
        
        // Pattern validation
        if (paramInfo.pattern.has_value() && !checkPattern(value, paramInfo.pattern.value())) return false;
        
        // Allowed values validation
        if (paramInfo.allowedValues.has_value() && 
            !checkAllowedValues(value, paramInfo.allowedValues.value())) return false;
        
        return true;
    }
    
    static std::vector<std::string> validateEntry(const ConfigEntry& entry, 
                                                 const ConfigParameterInfo& paramInfo) {
        std::vector<std::string> errors;
        
        // Check required
        if (paramInfo.required && entry.value.index() == 0) {
            errors.push_back("Required parameter '" + entry.key + "' is not set");
        }
        
        // Check type
        if (!isValidType(entry.value, paramInfo.type)) {
            errors.push_back("Invalid type for parameter '" + entry.key + 
                           "': expected " + paramInfo.type + ", got " + valueTypeToString(entry.value));
        }
        
        // Check constraints
        double val = getAsDouble(entry.value);
        if (paramInfo.minValue.has_value() && val < paramInfo.minValue.value()) {
            errors.push_back("Parameter '" + entry.key + "' value " + std::to_string(val) + 
                           " is less than minimum " + std::to_string(paramInfo.minValue.value()));
        }
        if (paramInfo.maxValue.has_value() && val > paramInfo.maxValue.value()) {
            errors.push_back("Parameter '" + entry.key + "' value " + std::to_string(val) + 
                           " exceeds maximum " + std::to_string(paramInfo.maxValue.value()));
        }
        
        if (paramInfo.pattern.has_value() && !checkPattern(entry.value, paramInfo.pattern.value())) {
            errors.push_back("Parameter '" + entry.key + "' value '" + 
                           getStringValue(entry.value) + "' does not match pattern " + 
                           paramInfo.pattern.value());
        }
        
        return errors;
    }
    
    static bool isValidType(const ConfigValue& value, const std::string& type) {
        if (type == "int") return std::holds_alternative<int>(value);
        if (type == "int64") return std::holds_alternative<int64_t>(value);
        if (type == "double") return std::holds_alternative<double>(value);
        if (type == "bool") return std::holds_alternative<bool>(value);
        if (type == "string") return std::holds_alternative<std::string>(value);
        if (type == "vector<int>") return std::holds_alternative<std::vector<int>>(value);
        if (type == "vector<double>") return std::holds_alternative<std::vector<double>>(value);
        if (type == "vector<string>") return std::holds_alternative<std::vector<std::string>>(value);
        return false;
    }
    
    static std::string valueTypeToString(const ConfigValue& value) {
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
    
    static std::string getStringValue(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        if (std::holds_alternative<std::vector<int>>(value)) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < std::get<std::vector<int>>(value).size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::get<std::vector<int>>(value)[i];
            }
            oss << "]";
            return oss.str();
        }
        if (std::holds_alternative<std::vector<double>>(value)) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < std::get<std::vector<double>>(value).size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::get<std::vector<double>>(value)[i];
            }
            oss << "]";
            return oss.str();
        }
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < std::get<std::vector<std::string>>(value).size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << std::get<std::vector<std::string>>(value)[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
        return "";
    }
    
    static double getAsDouble(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return std::get<int>(value);
        if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value);
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        throw std::runtime_error("Cannot convert to double");
    }
    
    static bool checkRange(const ConfigValue& value, double min, double max, 
                          const std::string& type) {
        double val = getAsDouble(value);
        if (type == "min") return val >= min;
        if (type == "max") return val <= max;
        return val >= min && val <= max;
    }
    
    static bool checkPattern(const ConfigValue& value, const std::string& pattern) {
        if (!std::holds_alternative<std::string>(value)) return false;
        const std::string& str = std::get<std::string>(value);
        return std::regex_match(str, std::regex(pattern));
    }
    
    static bool checkAllowedValues(const ConfigValue& value, 
                                  const std::vector<std::any>& allowed) {
        return std::find(allowed.begin(), allowed.end(), value) != allowed.end();
    }
};

// Configuration schema
class ConfigSchema {
private:
    std::vector<ConfigParameter> parameters_;
    std::vector<ConfigEnvironment> environments_;
    
public:
    ConfigSchema() = default;
    
    void addParameter(const ConfigParameter& param) {
        parameters_.push_back(param);
    }
    
    const std::vector<ConfigParameter>& getParameters() const { return parameters_; }
    
    const ConfigParameter* getParameter(const std::string& key) const {
        for (const auto& param : parameters_) {
            if (param.getKey() == key) return &param;
        }
        return nullptr;
    }
    
    bool validateSchema(const std::vector<ConfigEntry>& entries) const {
        for (const auto& entry : entries) {
            const ConfigParameter* param = getParameter(entry.key);
            if (!param) {
                // Parameter not defined in schema
                continue; // For now, allow undefined parameters
            }
            
            if (!ConfigValidator::validateEntry(entry, param->getInfo()).empty()) {
                return false;
            }
        }
        return true;
    }
    
    void setEnvironmentPreset(const std::string& environment) {
        // In a full implementation, this would set up environment-specific
        // default values and validation rules
    }
    
    std::vector<std::string> getValidationErrors(const std::vector<ConfigEntry>& entries) const {
        std::vector<std::string> errors;
        
        for (const auto& entry : entries) {
            const ConfigParameter* param = getParameter(entry.key);
            if (!param) continue;
            
            auto entryErrors = ConfigValidator::validateEntry(entry, param->getInfo());
            errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
        }
        
        return errors;
    }
};

// Configuration environment
class ConfigEnvironment {
private:
    std::string name_;
    std::map<std::string, std::any> defaults_;
    std::map<std::string, std::vector<ValidationRule>> validationRules_;
    
public:
    enum class Name {
        Development,
        Test,
        Staging,
        Production
    };
    
    ConfigEnvironment(const std::string& name) : name_(name) {}
    
    const std::string& getName() const { return name_; }
    
    void setDefault(const std::string& key, const std::any& value) {
        defaults_[key] = value;
    }
    
    std::any getDefault(const std::string& key) const {
        auto it = defaults_.find(key);
        if (it != defaults_.end()) return it->second;
        return std::any();
    }
    
    void addValidationRule(const std::string& key, const ValidationRule& rule) {
        validationRules_[key].push_back(rule);
    }
    
    const std::vector<ValidationRule>& getValidationRules(const std::string& key) const {
        auto it = validationRules_.find(key);
        if (it != validationRules_.end()) return it->second;
        static const std::vector<ValidationRule> empty;
        return empty;
    }
    
    static std::string nameToString(Name name) {
        switch (name) {
            case Name::Development: return "dev";
            case Name::Test: return "test";
            case Name::Staging: return "staging";
            case Name::Production: return "prod";
            default: return "unknown";
        }
    }
};

// Configuration builder for creating presets
class ConfigBuilder {
private:
    std::shared_ptr<Config> config_;
    
public:
    ConfigBuilder() : config_(std::make_shared<Config>()) {}
    
    ConfigBuilder& set(const std::string& key, int value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::vector<int>& value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::vector<double>& value) {
        config_->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, const std::vector<std::string>& value) {
        config_->set(key, value);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
};

// Enhanced configuration class with all new functionality
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Enhanced configuration methods
    bool validate() const;
    bool hasParameter(const std::string& key) const;
    ConfigParameterInfo getParameterInfo(const std::string& key) const;
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    std::shared_ptr<Config> clone() const;
    std::vector<std::string> diff(const Config& other) const;
    
    // Parameter management
    void addParameter(const ConfigParameter& param);
    void setEnvironment(const std::string& environment);
    const std::string& getEnvironment() const;
    void enableValidation(bool enabled);
    bool isValidationEnabled() const;
    
    // Serialization helpers
    std::vector<uint8_t> toBinary() const;
    static std::shared_ptr<Config> fromBinary(const std::vector<uint8_t>& data);
    
    // History tracking
    void addToHistory(const std::string& action, const std::string& description);
    std::vector<std::pair<std::string, std::string>> getHistory() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    ConfigValue getValue(const std::string& key) const;
}; // namespace nlm

} // namespace nlm
