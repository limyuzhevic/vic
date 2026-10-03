#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <regex>

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

// Validation result
enum class ValidationResult {
    Success,
    InvalidKey,
    InvalidType,
    InvalidRange,
    InvalidValue,
    ForbiddenOperation
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

// Configuration validation rule
struct ValidationRule {
    std::string keyPattern;  // Regex pattern for valid keys
    std::vector<std::type_index> allowedTypes;
    std::function<bool(const ConfigValue&)> valueValidator;
    bool required = false;
    std::string errorMessage;
};

class ConfigSchema {
public:
    /**
     * @brief Add a validation rule for configuration keys
     * 
     * Adds a new validation rule that defines acceptable patterns for configuration keys
     * and their expected types and validation constraints. Multiple rules can be added
     * to provide comprehensive validation.
     * 
     * @param rule The validation rule to add
     * @see ValidationRule
     * @note Key patterns are specified using regex notation
     */
    void addRule(const ValidationRule& rule);
    
    /**
     * @brief Validate a configuration key-value pair against the schema
     * 
     * Performs comprehensive validation of a configuration entry against all
     * registered rules and schema constraints. Returns a detailed validation result
     * indicating what, if anything, went wrong.
     * 
     * @param key The configuration key to validate
     * @param value The configuration value to validate
     * @return ValidationResult Detailed validation result indicating success or failure
     * @see ValidationResult
     */
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Check if a key matches any schema pattern
     * 
     * Tests whether a configuration key is syntactically valid according to
     * the schema rules. This is a quick check that doesn't validate values.
     * 
     * @param key The configuration key to check
     * @return true if the key matches any schema pattern
     */
    bool isValidKey(const std::string& key) const;
    
    /**
     * @brief Get a human-readable error message for validation failure
     * 
     * Generates a descriptive error message explaining why a configuration
     * entry failed validation. Useful for user feedback and debugging.
     * 
     * @param key The configuration key that failed validation
     * @param value The configuration value that failed validation
     * @return std::string Human-readable error message
     * @note Returns empty string if validation was successful
     */
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Clear all validation rules
     * 
     * Removes all validation rules from the schema, returning it to an empty state.
     * After calling this method, no keys or values will be validated against rules.
     * 
     * @note This operation cannot be undone
     * @see addRule
     */
    void clear();
    
    /**
     * @brief Load validation rules from a configuration file
     * 
     * Reads validation rules from a file and adds them to the schema.
     * The file format is expected to be key=value pairs with rule definitions.
     * File loading is subject to security validation.
     * 
     * @param filepath Path to the validation rules file
     * @return true if the rules were successfully loaded and parsed
     * @throws std::filesystem::filesystem_error if the file cannot be accessed
     * @note File paths are normalized to prevent directory traversal attacks
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load validation rules from a string
     * 
     * Parses validation rules from a string and adds them to the schema.
     * The string format is expected to be key=value pairs with rule definitions.
     * This method is useful for loading rules programmatically.
     * 
     * @param content String containing validation rule definitions
     * @return true if the rules were successfully parsed
     * @note String parsing is subject to the same security constraints as file loading
     */
    bool loadFromString(const std::string& content);
    
private:
    /**
     * @brief List of validation rules in the schema
     * 
     * Contains all validation rules that have been added to this schema.
     * Rules are evaluated in the order they were added.
     */
    std::vector<ValidationRule> rules_;
    
    /**
     * @brief Optimized lookup table for key pattern matching
     * 
     * Provides fast lookup of rules based on key patterns. When a rule with a
     * non-empty key pattern is added, it's also stored in this map for efficient
     * retrieval during validation.
     */
    std::unordered_map<std::string, ValidationRule> keyRuleMap_;
};

class Config {
public:
    /**
     * @brief Constructor - initializes an empty configuration
     * 
     * Creates a new Config instance with all internal structures properly initialized.
     * The configuration starts empty and requires explicit loading or setting of values
     * before use.
     */
    Config();
    
    /**
     * @brief Destructor - cleans up configuration resources
     * 
     * Properly releases all allocated memory and closes any open file handles.
     * Since this class uses the Pimpl idiom, the destructor is explicitly defined
     * to ensure complete cleanup of the implementation details.
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Copy operations are intentionally disabled to prevent shallow copies of
     * the internal implementation and ensure proper resource management.
     * @throws std::logic_error Always throws when attempting to copy
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Copy assignment operations are intentionally disabled to prevent shallow copies
     * of the internal implementation and ensure proper resource management.
     * @throws std::logic_error Always throws when attempting to copy
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (enabled)
     * 
     * Efficiently transfers ownership of internal resources from one Config object
     * to another, leaving the source object in a valid but unspecified state.
     * @param other The source Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (enabled)
     * 
     * Efficiently transfers ownership of internal resources, properly cleaning up
     * any existing resources in the target object before assignment.
     * @param other The source Config object to move from
     * @return Reference to this object after assignment
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file with security validation
     * 
     * Reads configuration data from a file using secure parsing techniques.
     * The file must be in key=value format with optional comments starting with # or /.
     * Path traversal attacks are prevented and all input is validated against
     * security rules before being accepted.
     * 
     * @param filepath Path to the configuration file to load
     * @return true if the file was successfully loaded and parsed, false otherwise
     * @throws std::filesystem::filesystem_error if the file cannot be accessed
     * @note File paths are normalized to prevent directory traversal attacks
     * @warning Configuration files are only parsed as key=value format for security
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in --key=value or -key value format.
     * All keys and values are validated for security and type safety before
     * being added to the configuration.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if all valid arguments were processed, false on error
     * @note Arguments are processed in order, with later arguments overriding earlier ones
     * @warning Command line parsing is limited to prevent injection attacks
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file with security validation
     * 
     * Writes the current configuration to a file in key=value format.
     * File paths are validated to prevent directory traversal attacks.
     * Values are properly escaped to ensure file safety.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if the file was successfully written, false otherwise
     * @throws std::filesystem::filesystem_error if the file cannot be written
     * @note File paths are normalized to prevent directory traversal attacks
     * @warning The format is intentionally simple for security reasons
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value of the specified type. If the key doesn't exist
     * or the value cannot be converted to the requested type, returns an empty optional.
     * 
     * @tparam T The type of value to retrieve (int, int64_t, double, bool, or string)
     * @param key The configuration key to retrieve
     * @return std::optional<T> The value if found and type matches, std::nullopt otherwise
     * @note Vector types are not supported by this templated version
     * @throws std::bad_variant_access if the value type doesn't match (caught internally)
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value of the specified type, returning a default value
     * if the key doesn't exist or the value cannot be converted.
     * 
     * @tparam T The type of value to retrieve (int, int64_t, double, bool, or string)
     * @param key The configuration key to retrieve
     * @param defaultValue The value to return if the key doesn't exist
     * @return T The configuration value or the default if not found
     * @note Vector types are not supported by this templated version
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with security validation
     * 
     * Sets a configuration value with the specified source (File, CommandLine, or Runtime).
     * The key and value are validated against security rules before being stored.
     * 
     * @tparam T Type of value to set (converted to ConfigValue internally)
     * @param key The configuration key to set
     * @param value The value to set
     * @param source The source of this configuration value (default: Runtime)
     * @throws std::invalid_argument if the key or value fails validation
     * @warning Keys are validated to prevent injection attacks
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a generic ConfigValue
     * 
     * Overloaded version that accepts the generic ConfigValue type directly.
     * Used internally by the templated set() functions.
     * 
     * @param key The configuration key to set
     * @param value The generic configuration value
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a string
     * 
     * Specialized version for string values with additional validation.
     * 
     * @param key The configuration key to set
     * @param value The string value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as an integer
     * 
     * Specialized version for integer values with range validation.
     * 
     * @param key The configuration key to set
     * @param value The integer value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a floating-point number
     * 
     * Specialized version for double values with range validation.
     * 
     * @param key The configuration key to set
     * @param value The floating-point value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a boolean
     * 
     * Specialized version for boolean values with validation.
     * 
     * @param key The configuration key to set
     * @param value The boolean value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Tests whether the specified key is present in the configuration.
     * 
     * @param key The configuration key to check
     * @return true if the key exists, false otherwise
     * @note Case-sensitive key matching
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key and its value
     * 
     * Removes the specified key and its associated value from the configuration.
     * The operation is performed safely with validation to prevent removal of
     * invalid or protected keys.
     * 
     * @param key The configuration key to remove
     * @note No operation is performed if the key doesn't exist
     * @warning Protected keys cannot be removed
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Retrieves a list of all keys currently in the configuration.
     * The returned vector contains all keys in insertion order.
     * 
     * @return std::vector<std::string> List of all configuration keys
     * @note The returned vector is a copy, modifications won't affect the config
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all entries from the configuration, leaving it in an empty state.
     * All sources (File, CommandLine, Runtime) are cleared simultaneously.
     * 
     * @note This operation cannot be undone
     */
    void clear();
    
    /**
     * @brief Get a formatted summary of the configuration
     * 
     * Generates a human-readable summary of all configuration entries, including
     * values and their sources. Useful for debugging and logging purposes.
     * 
     * @return std::string Formatted configuration summary
     * @note The format includes value types and source information
     */
    std::string summary() const;
    
    /**
     * @brief Check if a key is valid according to security rules
     * 
     * Validates a key against the current security rules and schema constraints.
     * This is useful for pre-validating keys before attempting to set them.
     * 
     * @param key The key to validate
     * @return true if the key is valid and safe to use
     */
    bool isValidKey(const std::string& key) const;
    
    /**
     * @brief Check if a key is in the forbidden keys list
     * 
     * Tests whether a key is in the internal list of forbidden keys that
     * should never be used for security reasons.
     * 
     * @param key The key to check
     * @return true if the key is forbidden and should not be used
     */
    bool isForbiddenKey(const std::string& key) const;
    
    /**
     * @brief Set up default security rules
     * 
     * Configures the default security validation rules for configuration keys.
     * This includes key format validation, type checking, and value validation rules.
     * Call this method to initialize the security configuration.
     * 
     * @note This method sets up comprehensive security validation rules
     * @see ValidationRule
     */
    void setupSecurityRules();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isDouble(const std::string& str);
    static std::string serializeValue(const ConfigValue& value);
    bool parseValue(const std::string& str, ConfigValue& value);
};

} // namespace nlm
