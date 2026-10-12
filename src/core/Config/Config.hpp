#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

// Forward declarations
class Config;

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Enhanced configuration source enumeration
enum class ConfigSource {
    Default = 0,      // Default from code
    File = 1,         // From configuration file
    CommandLine = 2,  // From command line
    Runtime = 3,      // Set at runtime
    Environment = 4,  // From environment variable
    Schema = 5        // From schema validation
} ;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    ValidationResult(bool v, const std::vector<std::string>& errs, 
                     const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << (valid ? "VALID" : "INVALID");
        if (!warnings.empty()) {
            oss << " (warnings: " << warnings.size() << ")";
        }
        if (!errors.empty()) {
            oss << " (errors: " << errors.size() << ")";
        }
        return oss.str();
    }
};

// Configuration schema entry for validation
struct SchemaEntry {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "vector", "object"
    std::string description;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;  // For numeric types
    bool required = false;
    
    SchemaEntry() = default;
    
    SchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "",
                bool req = false)
        : key(k), type(t), description(desc), required(req) {}
    
    bool isNumeric() const {
        return type == "int" || type == "int64" || type == "double";
    }
    
    bool isBoolean() const {
        return type == "bool";
    }
    
    bool isString() const {
        return type == "string";
    }
    
    bool isVector() const {
        return type == "vector";
    }
    
    std::string typeToCpp() const {
        if (type == "int") return "int";
        if (type == "int64") return "int64_t";
        if (type == "double") return "double";
        if (type == "bool") return "bool";
        if (type == "string") return "std::string";
        if (type == "vector") return "std::vector<...>";
        return "std::string"; // Default to string
    }
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationKey;  // Reference to schema entry
    bool modified = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), validationKey(), modified(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationKey(), modified(false) {}
    
    // Helper methods for type-safe value access
    template<typename T>
    std::optional<T> getValue() const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    bool setValue(const T& val) {
        try {
            value = val;
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << key << " = ";
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
        oss << " (" << static_cast<int>(source) << ")";
        return oss.str();
    }
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&)>;

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Loading from different sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromJson(const std::string& jsonStr);
    bool loadFromJsonFile(const std::string& filepath);
    
    // Saving to different formats
    bool saveToFile(const std::string& filepath) const;
    bool saveToJson(const std::string& filepath) const;
    std::string toJsonString() const;
    std::string toPrettyString() const;
    
    // Configuration operations
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value,
             ConfigSource source = ConfigSource::Runtime);
    
    // Configuration access with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration metadata and validation
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Enhanced configuration summary
    std::string summary() const;
    std::string getSourceName(ConfigSource source) const;
    
    // Configuration management
    void clear();
    void merge(const Config& other);
    bool isModified() const;
    
    // Schema and validation
    void addSchemaEntry(const SchemaEntry& entry);
    bool validate() const;
    ValidationResult validateAgainstSchema() const;
    SchemaEntry* getSchemaEntry(const std::string& key);
    const SchemaEntry* getSchemaEntry(const std::string& key) const;
    
    // Callbacks and listeners
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Statistics and statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::string getStatistics() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJson(const std::string& str);
    static std::string unescapeJson(const std::string& str);
    
    // Error handling
    static std::string getLastError();
    static void clearLastError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string lastError;
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
    void setEntry(ConfigEntry& entry, const ConfigValue& value, ConfigSource source);
    void updateSchemaCache();
    bool isValidType(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateAgainstSchema(const std::vector<ConfigEntry>& entries) const;
    
    // JSON serialization helpers
    static json configValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const json& j);
    static std::optional<ConfigValue> jsonToConfigValue(const json& j, const std::string& expectedType);
    
    // Environment variable helpers
    static std::string envVarToKey(const std::string& envVar);
    static std::string keyToEnvVar(const std::string& key);
};

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;
extern template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
extern template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
extern template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
extern template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm