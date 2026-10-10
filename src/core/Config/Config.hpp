#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Inherited
};

// Configuration category/section
enum class ConfigCategory {
    System,
    Neural,
    Simulation,
    Plasticity,
    Memory,
    Learning,
    Development,
    Visualization,
    Environment,
    Logging,
    Performance
};

// Configuration schema validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string name;
    std::string description;
    ConfigCategory category;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    std::string type; // For validation
    
    // Type validation functions
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> formatter;
    
    ConfigSchemaEntry() = default;
    
    ConfigSchemaEntry(
        const std::string& n,
        const std::string& desc,
        ConfigCategory cat = ConfigCategory::System,
        const std::optional<ConfigValue>& def = std::nullopt,
        bool req = false,
        const std::string& t = "auto"
    ) : name(n), description(desc), category(cat), defaultValue(def), required(req), type(t) {}
};

// Configuration group for nested structures
struct ConfigGroup {
    std::string name;
    std::string description;
    std::vector<ConfigSchemaEntry> entries;
    std::unordered_map<std::string, ConfigGroup> subgroups;
    bool hasSchema = false;
    
    ConfigGroup() = default;
    ConfigGroup(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    
    ConfigGroup& addEntry(const ConfigSchemaEntry& entry) {
        entries.push_back(entry);
        return *this;
    }
    
    ConfigGroup& addSubGroup(const ConfigGroup& group) {
        subgroups[group.name] = group;
        return *this;
    }
};

// Main configuration schema class
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add configuration entry
    ConfigSchema& addEntry(const ConfigSchemaEntry& entry);
    
    // Add configuration group
    ConfigSchema& addGroup(const ConfigGroup& group);
    
    // Get entry by name
    const ConfigSchemaEntry* getEntry(const std::string& name) const;
    
    // Get group by name
    const ConfigGroup* getGroup(const std::string& name) const;
    
    // Validate configuration against schema
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Get all entries
    const std::vector<ConfigSchemaEntry>& getAllEntries() const { return entries; }
    
    // Get all groups
    const std::vector<ConfigGroup>& getAllGroups() const { return groups; }
    
    // Save schema to JSON/YAML
    bool saveToFile(const std::string& filepath, bool isJSON = true) const;
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Clear all entries and groups
    void clear();
    
    // Get configuration summary
    std::string summary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string toCamelCase(const std::string& str) const;
    std::string toPascalCase(const std::string& str) const;
};

// Configuration value converter and validator
class ConfigValueConverter {
public:
    static ConfigValue fromJSON(const nlohmann::json& json);
    static ConfigValue fromYAML(const YAML::Node& yaml);
    
    static nlohmann::json toJSON(const ConfigValue& value);
    static YAML::Node toYAML(const ConfigValue& value);
    
    static std::string toString(const ConfigValue& value);
    static bool toBool(const ConfigValue& value);
    static int toInt(const ConfigValue& value);
    static double toDouble(const ConfigValue& value);
    
    static bool isValidType(const std::string& typeName);
    static std::vector<std::string> getSupportedTypes();
    
    static ValidationResult validate(const std::string& typeName, const ConfigValue& value);
};

// Enhanced configuration class with full JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const { return schema; }
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from file with format detection
    bool loadFromFileAuto(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool isJSON = true) const;
    
    // Get values with type safety
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
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeys(ConfigCategory category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Apply default configuration
    void applyDefaults();
    
    // Merge with another config (inheritance)
    void merge(const Config& other, ConfigSource source = ConfigSource::Inherited);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema;
    
    // Internal helpers
    bool loadFromJSONInternal(const nlohmann::json& json, ConfigSource source);
    bool loadFromYAMLInternal(const YAML::Node& yaml, ConfigSource source);
    std::string getKeyCategory(const std::string& key) const;
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toCamelCase(const std::string& str);
    static std::string toPascalCase(const std::string& str);
    
    friend class ConfigSchema;
};

// Forward declarations
class JSONConfigLoader;
class YAMLConfigLoader;

} // namespace nlm
