#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <variant>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<ConfigValue>,
    std::map<std::string, ConfigValue>
>;

// Configuration path - supports nested access with dot notation
class ConfigPath {
public:
    ConfigPath() = default;
    explicit ConfigPath(const std::string& path);
    
    // Get path components
    const std::vector<std::string>& getComponents() const { return components; }
    const std::string& getPath() const { return fullPath; }
    
    // Path operations
    ConfigPath parent() const;
    std::string lastComponent() const;
    bool isEmpty() const;
    
    // Comparison
    bool operator==(const ConfigPath& other) const;
    bool operator!=(const ConfigPath& other) const;
    
    // String representation
    std::string toString() const;
    
private:
    std::string fullPath;
    std::vector<std::string> components;
};

// Configuration source enumeration
enum class ConfigSource {
    DEFAULT,      // Hard-coded defaults
    CONFIG_FILE,  // External config file (YAML/JSON)
    COMMAND_LINE, // Command line arguments
    ENVIRONMENT,   // Environment variables
    RUNTIME,      // Runtime modifications
    COMMAND       // Runtime commands
};

// Configuration metadata
struct ConfigMetadata {
    ConfigSource source;
    std::string sourceDescription;
    std::string timestamp;
    std::string description;
    std::map<std::string, std::string> tags;
    std::vector<std::string> dependencies;
    
    ConfigMetadata() : source(ConfigSource::DEFAULT), timestamp("") {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigMetadata metadata;
    
    ConfigEntry() = default;
    ConfigEntry(const std::string& k, const ConfigValue& v) : key(k), value(v) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource source)
        : key(k), value(v) {
        metadata.source = source;
    }
};

// Configuration schema for type validation
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, ConfigParameterSchema> parameters;
    std::string version;
    std::vector<std::string> requiredParameters;
    
    // Validation methods
    bool validate(const ConfigEntry& entry) const;
    const ConfigParameterSchema* getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    std::vector<std::string> getAllParameterKeys() const;
};

// Parameter schema for individual configuration parameters
struct ConfigParameterSchema {
    std::string name;
    ConfigValue defaultValue;
    ConfigValue type; // Expected type for validation
    std::string description;
    bool required = false;
    std::vector<ConfigValue> allowedValues;
    std::pair<double, double> range; // For numeric types
    std::string pattern; // For string types (regex pattern)
    std::string category; // For grouping parameters
    std::vector<std::string> dependencies; // Other parameters this depends on
    
    // Validation
    bool isValid(const ConfigValue& value) const;
    std::string getTypeName() const;
};

// Configuration snapshot for rollback and comparison
struct ConfigSnapshot {
    std::string name;
    std::map<std::string, ConfigEntry> entries;
    std::string timestamp;
    std::string description;
    
    // Operations
    bool contains(const std::string& key) const;
    ConfigValue get(const std::string& key) const;
    size_t size() const;
    std::vector<std::string> getKeys() const;
    
    // Comparison
    bool operator==(const ConfigSnapshot& other) const;
    bool operator!=(const ConfigSnapshot& other) const;
    
    // File operations
    bool save(const std::string& filepath) const;
    static ConfigSnapshot load(const std::string& filepath);
};

// Main configuration manager
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Configuration entry management
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::RUNTIME);
    void set(const std::string& key, const ConfigValue& value, const ConfigMetadata& metadata);
    
    ConfigValue get(const std::string& key) const;
    ConfigValue get(const std::string& key, const ConfigValue& defaultValue) const;
    
    bool has(const std::string& key) const;
    bool remove(const std::string& key);
    
    // Configuration path support
    ConfigValue get(const ConfigPath& path) const;
    void set(const ConfigPath& path, const ConfigValue& value, ConfigSource source = ConfigSource::RUNTIME);
    bool has(const ConfigPath& path) const;
    
    // Configuration schema management
    void registerSchema(const ConfigSchema& schema);
    bool hasSchema(const std::string& name) const;
    const ConfigSchema* getSchema(const std::string& name) const;
    
    // Configuration source management
    void loadFromFile(const std::string& filepath);
    void loadFromEnvironment();
    void loadFromCommandLine(int argc, char** argv);
    
    // Configuration validation
    std::vector<std::string> validate() const;
    bool validate(const std::string& key) const;
    
    // Configuration operations
    ConfigSnapshot createSnapshot(const std::string& name, const std::string& description = "") const;
    void restoreSnapshot(const ConfigSnapshot& snapshot);
    bool hasSnapshot(const std::string& name) const;
    
    // Configuration queries
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& category) const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Configuration export/import
    std::string toJSON() const;
    std::string toYAML() const;
    std::string toINI() const;
    
    // Configuration comparison
    bool equals(const ConfigurationManager& other) const;
    std::map<std::string, ConfigValue> diff(const ConfigurationManager& other) const;
    
    // Configuration metadata
    ConfigMetadata getMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const ConfigMetadata& metadata);
    
    // Configuration categories
    void setCategory(const std::string& key, const std::string& category);
    std::string getCategory(const std::string& key) const;
    std::vector<std::string> getCategories() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration builder for easy construction
class ConfigurationBuilder {
public:
    ConfigurationBuilder();
    ~ConfigurationBuilder();
    
    // Set values
    ConfigurationBuilder& set(const std::string& key, const ConfigValue& value);
    ConfigurationBuilder& set(const std::string& key, int64_t value);
    ConfigurationBuilder& set(const std::string& key, double value);
    ConfigurationBuilder& set(const std::string& key, float value);
    ConfigurationBuilder& set(const std::string& key, bool value);
    ConfigurationBuilder& set(const std::string& key, const std::string& value);
    ConfigurationBuilder& set(const std::string& key, const std::vector<ConfigValue>& value);
    ConfigurationBuilder& set(const std::string& key, const std::map<std::string, ConfigValue>& value);
    
    // Set source
    ConfigurationBuilder& withSource(ConfigSource source);
    ConfigurationBuilder& withMetadata(const ConfigMetadata& metadata);
    
    // Set category
    ConfigurationBuilder& withCategory(const std::string& category);
    
    // Build
    std::map<std::string, ConfigEntry> build() const;
    
    // Presets
    static std::map<std::string, ConfigEntry> createDefaultConfig();
    static std::map<std::string, ConfigEntry> createDevelopmentConfig();
    static std::map<std::string, ConfigEntry> createProductionConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global configuration manager instance
ConfigurationManager& getGlobalConfiguration();

// Configuration validation functions
namespace ConfigValidation {
    // Type validation
    bool isInteger(const ConfigValue& value);
    bool isDouble(const ConfigValue& value);
    bool isFloat(const ConfigValue& value);
    bool isBool(const ConfigValue& value);
    bool isString(const ConfigValue& value);
    bool isArray(const ConfigValue& value);
    bool isObject(const ConfigValue& value);
    
    // Value validation
    bool isInRange(const ConfigValue& value, double min, double max);
    bool isInAllowedValues(const ConfigValue& value, const std::vector<ConfigValue>& allowed);
    bool matchesPattern(const std::string& value, const std::string& pattern);
    
    // Type conversion
    ConfigValue convertToType(const ConfigValue& value, ConfigValue targetType);
    std::string valueToString(const ConfigValue& value);
    ConfigValue stringToValue(const std::string& str);
    
    // Path manipulation
    std::vector<std::string> splitPath(const std::string& path);
    std::string joinPath(const std::vector<std::string>& components);
}

// Configuration file format support
namespace ConfigFileFormat {
    enum class Format {
        JSON,
        YAML,
        INI,
        PROPERTIES,
        TOML
    };
    
    // File operations
    bool saveToFile(const std::string& filepath, const ConfigurationManager& config, Format format = Format::JSON);
    bool loadFromFile(ConfigurationManager& config, const std::string& filepath, Format format = Format::JSON);
    
    // Format detection
    Format detectFormat(const std::string& filepath);
}

} // namespace nlm
