#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <map>
#include <regex>

#include "ValidationExceptions.hpp"
#include "Validators.hpp"
#include "ConfigurationManager.hpp"

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

/**
 * @class Config
 * @brief Configuration management system for NLM (Neural Learning Machine)
 * 
 * This class provides a flexible, type-safe configuration system that supports:
 * - File-based configuration (key=value format)
 * - Command-line argument configuration
 * - Runtime configuration changes
 * - Type-safe value retrieval with conversions
 * - Configuration source tracking (default, file, command-line, runtime)
 * - Configuration validation and schema management
 * 
 * @details The configuration system is hierarchical, with later sources
 * overriding earlier ones. Configuration sources have priority:
 * 1. Runtime (programmatically set)
 * 2. Command-line (arguments)
 * 3. File (config file)
 * 4. Default (hard-coded)
 * 
 * **Configuration File Format**:
 * ```
 * # This is a comment
 * # Key = value (value can be quoted)
 * simulation_timestep = 0.001
 * neuron_count = 1000
 * connection_probability = 0.1
 * stdp_ltp_weight = 0.01
 * stdp_ltd_weight = 0.012
 * stdp_tau = 20.0
 * random_seed = 42
 * ```
 * 
 * **Type Support**:
 * - Basic types: int, int64_t, double, bool, string
 * - Collections: vector<int>, vector<double>, vector<string>
 * - Conversion: Automatic type conversion with error handling
 * 
 * **Use Cases**:
 * - Simulation parameter configuration
 * - Learning rate and plasticity parameter tuning
 * - Memory system capacity settings
 * - Development stage parameters
 * - Neuromodulation thresholds
 * 
 * @see ../Config.cpp - implementation details
 * @see ../../brain/Brain.cpp - configuration usage examples
 */

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

/**
 * @class Config
 * @brief Configuration management system for NLM (Neural Learning Machine)
 * 
 * This class provides a flexible, type-safe configuration system that supports:
 * - File-based configuration (key=value format)
 * - Command-line argument configuration
 * - Runtime configuration changes
 * - Type-safe value retrieval with conversions
 * - Configuration source tracking (default, file, command-line, runtime)
 * - Configuration validation and schema management
 * - JSON/YAML file format support
 * - Environment variable support
 * - Configuration inheritance
 * - Configuration backup/restore functionality
 * - Configuration diff capabilities
 * - Configuration validation with range, type, and custom validators
 * - Configuration groups and conditional parameters
 * - Configuration versioning
 * - Configuration documentation generation
 * 
 * @details The configuration system is hierarchical, with later sources
 * overriding earlier ones. Configuration sources have priority:
 * 1. Runtime (programmatically set)
 * 2. Command-line (arguments)
 * 3. File (config file)
 * 4. Default (hard-coded)
 * 
 * **Configuration File Format** (key=value):
 * ```
 * # This is a comment
 * # Key = value (value can be quoted)
 * simulation_timestep = 0.001
 * neuron_count = 1000
 * connection_probability = 0.1
 * stdp_ltp_weight = 0.01
 * stdp_ltd_weight = 0.012
 * stdp_tau = 20.0
 * random_seed = 42
 * ```
 * 
 * **JSON Format**:
 * ```json
 * {
 *   "simulation_timestep": 0.001,
 *   "neuron_count": 1000,
 *   "connection_probability": 0.1,
 *   "stdp_ltp_weight": 0.01,
 *   "stdp_ltd_weight": 0.012,
 *   "stdp_tau": 20.0,
 *   "random_seed": 42
 * }
 * ```
 * 
 * **Type Support**:
 * - Basic types: int, int64_t, double, bool, string
 * - Collections: vector<int>, vector<double>, vector<string>
 * - Conversion: Automatic type conversion with error handling
 * 
 * **Validation Rules**:
 * - Required parameters must be present
 * - Numeric parameters must be within valid ranges
 * - String parameters must match patterns
 * - List parameters must meet size constraints
 * - Custom validators can be added for complex rules
 * 
 * @see ../Config.cpp - implementation details
 * @see ../../brain/Brain.cpp - configuration usage examples
 */

class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     * 
     * @details Creates a configuration object with no entries. All values must be
     * set explicitly or loaded from file/command-line before use. Default values
     * are provided by getOr() method.
     * 
     * @note This constructor creates a standalone configuration object.
     * For shared configuration across multiple components, use loadFromFile()
     * or loadFromArgs().
     */
    Config();
    
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (key=value format or JSON/YAML)
     * 
     * @details Loads configuration from a plain text file with key=value pairs,
     * JSON format, or YAML format. Supports comments (starting with # or //) and
     * quoted string values. File values have lower priority than command-line and
     * runtime values.
     * 
     * **File Parsing Examples** (key=value format):
     * ```
     * timestep = 0.001           # double
     * neuron_count = 1000         # int
     * enable_stdp = true          # bool
     * region_name = "Cortex"      # string
     * weights = [0.1, 0.2, 0.3]   # vector<double>
     * ```
     * 
     * **Syntax Rules**:
     * - Comments: Lines starting with # or // are ignored
     * - Whitespace: Surrounding whitespace is trimmed
     * - Values: Unquoted strings or quoted strings with " or '
     * - Vectors: Must be enclosed in square brackets, comma-separated
     * - Type detection: Automatic based on value format
     * 
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false if file not found or parse error
     * @throws std::runtime_error if file cannot be opened
     * @pre File must exist and be readable
     * @see loadFromArgs() - complementary loading method
     * @see loadFromJson() - JSON format loading
     * @see loadFromYaml() - YAML format loading
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @details Loads configuration from a JSON file. JSON values are automatically
     * converted to appropriate ConfigValue types. JSON files have lower priority
     * than command-line and runtime values.
     * 
     * **JSON Syntax**:
     * ```json
     * {
     *   "simulation_timestep": 0.001,
     *   "neuron_count": 1000,
     *   "connection_probability": 0.1,
     *   "enable_stdp": true,
     *   "name": "Cortex",
     *   "weights": [0.1, 0.2, 0.3]
     * }
     * ```
     * 
     * @param filepath Path to JSON configuration file
     * @return true if file loaded successfully, false if file not found or parse error
     * @see loadFromFile() - complementary loading method
     * @see loadFromYaml() - YAML format loading
     */
    bool loadFromJson(const std::string& filepath);
    
    /**
     * @brief Load configuration from YAML file
     * 
     * @details Loads configuration from a YAML file. YAML files have lower priority
     * than command-line and runtime values.
     * 
     * **YAML Syntax**:
     * ```yaml
     * simulation_timestep: 0.001
     * neuron_count: 1000
     * connection_probability: 0.1
     * enable_stdp: true
     * name: "Cortex"
     * weights: [0.1, 0.2, 0.3]
     * ```
     * 
     * @param filepath Path to YAML configuration file
     * @return true if file loaded successfully, false if file not found or parse error
     * @see loadFromFile() - complementary loading method
     * @see loadFromJson() - JSON format loading
     */
    bool loadFromYaml(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * @details Parses command-line arguments for configuration settings.
     * Supports both --key=value and -k value formats. Command-line values
     * have higher priority than file values but lower than runtime values.
     * 
     * **Command-Line Syntax**:
     * ```bash
     * program --neuron_count=1000 --timestep=0.001 --enable_stdp
     * program -n 1000 -t 0.001 -s
     * ```
     * 
     * **Argument Parsing**:
     * - --key=value: Long option with value
     * - -k value: Short option with following argument
     * - --flag: Boolean flag (sets to true)
     * - --no-flag: Boolean flag (sets to false)
     * - Value types: Auto-detected from string format
     * 
     * @param argc Number of command-line arguments (from main())
     * @param argv Array of command-line argument strings
     * @return true if arguments parsed successfully
     * @pre argv[0] should be program name
     * @see loadFromFile() - complementary loading method
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file (key=value format)
     * 
     * @details Writes current configuration to a file in key=value format.
     * This is useful for checkpointing, sharing, or documentation purposes.
     * Note that this writes placeholder values and descriptions, not actual values.
     * 
     * **Output Format**:
     * ```
     * # Configuration summary
     * timestep = PLACEHOLDER_VALUE
     * neuron_count = PLACEHOLDER_VALUE
     * ```
     * 
     * @param filepath Path where configuration will be saved
     * @return true if save succeeds, false on I/O error
     * @throws std::runtime_error if file cannot be opened
     * @pre Directory must be writable
     * @see loadFromFile() - complementary loading method
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @details Writes current configuration to a JSON file.
     * 
     * **JSON Output Format**:
     * ```json
     * {
     *   "simulation_timestep": 0.001,
     *   "neuron_count": 1000,
     *   "connection_probability": 0.1
     * }
     * ```
     * 
     * @param filepath Path where configuration will be saved
     * @return true if save succeeds, false on I/O error
     * @throws std::runtime_error if file cannot be opened
     */
    bool saveToJson(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @details Retrieves a configuration value by key, with automatic type
     * conversion and null safety. Returns std::nullopt if key doesn't exist
     * or if type conversion fails.
     * 
     * **Type Conversion Examples**:
     * ```
     * auto timestep = config.get<double>("timestep");     // double
     * auto count = config.get<int>("neuron_count");       // int
     * auto enabled = config.get<bool>("enable_stdp");     // bool
     * auto name = config.get<std::string>("name");       // string
     * ```
     * 
     * **Error Handling**:
     * - Key not found: Returns std::nullopt
     * - Type mismatch: Returns std::nullopt
     * - Invalid format: Returns std::nullopt
     * 
     * @tparam T Target type (must be convertible from ConfigValue)
     * @param key Configuration key to retrieve
     * @return std::optional<T> with value if found and convertible, std::nullopt otherwise
     * @note For default values, use getOr() instead
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @details Retrieves configuration value, returning default if key doesn't exist
     * or value is not found. This is the preferred method for configuration access
     * when default values are available.
     * 
     * **Default Value Examples**:
     * ```
     * double timestep = config->getOr<double>("timestep", 0.001);
     * size_t neurons = config->getOr<size_t>("neuron_count", 1000);
     * bool stdp = config->getOr<bool>("enable_stdp", true);
     * std::string name = config->getOr<std::string>("name", "Default");
     * ```
     * 
     * @tparam T Target type
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key not found
     * @return T configuration value or default if not found
     * @note Always returns a value (never std::nullopt)
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    /**
     * @brief Set configuration value with source tracking
     * 
     * @details Sets a configuration value and tracks its source (default, file,
     * command-line, or runtime). Later sources override earlier sources.
     * 
     * **Source Priority**:
     * 1. Runtime: Programmatically set during execution
     * 2. Command-line: Set via command-line arguments  
     * 3. File: Set via configuration file
     * 4. Default: Hard-coded defaults
     * 
     * **Value Assignment**:
     * - Sets value and updates source tracking
     * - Allows type conversion from string
     * - Maintains configuration entry history
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Origin of this configuration value
     * @post Value is set and source is recorded
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    /**
     * @brief Set configuration value (int version)
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    /**
     * @brief Set configuration value (double version)
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    /**
     * @brief Set configuration value (bool version)
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    /**
     * @brief Check if configuration key exists
     * 
     * @details Checks for presence of configuration key regardless of value.
     * Useful for conditional configuration setup.
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    // Remove key
    /**
     * @brief Remove configuration entry
     * 
     * @details Removes configuration entry for specified key if present.
     * This has higher priority than default values, so removing a key reverts
     * to default behavior.
     * 
     * @param key Configuration key to remove
     * @post Key is removed from configuration if present
     */
    void remove(const std::string& key);
    
    // Get all keys
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys (including defaults)
     * @note Order is implementation-defined
     */
    std::vector<std::string> getKeys() const;
    
    // Clear all
    /**
     * @brief Clear all configuration entries
     * 
     * @details Removes all configuration entries, reverting to default state.
     * This is useful for configuration reset or re-initialization.
     * 
     * @post Configuration contains no entries (only defaults)
     */
    void clear();
    
    // Get configuration summary
    /**
     * @brief Generate configuration summary report
     * 
     * @details Creates a human-readable summary of current configuration
     * including all entries, their values, and sources. Useful for debugging,
     * logging, and documentation.
     * 
     * **Summary Format**:
     * ```
     * Configuration (5 entries):
     *   timestep = 0.001 (Source: 1 - Default)
     *   neuron_count = 1000 (Source: 3 - CommandLine)
     *   enable_stdp = true (Source: 2 - File)
     *   name = "Cortex" (Source: 0 - Runtime)
     *   weights = [0.1, 0.2, 0.3] (Source: 4 - Runtime)
     * ```
     * 
     * @return Formatted string summary of configuration
     * @see source enum values in ConfigSource
     */
    std::string summary() const;
    
    // Validation methods
    /**
     * @brief Validate configuration against a schema
     * 
     * @details Validates the current configuration against a provided schema,
     * checking for required parameters, type compatibility, range constraints,
     * and custom validation rules. Returns detailed validation results.
     * 
     * @param schema Configuration schema to validate against
     * @return ValidationResult with detailed validation information
     */
    ValidationResult validate(const ConfigSchema& schema) const;
    
    /**
     * @brief Validate configuration against default brain system schema
     * 
     * @details Validates configuration against the built-in brain system schema,
     * checking all the specific validation rules for neuron, plasticity, memory,
     * and development parameters.
     * 
     * @return ValidationResult with detailed validation information
     */
    ValidationResult validateBrainSystem() const;
    
    /**
     * @brief Validate a specific parameter
     * 
     * @details Validates a specific parameter against its schema definition
     * and validation rules.
     * 
     * @param parameterName Name of the parameter to validate
     * @return std::optional<string> error message if validation fails, std::nullopt otherwise
     */
    std::optional<std::string> validateParameter(const std::string& parameterName) const;
    
    /**
     * @brief Add a parameter to the configuration schema
     * 
     * @details Defines a parameter in the configuration schema with type, requirements,
     * validation rules, and description.
     * 
     * @param name Parameter name
     * @param type Parameter type as string
     * @param required Whether the parameter is required
     * @param description Parameter description
     * @param defaultValue Default value for the parameter
     */
    void addParameterToSchema(const std::string& name, const std::string& type,
                             bool required = false, const std::string& description = "",
                             const std::string& defaultValue = "");
    
    /**
     * @brief Add a validator to a parameter
     * 
     * @details Adds a validator to an existing parameter schema definition.
     * 
     * @tparam ValidatorType Type of validator (RangeValidator, StringValidator, etc.)
     * @param parameterName Parameter name to add validator to
     * @param validator Validator instance
     */
    template<typename ValidatorType>
    void addValidator(const std::string& parameterName, const ValidatorType& validator);
    
    // Configuration management utilities
    /**
     * @brief Create a backup of the current configuration
     * 
     * @details Creates a backup file of the current configuration in key=value format.
     * 
     * @param backupPath Path where backup should be saved
     * @return true if backup created successfully, false otherwise
     */
    bool backup(const std::string& backupPath) const;
    
    /**
     * @brief Restore configuration from backup
     * 
     * @details Restores configuration from a backup file.
     * 
     * @param backupPath Path to backup file
     * @return true if restore successful, false otherwise
     */
    bool restore(const std::string& backupPath);
    
    /**
     * @brief Compare this configuration with another
     * 
     * @details Compares this configuration with another configuration and returns
     * a detailed diff report.
     * 
     * @param otherConfig Configuration to compare with
     * @return String containing detailed comparison report
     */
    std::string compareWith(const Config& otherConfig) const;
    
    /**
     * @brief Generate documentation for the configuration
     * 
     * @details Generates human-readable documentation for all configuration parameters.
     * 
     * @param title Title for the documentation
     * @return String containing formatted documentation
     */
    std::string generateDocumentation(const std::string& title = "Configuration Documentation") const;
    
    /**
     * @brief Generate a unique configuration ID for versioning
     * 
     * @details Creates a unique identifier for the current configuration state,
     * useful for versioning and comparison.
     * 
     * @return String containing configuration ID
     */
    std::string generateConfigId() const;
    
    /**
     * @brief Get all validation errors for the configuration
     * 
     * @details Gets all validation errors for the current configuration using
     * the default brain system schema.
     * 
     * @return Vector of error messages
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * @brief Get all validation warnings for the configuration
     * 
     * @details Gets all validation warnings for the current configuration using
     * the default brain system schema.
     * 
     * @return Vector of warning messages
     */
    std::vector<std::string> getValidationWarnings() const;
    
    // Configuration schema management
    /**
     * @brief Get the current configuration schema
     * 
     * @return Reference to the current configuration schema
     */
    const ConfigSchema& getSchema() const { return schema_; }
    
    /**
     * @brief Set the configuration schema
     * 
     * @details Sets a new configuration schema. This will replace any existing
     * schema definitions.
     * 
     * @param schema New configuration schema
     */
    void setSchema(const ConfigSchema& schema) { schema_ = schema; }
    
    /**
     * @brief Create brain system validation schema
     * 
     * @details Creates and returns the standard brain system configuration schema
     * with all the required validation rules for neuron, plasticity, memory, and
     * development parameters.
     * 
     * @return ConfigSchema for brain system validation
     */
    static ConfigSchema createBrainSystemSchema();
    
    /**
     * @brief Create enhanced brain system schema with inheritance
     * 
     * @details Creates an enhanced brain system schema that includes configuration
     * inheritance from a base schema.
     * 
     * @param baseSchema Optional base schema to inherit from
     * @return EnhancedConfigSchema for brain system validation
     */
    static EnhancedConfigSchema createEnhancedBrainSystemSchema(const ConfigSchema* baseSchema = nullptr);
    
    // Conditional configuration
    /**
     * @brief Check if a parameter should be validated based on conditional dependencies
     * 
     * @details Checks if a parameter should be validated based on its conditional
     * dependencies on other parameters.
     * 
     * @param parameterName Parameter name to check
     * @return true if parameter should be validated, false otherwise
     */
    bool shouldValidateParameter(const std::string& parameterName) const;
    
    // Environment variable support (placeholder for Phase 2)
    /**
     * @brief Load configuration from environment variables
     * 
     * @details Loads configuration from environment variables with optional prefix.
     * This is a placeholder for Phase 2 enhancement.
     * 
     * @param prefix Optional prefix for environment variables (e.g., "NLM_")
     * @return true if environment variables loaded successfully, false otherwise
     */
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Configuration groups (related parameters)
    /**
     * @brief Add a configuration group
     * 
     * @details Adds a configuration group that contains related parameters.
     * Groups help organize configuration parameters by functional area.
     * 
     * @param name Group name
     * @param description Group description
     */
    void addGroup(const std::string& name, const std::string& description = "");
    
    /**
     * @brief Get all configuration groups
     * 
     * @return Vector of all configuration group names
     */
    std::vector<std::string> getGroups() const;
    
    // Private members
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration schema
    ConfigSchema schema_;
    
    // Configuration groups
    std::map<std::string, std::string> groups_;
    
    // Environment variables prefix
    std::string envPrefix_ = "NLM_";
    
    // Validation caching for performance
    mutable std::map<std::string, ValidationResult> validationCache_;
    
    // Configuration version
    std::string configVersion_ = "1.0";
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseValue(const std::string& str, ConfigValue& value);
    static std::string valueToString(const ConfigValue& value) const;
    
    // Environment variable helpers (Phase 2)
    static std::string getEnvironmentVariable(const std::string& name);
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    
    // File format helpers
    static bool parseJsonFile(const std::string& filepath, std::map<std::string, std::string>& configMap);
    static bool parseYamlFile(const std::string& filepath, std::map<std::string, std::string>& configMap);
    static bool writeJsonFile(const std::string& filepath, const std::map<std::string, std::string>& configMap) const;
    static bool writeYamlFile(const std::string& filepath, const std::map<std::string, std::string>& configMap) const;
};

} // namespace nlm