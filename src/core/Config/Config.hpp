#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <set>
#include <cstdint>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
// Flexible type system supporting primitive types and collections
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

/** Configuration source enumeration indicating the origin of a configuration value
 * 
 * Determines priority of configuration values. Later sources override earlier ones
 * when conflicts arise, following a strict priority order.
 */
enum class ConfigSource {
public:
    /// Default configuration values loaded from internal defaults
    Default,
    /// Configuration loaded from external file (JSON, key=value, etc.)
    File,
    /// Configuration values from command line arguments
    CommandLine,
    /// Configuration values set at runtime via API
    Runtime
};

// Type validation enum
enum class ValidationType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector,
    RangeInt,
    RangeDouble,
    RegexString,
    Custom
};

// Configuration entry representing a single key-value pair
// Contains the value, its source, validation metadata, and metadata
struct ConfigEntry {
    std::string key;                    ///< Configuration key identifier
    ConfigValue value;                   ///< Configuration value
    ConfigSource source;                 ///< Source of this configuration value
    std::string description;            ///< Human-readable description of the configuration
    ValidationType validationType;       ///< Type of validation applied to this entry
    std::string validationConfig;       ///< Configuration string for validation rules (e.g., range limits)
    std::function<bool(const ConfigValue&)> customValidator; ///< Custom validation function for advanced validation rules
    
    /// Default constructor - initializes entry as default empty configuration
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    validationType(ValidationType::Custom), validationConfig(), customValidator() {}
    
    /** Constructor with all parameters
     * 
     * @param k Configuration key identifier
     * @param v Configuration value
     * @param s Source of this configuration value
     * @param desc Human-readable description of the configuration
     * @param vt Type of validation applied
     * @param vc Configuration string for validation rules
     * @param validator Custom validation function
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                ValidationType vt = ValidationType::Custom, const std::string& vc = "",
                const std::function<bool(const ConfigValue&)>& validator = nullptr)
        : key(k), value(v), source(s), description(desc), 
          validationType(vt), validationConfig(vc), customValidator(validator) {}
};

// Configuration validation result indicating whether a configuration value is valid
struct ValidationResult {
    bool valid;                        ///< Whether the validation passed
    std::string errorMessage;          ///< Descriptive error message if validation failed, empty if valid
    
    /** Constructor
     * 
     * @param v Whether validation passed
     * @param msg Error message if validation failed
     */
    ValidationResult(bool v = true, const std::string& msg = "") : valid(v), errorMessage(msg) {}
};

// Main configuration management class
// Centralized configuration system for NLM with support for multiple value types,
// source-based priority resolution, validation, and serialization.
//
// The configuration system manages application settings from multiple sources
// (defaults, files, command line, runtime) with a priority-based resolution
// mechanism. Each configuration value has metadata including validation rules,
// source tracking, and human-readable descriptions for documentation and error reporting.
//
// Usage example:
// @code
// Config& config = Config::getInstance();
// config.loadFromFile("nlm_config.json");
// config.loadFromArgs(argc, argv);
// int maxNeurons = config.getOr<int>("neuron_count", 1000);
// config.set("learning_rate", 0.01f, ConfigSource::Runtime);
// @endcode
class Config {
public:
    // Constructor and destructor
    Config();                           ///< Creates an empty configuration manager
    ~Config();                          ///< Cleans up configuration resources

    // Disable copying, enable moving
    Config(const Config&) = delete;      ///< Copy constructor disabled (non-copyable)
    Config& operator=(const Config&) = delete; ///< Copy assignment disabled (non-copyable)
    Config(Config&&) noexcept;           ///< Move constructor
    Config& operator=(Config&&) noexcept;///< Move assignment

    /** Load configuration from file (JSON format)
     * 
     * Parses configuration from a file in key=value format (or JSON in future versions).
     * Lines starting with # or // are treated as comments and ignored.
     * Later entries override earlier ones within the same file.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully opened and parsed, false otherwise
     * @throws May throw exceptions for file I/O errors or parsing failures
     * 
     * @note Phase 1 uses simple key=value format. Phase 2 will implement full JSON parsing
     * @see saveToFile()
     */
    bool loadFromFile(const std::string& filepath);

    /** Load configuration from command line arguments
     * 
     * Supports two formats:
     * - --key=value (preferred)
     * - -key value
     * 
     * Command line arguments override file and default values based on priority.
     * Boolean values can be specified as true/false, 1/0, or yes/no.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array (standard argv format)
     * @return true if parsing succeeded, false if arguments were malformed
     * 
     * @example
     * // Command line: ./nlm --neurons=2000 --learning_rate=0.001 --verbose
     * config.loadFromArgs(4, argv);  // argv[0] is program name
     */
    bool loadFromArgs(int argc, char** argv);

    /** Load default configuration values
     * 
     * Populates the configuration with sensible defaults for all settings.
     * Default values have the lowest priority (ConfigSource::Default) and can
     * be overridden by file, command line, or runtime settings.
     * 
     * @note Should be called after constructor if not loading from file/args
     * @see set()
     */
    void loadDefaults();

    /** Save configuration to file
     * 
     * Exports current configuration to a file in key=value format.
     * Only configuration values from higher-priority sources are saved
     * (File and CommandLine sources are preserved; Runtime values are not).
     * 
     * @param filepath Path where configuration should be saved
     * @return true if file was successfully written, false otherwise
     * @throws May throw exceptions for file I/O errors
     * 
     * @see loadFromFile()
     */
    bool saveToFile(const std::string& filepath) const;

    /** Get configuration value with type conversion
     * 
     * Retrieves a configuration value and attempts to convert it to the requested type.
     * If the value doesn't exist or type conversion fails, returns std::nullopt.
     * 
     * @tparam T Type to convert to (int, int64_t, double, bool, std::string, or vectors)
     * @param key Configuration key
     * @return std::optional<T> with converted value if found and valid, std::nullopt otherwise
     * 
     * @throw std::bad_variant_access if type conversion fails within variant
     * 
     * @example
     * auto neurons = config.get<int>("neuron_count");
     * if (neurons) { std::cout << "Neurons: " << *neurons << std::endl; }
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /** Get configuration value or return default
     * 
     * Convenience method that returns the value if found, otherwise returns
     * the specified default. Type conversion is performed if needed.
     * 
     * @tparam T Type to convert to
     * @param key Configuration key
     * @param defaultValue Value to return if key not found
     * @return Configuration value if found and convertible, otherwise defaultValue
     * 
     * @example
     * int max_neurons = config.getOr<int>("neuron_count", 1000);
     * // Always returns a value, never std::nullopt
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /** Get configuration value with source priority consideration
     * 
     * Returns the value only if it comes from a source with sufficient priority
     * (or equal/higher priority). Used for optional settings that should only be
     * applied when explicitly configured.
     * 
     * @tparam T Type to convert to
     * @param key Configuration key
     * @param defaultValue Default value if key not found or priority insufficient
     * @param minSource Minimum acceptable source priority (higher = more restrictive)
     * @return Value if found and priority meets requirement, otherwise defaultValue
     * 
     * @see ConfigSource enum for priority order
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, ConfigSource minSource) const;

    /** Set configuration value with explicit source
     * 
     * Assigns a configuration value with the specified source priority.
     * Higher priority sources override lower priority sources when conflicts occur.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Priority source of this configuration value
     * 
     * @example
     * config.set("simulation_timestep", 0.001, ConfigSource::Runtime);
     * config.set("neuron_count", 500, ConfigSource::CommandLine);
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    /** Set configuration value (overloaded for convenience)
     * 
     * Convenience overloads for common value types with default runtime source.
     * These provide type-safe setting of configuration values.
     * 
     * @param key Configuration key
     * @param value Configuration value (string type)
     * @param source Priority source (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);

    /** Validate configuration key exists and has appropriate type
     * 
     * Checks if a configuration key exists and whether its validation type matches
     * the expected type. Useful for validating configuration before use.
     * 
     * @param key Configuration key to validate
     * @return ValidationResult with details of any issues found
     */
    ValidationResult validateKey(const std::string& key) const;

    /** Validate a configuration value against its rules
     * 
     * Applies validation rules defined for a configuration key to a proposed value.
     * Checks type compatibility, range constraints, format requirements, etc.
     * 
     * @param key Configuration key
     * @param value Value to validate
     * @return ValidationResult indicating validity and error details
     */
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;

    /** Check if configuration value has a specific validation type
     * 
     * Queries whether a configuration entry is validated against a specific type
     * or validation rule. Useful for conditional validation logic.
     * 
     * @param key Configuration key
     * @param type Expected validation type
     * @return true if the key has this validation type
     */
    bool isValidType(const std::string& key, ValidationType type) const;

    // Validation helpers
    static ValidationResult validateInt(const ConfigValue& value, const std::string& config);
    static ValidationResult validateDouble(const ConfigValue& value, const std::string& config);
    static ValidationResult validateRange(const ConfigValue& value, const std::string& config);
    static ValidationResult validateString(const ConfigValue& value, const std::string& config);
    static ValidationResult validateCustom(const ConfigValue& value, const std::function<bool(const ConfigValue&)>& validator);

    // Type conversion functions
    static std::optional<int> toInt(const std::string& str);
    static std::optional<int64_t> toInt64(const std::string& str);
    static std::optional<double> toDouble(const std::string& str);
    static std::optional<bool> toBool(const std::string& str);
    static std::optional<std::string> toString(const ConfigValue& value);
    static std::optional<std::vector<int>> toIntVector(const std::string& str);
    static std::optional<std::vector<double>> toDoubleVector(const std::string& str);
    static std::optional<std::vector<std::string>> toStringVector(const std::string& str);

    /** Check if configuration contains a key
     * 
     * @param key Configuration key to check
     * @return true if the configuration contains this key
     */
    bool has(const std::string& key) const;

    /** Remove a configuration key
     * 
     * Deletes a configuration entry from the configuration. The key and all
     * entries for that key are removed regardless of source priority.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);

    /** Get all configuration keys
     * 
     * @return Vector of all configuration keys (unique)
     */
    std::vector<std::string> getKeys() const;

    /** Get configuration entries from a specific source
     * 
     * Retrieves all configuration entries that originated from the specified source.
     * Useful for saving only user-configured values (not defaults).
     * 
     * @param source Configuration source to filter by
     * @return Vector of ConfigEntry objects from that source
     */
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;

    /** Get the highest priority value for a key
     * 
     * Resolves conflicts by selecting the value from the highest priority source
     * among all entries with this key.
     * 
     * @param key Configuration key
     * @return std::optional<ConfigValue> with the highest priority value, or std::nullopt if key doesn't exist
     */
    std::optional<ConfigValue> getHighestPriority(const std::string& key) const;

    /** Clear all configuration
     * 
     * Removes all configuration entries, resetting the configuration to empty state.
     * This effectively undoes all previous configuration operations.
     */
    void clear();

    /** Generate a summary of all configuration
     * 
     * Creates a human-readable summary of all configuration values including
     * the key, value, and source priority for each entry.
     * 
     * @return Formatted string summary suitable for logging or debugging
     */
    std::string summary() const;

    /** Get the global configuration manager instance
     * 
     * Singleton access to the main configuration manager. Only one instance
     * exists throughout the program lifetime.
     * 
     * @return Reference to the global Config instance
     */
    static Config& getInstance();

    // Priority enumeration for source resolution
    enum class Priority {
        Lowest = 0,        ///< Lowest priority, overridden by all other sources
        Default = 1,       ///< Default values from internal defaults
        File = 2,          ///< File-based configuration
        CommandLine = 3,   ///< Command line configuration
        Runtime = 4        ///< Runtime configuration (highest priority)
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Internal helpers for configuration processing
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringEndsWith(const std::string& str, const std::string& suffix);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string formatValue(const ConfigValue& value, bool includeDescription = true) const;
    static std::string formatValue(const ConfigEntry& entry) const;

    // Type detection and conversion
    static ValidationType detectType(const std::string& str);
    static ConfigSource getSourcePriority(ConfigSource source);
};

} // namespace nlm
