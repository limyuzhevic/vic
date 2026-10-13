#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <regex>

namespace nlm {

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
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error("Configuration error" + (key.empty() ? "" : (" at key: " + key)) + ": " + message),
          key_(key) {}
    
    const std::string& key() const { return key_; }
    
private:
    std::string key_;
};

// Configuration schema definition
class ConfigSchema {
public:
    using ValidatorFunc = std::function<bool(const ConfigValue&)>;
    
    struct Field {
        std::string name;
        ConfigValue defaultValue;
        std::string description;
        ValidatorFunc validator;
        bool required;
        ConfigSource source;
    };
    
    void addField(const std::string& name, const ConfigValue& defaultValue,
                  const std::string& description = "",
                  ValidatorFunc validator = nullptr,
                  bool required = false,
                  ConfigSource source = ConfigSource::Default) {
        fields_[name] = Field{name, defaultValue, description, validator, required, source};
    }
    
    bool hasField(const std::string& name) const {
        return fields_.find(name) != fields_.end();
    }
    
    const Field* getField(const std::string& name) const {
        auto it = fields_.find(name);
        return it != fields_.end() ? &it->second : nullptr;
    }
    
    std::vector<std::string> getFieldNames() const {
        std::vector<std::string> names;
        names.reserve(fields_.size());
        for (const auto& pair : fields_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool validateAll(const Config& config) const {
        for (const auto& pair : fields_) {
            const auto& field = pair.second;
            if (!config.has(field.name)) {
                if (field.required) {
                    throw ConfigValidationError("Required field is missing", field.name);
                }
                continue;
            }
            
            auto value = config.getAny(field.name);
            if (field.validator && !field.validator(value)) {
                throw ConfigValidationError("Field validation failed", field.name);
            }
        }
        return true;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Schema (" << fields_.size() << " fields):\n";
        for (const auto& pair : fields_) {
            const auto& field = pair.second;
            oss << "  " << field.name << " = ";
            
            if (std::holds_alternative<int>(field.defaultValue)) {
                oss << std::get<int>(field.defaultValue);
            } else if (std::holds_alternative<int64_t>(field.defaultValue)) {
                oss << std::get<int64_t>(field.defaultValue);
            } else if (std::holds_alternative<double>(field.defaultValue)) {
                oss << std::get<double>(field.defaultValue);
            } else if (std::holds_alternative<bool>(field.defaultValue)) {
                oss << (std::get<bool>(field.defaultValue) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(field.defaultValue)) {
                oss << "\"" << std::get<std::string>(field.defaultValue) << "\"";
            }
            
            oss << " (" << (field.required ? "required" : "optional") << ")";
            if (!field.description.empty()) {
                oss << " - " << field.description;
            }
            oss << "\n";
        }
        return oss.str();
    }
    
private:
    std::unordered_map<std::string, Field> fields_;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool format = true) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    ConfigValue getAny(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation
    bool validate(const ConfigSchema& schema) const;
    
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
    
    // Get source info
    ConfigSource getSource(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigValue stringToValue(const std::string& str);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm
