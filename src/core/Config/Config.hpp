#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>

// Forward declarations for JSON/YAML
namespace nlohmann {
    class json;
}

namespace YAML {
    class Node;
}

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
    Environment,
    Runtime,
    Override
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error) : valid(v), errorMessage(error) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "vector"
    std::string description;
    ConfigValue defaultValue;
    std::string unit; // "seconds", "Hz", "bytes", etc.
    std::optional<std::function<ValidationResult(const ConfigValue&)>> validator;
    bool required = false;
    
    ConfigSchemaEntry() = default;
    ConfigSchemaEntry(const std::string& k, const std::string& t, const std::string& d, 
                     const ConfigValue& def = {}, const std::string& u = "")
        : key(k), type(t), description(d), defaultValue(def), unit(u) {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string unit;
    int64_t intValue;
    double doubleValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), unit(), 
                   intValue(0), doubleValue(0.0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
               const std::string& desc = "", const std::string& u = "")
        : key(k), value(v), source(s), description(desc), unit(u), 
          intValue(0), doubleValue(0.0) {
        if (std::holds_alternative<int64_t>(value)) {
            intValue = std::get<int64_t>(value);
        } else if (std::holds_alternative<int>(value)) {
            intValue = std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            doubleValue = std::get<double>(value);
        }
    }
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters with type inference
    void set(const std::string& key, const std::string& value) { set(key, value, ConfigSource::Runtime); }
    void set(const std::string& key, int value) { set(key, value, ConfigSource::Runtime); }
    void set(const std::string& key, double value) { set(key, value, ConfigSource::Runtime); }
    void set(const std::string& key, bool value) { set(key, value, ConfigSource::Runtime); }
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool isType(const std::string& key, const std::string& type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Configuration schema management
    static void initializeSchema();
    static std::vector<ConfigSchemaEntry> getSchema() { return schema_; }
    static ValidationResult validate(const std::string& key, const ConfigValue& value);
    static ValidationResult validateAll() { return validateAllImpl(); }
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get source information
    ConfigSource getSource(const std::string& key) const;
    bool isOverridden(const std::string& key) const;
    
    // Time-based configuration updates
    void applyTimeBasedUpdates();
    void scheduleUpdate(const std::string& key, const ConfigValue& value, 
                       const std::chrono::system_clock::time_point& time);
    
    // Merge with another config
    void merge(const Config& other, ConfigSource source = ConfigSource::Override);
    
    // Get default values for keys
    ConfigValue getDefault(const std::string& key) const;
    bool hasDefault(const std::string& key) const;
    
    // Configuration groups
    void setGroup(const std::string& key, const std::string& group);
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getKeysInGroup(const std::string& group) const;
    std::string getGroup(const std::string& key) const;
    
    // Import/export utilities
    bool exportToJSON(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    bool exportToYAML(const std::string& filepath) const;
    bool importFromYAML(const std::string& filepath);
    
    // Configuration statistics
    size_t getEntryCount() const { return pImpl->entries.size(); }
    double getMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static nlohmann::json variantToJSON(const ConfigValue& value);
    static ConfigValue jsonToVariant(const nlohmann::json& json);
    static std::string variantToString(const ConfigValue& value) const;
    
    // Schema management
    static std::vector<ConfigSchemaEntry> schema_;
    static bool schemaInitialized_;
    static std::map<std::string, std::string> keyToGroup_;
    static std::map<std::string, ConfigValue> defaultValues_;
    static std::vector<std::pair<std::string, std::pair<ConfigValue, std::chrono::system_clock::time_point>>> scheduledUpdates_;
    
    // Validation implementation
    static ValidationResult validateAllImpl();
    static ValidationResult validateSingle(const ConfigSchemaEntry& schema, const ConfigEntry& entry);
    
    // JSON/YAML helpers
    static bool parseJSON(const std::string& jsonString, std::vector<ConfigEntry>& entries);
    static bool parseYAML(const std::string& yamlString, std::vector<ConfigEntry>& entries);
    static std::string jsonToConfigString(const nlohmann::json& json);
    static std::string yamlToConfigString(const YAML::Node& node);
    
    // Environment variable parsing
    static bool parseEnvironmentVariable(const std::string& envName, const std::string& envValue,
                                         std::vector<ConfigEntry>& entries);
};

// Utility functions
inline std::string toString(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else {
        return "";
    }
}

inline std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

inline bool stringEndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string getEnvVar(const std::string& name) {
#ifdef _WIN32
    char buffer[1024];
    getenv_s(&size_t, buffer, sizeof(buffer), name.c_str());
    return std::string(buffer);
#else
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
#endif
}

} // namespace nlm