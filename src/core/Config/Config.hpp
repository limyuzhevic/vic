#pragma once

/**
 * Configuration System for NLM (Neural Learning Machine)
 * 
 * This file defines an enhanced configuration system for the NLM neural simulation framework.
 * The system provides:
 * - Type-safe configuration value storage
 * - Comprehensive validation framework
 * - Multiple configuration sources (file, command line, runtime)
 * - Rich type support (primitives, arrays, strings)
 * - Schema validation and custom validation rules
 * 
 * @author NLM Development Team
 * @version 2.0
 * @date 2026
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations for circular dependencies
class Config;

/**
 * Enhanced configuration value types supporting complex data structures.
 * 
 * The ConfigValue type system supports:
 * - Basic types: int, int64_t, double, bool, string
 * - Collection types: arrays of numeric or string values
 * - Type safety through std::variant for static type checking
 * - Flexible value storage for neural simulation parameters
 * 
 * @note Use template get<T>() for type-safe access
 * @warning Variant access can throw bad_variant_access if type mismatch
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

/**
 * Enumeration of configuration value sources.
 * 
 * Configuration sources are ordered by priority (higher number = higher priority):
 * Runtime > CommandLine > File > Default
 * 
 * This ordering determines how values are resolved when multiple sources define
 * the same configuration key.
 */
enum class ConfigSource {
public:
    /** Configuration defined in default configuration files */
    Default = 0,
    
    /** Configuration loaded from external files (JSON, YAML, etc.) */
    File = 1,
    
    /** Configuration passed via command line arguments */
    CommandLine = 2,
    
    /** Configuration set programmatically at runtime */
    Runtime = 3
};

/**
 * Comprehensive validation result structure.
 * 
 * Provides detailed feedback about configuration validation including:
 * - Overall validation status
 * - Specific error messages with detailed explanations
 * - Warning messages for potential issues
 * - Helper methods for error/warning checking
 * 
 * Used by validation framework to provide actionable feedback.
 */
struct ValidationResult {
    /** Overall validation status - true if no errors */
    bool isValid;
    
    /** List of validation error messages */
    std::vector<std::string> errors;
    
    /** List of validation warning messages */
    std::vector<std::string> warnings;
    
    /**
     * Default constructor creates valid empty result.
     */
    ValidationResult() : isValid(true) {}
    
    /**
     * Constructor for invalid results with error message.
     * 
     * @param valid Initial validation status
     * @param error Error message to add
     */
    ValidationResult(bool valid, const std::string& error) : isValid(valid) {
        errors.push_back(error);
    }
    
    /**
     * Add an error message to the validation result.
     * 
     * @param error Error message to add
     */
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    /**
     * Add a warning message to the validation result.
     * 
     * @param warning Warning message to add
     */
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    /**
     * Check if validation result contains errors.
     * 
     * @return True if any errors are present
     */
    bool hasErrors() const { return !errors.empty(); }
    
    /**
     * Check if validation result contains warnings.
     * 
     * @return True if any warnings are present
     */
    bool hasWarnings() const { return !warnings.empty(); }
};

/**
 * Type validation function signature for custom type checking.
 * 
 * User-defined validation functions should implement this signature to
 * provide custom type validation logic for configuration values.
 * 
 * @param value Configuration value to validate
 * @return True if value passes type validation
 */
using TypeValidator = std::function<bool(const ConfigValue&)>;

/**
 * Range validation function signature for numeric value checking.
 * 
 * User-defined validation functions should implement this signature to
 * provide custom range validation logic for numeric configuration values.
 * 
 * @param value Numeric value to validate
 * @return True if value passes range validation
 */
using RangeValidator = std::function<bool(double)>;

/**
 * Enumeration validation function signature for value set validation.
 * 
 * User-defined validation functions should implement this signature to
 * provide custom enumeration validation logic for string configuration values.
 * 
 * @param value String value to validate
 * @return True if value is a valid enumeration value
 */
using EnumValidator = std::function<bool(const std::string&)>;

/**
 * Enhanced configuration entry with validation support.
 * 
 * Extends basic configuration entries with type information and validation rules
 * to enable comprehensive validation and type safety.
 * 
 * @see ConfigEntry for basic implementation
 * @see ValidationResult for validation details
 */
struct ConfigEntry {
    /** Configuration key/name */
    std::string key;
    
    /** Stored configuration value */
    ConfigValue value;
    
    /** Source of this configuration value */
    ConfigSource source;
    
    /** Human-readable description of configuration purpose */
    std::string description;
    
    /** Inferred or explicit type name for validation */
    std::string type;
    
    /** Custom validation rules (regex, patterns, constraints) */
    std::string validationRules;
    
    /**
     * Default constructor creates empty entry.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * Basic constructor for simple configuration entries.
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of configuration purpose
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type("auto"), validationRules("") {}
    
    /**
     * Advanced constructor for entries with type validation.
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of configuration purpose
     * @param t Explicit type name for validation
     * @param rules Custom validation rules
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::string& t, const std::string& rules)
        : key(k), value(v), source(s), description(desc), type(t), validationRules(rules) {}
};

/**
 * Main configuration class with comprehensive validation and type safety.
 * 
 * Provides a robust configuration management system for the NLM neural simulation
 * framework with the following features:
 * 
 * - **Type Safety**: Template-based type-safe value access
 * - **Validation Framework**: Comprehensive validation with custom rules
 * - **Multiple Sources**: File, command line, and runtime configuration
 * - **Schema Management**: Custom type, range, and enumeration validators
 * - **Value Serialization**: Bidirectional serialization for persistence
 * - **Merge Operations**: Configuration merging with priority handling
 * - **Statistics and Analytics**: Entry counting and source analysis
 * 
 * @note Configuration values are immutable after setting (copy-on-write semantics)
 * @warning Type mismatches can cause std::bad_variant_access exceptions
 * @see ValidationResult for validation details
 * @see ConfigSource for source priority
 */
class Config {
public:
    /**
     * Default constructor creates empty configuration.
     */
    Config();
    
    /**
     * Destructor cleans up implementation details.
     */
    ~Config();
    
    /**
     * Disable copying to prevent shallow copies of implementation.
     */
    Config(const Config&) = delete;
    
    /**
     * Disable assignment to prevent shallow copies of implementation.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * Move constructor transfers ownership of implementation.
     */
    Config(Config&&) noexcept;
    
    /**
     * Move assignment transfers ownership of implementation.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * Load configuration from file (JSON/YAML format).
     * 
     * Supports multiple file formats through template implementations.
     * Currently implements basic key=value format for Phase 2.
     * 
     * @param filepath Path to configuration file
     * @return True if file was successfully loaded
     * @see saveToFile() for serialization
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Load configuration from command line arguments.
     * 
     * Supports standard command line formats:
     * --key=value (long option)
     * -key value (short option)
     * 
     * @param argc Argument count (from main())
     * @param argv Argument vector (from main())
     * @return True if arguments were successfully parsed
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * Save configuration to file with proper serialization.
     * 
     * Converts configuration values to human-readable format with:
     * - Proper quoting for string values
     * - Array syntax for collection values
     * - Boolean literals (true/false)
     * - Comments for each configuration entry
     * 
     * @param filepath Output file path
     * @return True if file was successfully saved
     * @see loadFromFile() for parsing
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Type-safe value access with automatic type checking.
     * 
     * Provides safe access to configuration values with compile-time type checking.
     * Returns std::nullopt if key doesn't exist or type mismatch occurs.
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @return Value if found and type matches, std::nullopt otherwise
     * @see getOr() for default values
     * @see set() for value assignment
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Value access with default fallback.
     * 
     * Retrieves configuration value or returns default if not found or
     * type mismatch occurs. Provides safe default value handling.
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key not found or type mismatch
     * @return Retrieved value or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set configuration value with basic validation.
     * 
     * Assigns configuration value with type checking and automatic
     * type inference for unknown types.
     * 
     * @param key Configuration key
     * @param value Configuration value to store
     * @param source Source of configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with string type.
     * 
     * Convenience overload for string configuration values.
     * 
     * @param key Configuration key
     * @param value String value to store
     * @param source Source of configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with integer type.
     * 
     * Convenience overload for integer configuration values.
     * 
     * @param key Configuration key
     * @param value Integer value to store
     * @param source Source of configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with double type.
     * 
     * Convenience overload for floating-point configuration values.
     * 
     * @param key Configuration key
     * @param value Double value to store
     * @param source Source of configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with boolean type.
     * 
     * Convenience overload for boolean configuration values.
     * 
     * @param key Configuration key
     * @param value Boolean value to store
     * @param source Source of configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with type and validation rules.
     * 
     * Advanced configuration setting with explicit type and validation rules.
     * Enables custom validation for specific configuration keys.
     * 
     * @param key Configuration key
     * @param value Configuration value to store
     * @param type Explicit type name for validation
     * @param validationRules Custom validation rules (regex, constraints)
     * @param source Source of configuration value
     */
    void set(const std::string& key, const ConfigValue& value, const std::string& type,
             const std::string& validationRules = "", ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Perform comprehensive validation of entire configuration.
     * 
     * Validates all configuration entries against their respective
     * type, range, and enumeration validators. Provides detailed error
     * and warning reporting.
     * 
     * @return ValidationResult containing all validation issues
     * @see validate(const std::string& key) for single-key validation
     */
    ValidationResult validate() const;
    
    /**
     * Validate a single configuration key.
     * 
     * Performs type, range, and enumeration validation for a specific
     * configuration key. Useful for real-time validation during execution.
     * 
     * @param key Configuration key to validate
     * @return ValidationResult for the specific key
     */
    ValidationResult validate(const std::string& key) const;
    
    /**
     * Check if configuration value matches expected type.
     * 
     * Performs type validation without additional constraints. Useful
     * for runtime type checking and dynamic validation.
     * 
     * @param key Configuration key to validate
     * @param expectedType Expected type name (e.g., "int", "string", "double[]")
     * @return True if value matches expected type
     */
    bool validateType(const std::string& key, const std::string& expectedType) const;
    
    /**
     * Check if configuration value falls within specified range.
     * 
     * Validates numeric configuration values against minimum and maximum
     * constraints. Only applies to numeric types (int, double).
     * 
     * @param key Configuration key to validate
     * @param min Minimum allowed value (inclusive)
     * @param max Maximum allowed value (inclusive)
     * @return True if value within range or not numeric
     */
    bool validateRange(const std::string& key, double min, double max) const;
    
    /**
     * Check if configuration value is a valid enumeration value.
     * 
     * Validates string configuration values against allowed value set.
     * Only applies to string types.
     * 
     * @param key Configuration key to validate
     * @param allowedValues Set of allowed string values
     * @return True if value is in allowed set or not string type
     */
    bool validateEnum(const std::string& key, const std::vector<std::string>& allowedValues) const;
    
    /**
     * Check if configuration key exists.
     * 
     * @param key Configuration key to check
     * @return True if key is present in configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove configuration key from configuration.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * Get all configuration keys.
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Clear all configuration entries.
     * 
     * Removes all configuration values and resets validation state.
     */
    void clear();
    
    /**
     * Get human-readable configuration summary.
     * 
     * Provides formatted overview of configuration including:
     * - Entry count
     * - Value types and sources
     * - Validation status
     * - Usage statistics
     * 
     * @return Formatted configuration summary
     */
    std::string summary() const;
    
    /**
     * Get all validation errors as formatted string.
     * 
     * @return String containing all validation error messages
     */
    std::string getValidationErrors() const;
    
    /**
     * Get all validation warnings as formatted string.
     * 
     * @return String containing all validation warning messages
     */
    std::string getValidationWarnings() const;
    
    /**
     * Add custom type validator for specific type name.
     * 
     * Registers a user-defined type validator for custom type validation.
     * Validators are used during validation() calls for matching types.
     * 
     * @param typeName Type name to register validator for
     * @param validator Function that validates values of this type
     */
    void addTypeValidator(const std::string& typeName, TypeValidator validator);
    
    /**
     * Add custom range validator for specific type name.
     * 
     * Registers a user-defined range validator for numeric type validation.
     * Validators are used during validation() calls for matching types.
     * 
     * @param typeName Type name to register range validator for
     * @param validator Function that validates ranges for this type
     */
    void addRangeValidator(const std::string& typeName, RangeValidator validator);
    
    /**
     * Add custom enumeration validator for specific type name.
     * 
     * Registers a user-defined enumeration validator for string validation.
     * Validators are used during validation() calls for matching types.
     * 
     * @param typeName Type name to register enum validator for
     * @param validator Function that validates enumerations for this type
     */
    void addEnumValidator(const std::string& typeName, EnumValidator validator);
    
    /**
     * Merge another configuration into this one.
     * 
     * Combines configuration from another Config object, with values
     * from the source Config taking precedence (higher priority).
     * 
     * @param other Configuration to merge into this one
     * @param source Priority source for merged values
     */
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Get total number of configuration entries.
     * 
     * @return Total count of all configuration entries
     */
    size_t getEntryCount() const;
    
    /**
     * Get number of configuration entries from specific source.
     * 
     * @param source Configuration source to count
     * @return Number of entries from specified source
     */
    size_t getEntriesBySource(ConfigSource source) const;
    
private:
    /**
     * Internal implementation structure for PIMPL pattern.
     * 
     * Contains all configuration data and validation schema. Forward-declared
     * in header to hide implementation details.
     */
    struct Impl;
    
    /**
     * Pointer to internal implementation (PIMPL idiom).
     * 
     * Encapsulates implementation details and provides value semantics
     * through controlled copying and moving.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Helper method to trim whitespace from strings.
     * 
     * Removes leading and trailing whitespace characters for proper
     * parsing and string processing.
     * 
     * @param str Input string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * Helper method to convert string to lowercase.
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * Helper method to convert string to uppercase.
     * 
     * @param str Input string
     * @return Uppercase version of input string
     */
    static std::string toUpper(const std::string& str);
    
    /**
     * Check if string represents a valid integer value.
     * 
     * @param str String to parse as integer
     * @param value Output parameter for parsed integer value
     * @return True if string is valid integer
     */
    static bool isInteger(const std::string& str, int64_t& value);
    
    /**
     * Check if string represents a valid double value.
     * 
     * @param str String to parse as double
     * @param value Output parameter for parsed double value
     * @return True if string is valid double
     */
    static bool isDouble(const std::string& str, double& value);
    
    /**
     * Check if string represents a valid boolean value.
     * 
     * @param str String to parse as boolean
     * @param value Output parameter for parsed boolean value
     * @return True if string is valid boolean
     */
    static bool isBoolean(const std::string& str, bool& value);
    
    /**
     * Serialize configuration value to string representation.
     * 
     * Converts ConfigValue to human-readable string format suitable for:
     * - File storage
     * - Logging
     * - Error reporting
     * 
     * @param value Configuration value to serialize
     * @return String representation of value
     */
    static std::string serializeValue(const ConfigValue& value);
    
    /**
     * Deserialize string to configuration value.
     * 
     * Parses string representation back to ConfigValue for:
     * - File loading
     * - Configuration recovery
     * - Dynamic configuration
     * 
     * @param str String to deserialize
     * @param value Output parameter for deserialized value
     * @return True if string was successfully parsed
     */
    static bool deserializeValue(const std::string& str, ConfigValue& value);
    
    /**
     * Infer configuration value type from value content.
     * 
     * Automatically determines the type of configuration value for
     * validation and type checking purposes. Used when type is not explicitly
     * specified in configuration entry.
     * 
     * @param value Configuration value to analyze
     * @return Inferred type name (e.g., "int", "string", "double[]")
     */
    static std::string inferType(const ConfigValue& value);
};

} ; // namespace nlm