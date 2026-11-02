#pragma once

/**
 * @file ConfigError.hpp
 * @brief Error handling for the NLM configuration system
 * 
 * This file defines comprehensive error classes for configuration-related
 * operations in the NLM brain simulation. The error handling system provides
 * detailed error information for configuration parsing, validation, and
 * file operations.
 * 
 * @author NLM Research Team
 * @date Phase 6 Enhancement
 * @version 2.0
 * 
 * @error_categories
 * - ConfigLoadError: File loading and parsing errors
 * - ConfigValidationError: Configuration validation errors
 * - ConfigRangeError: Value range validation errors
 * - ConfigTypeError: Type conversion and validation errors
 * - ConfigSecurityError: Security-related configuration errors
 */

#include <stdexcept>
#include <string>
#include <vector>

namespace nlm {

/**
 * @class ConfigError
 * @brief Base exception class for all configuration errors
 * 
 * This is the base class for all configuration-related exceptions.
 * All configuration errors derive from this class, allowing for
 * consistent error handling across the configuration system.
 * 
 * @details The ConfigError class provides:
 * - Error category identification
 * - Detailed error messages
 * - Error code for programmatic error handling
 * - Context information for debugging
 * 
 * @note All configuration exceptions should be caught using
 *       std::exception and appropriate error handling mechanisms.
 */
class ConfigError : public std::runtime_error {
protected:
    /**
     * @brief Protected constructor to enforce use of derived classes
     * 
     * @param message Detailed error message
     * @param errorCode Numeric error code for programmatic handling
     */
    ConfigError(const std::string& message, int errorCode = 1)
        : std::runtime_error(message), errorCode_(errorCode) {}
    
public:
    /**
     * @brief Get the numeric error code
     * 
     * @return Error code for programmatic error handling
     */
    virtual int getErrorCode() const { return errorCode_; }
    
    /**
     * @brief Get the error category
     * 
     * @return String representing the error category
     */
    virtual const char* getErrorCategory() const { return "ConfigError"; }
    
private:
    int errorCode_;
};

/**
 * @class ConfigLoadError
 * @brief Exception thrown during configuration file loading
 * 
 * This exception is thrown when errors occur during configuration file
 * loading, including file not found, permission issues, and parsing errors.
 * It provides specific information about what went wrong during loading.
 */
class ConfigLoadError : public ConfigError {
public:
    /**
     * @brief Construct a load error for file not found
     * 
     * @param filepath Path to the file that was not found
     */
    ConfigLoadError(const std::string& filepath)
        : ConfigError("Configuration file not found: " + filepath, 100) {}
    
    /**
     * @brief Construct a load error for permission denied
     * 
     * @param filepath Path to the file with permission issues
     */
    ConfigLoadError(const std::string& filepath, const std::string& reason)
        : ConfigError("Permission denied accessing configuration file " + filepath + ": " + reason, 101) {}
    
    /**
     * @brief Construct a load error for parsing failure
     * 
     * @param filepath Path to the file with parsing issues
     * @param lineNumber Line number where parsing failed
     * @param details Parsing error details
     */
    ConfigLoadError(const std::string& filepath, int lineNumber, const std::string& details)
        : ConfigError("Configuration file parsing error in " + filepath + " at line " + 
                     std::to_string(lineNumber) + ": " + details, 102) {}
    
    /**
     * @brief Construct a load error for malformed content
     * 
     * @param filepath Path to the file with malformed content
     * @param reason Description of malformed content
     */
    ConfigLoadError(const std::string& filepath, const std::string& reason, const std::string& malformedContent)
        : ConfigError("Configuration file " + filepath + " contains malformed content: " + 
                     reason + ". Content: " + malformedContent, 103) {}
    
    const char* getErrorCategory() const override { return "ConfigLoadError"; }
};

/**
 * @class ConfigValidationError
 * @brief Exception thrown for configuration validation failures
 * 
 * This exception is thrown when configuration values fail validation,
 * including type mismatches, range violations, and required field errors.
 * It helps users identify and fix configuration issues.
 */
class ConfigValidationError : public ConfigError {
public:
    /**
     * @brief Construct a validation error for type mismatch
     * 
     * @param key Configuration key with type mismatch
     * @param expected Expected type
     * @param actual Actual type
     */
    ConfigValidationError(const std::string& key, const std::string& expected, const std::string& actual)
        : ConfigError("Configuration key '" + key + "' type mismatch: expected " + expected + 
                     ", got " + actual, 200) {}
    
    /**
     * @brief Construct a validation error for range violation
     * 
     * @param key Configuration key with range violation
     * @param value Value that violated the range
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     */
    ConfigValidationError(const std::string& key, double value, double min, double max)
        : ConfigError("Configuration key '" + key + "' value " + std::to_string(value) + 
                     " outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]", 201) {}
    
    /**
     * @brief Construct a validation error for required key
     * 
     * @param key Missing required configuration key
     */
    ConfigValidationError(const std::string& key)
        : ConfigError("Required configuration key is missing: '" + key + "'", 202) {}
    
    /**
     * @brief Construct a validation error for invalid key name
     * 
     * @param key Invalid configuration key name
     */
    ConfigValidationError(const std::string& key, const std::string& reason)
        : ConfigError("Invalid configuration key name '" + key + "': " + reason, 203) {}
    
    const char* getErrorCategory() const override { return "ConfigValidationError"; }
};

/**
 * @class ConfigSecurityError
 * @brief Exception thrown for security-related configuration issues
 * 
 * This exception is thrown when configuration values could pose security
 * risks or when input validation fails for security reasons. It protects
 * the system from malicious or malformed configuration inputs.
 */
class ConfigSecurityError : public ConfigError {
public:
    /**
     * @brief Construct a security error for buffer overflow attempt
     * 
     * @param key Configuration key that could cause buffer overflow
     * @param value Value that exceeds safe limits
     */
    ConfigSecurityError(const std::string& key, const std::string& value)
        : ConfigError("Potential buffer overflow in configuration key '" + key + ": " + value, 300) {}
    
    /**
     * @brief Construct a security error for injection attempt
     * 
     * @param key Configuration key with injection attempt
     * @param value Value containing suspicious characters
     */
    ConfigSecurityError(const std::string& key, const std::string& value, const std::string& threat)
        : ConfigError("Security threat detected in configuration key '" + key + ": " + threat + ". Value: " + value, 301) {}
    
    /**
     * @brief Construct a security error for excessive resource allocation
     * 
     * @param key Configuration key allocating excessive resources
     * @param value Resource allocation value
     */
    ConfigSecurityError(const std::string& key, double value, double maxSafeValue)
        : ConfigError("Excessive resource allocation in configuration key '" + key + ": " + 
                     std::to_string(value) + " exceeds safe limit of " + std::to_string(maxSafeValue), 302) {}
    
    const char* getErrorCategory() const override { return "ConfigSecurityError"; }
};

/**
 * @class ConfigValueError
 * @brief Exception thrown for specific value conversion errors
 * 
 * This exception is thrown when configuration values cannot be converted
 * to the requested type or when specific value constraints are violated.
 * It provides detailed information about conversion failures.
 */
class ConfigValueError : public ConfigError {
public:
    /**
     * @brief Construct a value error for failed type conversion
     * 
     * @param key Configuration key with conversion failure
     * @param value Original value
     * @param targetType Target type
     * @param reason Conversion failure reason
     */
    ConfigValueError(const std::string& key, const std::string& value, const std::string& targetType, const std::string& reason)
        : ConfigError("Cannot convert configuration key '" + key + "' value '" + value + "' to " + targetType + ": " + reason, 400) {}
    
    /**
     * @brief Construct a value error for invalid format
     * 
     * @param key Configuration key with invalid format
     * @param value Invalid value
     * @param expectedFormat Expected format
     */
    ConfigValueError(const std::string& key, const std::string& value, const std::string& expectedFormat)
        : ConfigError("Invalid format for configuration key '" + key + "': expected '" + expectedFormat + ", got '" + value + "'", 401) {}
    
    /**
     * @brief Construct a value error for missing required field
     * 
     * @param key Configuration key with missing required field
     * @param field Missing field name
     */
    ConfigValueError(const std::string& key, const std::string& field)
        : ConfigError("Missing required field '" + field + "' in configuration key '" + key + "'", 402) {}
    
    const char* getErrorCategory() const override { return "ConfigValueError"; }
};

/**
 * @class ConfigIOException
 * @brief Exception thrown for I/O operations on configuration files
 * 
 * This exception is thrown when errors occur during file operations,
 * including reading, writing, and file system operations. It provides
 * detailed information about I/O failures.
 */
class ConfigIOException : public ConfigError {
public:
    /**
     * @brief Construct an I/O error for file read failure
     * 
     * @param filepath Path to file that couldn't be read
     * @param reason Reason for read failure
     */
    ConfigIOException(const std::string& filepath, const std::string& reason)
        : ConfigError("Failed to read configuration file '" + filepath + "': " + reason, 500) {}
    
    /**
     * @brief Construct an I/O error for file write failure
     * 
     * @param filepath Path to file that couldn't be written
     * @param reason Reason for write failure
     */
    ConfigIOException(const std::string& filepath, const std::string& reason, bool isWrite = true)
        : ConfigError(std::string("Failed to ") + (isWrite ? "write" : "read") + 
                     " configuration file '" + filepath + "': " + reason, 501) {}
    
    /**
     * @brief Construct an I/O error for file permission
     * 
     * @param filepath Path to file with permission issues
     */
    ConfigIOException(const std::string& filepath, const std::string& reason, bool isWrite, bool isPermission)
        : ConfigError(std::string("Permission error ") + (isWrite ? "writing to" : "reading") + 
                     " configuration file '" + filepath + "': " + reason, 502) {}
    
    const char* getErrorCategory() const override { return "ConfigIOException"; }
};

/**
 * @brief Exception thrown for argument parsing errors
 * 
 * This exception is thrown when command-line argument parsing fails,
 * including invalid argument formats and missing required arguments.
 */
class ConfigArgError : public ConfigError {
public:
    /**
     * @brief Construct an argument error for invalid argument format
     * 
     * @param arg Argument that failed validation
     * @param expected Expected format
     */
    ConfigArgError(const std::string& arg, const std::string& expected)
        : ConfigError("Invalid argument format: expected '" + expected + ", got '" + arg + "'", 600) {}
    
    /**
     * @brief Construct an argument error for missing required argument
     * 
     * @param arg Required argument that was missing
     */
    ConfigArgError(const std::string& arg)
        : ConfigError("Missing required argument: '" + arg + "'", 601) {}
    
    /**
     * @brief Construct an argument error for conflicting arguments
     * 
     * @param arg1 First conflicting argument
     * @param arg2 Second conflicting argument
     */
    ConfigArgError(const std::string& arg1, const std::string& arg2)
        : ConfigError("Conflicting arguments: '" + arg1 + "' and '" + arg2 + "'", 602) {}
    
    const char* getErrorCategory() const override { return "ConfigArgError"; }
};

/**
 * @typedef ConfigErrorPtr
 * @brief Smart pointer for configuration error handling
 * 
 * This type provides a convenient way to handle configuration errors
 * using modern C++ practices. It automatically manages memory for
 * exception objects and provides reference counting.
 */
using ConfigErrorPtr = std::shared_ptr<ConfigError>;

/**
 * @typedef ConfigErrorVector
 * @brief Vector for collecting multiple configuration errors
 * 
 * This type is used for collecting multiple configuration errors that
 * occur during validation or loading. It allows for comprehensive error
 * reporting with all detected issues.
 */
using ConfigErrorVector = std::vector<ConfigErrorPtr>;

/**
 * @brief Helper function to validate key names
 * 
 * @param key Key name to validate
 * @throws ConfigSecurityError if key name is invalid
 */
void validateKeyName(const std::string& key);

/**
 * @brief Helper function to validate value ranges
 * 
 * @param key Configuration key
 * @param value Value to validate
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @throws ConfigValidationError if value is out of range
 * @throws ConfigSecurityError if value could cause security issues
 */
void validateValueRange(const std::string& key, double value, double min, double max);

/**
 * @brief Helper function to check for security threats
 * 
 * @param key Configuration key
 * @param value Value to check
 * @throws ConfigSecurityError if value contains security threats
 */
void checkSecurityThreats(const std::string& key, const std::string& value);

/**
 * @brief Helper function to validate string value format
 * 
 * @param key Configuration key
 * @param value String value to validate
 * @param expectedFormat Expected format
 * @throws ConfigValueError if format is invalid
 */
void validateStringFormat(const std::string& key, const std::string& value, const std::string& expectedFormat);

} // namespace nlm