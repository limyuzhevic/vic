// Configuration entry
/**
 * @struct ConfigEntry
 * @brief Stores a single configuration parameter with metadata
 * 
 * ConfigEntry represents a named configuration value with its source and
 * descriptive information for documentation purposes.
 */
struct ConfigEntry {
    /**
     * @brief The configuration key/name
     */
    std::string key;
    
    /**
     * @brief The configuration value (supports multiple types via variant)
     */
    ConfigValue value;
    
    /**
     * @brief How this value was obtained (default, file, command line, or runtime)
     */
    ConfigSource source;
    
    /**
     * @brief Human-readable description of the configuration option
     */
    std::string description;
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a new ConfigEntry
     * 
     * @param k Configuration key/name
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of the configuration option
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @class Config
 * @brief Central configuration manager for NLM parameters
 * 
 * The Config class provides a type-safe interface for storing, retrieving,
 * and modifying configuration parameters. It supports multiple value types
 * (int, double, bool, string, and arrays) and tracks the source of each
 * configuration value to handle precedence rules.
 * 
 * Configuration values can come from:
 * - Default: Compiled-in default values
 * - File: Loaded from JSON/YAML configuration files
 * - CommandLine: Parsed from command line arguments
 * - Runtime: Set programmatically during execution
 * 
 * The last source wins in case of conflicts (File > CommandLine > Runtime > Default)
 */

// Forward declarations
class Config;

// Configuration value types
/**
 * @typedef ConfigValue
 * @brief Variant type supporting multiple configuration value types
 * 
 * ConfigValue can store any of the following types:
 * - int: Integer values
 * - int64_t: 64-bit integer values  
 * - double: Floating-point values
 * - bool: Boolean flags
 * - std::string: String values
 * - std::vector<int>: Integer arrays
 * - std::vector<double>: Floating-point arrays
 * - std::vector<std::string>: String arrays
 * 
 * @note When retrieving values with get<T>(), type checking is performed
 * and appropriate casting/conversion is applied.
 */
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
/**
 * @enum class ConfigSource
 * @brief Enumeration of configuration value sources
 * 
 * Each configuration value is tagged with its source to determine
 * precedence when multiple sources provide conflicting values.
 * 
 * @note Source precedence (highest to lowest):
 * 1. File
 * 2. CommandLine  
 * 3. Runtime
 * 4. Default
 */
enum class ConfigSource {
    /**
     * @brief Default values (compiled into the binary)
     */
    Default,
    
    /**
     * @brief Values loaded from configuration files (JSON/YAML)
     */
    File,
    
    /**
     * @brief Values parsed from command line arguments
     */
    CommandLine,
    
    /**
     * @brief Values set programmatically at runtime
     */
    Runtime
};

namespace nlm {

// JSON/YAML conversion helper functions
/**
 * @brief Convert a ConfigValue to nlohmann::json representation
 * 
 * @param value Configuration value to convert
 * @return nlohmann::json JSON representation of the configuration value
 * @note Handles all ConfigValue types including arrays
 */
nlohmann::json configValueToJson(const ConfigValue& value);

/**
 * @brief Convert nlohmann::json to ConfigValue
 * 
 * @param j JSON value to convert
 * @return ConfigValue Converted configuration value
 * @note Supports int, double, bool, string, and array types
 * @warning May throw if JSON type is unsupported
 */
ConfigValue jsonToConfigValue(const nlohmann::json& j);

/**
 * @brief Convert a ConfigValue to YAML::Node representation
 * 
 * @param value Configuration value to convert
 * @return YAML::Node YAML representation of the configuration value
 * @note Handles all ConfigValue types including arrays
 */
YAML::Node configValueToYaml(const ConfigValue& value);

/**
 * @brief Convert YAML::Node to ConfigValue
 * 
 * @param node YAML node to convert
 * @return ConfigValue Converted configuration value
 * @note Supports scalar and sequence types in YAML
 * @warning May throw if YAML type is unsupported
 */
ConfigValue yamlToConfigValue(const YAML::Node& node);

// File format detection
/**
 * @brief Detect file format based on file extension and content
 * 
 * @param filepath Path to the configuration file
 * @return std::string File format identifier ("json" or "yaml")
 * @note Uses file extension if available, otherwise scans content
 */
std::string detectFileFormat(const std::string& filepath);

/**
 * @brief Check if file appears to be JSON format
 * 
 * @param filepath Path to the file to check
 * @return true if file extension suggests JSON format
 */
bool isJsonFile(const std::string& filepath);

/**
 * @brief Check if file appears to be YAML format
 * 
 * @param filepath Path to the file to check
 * @return true if file extension suggests YAML format
 */
bool isYamlFile(const std::string& filepath);

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
     /**
     * @brief Prevent copying to ensure singleton-like behavior
     * 
     * Copy construction and assignment are disabled to maintain
     * configuration consistency and prevent unexpected behavior.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Prevent copy assignment
     * 
     * @return Config& Reference to this object (never returns)
     */
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    /**
     * @brief Load configuration from a JSON file
     * 
     * Reads configuration values from a file and merges them with existing
     * configuration. File values take highest precedence.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was successfully loaded and parsed
     * @return false if file couldn't be opened or contained invalid JSON
     */
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or --key value
     * and updates the configuration. Command line values have medium precedence.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if arguments were successfully parsed
     * @return false if argument parsing failed
     */
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    /**
     * @brief Save configuration to a file
     * 
     * Writes the current configuration to a file in JSON format.
     * Only runtime and command line values are saved (not defaults).
     * 
     * @param filepath Path where the configuration file should be saved
     * @return true if file was successfully written
     * @return false if file couldn't be written
     */
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves the configuration value associated with the given key and
     * attempts to convert it to the requested type T. If the key doesn't exist
     * or the value cannot be converted to type T, returns std::nullopt.
     * 
     * @tparam T Type to convert the configuration value to (int, double, bool, string, or array types)
     * @param key Configuration key to look up
     * @return std::optional<T> The converted value if successful, std::nullopt otherwise
     * 
     * @note Supported types match ConfigValue variants. For arrays, T should be std::vector<TElement>.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves the configuration value associated with the given key and
     * converts it to the requested type T. If the key doesn't exist or the
     * value cannot be converted, returns the provided default value.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key doesn't exist or conversion fails
     * @return T The converted value or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    /**
     * @brief Set a configuration value with explicit source
     * 
     * Sets or updates a configuration value and tracks its source for precedence.
     * The source determines the value's precedence in the configuration hierarchy.
     * 
     * @param key Configuration key to set
     * @param value Configuration value (as ConfigValue variant)
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from string
     * 
     * Convenience method for setting string values. Attempts to convert the
     * string to the appropriate type based on the key name or context.
     * 
     * @param key Configuration key to set
     * @param value String value to convert and store
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from integer
     * 
     * Convenience method for setting integer values.
     * 
     * @param key Configuration key to set
     * @param value Integer value to store
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from double
     * 
     * Convenience method for setting floating-point values.
     * 
     * @param key Configuration key to set
     * @param value Double value to store
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from boolean
     * 
     * Convenience method for setting boolean values.
     * 
     * @param key Configuration key to set
     * @param value Boolean value to store
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    /**
     * @brief Validate the entire configuration
     * 
     * Validates all configuration parameters against their schema rules.
     * 
     * @return std::vector<ValidationError> Vector of validation errors, empty if valid
     */
    std::vector<ValidationError> validateConfig() const;
    
    /**
     * @brief Validate a single configuration parameter
     * 
     * Validates a single configuration parameter against its schema rules.
     * 
     * @param key Configuration key to validate
     * @return std::vector<ValidationError> Vector of validation errors, empty if valid
     */
    std::vector<ValidationError> validateParameter(const std::string& key) const;
    
    /**
     * @brief Get validation error message for a parameter
     * 
     * @param key Configuration key to get error for
     * @return std::string Error message, empty string if no error
     */
    std::string getValidationError(const std::string& key) const;
    
    /**
     * @brief Register a configuration parameter with validation rules
     * 
     * @param param ConfigParameter with validation rules
     */
    void registerParameter(const ConfigParameter& param);
    
    // Remove key
    /**
     * @brief Remove a configuration key
     * 
     * Deletes the configuration key and its associated value from the configuration.
     * The key is removed regardless of its source.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    // Get all keys
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    // Clear all
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration keys and resets to default state.
     * Note: This does not affect default values (they remain available).
     */
    void clear();
    
    // Get configuration summary
    /**
     * @brief Get a formatted summary of all configuration values
     * 
     * Generates a human-readable string containing all configuration values,
     * their sources, and types for debugging and inspection purposes.
     * 
     * @return std::string Formatted configuration summary
     */
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    bool validateParameter(const std::string& key) const;
    std::string getValidationError(const std::string& key) const;
    std::string validateAll() const;
    
    // Parameter information for validation and documentation
    struct ConfigParameter {
        std::string key;
        std::string type;  // "int", "int64_t", "double", "bool", "string", "vector<int>", etc.
        std::string description;
        std::string defaultValue;
        bool required;
        std::string validationRule;  // Optional validation rule (e.g., "range:0-100", "positive")
        
        ConfigParameter() : key(), type(), description(), defaultValue(), required(false), validationRule() {}
        ConfigParameter(const std::string& k, const std::string& t, const std::string& desc, 
                       const std::string& def = "", bool req = false, const std::string& rule = "")
            : key(k), type(t), description(desc), defaultValue(def), required(req), validationRule(rule) {}
    };
    
    static std::vector<ConfigParameter> getParameters();
    static bool registerParameter(const ConfigParameter& param);
    
private:
    // Internal validation helpers
    bool validateInt(const std::string& key, const int& value) const;
    bool validateDouble(const std::string& key, const double& value) const;
    bool validateFloat(const std::string& key, const float& value) const;
    bool validateString(const std::string& key, const std::string& value) const;
    bool validateBool(const std::string& key, const bool& value) const;
    bool validateRange(const std::string& key, double min, double max) const;
    bool validatePositive(const std::string& key) const;
    bool validateProbability(const std::string& key) const;
    
    std::string getIntErrorMessage(const std::string& key, const int& value) const;
    std::string getDoubleErrorMessage(const std::string& key, const double& value) const;
    std::string getFloatErrorMessage(const std::string& key, const float& value) const;
    std::string getStringErrorMessage(const std::string& key, const std::string& value) const;
    std::string getRangeErrorMessage(const std::string& key, double min, double max) const;
    std::string getPositiveErrorMessage(const std::string& key) const;
    std::string getProbabilityErrorMessage(const std::string& key) const;
};

} // namespace nlm

// Configuration parameter schema definition
/**
 * @struct ConfigParameter
 * @brief Defines validation rules and metadata for a configuration parameter
 * 
 * ConfigParameter contains validation rules and metadata for a configuration
 * parameter including allowed value ranges, type constraints, and descriptions.
 * 
 */
struct ConfigParameter {
    /**
     * @brief The configuration key/name
     */
    std::string key;
    
    /**
     * @brief Default value for this parameter
     */
    ConfigValue defaultValue;
    
    /**
     * @brief Human-readable description of the configuration option
     */
    std::string description;
    
    /**
     * @brief Minimum allowed value (for numeric types)
     */
    std::optional<double> minValue;
    
    /**
     * @brief Maximum allowed value (for numeric types)
     */
    std::optional<double> maxValue;
    
    /**
     * @brief Minimum allowed integer value
     */
    std::optional<int64_t> minIntValue;
    
    /**
     * @brief Maximum allowed integer value
     */
    std::optional<int64_t> maxIntValue;
    
    /**
     * @brief Whether the value must be positive
     */
    bool positive = false;
    
    /**
     * @brief Whether the value must be non-negative
     */
    bool nonNegative = false;
    
    /**
     * @brief Whether the value must be between 0 and 1 (for probabilities)
     */
    bool probabilityRange = false;
    
    /**
     * @brief Minimum allowed length (for strings and arrays)
     */
    std::optional<size_t> minLength;
    
    /**
     * @brief Maximum allowed length (for strings and arrays)
     */
    std::optional<size_t> maxLength;
    
    /**
     * @brief Regular expression pattern for string validation
     */
    std::string pattern;
    
    /**
     * @brief Whether this parameter is required
     */
    bool required = false;
    
    /**
     * @brief Default constructor
     */
    ConfigParameter() : key(), defaultValue(), description() {}
    
    /**
     * @brief Construct a new ConfigParameter with basic validation
     * 
     * @param k Configuration key/name
     * @param v Default value
     * @param desc Description
     * @param min Minimum value
     * @param max Maximum value
     */
    ConfigParameter(const std::string& k, const ConfigValue& v, const std::string& desc = "",
                   const std::optional<double>& min = std::nullopt, 
                   const std::optional<double>& max = std::nullopt)
        : key(k), defaultValue(v), description(desc), minValue(min), maxValue(max) {}
    
    /**
     * @brief Construct a new ConfigParameter with integer range
     * 
     * @param k Configuration key/name
     * @param v Default value
     * @param desc Description
     * @param minInt Minimum integer value
     * @param maxInt Maximum integer value
     */
    ConfigParameter(const std::string& k, const ConfigValue& v, const std::string& desc,
                   int64_t minInt, int64_t maxInt)
        : key(k), defaultValue(v), description(desc), minIntValue(minInt), maxIntValue(maxInt) {}
    
    /**
     * @brief Construct a new ConfigParameter with probability range
     * 
     * @param k Configuration key/name
     * @param v Default value
     * @param desc Description
     */
    ConfigParameter(const std::string& k, const ConfigValue& v, const std::string& desc)
        : key(k), defaultValue(v), description(desc), probabilityRange(true) {}
};

// Validation error structure
/**
 * @struct ValidationError
 * @brief Stores a validation error message
 * 
 */
struct ValidationError {
    /**
     * @brief The configuration key that failed validation
     */
    std::string key;
    
    /**
     * @brief The validation error message
     */
    std::string message;
    
    /**
     * @brief Default constructor
     */
    ValidationError() : key(), message() {}
    
    /**
     * @brief Construct a new ValidationError
     * 
     * @param k Configuration key
     * @param msg Error message
     */
    ValidationError(const std::string& k, const std::string& msg) : key(k), message(msg) {}
};

// Configuration entry
/**
 * @struct ConfigEntry
 * @brief Stores a single configuration parameter with metadata
 * 
 * ConfigEntry represents a named configuration value with its source and
 * descriptive information for documentation purposes.
 * 
 */
struct ConfigEntry {
    /**
     * @brief The configuration key/name
     */
    std::string key;
    
    /**
     * @brief The configuration value (supports multiple types via variant)
     */
    ConfigValue value;
    
    /**
     * @brief How this value was obtained (default, file, command line, or runtime)
     */
    ConfigSource source;
    
    /**
     * @brief Human-readable description of the configuration option
     */
    std::string description;
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a new ConfigEntry
     * 
     * @param k Configuration key/name
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of the configuration option
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};
