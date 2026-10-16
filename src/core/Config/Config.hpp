#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <typeinfo>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;
class ConfigValidationError;

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

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationError : public ConfigError {
public:
    enum class ErrorType {
        MissingRequired,
        TypeMismatch,
        ValueOutOfRange,
        InvalidFormat,
        DuplicateKey,
        SchemaMismatch
    };
    
    ConfigValidationError(ErrorType type, const std::string& key, const std::string& message)
        : ConfigError("Config validation error: " + message),
          errorType(type), configKey(key) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getConfigKey() const { return configKey; }
    
private:
    ErrorType errorType;
    std::string configKey;
};

// Configuration schema element types
enum class SchemaType {
    Integer,
    Double,
    Boolean,
    String,
    IntegerArray,
    DoubleArray,
    StringArray,
    Object
};

// Schema validation rules
struct SchemaValidationRule {
    enum class ConstraintType {
        None,
        Required,
        MinValue,
        MaxValue,
        MinLength,
        MaxLength,
        Pattern,
        Enum,
        Default
    };
    
    ConstraintType type;
    std::string value;
    std::string description;
    
    SchemaValidationRule() : type(ConstraintType::None), value(), description() {}
    SchemaValidationRule(ConstraintType t, const std::string& v, const std::string& d = "")
        : type(t), value(v), description(d) {}
};

// Configuration schema
class ConfigSchema {
public:
    struct SchemaEntry {
        std::string key;
        SchemaType type;
        std::vector<SchemaValidationRule> rules;
        std::string description;
        bool required;
        std::string defaultValue;
        
        SchemaEntry() : type(SchemaType::String), required(false) {}
        SchemaEntry(const std::string& k, SchemaType t, bool req = false, const std::string& desc = "")
            : key(k), type(t), required(req), description(desc) {}
    };
    
    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    // Add schema entry
    void addEntry(const SchemaEntry& entry);
    
    // Get schema entry by key
    const SchemaEntry* getEntry(const std::string& key) const;
    
    // Check if key exists in schema
    bool hasEntry(const std::string& key) const;
    
    // Validate against schema
    std::vector<ConfigValidationError> validate(const Config& config) const;
    
    // Get required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear schema
    void clear();
    
    // Export schema as JSON
    std::string exportAsJSON() const;
    
    // Import schema from JSON
    bool importFromJSON(const std::string& json);
    
private:
    std::unordered_map<std::string, SchemaEntry> entries;
};

// Configuration version info
struct ConfigVersion {
    int major;
    int minor;
    int patch;
    std::string description;
    std::chrono::system_clock::time_point createdAt;
    
    ConfigVersion() : major(1), minor(0), patch(0), createdAt(std::chrono::system_clock::now()) {}
    ConfigVersion(int maj, int min, int patch, const std::string& desc = "")
        : major(maj), minor(min), patch(patch), description(desc), createdAt(std::chrono::system_clock::now()) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << major << "." << minor << "." << patch;
        if (!description.empty()) {
            oss << " (" << description << ")";
        }
        return oss.str();
    }
    
    bool operator==(const ConfigVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator<(const ConfigVersion& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

// JSON/YAML parser result
class ParseResult {
public:
    enum class ResultType {
        Success,
        Error
    };
    
    ParseResult() : type(ResultType::Success) {}
    ParseResult(const std::string& error) : type(ResultType::Error), errorMessage(error) {}
    
    bool success() const { return type == ResultType::Success; }
    const std::string& getError() const { return errorMessage; }
    
private:
    ResultType type;
    std::string errorMessage;
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
    
    // Load from JSON string
    bool loadFromJSON(const std::string& json);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yaml);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    std::string saveToJSON() const;
    
    // Save as YAML
    std::string saveToYAML() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw ConfigValue
    std::optional<ConfigValue> getRaw(const std::string& key) const;
    
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
    
    // Schema validation
    std::vector<ConfigValidationError> validateAgainstSchema(const ConfigSchema& schema) const;
    
    // Type conversion helpers
    static std::optional<ConfigValue> stringToConfigValue(const std::string& value);
    template<typename T>
    static std::string configValueToString(const ConfigValue& value);
    
    // JSON/YAML parsing helpers
    static ParseResult parseJSON(const std::string& json, std::unordered_map<std::string, ConfigValue>& out);
    static ParseResult parseYAML(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out);
    static std::string serializeToJSON(const std::unordered_map<std::string, ConfigValue>& data) const;
    static std::string serializeToYAML(const std::unordered_map<std::string, ConfigValue>& data) const;
    
    // Configuration versioning
    void setVersion(const ConfigVersion& version);
    ConfigVersion getVersion() const;
    std::string getVersionString() const;
    
    // Configuration validation methods
    bool isValidType(const std::string& key, const ConfigValue& value) const;
    bool isValidRange(const std::string& key, const ConfigValue& value) const;
    bool isValidLength(const std::string& key, const ConfigValue& value) const;
    bool isValidPattern(const std::string& key, const ConfigValue& value) const;
    
    // Configuration export/import
    std::string exportAsJSON() const;
    std::string exportAsYAML() const;
    std::string exportAsINI() const;
    
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    bool importFromINI(const std::string& ini);
    
    // Configuration inheritance and merging
    void mergeFrom(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void inheritFrom(const Config& parent, const std::vector<std::string>& path = {});
    
    // Configuration documentation
    std::string generateDocumentation() const;
    std::string getDocumentation(const std::string& key) const;
    
    // Get configuration statistics
    std::string getStats() const;
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Set description for a key
    void setDescription(const std::string& key, const std::string& description);
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string escapeJSON(const std::string& str);
    static std::string unescapeJSON(const std::string& str);
    static std::string escapeYAML(const std::string& str);
    static std::string unescapeYAML(const std::string& str);
    
    // JSON parsing implementation
    static ParseResult parseJSONInternal(const std::string& json, std::unordered_map<std::string, ConfigValue>& out, size_t& pos);
    static ParseResult parseJSONValue(const std::string& json, ConfigValue& out, size_t& pos);
    static ParseResult parseJSONObject(const std::string& json, std::unordered_map<std::string, ConfigValue>& out, size_t& pos);
    static ParseResult parseJSONArray(const std::string& json, std::vector<ConfigValue>& out, size_t& pos);
    static ParseResult parseJSONString(const std::string& json, std::string& out, size_t& pos);
    static ParseResult parseJSONNumber(const std::string& json, ConfigValue& out, size_t& pos);
    static ParseResult parseJSONBoolean(const std::string& json, ConfigValue& out, size_t& pos);
    static ParseResult parseJSONNull(const std::string& json, ConfigValue& out, size_t& pos);
    
    // YAML parsing implementation
    static ParseResult parseYAMLInternal(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out, size_t& pos);
    static ParseResult parseYAMLDocument(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out, size_t& pos);
    static ParseResult parseYAMLKeyValue(const std::string& yaml, std::pair<std::string, ConfigValue>& out, size_t& pos);
    static ParseResult parseYAMLString(const std::string& yaml, std::string& out, size_t& pos);
    static ParseResult parseYAMLNumber(const std::string& yaml, ConfigValue& out, size_t& pos);
    static ParseResult parseYAMLScalar(const std::string& yaml, ConfigValue& out, size_t& pos);
    
    // Validation implementation
    static bool validateType(const std::string& key, const ConfigValue& value, SchemaType type);
    static bool validateConstraints(const std::string& key, const ConfigValue& value, const ConfigSchema& schema);
    
    // Serialization implementation
    static std::string serializeJSON(const std::unordered_map<std::string, ConfigValue>& data) const;
    static std::string serializeYAML(const std::unordered_map<std::string, ConfigValue>& data) const;
    static std::string serializeINI(const std::vector<ConfigEntry>& entries) const;
};

} // namespace nlm
