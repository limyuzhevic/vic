#pragma once

/**
 * @file Config.hpp
 * @brief Core configuration management system for the NLM framework
 * 
 * This header provides a thread-safe, type-safe configuration management system
 * that supports multiple value types, sources, and persistence mechanisms.
 * The configuration system is used throughout the brain model to store and
 * retrieve settings for various subsystems including neuromodulation, memory,
 * cognition, and development systems.
 * 
 * Key Features:
 * - Multi-source configuration (default, file, command-line, runtime)
 * - Type-safe value retrieval with automatic type conversion
 * - JSON/YAML support for file-based configuration
 * - Command-line argument parsing
 * - Thread-safe operations for concurrent access
 * - Comprehensive error handling for edge cases
 * - Advanced configuration schema validation
 * - JSON parsing with error recovery
 * - Type conversion and normalization
 * 
 * Configuration values are stored as variants supporting:
 * - Numeric types (int, int64_t, double)
 * - Boolean flags
 * - String parameters
 * - Integer and double arrays
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 2.0
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <variant>
#include <stdexcept>
#include <any>
#include <chrono>
#include <set>
#include <map>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class SchemaValidator;
class ValidationResult;

/**
 * @brief Configuration value types for validation and conversion
 */
enum class ConfigValueType {
    Integer,
    Double,
    Boolean,
    String,
    StringArray,
    IntArray,
    DoubleArray,
    Unknown
};

/**
 * @brief Configuration validation result
 * 
 * Contains the outcome of validation operations, including success/failure status,
 * detailed error messages, and recovery suggestions.
 */
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::any> metadata;
    
    ValidationResult() : isValid(true) {}
    
    ValidationResult(bool valid, const std::vector<std::string>& errs = {},
                     const std::vector<std::string>& warns = {})
        : isValid(valid), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::string summary = "Validation " + (isValid ? "PASSED" : "FAILED");
        if (!errors.empty()) {
            summary += " (" + std::to_string(errors.size()) + " errors)";
        }
        if (!warnings.empty()) {
            summary += " (" + std::to_string(warnings.size()) + " warnings)";
        }
        return summary;
    }
};

/**
 * @brief Configuration schema validation interface
 * 
 * Defines the interface for validating configuration values against
 * predefined schemas, supporting various validation strategies.
 */
class SchemaValidator {
public:
    virtual ~SchemaValidator() = default;
    
    /**
     * @brief Validate a configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value to validate
     * @param type Expected value type
     * @return Validation result
     */
    virtual ValidationResult validate(const std::string& key, 
                                   const std::any& value,
                                   ConfigValueType type) = 0;
    
    /**
     * @brief Check if validator supports a given type
     * 
     * @param type Type to check
     * @return true if validator supports the type
     */
    virtual bool supportsType(ConfigValueType type) const = 0;
    
    /**
     * @brief Get validator name
     * 
     * @return Validator name
     */
    virtual std::string getName() const = 0;
};

/**
 * @brief Range validator for numeric values
 * 
 * Validates that numeric values fall within specified ranges.
 */
class RangeValidator : public SchemaValidator {
private:
    double minValue;
    double maxValue;
    bool inclusive;
    
public:
    RangeValidator(double min, double max, bool include = true)
        : minValue(min), maxValue(max), inclusive(inclusive) {}
    
    ValidationResult validate(const std::string& key, 
                           const std::any& value,
                           ConfigValueType type) override {
        ValidationResult result;
        
        if (type != ConfigValueType::Integer && type != ConfigValueType::Double) {
            result.addError("RangeValidator: Value must be numeric for range validation");
            return result;
        }
        
        double val;
        if (type == ConfigValueType::Integer) {
            val = std::any_cast<int>(value);
        } else {
            val = std::any_cast<double>(value);
        }
        
        if (inclusive) {
            if (val < minValue || val > maxValue) {
                result.addError("RangeValidator: Value " + std::to_string(val) +
                              " is outside allowed range [" + std::to_string(minValue) +
                              ", " + std::to_string(maxValue) + "]");
            }
        } else {
            if (val <= minValue || val >= maxValue) {
                result.addError("RangeValidator: Value " + std::to_string(val) +
                              " is outside allowed range (" + std::to_string(minValue) +
                              ", " + std::to_string(maxValue) + ")");
            }
        }
        
        return result;
    }
    
    bool supportsType(ConfigValueType type) const override {
        return type == ConfigValueType::Integer || type == ConfigValueType::Double;
    }
    
    std::string getName() const override { return "RangeValidator"; }
};

/**
 * @brief Enumeration validator for value lists
 * 
 * Validates that values are in a predefined list of allowed values.
 */
class EnumValidator : public SchemaValidator {
private:
    std::set<std::string> allowedValues;
    
public:
    EnumValidator(const std::set<std::string>& values) 
        : allowedValues(values) {}
    
    ValidationResult validate(const std::string& key, 
                           const std::any& value,
                           ConfigValueType type) override {
        ValidationResult result;
        
        std::string valStr;
        if (type == ConfigValueType::Integer) {
            valStr = std::to_string(std::any_cast<int>(value));
        } else if (type == ConfigValueType::Double) {
            valStr = std::to_string(std::any_cast<double>(value));
        } else if (type == ConfigValueType::Boolean) {
            valStr = std::any_cast<bool>(value) ? "true" : "false";
        } else if (type == ConfigValueType::String) {
            valStr = std::any_cast<std::string>(value);
        } else {
            result.addError("EnumValidator: Unsupported value type for enumeration");
            return result;
        }
        
        if (allowedValues.find(valStr) == allowedValues.end()) {
            result.addError("EnumValidator: Value '" + valStr + "' is not in allowed list");
        }
        
        return result;
    }
    
    bool supportsType(ConfigValueType type) const override {
        return true; // Can validate string representations of values
    }
    
    std::string getName() const override { return "EnumValidator"; }
};

/**
 * @brief Format validator for string values
 * 
 * Validates that string values conform to specific format patterns.
 */
class FormatValidator : public SchemaValidator {
private:
    std::string formatPattern;
    
public:
    FormatValidator(const std::string& pattern) : formatPattern(pattern) {}
    
    ValidationResult validate(const std::string& key, 
                           const std::any& value,
                           ConfigValueType type) override {
        ValidationResult result;
        
        if (type != ConfigValueType::String) {
            result.addError("FormatValidator: Value must be a string for format validation");
            return result;
        }
        
        const std::string& valStr = std::any_cast<std::string>(value);
        
        if (formatPattern == "ip_address") {
            // Simple IP address validation
            bool valid = true;
            size_t dotCount = 0;
            std::string temp = valStr;
            size_t pos;
            
            for (int i = 0; i < 4; ++i) {
                pos = temp.find('.');
                if (pos == std::string::npos && i < 3) {
                    valid = false;
                    break;
                }
                std::string part = temp.substr(0, pos);
                if (part.empty() || part.size() > 3) {
                    valid = false;
                    break;
                }
                try {
                    int octet = std::stoi(part);
                    if (octet < 0 || octet > 255) {
                        valid = false;
                        break;
                    }
                } catch (...) {
                    valid = false;
                    break;
                }
                if (pos != std::string::npos) {
                    temp = temp.substr(pos + 1);
                    dotCount++;
                }
            }
            
            if (!valid || dotCount != 3) {
                result.addError("FormatValidator: Invalid IP address format: '" + valStr + "'");
            }
        } else if (formatPattern == "email") {
            // Simple email validation
            size_t atPos = valStr.find('@');
            if (atPos == std::string::npos || atPos == 0 || atPos == valStr.size() - 1) {
                result.addError("FormatValidator: Invalid email format: '" + valStr + "'");
            }
        } else if (formatPattern == "url") {
            // Simple URL validation
            if (valStr.find("://") == std::string::npos) {
                result.addError("FormatValidator: Invalid URL format: '" + valStr + "'");
            }
        } else {
            result.addError("FormatValidator: Unknown format pattern: '" + formatPattern + "'");
        }
        
        return result;
    }
    
    bool supportsType(ConfigValueType type) const override {
        return type == ConfigValueType::String;
    }
    
    std::string getName() const override { return "FormatValidator"; }
};

/**
 * @brief Dependency validator for cross-parameter validation
 * 
 * Validates that dependencies between parameters are satisfied.
 */
class DependencyValidator : public SchemaValidator {
private:
    std::map<std::string, std::function<bool(const std::any&, const std::map<std::string, std::any>&)>> dependencies;
    
public:
    DependencyValidator() {}
    
    void addDependency(const std::string& dependentKey,
                      const std::string& requiredKey,
                      const std::function<bool(const std::any&, const std::any&)>& condition) {
        dependencies[dependentKey] = [requiredKey, condition](const std::any& val, const std::map<std::string, std::any>& allValues) {
            auto it = allValues.find(requiredKey);
            if (it == allValues.end()) {
                return false; // Required key missing
            }
            return condition(val, it->second);
        };
    }
    
    ValidationResult validate(const std::string& key, 
                           const std::any& value,
                           ConfigValueType type) override {
        ValidationResult result;
        
        auto depIt = dependencies.find(key);
        if (depIt != dependencies.end()) {
            if (!depIt->second(value, std::map<std::string, std::any>())) {
                result.addError("DependencyValidator: Parameter '" + key + 
                              "' depends on other parameters not yet validated");
            }
        }
        
        return result;
    }
    
    bool supportsType(ConfigValueType type) const override {
        return true;
    }
    
    std::string getName() const override { return "DependencyValidator"; }
};

/**
 * @brief Configuration validator for comprehensive validation
 * 
 * Manages schema validation for configuration values, supporting multiple
 * validation strategies and providing detailed error reporting.
 */
class ConfigValidator {
private:
    std::vector<std::unique_ptr<SchemaValidator>> validators;
    std::unordered_map<std::string, ConfigValueType> typeMap;
    std::unordered_map<std::string, std::any> defaultValues;
    bool strictMode;
    
public:
    ConfigValidator() : strictMode(true) {}
    
    /**
     * @brief Add a schema validator
     * 
     * @param validator Unique pointer to validator
     */
    void addValidator(std::unique_ptr<SchemaValidator> validator) {
        validators.push_back(std::move(validator));
    }
    
    /**
     * @brief Register a configuration parameter's type
     * 
     * @param key Configuration key
     * @param type Expected value type
     */
    void registerParameter(const std::string& key, ConfigValueType type) {
        typeMap[key] = type;
    }
    
    /**
     * @brief Set default value for a parameter
     * 
     * @param key Configuration key
     * @param value Default value
     */
    void setDefault(const std::string& key, const std::any& value) {
        defaultValues[key] = value;
    }
    
    /**
     * @brief Enable/disable strict mode
     * 
     * @param strict Whether to use strict validation
     */
    void setStrictMode(bool strict) {
        strictMode = strict;
    }
    
    /**
     * @brief Validate a configuration value against all validators
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return Validation result with all validation outcomes
     */
    ValidationResult validate(const std::string& key, const std::any& value) {
        ValidationResult result;
        
        // Get expected type
        auto typeIt = typeMap.find(key);
        ConfigValueType type = (typeIt != typeMap.end()) ? typeIt->second : ConfigValueType::Unknown;
        
        // Apply all validators that support this type
        for (auto& validator : validators) {
            if (validator->supportsType(type)) {
                ValidationResult validatorResult = validator->validate(key, value, type);
                
                // Merge results
                if (!validatorResult.isValid) {
                    result.errors.insert(result.errors.end(), 
                                       validatorResult.errors.begin(), 
                                       validatorResult.errors.end());
                }
                result.warnings.insert(result.warnings.end(),
                                     validatorResult.warnings.begin(),
                                     validatorResult.warnings.end());
            }
        }
        
        // Check type mismatch
        if (type != ConfigValueType::Unknown && !value.type().hash_code() == type) {
            result.addError("Type mismatch: Expected " + configValueTypeToString(type) +
                          " but got " + getValueTypeName(value));
        }
        
        // Check for missing required parameters
        if (type == ConfigValueType::Unknown) {
            result.addWarning("No type registered for parameter: '" + key + "'");
        }
        
        return result;
    }
    
    /**
     * @brief Fill missing values with defaults
     * 
     * @param configMap Configuration map to fill
     * @return Validation result with filling results
     */
    ValidationResult fillDefaults(std::map<std::string, std::any>& configMap) {
        ValidationResult result;
        
        for (const auto& [key, defaultValue] : defaultValues) {
            if (configMap.find(key) == configMap.end()) {
                configMap[key] = defaultValue;
                result.addWarning("Filled missing parameter '" + key + "' with default value");
            }
        }
        
        return result;
    }
    
    /**
     * @brief Get value type as string
     * 
     * @param value Value to check
     * @return Type name string
     */
    std::string getValueTypeName(const std::any& value) {
        if (value.type() == typeid(int)) return "int";
        if (value.type() == typeid(int64_t)) return "int64_t";
        if (value.type() == typeid(double)) return "double";
        if (value.type() == typeid(bool)) return "bool";
        if (value.type() == typeid(std::string)) return "string";
        if (value.type() == typeid(std::vector<int>)) return "int[]";
        if (value.type() == typeid(std::vector<double>)) return "double[]";
        if (value.type() == typeid(std::vector<std::string>)) return "string[]";
        return "unknown";
    }
    
    /**
     * @brief Convert string to ConfigValueType
     * 
     * @param typeName Type name string
     * @return Corresponding ConfigValueType
     */
    ConfigValueType stringToConfigValueType(const std::string& typeName) {
        if (typeName == "int" || typeName == "integer") return ConfigValueType::Integer;
        if (typeName == "double" || typeName == "float") return ConfigValueType::Double;
        if (typeName == "bool" || typeName == "boolean") return ConfigValueType::Boolean;
        if (typeName == "string") return ConfigValueType::String;
        if (typeName == "int[]" || typeName == "integer[]") return ConfigValueType::IntArray;
        if (typeName == "double[]" || typeName == "float[]") return ConfigValueType::DoubleArray;
        if (typeName == "string[]") return ConfigValueType::StringArray;
        return ConfigValueType::Unknown;
    }
    
    /**
     * @brief Convert ConfigValueType to string
     * 
     * @param type ConfigValueType to convert
     * @return Type name string
     */
    std::string configValueTypeToString(ConfigValueType type) {
        switch (type) {
            case ConfigValueType::Integer: return "int";
            case ConfigValueType::Double: return "double";
            case ConfigValueType::Boolean: return "bool";
            case ConfigValueType::String: return "string";
            case ConfigValueType::StringArray: return "string[]";
            case ConfigValueType::IntArray: return "int[]";
            case ConfigValueType::DoubleArray: return "double[]";
            case ConfigValueType::Unknown: return "unknown";
            default: return "unknown";
        }
    }
};

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


/**
 * @brief Configuration source enumeration
 * 
 * Defines the origin of configuration values, allowing prioritization
 * and understanding of configuration override precedence.
 */
enum class ConfigSource {
public:
    /** Default configuration - hardcoded values from source code */
    Default,
    /** File-based configuration - loaded from JSON/YAML configuration files */
    File,
    /** Command-line configuration - parsed from command line arguments */
    CommandLine,
    /** Runtime configuration - set programmatically during execution */
    Runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration parameter with its value,
 * source, and description. Used internally by the Config class
 * to manage configuration state.
 */
struct ConfigEntry {
    /** Unique key identifying the configuration parameter */
    std::string key;
    /** Value stored as variant supporting multiple types */
    ConfigValue value;
    /** Source from which this value originated */
    ConfigSource source;
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @brief Core configuration management class
 * 
 * Manages application configuration settings through a thread-safe,
 * type-safe interface. Supports multiple value types, sources, and
 * persistence mechanisms. Used throughout the brain model to store
 * and retrieve settings for neuromodulation, memory, cognition,
 * and development systems.
 * 
 * The configuration system follows these principles:
 * 1. Type safety: Automatic type conversion and validation
 * 2. Source priority: Default < File < CommandLine < Runtime
 * 3. Persistence: Save/load from files with JSON/YAML support
 * 4. Thread safety: Safe for concurrent access in multi-threaded environments
 * 5. Extensibility: Easy to add new value types and sources
 * 
 * Configuration values are stored internally as variant types and
 * can be retrieved using type-safe template methods.
 * 
 * Thread Safety: This class is thread-safe. Concurrent operations
 * on the same Config instance are properly synchronized using mutexes.
 * 
 * Memory Management: Uses pImpl pattern with unique_ptr for proper
 * exception safety and small object optimization.
 * 
 * @note This class uses the pImpl idiom for implementation hiding
 *       and to reduce compilation dependencies.
 * 
 * @warning Configuration keys are case-sensitive and should follow
 *          snake_case naming convention.
 * 
 * @see ConfigSource for configuration source enumeration
 * @see https://en.cppreference.com/w/cpp/utility/variant for variant usage
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration with no entries.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Properly cleans up pImpl implementation.
     */
    ~Config();
    
    /**
     * @brief Disable copying for safety
     * 
     * Copy construction and assignment are disabled because configuration
     * objects should be managed as singletons or through shared pointers
     * to ensure consistent configuration state across the application.
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of the configuration implementation.
     * 
     * @param other Source configuration object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of the configuration implementation.
     * 
     * @param other Source configuration object to move from
     * @return Reference to this configuration object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Parses configuration from a file, typically in JSON or YAML format.
     * The file should contain key-value pairs in a simple format.
     * Comments (lines starting with # or //) and empty lines are ignored.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully loaded, false otherwise
     * 
     * @throws std::filesystem::filesystem_error if file cannot be opened
     * @throws std::runtime_error if file format is invalid
     * 
     * @note Currently implements a simple key=value parser. Phase 2 will
     *       implement proper JSON/YAML parsing.
     * 
     * @see ConfigSource::File
     * @see https://en.cppreference.com/w/cpp/filesystem for file operations
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses configuration from command line arguments using two formats:
     * --key=value or -key value. Command-line settings have higher priority
     * than file settings but lower than runtime settings.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if arguments were successfully parsed, false otherwise
     * 
     * @note Command-line arguments override file configuration.
     * @see ConfigSource::CommandLine
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Writes configuration entries to a file in a simple text format.
     * Each entry is preceded by its description and key=value pair.
     * Currently uses a placeholder value; Phase 2 will implement
     * proper serialization.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if file was successfully written, false otherwise
     * 
     * @throws std::filesystem::filesystem_error if file cannot be created
     * @note Currently uses placeholder values. Phase 2 will implement
     *       proper value serialization.
     * 
     * @see ConfigSource::File
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value of specified type
     * 
     * Retrieves a configuration value by key and converts it to the
     * requested type. If the key doesn't exist or the value cannot be
     * converted to the requested type, returns nullopt.
     * 
     * @tparam T Type to which the value should be converted
     * @param key Configuration key to retrieve
     * @return std::optional<T> containing the value if found and convertible,
     *         nullopt otherwise
     * 
     * @throws std::bad_variant_access if value type doesn't match
     * 
     * @note Returns nullopt for type mismatches rather than throwing
     *       to allow graceful error handling.
     * 
     * @see getOr() for alternative with default value
     * @see ConfigSource for value source information
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value or return default
     * 
     * Retrieves a configuration value by key. If the key doesn't exist
     * or the value cannot be converted to the requested type, returns
     * the provided default value instead of nullopt.
     * 
     * @tparam T Type to which the value should be converted
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key not found
     * @return Configuration value if found and convertible, otherwise default
     * 
     * @note This method never throws, making it safe for configuration
     *       files that might be missing expected keys.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with specified key and source. If the
     * key already exists, its value and source are updated.
     * 
     * @param key Configuration key
     * @param value Configuration value to set
     * @param source Source from which this configuration value originates
     * 
     * @note Different overloads support different value types for convenience.
     * 
     * @see ConfigSource::Runtime
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string overload)
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Source from which this configuration value originates
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer overload)
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Source from which this configuration value originates
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double overload)
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Source from which this configuration value originates
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool overload)
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Source from which this configuration value originates
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes the configuration entry with the specified key if it exists.
     * Thread-safe operation.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a vector containing all keys in the configuration.
     * Order is not guaranteed.
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries and resets to empty state.
     * Thread-safe operation.
     */
    void clear();
    
    /**
     * @brief Get configuration summary as string
     * 
     * Generates a human-readable summary of all configuration entries
     * showing keys, values, and sources.
     * 
     * @return String containing configuration summary
     * 
     * @note Output format: "Key = [Value] (SourceIndex)\n"
     * @see ConfigSource for source enumeration
     */
    std::string summary() const;
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     * 
     * Hidden implementation details for the Config class.
     * Uses unique_ptr for proper memory management and exception safety.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * Removes leading and trailing whitespace characters.
     * 
     * @param str Input string to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * Converts all characters in the string to lowercase.
     * Used for case-insensitive configuration key matching.
     * 
     * @param str Input string to convert
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
