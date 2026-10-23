#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>
#include "Recovery.hpp"

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", const ConfigValue& value = {})
        : std::runtime_error(message), key(key), value(value) {}
    
    std::string getKey() const { return key; }
    ConfigValue getValue() const { return value; }
    
private:
    std::string key;
    ConfigValue value;
};

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    /**
     * @brief Unique identifier for the configuration parameter
     * 
     * The key must be a valid C++ identifier and should be descriptive of
     * the configuration parameter's purpose. Keys are case-sensitive.
     */
    std::string key;
    
    /**
     * @brief Expected data type for the configuration parameter
     * 
     * Valid type strings are: "int", "int64_t", "double", "bool", "string",
     * "vector<int>", "vector<double>", "vector<string>". The type determines
     * how the configuration value is parsed and validated.
     */
    std::string type;  // "int", "int64_t", "double", "bool", "string", "vector<int>", etc.
    
    /**
     * @brief Allowed configuration sources for this parameter
     * 
     * Specifies which configuration sources can provide values for this
     * parameter. For example, some parameters may only be set via CommandLine
     * for overriding, while others may be set from Default, File, or Runtime
     * sources.
     */
    ConfigSource source;  // Where this setting can come from
    
    /**
     * @brief Human-readable description of the configuration parameter
     * 
     * This description provides context about what the parameter controls,
     * its purpose, and any additional information about its usage. It is
     * used in configuration documentation and help messages.
     */
    std::string description;
    
    /**
     * @brief Minimum allowed value for numeric parameters
     * 
     * When set, the configuration value must be greater than or equal to this
     * minimum value. This constraint is only applicable to numeric types.
     */
    std::optional<double> min_value;
    
    /**
     * @brief Maximum allowed value for numeric parameters
     * 
     * When set, the configuration value must be less than or equal to this
     * maximum value. This constraint is only applicable to numeric types.
     */
    std::optional<double> max_value;
    
    /**
     * @brief Minimum allowed length for string or container parameters
     * 
     * When set, the configuration value's length (for strings or containers)
     * must be greater than or equal to this minimum value.
     */
    std::optional<size_t> min_length;
    
    /**
     * @brief Maximum allowed length for string or container parameters
     * 
     * When set, the configuration value's length (for strings or containers)
     * must be less than or equal to this maximum value.
     */
    std::optional<size_t> max_length;
    
    /**
     * @brief Regular expression pattern for string validation
     * 
     * When set, the configuration string value must match this regular
     * expression pattern. This is useful for validating formats like
     * IP addresses, file paths, or custom string formats.
     */
    std::optional<std::string> pattern;  // For string validation
    
    /**
     * @brief Default value for the configuration parameter
     * 
     * When a configuration parameter is not explicitly set by any source,
     * this default value will be used. The default value must match the
     * declared type and constraints of the schema entry.
     */
    std::optional<ConfigValue> default_value;
    
    /**
     * @brief Default constructor for ConfigSchemaEntry
     * 
     * Creates an empty schema entry. All fields are initialized to their
     * default values (empty strings, ConfigSource::Default, etc.).
     */
    ConfigSchemaEntry()
        : key(), type(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a ConfigSchemaEntry with all parameters
     * 
     * @param k Configuration key
     * @param t Expected type as string
     * @param s Allowed configuration sources
     * @param desc Human-readable description
     * @param def Optional default value
     */
    ConfigSchemaEntry(const std::string& k, const std::string& t, ConfigSource s, 
                     const std::string& desc = "", const std::optional<ConfigValue>& def = std::nullopt)
        : key(k), type(t), source(s), description(desc), default_value(def) {}
};

// Configuration schema class
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a schema entry
    void addEntry(const ConfigSchemaEntry& entry);
    
    // Check if key is in schema
    bool hasKey(const std::string& key) const;
    
    // Get schema entry for key
    const ConfigSchemaEntry* getEntry(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get entries for specific source
    std::vector<ConfigSchemaEntry> getEntriesForSource(ConfigSource source) const;
    
    // Validate a configuration value against schema
    void validate(const std::string& key, const ConfigValue& value) const;
    
    // Validate all current entries
    void validateAll(const std::map<std::string, ConfigEntry>& entries) const;
    
    // Apply defaults for missing entries
    void applyDefaults(std::map<std::string, ConfigEntry>& entries) const;
    
    // Get description of all entries
    std::string getSummary() const;
    
private:
    std::vector<ConfigSchemaEntry> entries_;
    
    // Type checking
    bool checkType(const std::string& type, const ConfigValue& value) const;
    
    // Value range validation
    void validateRange(const ConfigSchemaEntry& entry, const ConfigValue& value) const;
    
    // String pattern validation
    void validatePattern(const ConfigSchemaEntry& entry, const ConfigValue& value) const;
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

// Main configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Initialize with schema
    void initializeSchema();
    
    // Get the schema
    const ConfigSchema& getSchema() const;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from key-value pairs
    void loadFromKV(const std::map<std::string, std::string>& kvPairs);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
