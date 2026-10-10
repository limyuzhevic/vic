#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <filesystem>
#include <chrono>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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
    Environment,
    Runtime
};

// Configuration parameter type
enum class ParamType {
    Integer,
    Integer64,
    Float,
    Boolean,
    String,
    StringList,
    IntList,
    FloatList
};

// Parameter validation rules
struct ParamValidation {
    std::string name;
    ParamType type;
    std::optional<double> min;
    std::optional<double> max;
    std::optional<size_t> minSize;
    std::optional<size_t> maxSize;
    std::optional<std::string> pattern;
    std::string description;
    bool required;
    std::string defaultValue;
    std::function<bool(const ConfigValue&)> customValidator;
    
    ParamValidation(const std::string& n, ParamType t, const std::string& desc = "")
        : name(n), type(t), description(desc), required(false) {}
    
    bool validate(const ConfigValue& value) const;
    std::string typeToString() const;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
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
    
    // Load from file (JSON/YAML format with auto-detection)
    bool loadFromFile(const std::string& filepath);
    bool loadFromFile(const std::filesystem::path& filepath);
    
    // Save to file (JSON format by default)
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    bool saveToFile(const std::filesystem::path& filepath, bool pretty = false) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration management
    std::vector<std::string> getConfigFiles() const { return configFiles; }
    bool addConfigFile(const std::string& filepath);
    bool removeConfigFile(const std::string& filepath);
    void clearConfigFiles();
    
    // Configuration validation
    bool validate() const;
    std::string validateAndGetErrors() const;
    
    // Cross-parameter validation
    bool validateCrossParameters() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Configuration backup
    bool backup(const std::string& backupPath = "") const;
    bool restoreFromBackup(const std::string& backupPath = "") const;
    
    // Configuration templates
    static Config loadTemplate(const std::string& templateName);
    static bool saveTemplate(const std::string& templateName, const Config& config);
    static std::vector<std::string> getAvailableTemplates();
    
    // Configuration migration
    bool migrateFromOldFormat(const std::string& oldFormatFile);
    
    // Parameter management
    void registerParameter(const std::string& key, const ParamValidation& param);
    std::optional<ParamValidation> getParameterInfo(const std::string& key) const;
    std::vector<std::string> getRegisteredParameterKeys() const;
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getFileEntryCount(const std::string& filepath) const;
    
    // Configuration logging
    void enableChangeLogging(const std::string& logFile = "");
    void disableChangeLogging();
    bool isChangeLoggingEnabled() const;
    
    // Environment variable support
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Configuration schema
    nlohmann::json exportToSchema() const;
    bool importFromSchema(const nlohmann::json& schema);
    
    // Format detection and conversion
    static std::string detectFormat(const std::string& filepath);
    static bool convertToFormat(const std::string& fromPath, const std::string& toPath);
    
    // Debug information
    std::string getDebugInfo() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getTimestampString(const std::chrono::system_clock::time_point& tp);
    static std::string formatValue(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<std::string> configFiles;
    bool changeLoggingEnabled;
    std::string changeLogFile;
    std::map<std::string, ParamValidation> registeredParameters;
    
    bool parseJSON(const std::string& content, ConfigSource source);
    bool parseYAML(const std::string& content, ConfigSource source);
    bool parseLegacy(const std::string& content, ConfigSource source);
    nlohmann::json exportToJSON() const;
    void importFromJSON(const nlohmann::json& data, ConfigSource source);
    bool isValueValid(const ConfigEntry& entry) const;
    std::string getTypeErrorMessage(const ParamValidation& param, const ConfigValue& value) const;
    static ParamType inferTypeFromString(const std::string& valueStr);
};

} // namespace nlm

// Template implementations
namespace nlm {

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline bool ParamValidation::validate(const ConfigValue& value) const {
    // Type validation
    if (type == ParamType::Integer) {
        if (!std::holds_alternative<int>(value)) return false;
    } else if (type == ParamType::Integer64) {
        if (!std::holds_alternative<int64_t>(value)) return false;
    } else if (type == ParamType::Float) {
        if (!std::holds_alternative<double>(value)) return false;
    } else if (type == ParamType::Boolean) {
        if (!std::holds_alternative<bool>(value)) return false;
    } else if (type == ParamType::String) {
        if (!std::holds_alternative<std::string>(value)) return false;
    } else if (type == ParamType::StringList) {
        if (!std::holds_alternative<std::vector<std::string>>(value)) return false;
    } else if (type == ParamType::IntList) {
        if (!std::holds_alternative<std::vector<int>>(value)) return false;
    } else if (type == ParamType::FloatList) {
        if (!std::holds_alternative<std::vector<double>>(value)) return false;
    }
    
    // Range validation
    if (min.has_value() || max.has_value()) {
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = static_cast<double>(std::get<int>(value));
        else if (std::holds_alternative<int64_t>(value)) val = static_cast<double>(std::get<int64_t>(value));
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        else if (std::holds_alternative<bool>(value)) val = std::get<bool>(value) ? 1.0 : 0.0;
        
        if (min.has_value() && val < min.value()) return false;
        if (max.has_value() && val > max.value()) return false;
    }
    
    // Size validation
    if (minSize.has_value() || maxSize.has_value()) {
        size_t size = 0;
        if (std::holds_alternative<std::string>(value)) size = std::get<std::string>(value).size();
        else if (std::holds_alternative<std::vector<int>>(value)) size = std::get<std::vector<int>>(value).size();
        else if (std::holds_alternative<std::vector<double>>(value)) size = std::get<std::vector<double>>(value).size();
        else if (std::holds_alternative<std::vector<std::string>>(value)) size = std::get<std::vector<std::string>>(value).size();
        
        if (minSize.has_value() && size < minSize.value()) return false;
        if (maxSize.has_value() && size > maxSize.value()) return false;
    }
    
    // Custom validation
    if (customValidator && !customValidator(value)) return false;
    
    return true;
}

inline std::string ParamValidation::typeToString() const {
    switch (type) {
        case ParamType::Integer: return "integer";
        case ParamType::Integer64: return "integer64";
        case ParamType::Float: return "float";
        case ParamType::Boolean: return "boolean";
        case ParamType::String: return "string";
        case ParamType::StringList: return "string_list";
        case ParamType::IntList: return "int_list";
        case ParamType::FloatList: return "float_list";
        default: return "unknown";
    }
}

} // namespace nlm
