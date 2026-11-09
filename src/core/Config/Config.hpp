#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <regex>

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
    Runtime
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

// Configuration data types
enum class ConfigDataType {
    Integer,
    Float,
    Boolean,
    String,
    IntVector,
    FloatVector,
    StringVector,
    Object,
    Array
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigDataType type;
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    std::function<bool(const ConfigValue&)> validator;
    std::vector<std::string> dependencies; // Keys that must be present
    std::string group; // For organization
    
    ConfigSchemaEntry(const std::string& k, ConfigDataType t, const std::string& desc = "")
        : key(k), type(t), description(desc), required(false) {}
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
    
    // Load from file (supports JSON, INI, YAML, and key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from INI file
    bool loadFromINI(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type conversion helpers
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    
    // Validation
    bool validate(const ConfigSchemaEntry& schema) const;
    bool validateAll(const std::vector<ConfigSchemaEntry>& schema) const;
    
    // Schema management
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    void removeSchemaEntry(const std::string& key);
    std::optional<ConfigSchemaEntry> getSchemaEntry(const std::string& key) const;
    std::vector<ConfigSchemaEntry> getSchemaEntries() const;
    
    // Configuration helpers
    static std::vector<std::string> parseCommandLineArgs(int argc, char** argv);
    static std::string generateHelp(const std::vector<ConfigSchemaEntry>& schema);
    static std::string getVersionInfo();
    
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
    
    // Serialization helpers
    static std::string toJSON(const Config& config);
    static std::string toINI(const Config& config);
    static std::string toYAML(const Config& config);
    
    // Command line parsing helpers
    static bool parseBoolFlag(const std::string& arg);
    static std::optional<int64_t> parseIntArg(const std::string& arg);
    static std::optional<double> parseDoubleArg(const std::string& arg);
    static std::vector<std::string> parseListArg(const std::string& arg);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unordered_map<std::string, ConfigSchemaEntry> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string trimQuotes(const std::string& str);
    static ConfigSource detectSourceFromFile(const std::string& filepath);
    static bool isJSONFile(const std::string& filepath);
    static bool isINIFile(const std::string& filepath);
    static bool isYAMLLFile(const std::string& filepath);
    
    // File format detection and parsing
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    static bool parseJSONFromStream(std::istream& stream);
    static bool parseINIFromStream(std::istream& stream);
    static bool parseYAMLFromStream(std::istream& stream);
    
    // Format conversion
    static std::string configValueToJSON(const ConfigValue& value) const;
    static std::optional<ConfigValue> jsonValueToConfigValue(const std::string& jsonStr) const;
};

// Configuration helpers
namespace config {
    // Common schema definitions
    extern const std::vector<ConfigSchemaEntry> defaultSchema;
    
    // Helper functions
    std::string formatConfigValue(const ConfigValue& value);
    bool compareConfigValues(const ConfigValue& a, const ConfigValue& b);
    std::vector<std::string> getValueTypeNames(ConfigDataType type);
    
    // Validation helpers
    bool validateRange(const ConfigValue& value, double min, double max);
    bool validatePattern(const ConfigValue& value, const std::string& pattern);
    bool validateEnum(const ConfigValue& value, const std::vector<std::string>& allowed);
}

} // namespace nlm
