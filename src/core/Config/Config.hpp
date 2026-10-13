// Enhanced configuration system for NLM
// Phase 1: Advanced configuration management with validation

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <stdexcept>

// Include nlohmann/json for JSON support
// This assumes nlohmann/json is available in the build system

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

// Configuration value types with type safety
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    StringList,
    DoubleList,
    IntList,
    Unknown
};

// Configuration validation error
struct ConfigValidationError {
    std::string message;
    std::string key;
    ConfigType expected_type;
    std::string actual_value_str;
    
    ConfigValidationError(const std::string& msg, const std::string& key, ConfigType expected,
                         const std::string& actual)
        : message(msg), key(key), expected_type(expected), actual_value_str(actual) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigType type;
    std::any default_value;
    std::string description;
    bool required;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigSchemaEntry(const std::string& k, ConfigType t, const std::any& def_val = nullptr,
                     const std::string& desc = "", bool req = false)
        : key(k), type(t), default_value(def_val), description(desc), required(req) {}
    
    bool validate(const ConfigValue& value) const {
        if (validator) {
            return validator(value);
        }
        return true; // No validator means type checking only
    }
}; 

// Configuration inheritance hierarchy
struct ConfigInheritance {
    std::string parent_key;
    std::string child_key;
    float weight;
    
    ConfigInheritance(const std::string& parent, const std::string& child, float w = 1.0f)
        : parent_key(parent), child_key(child), weight(w) {}
};

// Advanced configuration class with validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // === Core functionality ===
    
    // Load from file with automatic format detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from INI format (basic key=value with comments)
    bool loadFromINI(const std::string& iniStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with specified format
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // === Enhanced value access with type safety ===
    
    // Get value with type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get value with default and type checking
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw value with type introspection
    bool getType(const std::string& key, ConfigType& type) const;
    std::string getValueAsString(const std::string& key) const;
    
    // === Advanced configuration features ===
    
    // Set value with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set value with schema validation
    bool setValidated(const std::string& key, const ConfigValue& value, const ConfigSchemaEntry& schema, ConfigSource source = ConfigSource::Runtime);
    
    // Merge with another configuration
    void merge(const Config& other, float weight = 1.0f);
    
    // Inherit from parent configuration
    void inheritFrom(const Config& parent, const std::vector<ConfigInheritance>& rules);
    
    // === Configuration management ===
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, ConfigType type) const;
    
    // Get all keys with specific type
    std::vector<std::string> getKeysOfType(ConfigType type) const;
    
    // Get configuration description
    std::string getKeyDescription(const std::string& key) const;
    
    // Remove key with validation
    void remove(const std::string& key);
    
    // Clear all configuration
    void clear();
    
    // === Configuration validation ===
    
    // Validate entire configuration against schema
    std::vector<ConfigValidationError> validate(const ConfigSchema& schema) const;
    
    // Validate individual key
    bool validateKey(const std::string& key, const ConfigSchemaEntry& schema) const;
    
    // Check if configuration is complete (all required keys present)
    bool isComplete(const ConfigSchema& schema) const;
    
    // Get missing required keys
    std::vector<std::string> getMissingRequiredKeys(const ConfigSchema& schema) const;
    
    // === Configuration utilities ===
    
    // Get configuration summary with type information
    std::string summary() const;
    
    // Get configuration statistics
    std::string getStats() const;
    
    // Validate file format
    static bool isJSON(const std::string& str);
    static bool isINI(const std::string& str);
    static bool isYAML(const std::string& str);
    
    // Get file format from extension
    static std::string getFileFormatFromExtension(const std::string& filepath);
    
    // === Schema management ===
    
    // Create and manage configuration schemas
    static ConfigSchema createDefaultSchema();
    static ConfigSchema createPhase2Schema();
    static ConfigSchema createPhase4Schema();
    
    // Schema validation helpers
    static bool isPositiveInt(const ConfigValue& value);
    static bool isNonNegativeDouble(const ConfigValue& value);
    static bool isBoolean(const ConfigValue& value);
    static bool isString(const ConfigValue& value);
    static bool isStringList(const ConfigValue& value);
    static bool isDoubleList(const ConfigValue& value);
    static bool isIntList(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static ConfigType getConfigType(const ConfigValue& value);
    static bool convertToTargetType(const ConfigValue& source, ConfigType targetType, ConfigValue& dest);
    static std::string configValueToString(const ConfigValue& value);
    static bool parseJSONFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
    static bool parseINIFile(const std::string& content, std::map<std::string, ConfigValue>& entries);
}; 

// Configuration schema class
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add schema entry
    void addEntry(const ConfigSchemaEntry& entry);
    
    // Get schema entry
    std::optional<ConfigSchemaEntry> getEntry(const std::string& key) const;
    
    // Check if schema has entry
    bool hasEntry(const std::string& key) const;
    
    // Get all entries
    std::vector<ConfigSchemaEntry> getAllEntries() const;
    
    // Get required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Validate schema (internal consistency)
    std::vector<std::string> validateSchema() const;
    
    // Get schema summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration value type utilities
namespace ConfigUtils {
    // Type checking functions
    bool isInt(const ConfigValue& value);
    bool isInt64(const ConfigValue& value);
    bool isDouble(const ConfigValue& value);
    bool isBool(const ConfigValue& value);
    bool isString(const ConfigValue& value);
    bool isStringList(const ConfigValue& value);
    bool isDoubleList(const ConfigValue& value);
    bool isIntList(const ConfigValue& value);
    
    // Type conversion functions
    int toInt(const ConfigValue& value);
    int64_t toInt64(const ConfigValue& value);
    double toDouble(const ConfigValue& value);
    bool toBool(const ConfigValue& value);
    std::string toString(const ConfigValue& value);
    
    // List access functions
    template<typename T>
    std::vector<T> toList(const ConfigValue& value);
    
    // Validation functions
    bool isPositive(const ConfigValue& value);
    bool isNonNegative(const ConfigValue& value);
    bool isInRange(const ConfigValue& value, double min, double max);
    bool matchesPattern(const ConfigValue& value, const std::string& pattern);
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int stringToInt(const std::string& str);
}

// Phase-specific configuration schemas
namespace PhaseConfigs {
    // Phase 2: Real Neural Computation
    ConfigSchema getPhase2ConfigSchema();
    
    // Phase 4: Emerging Cognition
    ConfigSchema getPhase4ConfigSchema();
    
    // Development stage configurations
    ConfigSchema getDevelopmentConfig(DevelopmentalStage stage);
    
    // Experiment configurations
    ConfigSchema getExperimentConfig(const std::string& experimentName);
}

} // namespace nlm

// JSON serialization support for ConfigValue
namespace nlohmann {
template<typename T>
struct adl_serializer<nlm::ConfigValue> {
    static void to_json(json& j, const nlm::ConfigValue& val) {
        // Handle different variant types
        if (val.index() == 0) { // int
            j = std::get<int>(val);
        } else if (val.index() == 1) { // int64_t
            j = std::get<int64_t>(val);
        } else if (val.index() == 2) { // double
            j = std::get<double>(val);
        } else if (val.index() == 3) { // bool
            j = std::get<bool>(val);
        } else if (val.index() == 4) { // string
            j = std::get<std::string>(val);
        } else if (val.index() == 5) { // vector<int>
            j = std::get<std::vector<int>>(val);
        } else if (val.index() == 6) { // vector<double>
            j = std::get<std::vector<double>>(val);
        } else if (val.index() == 7) { // vector<string>
            j = std::get<std::vector<std::string>>(val);
        }
    }
    
    static void from_json(const json& j, nlm::ConfigValue& val) {
        // Try to detect type from JSON
        if (j.is_number_integer()) {
            val = j.get<int>();
        } else if (j.is_number_float()) {
            val = j.get<double>();
        } else if (j.is_boolean()) {
            val = j.get<bool>();
        } else if (j.is_string()) {
            // Try to parse as list first
            try {
                auto int_list = j.get<std::vector<int>>();
                val = int_list;
                return;
            } catch (...) {}
            
            try {
                auto double_list = j.get<std::vector<double>>();
                val = double_list;
                return;
            } catch (...) {}
            
            try {
                auto string_list = j.get<std::vector<std::string>>();
                val = string_list;
                return;
            } catch (...) {}
            
            val = j.get<std::string>();
        } else if (j.is_array()) {
            // Try to determine array type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    val = j.get<std::vector<int>>();
                } else if (j[0].is_number_float()) {
                    val = j.get<std::vector<double>>();
                } else {
                    val = j.get<std::vector<std::string>>();
                }
            } else {
                val = std::vector<std::string>();
            }
        }
    }
};
} // namespace nlohmann

// Helper functions for configuration management
namespace ConfigHelpers {
    // Configuration file operations
    bool fileExists(const std::string& filepath);
    std::string getFileExtension(const std::string& filepath);
    std::string getFileNameWithoutExtension(const std::string& filepath);
    
    // Configuration validation
    bool isValidKey(const std::string& key);
    bool isValidConfigValue(const ConfigValue& value);
    
    // Configuration statistics
    size_t countEntries(const std::map<std::string, ConfigValue>& entries);
    size_t countEntriesByType(const std::map<std::string, ConfigValue>& entries, nlm::ConfigType type);
    
    // Configuration merging
    void mergeEntries(std::map<std::string, ConfigValue>& target, const std::map<std::string, ConfigValue>& source, float weight = 1.0f);
    
    // Configuration inheritance
    bool resolveInheritance(const std::map<std::string, ConfigValue>& parent, 
                           std::map<std::string, ConfigValue>& child,
                           const std::vector<nlm::ConfigInheritance>& rules);
    
    // Configuration backup and restore
    bool backupConfig(const std::string& sourcePath, const std::string& backupPath);
    bool restoreConfig(const std::string& backupPath, const std::string& targetPath);
}

} // namespace nlm