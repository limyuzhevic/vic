//#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlm {

// Enhanced configuration value types with more comprehensive support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, double>
>;

// Enhanced configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    UserInput,
    System
};

// Advanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::chrono::system_clock::time_point timestamp;
    bool validated;
    bool deprecated;
    std::string deprecationMessage;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
        category(), timestamp(), validated(false), deprecated(false), deprecationMessage() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "general")
        : key(k), value(v), source(s), description(desc), category(cat), 
          timestamp(std::chrono::system_clock::now()), validated(true), 
          deprecated(false), deprecationMessage() {}
    
    // Type checking and conversion
    template<typename T>
    bool isType() const {
        return std::holds_alternative<T>(value);
    }
    
    template<typename T>
    T getValue() const {
        return std::get<T>(value);
    }
    
    template<typename T>
    bool getValue(T& out) const {
        if (isType<T>()) {
            out = getValue<T>();
            return true;
        }
        return false;
    }
    
    // Validation
    virtual bool validate() const { return true; }
    
    // String representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << sourceToString(source) << "] " << key << " = ";
        
        if (isType<int>()) {
            oss << getValue<int>();
        } else if (isType<double>()) {
            oss << getValue<double>();
        } else if (isType<bool>()) {
            oss << (getValue<bool>() ? "true" : "false");
        } else if (isType<std::string>()) {
            oss << "\"" << getValue<std::string>() << "\"";
        } else {
            oss << "[complex value]";
        }
        
        return oss.str();
    }
    
    std::string sourceToString(ConfigSource source) const {
        switch (source) {
            case ConfigSource::Default: return "DEFAULT";
            case ConfigSource::File: return "FILE";
            case ConfigSource::CommandLine: return "COMMAND_LINE";
            case ConfigSource::Runtime: return "RUNTIME";
            case ConfigSource::Environment: return "ENVIRONMENT";
            case ConfigSource::UserInput: return "USER_INPUT";
            case ConfigSource::System: return "SYSTEM";
            default: return "UNKNOWN";
        }
    }
};

// Configuration schema definition for validation and documentation
struct ConfigSchema {
    std::string key;
    std::string type;
    ConfigValue defaultValue;
    std::string description;
    std::string category;
    std::function<bool(const ConfigValue&)> validator;
    std::map<std::string, std::string> metadata;
    
    ConfigSchema(const std::string& k, const std::string& t, const ConfigValue& def,
                 const std::string& desc = "", const std::string& cat = "general",
                 std::function<bool(const ConfigValue&)> validator = nullptr)
        : key(k), type(t), defaultValue(def), description(desc), category(cat),
          validator(validator) {}
};

// Advanced configuration manager with comprehensive features
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    // Schema management
    void registerSchema(const ConfigSchema& schema);
    void registerSchemas(const std::vector<ConfigSchema>& schemas);
    std::vector<ConfigSchema> getAllSchemas() const;
    std::optional<ConfigSchema> getSchema(const std::string& key) const;
    
    // Enhanced loading from various sources
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromCommandLine(int argc, char** argv, ConfigSource source = ConfigSource::CommandLine);
    bool loadFromEnvironment(const std::string& prefix = "NLM_", ConfigSource source = ConfigSource::Environment);
    bool loadFromUserInput(const std::string& prompt = "", ConfigSource source = ConfigSource::UserInput);
    
    // Enhanced value setting with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe getter methods
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    bool get(const std::string& key, T& out) const;
    
    // Advanced validation and error handling
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration categories and organization
    void setCategory(const std::string& key, const std::string& category);
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getKeysInCategory(const std::string& category) const;
    
    // Configuration search and filtering
    std::vector<ConfigEntry> search(const std::string& pattern) const;
    std::vector<ConfigEntry> getBySource(ConfigSource source) const;
    std::vector<ConfigEntry> getByCategory(const std::string& category) const;
    
    // Configuration comparison and merging
    bool equals(const ConfigManager& other) const;
    void merge(const ConfigManager& other, bool overwrite = true);
    std::unique_ptr<ConfigManager> copy() const;
    
    // Configuration export and import
    bool exportToJSON(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    bool importFromYAML(const std::string& filepath);
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    
    // Advanced command-line argument parsing
    struct CommandLineOptions {
        std::map<std::string, std::string> options;
        std::vector<std::string> positionalArgs;
        std::map<std::string, std::vector<std::string>> multiOptions;
        
        std::string getOption(const std::string& key, const std::string& defaultValue = "") const;
        int getOptionAsInt(const std::string& key, int defaultValue = 0) const;
        double getOptionAsDouble(const std::string& key, double defaultValue = 0.0) const;
        bool getOptionAsBool(const std::string& key, bool defaultValue = false) const;
        std::vector<std::string> getMultiOption(const std::string& key) const;
        bool hasOption(const std::string& key) const;
    };
    
    CommandLineOptions parseCommandLine(int argc, char** argv) const;
    
    // Configuration presets and templates
    void addPreset(const std::string& name, const ConfigManager& config);
    std::optional<ConfigManager> getPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    void removePreset(const std::string& name);
    
    // Performance optimization
    void optimizeMemoryUsage();
    size_t getMemoryUsage() const;
    void clearUnusedEntries();
    
    // Statistics and monitoring
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    double getMemoryUsagePercentage() const;
    std::chrono::system_clock::time_point getLastModifiedTime() const;
    
    // Configuration reset and maintenance
    void clear();
    void clearBySource(ConfigSource source);
    void clearByCategory(const std::string& category);
    void resetToDefaults();
    void reloadFromFiles();
    
    // Debug and diagnostic
    std::string debugInfo() const;
    void printSummary() const;
    void printValidationReport() const;
    
    // Callback system for configuration changes
    using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source)>
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void setChangeCallbacksEnabled(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source);
    bool validateEntry(const ConfigEntry& entry) const;
    std::vector<std::string> validateAllEntries() const;
    void updateSourceTimestamp(ConfigSource source);
    bool isValueValid(const ConfigValue& value, const std::string& typeName) const;
    ConfigValue convertValue(const ConfigValue& value, const std::string& targetType) const;
    
    // Command-line parsing helpers
    static CommandLineOptions parseStandardOptions(int argc, char** argv);
    static std::vector<std::string> extractArgs(int argc, char** argv);
    static std::map<std::string, std::string> parseOptions(const std::vector<std::string>& args);
    static std::vector<std::string> parseMultiOptions(const std::vector<std::string>& args);
    static std::vector<std::string> extractPositionalArgs(const std::vector<std::string>& args);
    
    // Schema validation helpers
    static bool validateValue(const ConfigValue& value, const std::string& typeName);
    static std::string typeToString(const ConfigValue& value);
    static ConfigValue getDefaultValueForType(const std::string& typeName);
    
    // File format helpers
    static bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string generateJSON(const std::vector<ConfigEntry>& entries);
    static std::string generateYAML(const std::vector<ConfigEntry>& entries);
};

// Global configuration manager instance
inline ConfigManager& getConfigManager() {
    static ConfigManager manager;
    return manager;
}

// Convenience functions for common configuration tasks
inline void loadConfigFromFile(const std::string& filepath) {
    getConfigManager().loadFromFile(filepath);
}

inline void loadConfigFromCommandLine(int argc, char** argv) {
    getConfigManager().loadFromCommandLine(argc, argv);
}

inline void setConfig(const std::string& key, const std::string& value) {
    getConfigManager().set(key, value);
}

inline void setConfig(const std::string& key, int value) {
    getConfigManager().set(key, value);
}

inline void setConfig(const std::string& key, double value) {
    getConfigManager().set(key, value);
}

inline void setConfig(const std::string& key, bool value) {
    getConfigManager().set(key, value);
}

inline std::optional<std::string> getConfigString(const std::string& key) {
    return getConfigManager().get<std::string>(key);
}

inline std::optional<int> getConfigInt(const std::string& key) {
    return getConfigManager().get<int>(key);
}

inline std::optional<double> getConfigDouble(const std::string& key) {
    return getConfigManager().get<double>(key);
}

inline std::optional<bool> getConfigBool(const std::string& key) {
    return getConfigManager().get<bool>(key);
}

// Legacy compatibility
inline bool loadFromFile(const std::string& filepath) {
    return getConfigManager().loadFromFile(filepath);
}

inline bool loadFromArgs(int argc, char** argv) {
    return getConfigManager().loadFromCommandLine(argc, argv);
}

inline std::optional<std::string> get(const std::string& key) {
    return getConfigManager().get<std::string>(key);
}

inline void set(const std::string& key, const std::string& value) {
    getConfigManager().set(key, value);
}

inline std::optional<int> getOr(const std::string& key, int defaultValue) {
    return getConfigManager().getOr<int>(key, defaultValue);
}

inline std::optional<double> getOr(const std::string& key, double defaultValue) {
    return getConfigManager().getOr<double>(key, defaultValue);
}

inline bool has(const std::string& key) {
    return getConfigManager().has(key);
}

inline void clear() {
    getConfigManager().clear();
}

} // namespace nlm