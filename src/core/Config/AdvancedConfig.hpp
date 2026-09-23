#pragma once

/**
 * Advanced Configuration System for NLM
 * 
 * This module provides advanced configuration management for the NLM (Neural Learning Machine)
 * project with support for multiple formats, validation, schema definition, and enhanced
 * features beyond the basic key=value format.
 * 
 * Features:
 * - JSON and YAML format support
 * - Configuration schema validation
 * - Configuration inheritance and merging
 * - Environment variable expansion
 * - Type validation and coercion
 * - Configuration templates
 * - Hot reload support
 * - Configuration comparison and diffing
 * 
 * Key Components:
 * - ConfigValue: Enhanced variant type with better type safety
 * - ConfigSchema: Schema definition and validation
 * - ConfigManager: Main configuration manager with advanced features
 * - ConfigValidator: Configuration validation and type checking
 * - ConfigMerger: Configuration merging and inheritance
 * 
 * Benefits:
 * - Better integration with modern tools (visual editors, IDEs)
 * - Improved type safety and error messages
 * - Better support for complex configuration
 * - Enhanced validation and error reporting
 * - Better maintainability and documentation
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <any>
#include <typeindex>
#include <regex>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <set>
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * Enhanced configuration value types
 */
using EnhancedConfigValue = std::variant<
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::map<std::string, std::any>  // For nested objects
>;

/**
 * Configuration data type for schema validation
 */
enum class ConfigDataType {
    None,
    Integer,
    Float,
    Boolean,
    String,
    IntegerArray,
    FloatArray,
    BooleanArray,
    StringArray,
    Object,
    Array
};

/**
 * Configuration constraint types
 */
enum class ConstraintType {
    None,
    Range,
    Min,
    Max,
    Pattern,
    Length,
    Choices,
    Custom
};

/**
 * Configuration constraint
 */
struct ConfigConstraint {
    ConstraintType type;
    std::string errorMessage;
    std::any value;
    
    ConfigConstraint() : type(ConstraintType::None), errorMessage("") {}
    
    ConfigConstraint(ConstraintType t, const std::string& msg, const std::any& val = std::any())
        : type(t), errorMessage(msg), value(val) {}
};

/**
 * Configuration schema field
 */
struct ConfigField {
    std::string name;
    ConfigDataType dataType;
    EnhancedConfigValue defaultValue;
    bool required;
    std::string description;
    std::vector<ConfigConstraint> constraints;
    bool readOnly;
    std::string category;
    std::string validationRegex;
    double validationMin;
    double validationMax;
    size_t validationMinLength;
    size_t validationMaxLength;
    std::set<std::string> validChoices;
    std::function<bool(const EnhancedConfigValue&, const ConfigField&)> customValidator;
    
    ConfigField()
        : dataType(ConfigDataType::None)
        , required(false)
        , readOnly(false)
        , validationMin(0.0)
        , validationMax(0.0)
        , validationMinLength(0)
        , validationMaxLength(0) {}
    
    ConfigField(const std::string& name, ConfigDataType type, 
                const EnhancedConfigValue& defaultVal = EnhancedConfigValue(),
                bool req = false, const std::string& desc = "")
        : name(name)
        , dataType(type)
        , defaultValue(defaultVal)
        , required(req)
        , description(desc)
        , readOnly(false)
        , validationMin(0.0)
        , validationMax(0.0)
        , validationMinLength(0)
        , validationMaxLength(0) {}
};

/**
 * Configuration schema
 */
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Disable copying
    ConfigSchema(const ConfigSchema&) = delete;
    ConfigSchema& operator=(const ConfigSchema&) = delete;
    
    /**
     * Add a field to the schema
     */
    void addField(const ConfigField& field);
    
    /**
     * Add a field with type and default value
     */
    void addField(const std::string& name, ConfigDataType dataType,
                 const EnhancedConfigValue& defaultValue = EnhancedConfigValue(),
                 bool required = false, const std::string& description = "");
    
    /**
     * Get field by name
     */
    const ConfigField* getField(const std::string& name) const;
    
    /**
     * Check if field exists
     */
    bool hasField(const std::string& name) const;
    
    /**
     * Get all field names
     */
    std::vector<std::string> getFieldNames() const;
    
    /**
     * Validate configuration against schema
     */
    std::vector<std::string> validate(const std::unordered_map<std::string, EnhancedConfigValue>& config) const;
    
    /**
     * Get validation error for a specific field
     */
    std::string validateField(const std::string& fieldName, 
                             const EnhancedConfigValue& value) const;
    
    /**
     * Get all fields
     */
    const std::vector<ConfigField>& getFields() const;
    
    /**
     * Merge with another schema (child fields override parent)
     */
    void merge(const ConfigSchema& other);
    
    /**
     * Clear all fields
     */
    void clear();
    
    /**
     * Get schema summary
     */
    std::string getSummary() const;
    
    /**
     * Save schema to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Load schema from file
     */
    bool loadFromFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Configuration validator with enhanced type checking
 */
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    /**
     * Validate value against field type
     */
    bool validateType(const std::string& fieldName, const ConfigDataType& fieldType,
                     const EnhancedConfigValue& value) const;
    
    /**
     * Validate value against constraints
     */
    std::vector<std::string> validateConstraints(const EnhancedConfigValue& value,
                                                 const ConfigField& field) const;
    
    /**
     * Validate entire configuration against schema
     */
    std::vector<std::string> validate(const std::unordered_map<std::string, EnhancedConfigValue>& config,
                                     const ConfigSchema& schema) const;
    
    /**
     * Convert value to target type
     */
    std::optional<EnhancedConfigValue> convertValue(const EnhancedConfigValue& value,
                                                   ConfigDataType targetType) const;
    
    /**
     * Get type from value
     */
    static ConfigDataType getType(const EnhancedConfigValue& value);
    
    /**
     * Format value as string
     */
    static std::string valueToString(const EnhancedConfigValue& value);
    
    /**
     * Get value type name
     */
    static const char* getTypeName(ConfigDataType type);
    
    /**
     * Check if value matches type
     */
    static bool isType(const EnhancedConfigValue& value, ConfigDataType type);
};

/**
 * Enhanced configuration manager with advanced features
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    /**
     * Load configuration from file (JSON/YAML)
     */
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    /**
     * Load configuration from JSON string
     */
    bool loadFromJSON(const std::string& json, bool validate = true);
    
    /**
     * Load configuration from YAML string
     */
    bool loadFromYAML(const std::string& yaml, bool validate = true);
    
    /**
     * Save configuration to file
     */
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    
    /**
     * Get configuration value
     */
    std::optional<EnhancedConfigValue> get(const std::string& key) const;
    
    /**
     * Get configuration value with type conversion
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Get configuration value with default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set configuration value
     */
    bool set(const std::string& key, const EnhancedConfigValue& value, bool validate = true);
    
    /**
     * Set configuration value with type
     */
    bool set(const std::string& key, ConfigDataType type, const EnhancedConfigValue& value, bool validate = true);
    
    /**
     * Set configuration value with type conversion
     */
    template<typename T>
    bool set(const std::string& key, const T& value, bool validate = true);
    
    /**
     * Check if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove key
     */
    bool remove(const std::string& key);
    
    /**
     * Get all keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Clear all configuration
     */
    void clear();
    
    /**
     * Merge with another configuration (other overrides this)
     */
    bool merge(const ConfigManager& other);
    
    /**
     * Get configuration summary
     */
    std::string getSummary() const;
    
    /**
     * Validate configuration against schema
     */
    std::vector<std::string> validate(const ConfigSchema& schema) const;
    
    /**
     * Set configuration schema
     */
    void setSchema(const ConfigSchema& schema);
    
    /**
     * Get configuration schema
     */
    const ConfigSchema& getSchema() const;
    
    /**
     * Enable/disable validation
     */
    void setValidation(bool enabled);
    
    /**
     * Check if validation is enabled
     */
    bool isValidationEnabled() const;
    
    /**
     * Enable/disable environment variable expansion
     */
    void setEnvironmentExpansion(bool enabled);
    
    /**
     * Check if environment variable expansion is enabled
     */
    bool isEnvironmentExpansionEnabled() const;
    
    /**
     * Enable/disable type coercion
     */
    void setTypeCoercion(bool enabled);
    
    /**
     * Check if type coercion is enabled
     */
    bool isTypeCoercionEnabled() const;
    
    /**
     * Export configuration to JSON
     */
    std::string exportToJSON(bool pretty = true) const;
    
    /**
     * Export configuration to YAML
     */
    std::string exportToYAML(bool pretty = true) const;
    
    /**
     * Import configuration from JSON
     */
    bool importFromJSON(const std::string& json);
    
    /**
     * Import configuration from YAML
     */
    bool importFromYAML(const std::string& yaml);
    
    /**
     * Get configuration diff
     */
    std::vector<std::pair<std::string, EnhancedConfigValue>> diff(const ConfigManager& other) const;
    
    /**
     * Save configuration snapshot
     */
    bool saveSnapshot(const std::string& name);
    
    /**
     * Load configuration snapshot
     */
    bool loadSnapshot(const std::string& name);
    
    /**
     * Get available snapshots
     */
    std::vector<std::string> getSnapshots() const;
    
    /**
     * Delete snapshot
     */
    bool deleteSnapshot(const std::string& name);
    
    /**
     * Get global configuration manager instance
     */
    static ConfigManager& getGlobal();
    
    /**
     * Set global configuration manager
     */
    static void setGlobal(std::shared_ptr<ConfigManager> manager);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool expandEnvironmentVariables(std::string& value) const;
    std::optional<EnhancedConfigValue> coerceType(const EnhancedConfigValue& value, ConfigDataType targetType) const;
    bool resolveReference(const std::string& key, EnhancedConfigValue& value) const;
};

/**
 * Convenience functions for common configuration operations
 */
namespace ConfigUtils {
    
    /**
     * Load configuration from file with automatic format detection
     */
    bool loadConfig(ConfigManager& config, const std::string& filepath);
    
    /**
     * Create schema with common NLM settings
     */
    ConfigSchema createDefaultNLMSchema();
    
    /**
     * Merge command line arguments into configuration
     */
    bool mergeCommandLineArgs(ConfigManager& config, int argc, char** argv);
    
    /**
     * Expand environment variables in configuration values
     */
    void expandEnvironmentVariables(ConfigManager& config);
    
    /**
     * Validate configuration against default schema
     */
    std::vector<std::string> validateConfig(const ConfigManager& config);
    
    /**
     * Export configuration to multiple formats
     */
    bool exportConfig(ConfigManager& config, const std::string& basePath);
    
    /**
     * Import configuration from multiple formats
     */
    bool importConfig(ConfigManager& config, const std::string& basePath);
    
} // namespace ConfigUtils

} // namespace nlm
