#pragma once

/**
 * @file Config.hpp
 * @brief Core configuration management system for NLM
 * 
 * Comprehensive configuration system providing:
 * - Key-value storage with multiple configuration sources
 * - JSON file I/O with automatic backup and recovery
 * - Command-line argument parsing
 * - Type-safe value access with automatic conversion
 * - Schema validation and parameter validation
 * - Thread-safe concurrent access
 * - Comprehensive error handling and recovery
 * 
 * Configuration sources and precedence (highest to lowest):
 * 1. Runtime (set() with ConfigSource::Runtime)
 * 2. Command Line (loadFromArgs())
 * 3. File (loadFromFile())
 * 4. Default (setDefaultConfig())
 * 
 * Supported value types:
 * - int, int64_t
 * - double
 * - bool
 * - std::string
 * - std::vector<int>
 * - std::vector<double>
 * - std::vector<std::string>
 * 
 * @author NLM Development Team
 * @date Phase 1 Implementation
 * @version 1.0
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include <cstring>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Error codes for configuration operations
/**
 * @enum ConfigErrorCode
 * @brief Comprehensive error codes for configuration operations
 * 
 * Detailed error codes for all configuration failures:
 * @error Success - Operation completed successfully
 * @error FileNotFound - Configuration file does not exist
 * @error FilePermissionDenied - Insufficient permissions to access file
 * @error FileReadError - Error reading configuration file content
 * @error FileWriteError - Error writing configuration file content
 * @error JsonParseError - JSON parsing failed (syntax error, invalid format)
 * @error JsonValidationError - JSON validation failed (schema violation)
 * @error InvalidKeyFormat - Configuration key name does not match pattern
 * @error InvalidValueType - Attempted to access value as wrong type
 * @error InvalidValueRange - Numeric value outside valid range
 * @error DuplicateKey - Multiple entries with same key (duplicate)
 * @error SchemaValidationError - Configuration fails schema validation
 * @error ThreadLockTimeout - Thread synchronization timeout
 * @error ThreadLockError - Thread synchronization failure
 * @error BackupError - Backup operation failed
 * @error RestoreError - Restore operation failed
 * @error TransactionError - Configuration transaction failed
 * @error UnknownError - Unspecified or unexpected error
 * 
 * @see getLastError() for retrieving current error code
 * @see errorCodeToString() for human-readable error messages
 * 
 * Usage example:
 * @code{.cpp}
 * try {
 *     config.loadFromFile("config.json");
 *     if (Config::getLastError() != ConfigErrorCode::Success) {
 *         std::cerr << "Load failed: " 
 *                   << Config::errorCodeToString(Config::getLastError());
 *     }
 * } catch (const ConfigException& e) {
 *     // Handle exception
 * }
 * @endcode
 */
enum class ConfigErrorCode {
    Success = 0,
    FileNotFound,
    FilePermissionDenied,
    FileReadError,
    FileWriteError,
    JsonParseError,
    JsonValidationError,
    InvalidKeyFormat,
    InvalidValueType,
    InvalidValueRange,
    DuplicateKey,
    SchemaValidationError,
    ThreadLockTimeout,
    ThreadLockError,
    BackupError,
    RestoreError,
    TransactionError,
    UnknownError
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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with metadata
/**
 * @struct ConfigEntry
 * @brief Single configuration entry storing key-value pair with metadata
 * 
 * Represents a single configuration parameter with its source and description:
 * 
 * @field key Configuration key (alphanumeric with underscores and dashes)
 * @field value Configuration value (type-safe storage)
 * @field source Origin of this configuration value
 * @field description Human-readable explanation of configuration parameter
 * 
 * Configuration sources determine precedence in merging:
 * - Default: Built-in defaults, lowest priority
 * - File: Loaded from configuration file
 * - CommandLine: Set via command-line arguments
 * - Runtime: Set programmatically during execution
 * 
 * Runtime sources override earlier sources when the same key is set multiple times
 * 
 * Usage example:
 * @code{.cpp}
 * ConfigEntry entry("learning_rate", 0.01, ConfigSource::File, 
 *                   "Learning rate for neural network training");
 * @endcode
 * 
 * @see ConfigSource for detailed source information
 * @see Config::set() for setting configuration values
 */
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Exception class for configuration errors
/**
 * @class ConfigException
 * @brief Base exception class for all configuration errors
 * 
 * Comprehensive exception handling for configuration operations:
 * 
 * @inherits std::runtime_error for standard exception compatibility
 * 
 * @field errorCode_ Configuration error code for programmatic error handling
 * 
 * @param message Human-readable error description
 * @param errorCode Configuration error code (default: UnknownError)
 * 
 * @throws std::invalid_argument for invalid parameter values
 * @throws std::runtime_error for runtime configuration errors
 * 
 * Error handling example:
 * @code{.cpp}
 * try {
 *     config.loadFromFile("config.json");
 * } catch (const ConfigException& e) {
 *     std::cerr << "Configuration error: " << e.what() << std::endl;
 *     std::cerr << "Error code: " << static_cast<int>(e.errorCode()) << std::endl;
 *     
 *     // Programmatic error handling
 *     switch (e.errorCode()) {
 *         case ConfigErrorCode::FileNotFound:
 *             // Handle missing file
 *             break;
 *         case ConfigErrorCode::JsonParseError:
 *             // Handle JSON parsing error
 *             break;
 *         default:
 *             // Handle other errors
 *     }
 * }
 * @endcode
 * 
 * @see ConfigErrorCode for available error codes
 * @see getLastError() for retrieving current error code
 * @see Config::errorCodeToString() for error message conversion
 */
class ConfigException : public std::runtime_error {
public:
    /**
     * @brief Construct configuration exception with error code
     * 
     * @param message Human-readable error description
     * @param errorCode Configuration error code for programmatic handling
     */
    ConfigException(const std::string& message, ConfigErrorCode errorCode = ConfigErrorCode::UnknownError)
        : std::runtime_error(message), errorCode_(errorCode) {}
    
    /**
     * @brief Get the configuration error code
     * 
     * @return Current error code for programmatic error handling
     */
    ConfigErrorCode errorCode() const { return errorCode_; }
    
private:
    ConfigErrorCode errorCode_;
};

// Schema validation function type
using SchemaValidator = std::function<bool(const std::string&, const ConfigValue&)>;

// Schema validation function type
/**
 * @typedef SchemaValidator
 * @brief Function type for configuration schema validation
 * 
 * Validation function for enforcing configuration constraints:
 * 
 * @param key Configuration key to validate
 * @param value Configuration value to validate
 * 
 * @return bool True if validation passes, false otherwise
 * @retval true Configuration value is valid
 * @retval false Configuration value fails validation
 * 
 * @throws std::exception Any exception during validation
 * 
 * Validation function examples:
 * 
 * Example 1: Type validation
 * @code{.cpp}
 * auto validateInt = [](const std::string& key, const ConfigValue& value) {
 *     try {
 *         std::get<int>(value);
 *         return true;
 *     } catch (const std::bad_variant_access&) {
 *         return false;
 *     }
 * };
 * @endcode
 * 
 * Example 2: Range validation
 * @code{.cpp}
 * auto validateRange = [](const std::string& key, const ConfigValue& value) {
 *     try {
 *         double val = std::get<double>(value);
 *         return val >= 0.0 && val <= 1.0;
 *     } catch (const std::bad_variant_access&) {
 *         return false;
 *     }
 * };
 * @endcode
 * 
 * @see addSchemaValidator() for adding validation rules
 * @see validateSchema() for running all validations
 * @see ConfigEntry for configuration structure
 */
using SchemaValidator = std::function<bool(const std::string&, const ConfigValue&)>;

/**
 * @class Config
 * @brief Core configuration management system
 * 
 * Comprehensive configuration system providing:
 * 
 * **Storage Management:**
 * - Key-value storage with automatic type safety
 * - Multiple configuration sources with precedence rules
 * - Thread-safe concurrent access with mutex protection
 * - Memory-efficient pImpl pattern implementation
 * 
 * **I/O Operations:**
 * - JSON file loading/saving with automatic backup
 * - Command-line argument parsing
 * - File validation (existence, permissions, format)
 * - Backup and recovery mechanisms
 * 
 * **Validation & Safety:**
 * - Schema validation with custom validators
 * - Parameter validation (types, ranges, formats)
 * - Thread synchronization with timeout protection
 * - Comprehensive error handling and recovery
 * 
 * **Type Safety:**
 * - Automatic type conversion for get() methods
 * - Constexpr validation functions
 * - Variant-based value storage
 * - Template-based type-safe accessors
 * 
 * **Thread Safety:**
 * - Recursive mutex for file operations
 * - Atomic error state tracking
 * - Mutex-protected schema and default config
 * 
 * @see ConfigErrorCode for error handling
 * @see SchemaValidator for validation
 * @see ConfigSource for configuration sources
 * 
 * Usage example:
 * @code{.cpp}
 * #include "Config.hpp"
 * 
 * int main() {
 *     // Create configuration
 *     nlm::Config config;
 *     
 *     // Set configuration values
 *     config.set("learning_rate", 0.01);
 *     config.set("batch_size", 32);
 *     config.set("enabled", true);
 *     config.set("name", "neural_network");
 *     
 *     // Get values with type safety
 *     auto learning_rate = config.get<double>("learning_rate");
 *     auto batch_size = config.get<int>("batch_size");
 *     
 *     // Load from file
 *     config.loadFromFile("config.json");
 *     
 *     // Save to file
 *     config.saveToFile("output_config.json");
 *     
 *     // Validate configuration
 *     config.validateSchema();
 *     
 *     return 0;
 * }
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     * 
     * Creates a new configuration object with no entries. Configuration
     * must be populated using set(), loadFromFile(), or loadFromArgs()
     * before use. Initial error state is set to Success.
     * 
     * @see set() for setting configuration values
     * @see loadFromFile() for loading from JSON files
     * @see loadFromArgs() for loading from command line
     */
    Config();
    
    /**
     * @brief Destructor cleans up configuration resources
     * 
     * Automatically cleans up internal implementation details using
     * pImpl pattern for proper resource management. Configuration state
     * is properly released to prevent memory leaks.
     */
    ~Config();
    
    /**
     * @brief Copy constructor is deleted to prevent shallow copies
     * 
     * Copy operations are disabled to prevent shallow copies of configuration
     * that could lead to data corruption. Use move constructor instead:
     * 
     * @code{.cpp}
     * Config config1;
     * Config config2(std::move(config1)); // Safe move
     * @endcode
     * 
     * @see operator=() for move assignment operator
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent shallow copies
     * 
     * @see operator=() for move assignment operator
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor transfers ownership of configuration
     * 
     * @param other Configuration to move from (left-hand side)
     * 
     * Transfer ownership of configuration entries and metadata from
     * another configuration object. Source configuration becomes empty
     * after move, preventing resource duplication and ensuring proper
     * ownership semantics.
     * 
     * @see operator=() for move assignment operator
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator transfers ownership of configuration
     * 
     * @param other Configuration to move from (right-hand side)
     * 
     * Transfer ownership of configuration entries and metadata from
     * another configuration object. Proper cleanup of source configuration
     * ensures no resource leaks and maintains memory safety.
     * 
     * @return Reference to this configuration (for chaining)
     * 
     * @see Config(Config&&) move constructor
     */
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    /**
     * @brief Load configuration from JSON file
     * 
     * Loads configuration entries from a JSON file with comprehensive error
     * handling and automatic backup creation. The file must contain a JSON
     * object with key-value pairs.
     * 
     * @param filepath Path to JSON configuration file
     * 
     * @return bool True if configuration loaded successfully, false otherwise
     * @retval true Configuration loaded successfully
     * @retval false Failed to load configuration
     * 
     * @throws ConfigException if file validation fails
     * @throws std::exception if JSON parsing fails
     * 
     * @error FileNotFound if file does not exist
     * @error FilePermissionDenied if file is not readable
     * @error FileReadError if file cannot be read
     * @error JsonParseError if JSON syntax is invalid
     * @error InvalidKeyFormat if any key name is invalid
     * 
     * @post Configuration entries are cleared and replaced with file content
     * 
     * @see validatePath() for file path validation
     * @see createBackup() for backup creation
     * @see loadFromFile() alternative overload with createIfMissing
     * 
     * File format example:
     * @code{.json}
     * {
     *     "learning_rate": 0.01,
     *     "batch_size": 32,
     *     "enabled": true,
     *     "name": "neural_network",
     *     "learning_rate_desc": "Learning rate for training"
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from JSON file with auto-creation option
     * 
     * Loads configuration from file with option to create empty file if missing:
     * 
     * @param filepath Path to JSON configuration file
     * @param createIfMissing If true, create empty file when it doesn't exist
     * 
     * @return bool True if configuration loaded successfully, false otherwise
     * 
     * @see loadFromFile() for simpler version
     * 
     * Usage with createIfMissing:
     * @code{.cpp}
     * // Create config file if it doesn't exist
     * if (!config.loadFromFile("config.json", true)) {
     *     // Handle error (file created but empty)
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath, bool createIfMissing);

    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses standard NLM command line options and updates configuration:
     * 
     * Supported formats:
     * - `--key=value` (long format)
     * - `-k value` (short format)
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument vector (argv[0] is program name)
     * 
     * @return bool True if parsing succeeded, false otherwise
     * 
     * @error InvalidKeyFormat if any key name is invalid
     * @error InvalidValueType if value cannot be converted to proper type
     * 
     * @post Configuration updated with command line values (CommandLine source)
     * 
     * @see loadFromArgs() for pybind11 binding
     * @see ConfigSource::CommandLine for source precedence
     * 
     * Command line usage:
     * @code{.bash}
     * ./nlm --learning_rate=0.01 --batch_size=32 --enabled=true
     * @endcode
     */
    bool loadFromArgs(int argc, char** argv);

    /**
     * @brief Save configuration to JSON file
     * 
     * Saves configuration entries to a JSON file with automatic backup
     * creation and directory creation if needed. Configuration is formatted
     * with indentation for human readability.
     * 
     * @param filepath Path where JSON configuration will be saved
     * 
     * @return bool True if saved successfully, false otherwise
     * 
     * @error FilePermissionDenied if directory is not writable
     * @error FileWriteError if file cannot be written
     * @error BackupError if backup creation fails
     * 
     * @post Configuration saved to file, backup created if requested
     * 
     * @see saveToFile() alternative overload without backup
     * @see createBackup() for backup creation
     * @see validatePath() for file path validation
     * 
     * Output format includes descriptions:
     * @code{.json}
     * {
     *     "learning_rate": 0.01,
     *     "learning_rate_desc": "Learning rate for training",
     *     "batch_size": 32,
     *     "batch_size_desc": "Batch size for training",
     *     "enabled": true
     * }
     * @endcode
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to JSON file with backup option
     * 
     * @param filepath Path where JSON configuration will be saved
     * @param createBackup If true, create backup before saving
     * 
     * @return bool True if saved successfully, false otherwise
     * 
     * @see saveToFile() for simpler version
     */
    bool saveToFile(const std::string& filepath, bool createBackup) const;
    
    // Get values
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves configuration value with automatic type detection and conversion:
     * 
     * Supported types: int, int64_t, double, bool, std::string
     * 
     * @tparam T Desired type (int, double, bool, or std::string)
     * @param key Configuration key to retrieve
     * 
     * @return std::optional<T> Value if found and type matches, std::nullopt otherwise
     * @retval std::nullopt Key not found or value type mismatch
     * 
     * @throws ConfigException if value type is invalid
     * 
     * @error InvalidValueType if value exists but has wrong type
     * @error UnknownError for other errors
     * 
     * @see getOr() for fallback to default value
     * @see set() for setting values
     * 
     * Type-safe usage:
     * @code{.cpp}
     * auto learning_rate = config.get<double>("learning_rate");
     * if (learning_rate) {
     *     std::cout << "Learning rate: " << *learning_rate << std::endl;
     * } else {
     *     std::cout << "Learning rate not set" << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves configuration value with automatic type conversion. If the key
     * does not exist, returns the provided default value.
     * 
     * @tparam T Desired type (int, double, bool, or std::string)
     * @param key Configuration key to retrieve
     * @param defaultValue Value returned if key does not exist
     * 
     * @return T Configuration value or default if key not found
     * 
     * @see get() for returning std::nullopt when key not found
     * 
     * Type-safe usage:
     * @code{.cpp}
     * auto learning_rate = config.getOr("learning_rate", 0.01);
     * std::cout << "Learning rate: " << learning_rate << std::endl;
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set configuration value with type safety
     * 
     * Sets configuration value with automatic type conversion from string:
     * 
     * @param key Configuration key to set
     * @param value Configuration value (any type convertible to ConfigValue)
     * @param source Configuration source (default: Runtime)
     * 
     * @error InvalidKeyFormat if key name is invalid
     * @error InvalidValueType if value cannot be converted to ConfigValue
     * 
     * @post Value is stored with specified source
     * 
     * @see ConfigSource for available source types
     * @see get() for retrieving values
     * 
     * Usage with automatic type detection:
     * @code{.cpp}
     * config.set("learning_rate", 0.01);      // double
     * config.set("batch_size", 32);          // int
     * config.set("enabled", true);           // bool
     * config.set("name", "neural_network");   // string
     * config.set("array_param", "[1,2,3]");  // vector<int>
     * @endcode
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key to set
     * @param value String value to convert to ConfigValue
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int version)
     * 
     * @param key Configuration key to set
     * @param value Integer value
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double version)
     * 
     * @param key Configuration key to set
     * @param value Floating-point value
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool version)
     * 
     * @param key Configuration key to set
     * @param value Boolean value
     * @param source Configuration source (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * 
     * @return bool True if key exists in configuration, false otherwise
     * 
     * @see remove() for removing keys
     * @see getKeys() for getting all keys
     * 
     * Usage:
     * @code{.cpp}
     * if (config.has("learning_rate")) {
     *     auto learning_rate = config.get<double>("learning_rate");
     * }
     * @endcode
     */
    bool has(const std::string& key) const;

    /**
     * @brief Remove configuration key and its value
     * 
     * @param key Configuration key to remove
     * 
     * @post Key and value are removed from configuration
     * 
     * @see has() for checking key existence
     * @see getKeys() for listing keys
     * 
     * @code{.cpp}
     * config.remove("learning_rate");
     * @endcode
     */
    void remove(const std::string& key);

    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration keys
     * 
     * @return Vector containing all configuration keys in insertion order
     * 
     * @see has() for checking individual keys
     * @see remove() for removing keys
     * @see items() for getting key-value pairs
     * 
     * Usage:
     * @code{.cpp}
     * auto keys = config.getKeys();
     * for (const auto& key : keys) {
     *     std::cout << "Key: " << key << std::endl;
     * }
     * @endcode
     */
    std::vector<std::string> getKeys() const;

    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries, resetting to empty state.
     * Note: This does not affect default configuration values.
     * 
     * @post All configuration entries are removed
     * 
     * @see getKeys() for checking number of entries
     * @see getConfig() for getting internal configuration
     * 
     * Usage:
     * @code{.cpp}
     * config.clear();
     * std::cout << "Configuration cleared. Keys: " << config.getKeys().size() << std::endl;
     * @endcode
     */
    void clear();

    /**
     * @brief Get configuration summary string
     * 
     * Generates a human-readable summary of configuration state:
     * 
     * @return std::string Summary string with entry count and source information
     * 
     * @return Format: "Configuration (N entries):\n  key = value (source)\n..."
     * 
     * @see getKeys() for key list
     * @see get() for individual values
     * 
     * Usage:
     * @code{.cpp}
     * std::cout << config.summary() << std::endl;
     * @endcode
     */
    std::string summary() const;
    
    // Error handling
    static std::string errorCodeToString(ConfigErrorCode errorCode);
    static ConfigErrorCode getLastError();
    static void clearLastError();
    
    // Validation
    static bool validateKey(const std::string& key);
    static bool validatePath(const std::string& path, bool checkWritePermission = false);
    
    // Error handling
    /**
     * @brief Convert error code to human-readable string
     * 
     * @param errorCode Configuration error code
     * 
     * @return std::string Human-readable error message
     * 
     * @see getLastError() for current error code
     * @see errorCodeToString() static version
     * 
     * Examples:
     * @code{.cpp}
     * ConfigErrorCode error = Config::getLastError();
     * std::cout << "Error: " << Config::errorCodeToString(error) << std::endl;
     * @endcode
     */
    static std::string errorCodeToString(ConfigErrorCode errorCode);
    
    /**
     * @brief Get last error code
     * 
     * @return ConfigErrorCode Last error code, or Success if no error
     * 
     * @see setLastError() for setting error
     * @see clearLastError() for clearing error
     * 
     * Usage:
     * @code{.cpp}
     * if (config.loadFromFile("config.json")) {
     *     std::cout << "Success!" << std::endl;
     * } else {
     *     ConfigErrorCode error = Config::getLastError();
     *     if (error != ConfigErrorCode::Success) {
     *         std::cerr << "Failed: " << Config::errorCodeToString(error) << std::endl;
     *     }
     * }
     * @endcode
     */
    static ConfigErrorCode getLastError();
    
    /**
     * @brief Clear last error code
     * 
     * Resets the internal error state to Success. Used to indicate
     * that a previous operation has been resolved.
     * 
     * @post Last error code is set to Success
     * 
     * @see getLastError() for retrieving error
     * @see errorCodeToString() for error messages
     * 
     * Usage:
     * @code{.cpp}
     * // After successful operation
     * config.loadFromFile("config.json");
     * Config::clearLastError();
     * 
     * // Or reset error state
     * Config::clearLastError();
     * @endcode
     */
    static void clearLastError();

    /**
     * @brief Validate configuration key format
     * 
     * Validates that a configuration key follows naming conventions:
     * alphanumeric characters with underscores and dashes only.
     * 
     * @param key Configuration key to validate
     * 
     * @return bool True if key format is valid, false otherwise
     * 
     * @retval true Key follows format: [a-zA-Z0-9_-]+
     * @retval false Key contains invalid characters
     * 
     * @see validateKey() for validation implementation
     * @see validatePath() for file path validation
     * @see loadFromArgs() for key validation in command line
     * 
     * Valid examples:
     * - "learning_rate"
     * - "batch_size"
     * - "neural_network_name"
     * - "max_neurons_100"
     * 
     * Invalid examples:
     * - "learning rate" (space)
     * - "learning.rate" (dot)
     * - "learning@rate" (special char)
     */
    static bool validateKey(const std::string& key);
    
    /**
     * @brief Validate file path for configuration operations
     * 
     * Validates file path for configuration file operations:
     * 
     * @param path File path to validate
     * @param checkWritePermission If true, checks write permissions
     * 
     * @return bool True if path is valid, false otherwise
     * 
     * @retval true Path exists, is accessible, and permissions are correct
     * @retval false Path is invalid or lacks required permissions
     * 
     * Validation checks:
     * - Path is not empty or contain null bytes
     * - Parent directory exists and is accessible
     * - Directory is actually a directory (not a file)
     * - Read permission verified (for reading)
     * - Write permission verified (for writing)
     * 
     * @see validateKey() for key validation
     * @see loadFromFile() for file loading validation
     * @see saveToFile() for file saving validation
     * 
     * Usage:
     * @code{.cpp}
     * if (validatePath("/path/to/config.json", true)) {
     *     config.loadFromFile("/path/to/config.json");
     * } else {
     *     std::cerr << "Invalid configuration file path" << std::endl;
     * }
     * @endcode
     */
    static bool validatePath(const std::string& path, bool checkWritePermission = false);

    /**
     * @brief Acquire thread lock for configuration operations
     * 
     * Acquires exclusive file lock for thread-safe configuration operations:
     * 
     * @param lockFilePath Path to lock file
     * @param timeoutMs Timeout in milliseconds (default: 5000ms)
     * 
     * @return bool True if lock acquired successfully, false otherwise
     * 
     * @retval true Lock acquired and process info written to lock file
     * @retval false Timeout or lock acquisition failed
     * 
     * Uses flock() system call for cross-platform file locking:
     * - Creates lock file if it doesn't exist
     * - Attempts exclusive write lock with timeout
     * - Writes process ID and timestamp to lock file
     * - Automatic cleanup on failure or success
     * 
     * @see releaseLock() for releasing lock
     * @see acquireLock() for pybind11 binding
     * 
     * Usage:
     * @code{.cpp}
     * if (Config::acquireLock("/tmp/config.lock", 1000)) {
     *     // Critical section
     *     config.loadFromFile("config.json");
     *     
     *     // Do work...
     *     
     *     Config::releaseLock("/tmp/config.lock");
     * } else {
     *     std::cerr << "Could not acquire configuration lock" << std::endl;
     * }
     * @endcode
     */
    static bool acquireLock(const std::string& lockFilePath, int timeoutMs = 5000);
    
    /**
     * @brief Release thread lock for configuration operations
     * 
     * Releases file lock acquired by acquireLock():
     * 
     * @param lockFilePath Path to lock file
     * 
     * @post Lock file is removed and lock is released
     * 
     * @see acquireLock() for acquiring lock
     * @see releaseLock() for pybind11 binding
     */
    static void releaseLock(const std::string& lockFilePath);

    /**
     * @brief Create backup of configuration file
     * 
     * Creates timestamped backup of configuration file with automatic
     * directory creation:
     * 
     * @param filepath Original configuration file path
     * @param backupDir Backup directory (default: same as original file)
     * 
     * @return bool True if backup created successfully, false otherwise
     * 
     * @error BackupError if backup creation fails
     * @error FileWriteError if backup cannot be written
     * 
     * @post Backup file created with timestamp in filename:
     * `filename.backup_YYYYMMDD_HHMMSS`
     * 
     * @see restoreFromBackup() for restoring from backup
     * @see getBackupFiles() for listing backup files
     * 
     * Backup format example:
     * @code{.bash}
     * config.json.backup_20240101_120000
     * @endcode
     */
    static bool createBackup(const std::string& filepath, const std::string& backupDir = "");
    
    /**
     * @brief Restore configuration from backup
     * 
     * Restores configuration file from backup with index-based selection:
     * 
     * @param filepath Original configuration file path
     * @param backupIndex Backup index (0 = newest, 1 = second newest, etc.)
     * 
     * @return bool True if restored successfully, false otherwise
     * 
     * @error RestoreError if restoration fails
     * @error FileReadError if backup cannot be read
     * @error FileWriteError if original file cannot be written
     * 
     * @post Configuration file replaced with backup content
     * 
     * @see createBackup() for creating backups
     * @see getBackupFiles() for available backups
     * 
     * Usage:
     * @code{.cpp}
     * // Restore from newest backup
     * if (Config::restoreFromBackup("config.json", 0)) {
     *     std::cout << "Configuration restored from newest backup" << std::endl;
     * }
     * 
     * // Restore from second newest backup
     * if (Config::restoreFromBackup("config.json", 1)) {
     *     std::cout << "Configuration restored from second newest backup" << std::endl;
     * }
     * @endcode
     */
    static bool restoreFromBackup(const std::string& filepath, int backupIndex = 0);
    
    /**
     * @brief Get list of backup files for configuration
     * 
     * Gets list of available backup files for a configuration file:
     * 
     * @param filepath Original configuration file path
     * 
     * @return std::vector<std::string> List of backup file paths
     * 
     * @return Vector sorted by timestamp (newest first)
     * 
     * @see restoreFromBackup() for restoring from backup
     * @see createBackup() for creating backups
     * 
     * Usage:
     * @code{.cpp}
     * auto backups = Config::getBackupFiles("config.json");
     * for (size_t i = 0; i < backups.size(); ++i) {
     *     std::cout << i << ": " << backups[i] << std::endl;
     * }
     * @endcode
     */
    static std::vector<std::string> getBackupFiles(const std::string& filepath);

    /**
     * @brief Merge another configuration into this one
     * 
     * Merges entries from another configuration into this configuration:
     * 
     * @param otherConfig Configuration to merge from
     * 
     * @return bool True if merge succeeded, false otherwise
     * 
     * @post Entries from otherConfig added to this configuration
     * 
     * @see mergeFrom() for pybind11 binding
     * @see setDefaultConfig() for setting default values
     * 
     * Merge behavior:
     * - Non-duplicate keys are added
     * - Duplicate keys are skipped (preserving existing values)
     * - Configuration source is set to Runtime for merged entries
     * 
     * Usage:
     * @code{.cpp}
     * Config config1, config2;
     * config1.set("learning_rate", 0.01);
     * config2.set("batch_size", 32);
     * config1.mergeFrom(config2);
     * 
     * // Result: both learning_rate and batch_size in config1
     * @endcode
     */
    bool mergeFrom(const Config& otherConfig);
    
    /**
     * @brief Convert configuration to map
     * 
     * Converts configuration entries to std::map for easy serialization:
     * 
     * @return std::map<std::string, ConfigValue> Configuration as map
     * 
     * @return Map with key-value pairs in alphabetical order
     * 
     * @see fromMap() for converting from map
     * @see toMap() for pybind11 binding
     * 
     * Usage:
     * @code{.cpp}
     * auto configMap = config.toMap();
     * for (const auto& pair : configMap) {
     *     std::cout << pair.first << ": " << pair.second << std::endl;
     * }
     * @endcode
     */
    std::map<std::string, ConfigValue> toMap() const;
    
    /**
     * @brief Load configuration from map
     * 
     * Loads configuration entries from std::map:
     * 
     * @param configMap Configuration as key-value map
     * 
     * @return bool True if loaded successfully, false otherwise
     * 
     * @post Configuration cleared and populated from map
     * 
     * @see toMap() for converting to map
     * @see fromMap() for pybind11 binding
     * 
     * Usage:
     * @code{.cpp}
     * std::map<std::string, ConfigValue> configMap;
     * configMap["learning_rate"] = 0.01;
     * configMap["batch_size"] = 32;
     * config.fromMap(configMap);
     * @endcode
     */
    bool fromMap(const std::map<std::string, ConfigValue>& configMap);

    /**
     * @brief Add schema validator for configuration key
     * 
     * Adds custom validation function for a specific configuration key:
     * 
     * @param key Configuration key to validate
     * @param validator Validation function (lambda or std::function)
     * 
     * @post Validator added to schema validation system
     * 
     * @error InvalidKeyFormat if key name is invalid
     * 
     * @see SchemaValidator for validation function type
     * @see validateSchema() for running validations
     * 
     * Custom validator example:
     * @code{.cpp}
     * // Validate learning rate range
     * auto validateLearningRate = [](const std::string& key, const ConfigValue& value) {
     *     try {
     *         double rate = std::get<double>(value);
     *         return rate >= 0.0 && rate <= 1.0; // 0% to 100% learning rate
     *     } catch (const std::bad_variant_access&) {
     *         return false;
     *     }
     * };
     * 
     * config.addSchemaValidator("learning_rate", validateLearningRate);
     * @endcode
     */
    void addSchemaValidator(const std::string& key, const SchemaValidator& validator);
    
    /**
     * @brief Validate configuration against schema
     * 
     * Validates all configuration entries against registered schema validators:
     * 
     * @return bool True if all validations pass, false otherwise
     * 
     * @retval true All configuration entries are valid
     * @retval false At least one validation failed
     * 
     * @error SchemaValidationError if validation fails
     * 
     * @post Last error is set to SchemaValidationError on failure
     * 
     * @see addSchemaValidator() for adding validators
     * @see validateSchema() for pybind11 binding
     * 
     * Usage:
     * @code{.cpp}
     * if (config.validateSchema()) {
     *     std::cout << "Configuration is valid" << std::endl;
     * } else {
     *     std::cerr << "Validation failed: " 
     *               << Config::errorCodeToString(Config::getLastError()) << std::endl;
     * }
     * @endcode
     */
    bool validateSchema() const;

    /**
     * @brief Set default configuration value
     * 
     * Sets default configuration value (lowest priority source):
     * 
     * @param key Configuration key to set
     * @param value Default value to set
     * 
     * @post Default value set for key
     * 
     * @see getDefault() for retrieving default values
     * @see set() for runtime setting
     * 
     * Default values are used when key is not set by:
     * 1. Runtime (set())
     * 2. Command line (loadFromArgs())
     * 3. File (loadFromFile())
     * 
     * Usage:
     * @code{.cpp}
     * Config::setDefaultConfig("learning_rate", 0.01);
     * Config::setDefaultConfig("batch_size", 32);
     * 
     * // If not set elsewhere, these values will be used
     * auto lr = config.getOr("learning_rate", 0.0); // May use default
     * @endcode
     */
    static void setDefaultConfig(const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Get default configuration value
     * 
     * Retrieves default configuration value:
     * 
     * @param key Configuration key to retrieve
     * 
     * @return std::optional<ConfigValue> Default value if set, std::nullopt otherwise
     * 
     * @retval std::nullopt No default value for key
     * 
     * @see setDefaultConfig() for setting defaults
     * @see getDefault() for pybind11 binding
     * 
     * Usage:
     * @code{.cpp}
     * auto default_lr = Config::getDefault("learning_rate");
     * if (default_lr) {
     *     // Has default value
     * }
     * @endcode
     */
    static std::optional<ConfigValue> getDefault(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    static bool stringToConfigValue(const std::string& str, ConfigValue& value);
    static std::string jsonToString(const nlohmann::json& j);
    static bool stringToJson(const std::string& str, nlohmann::json& j);
    static bool jsonToConfigValue(const nlohmann::json& j, ConfigValue& value);
    
    // Thread synchronization
    static std::mutex fileAccessMutex_;
    static std::atomic<ConfigErrorCode> lastError_;
    static std::map<std::string, SchemaValidator> schemaValidators_;
    static std::map<std::string, ConfigValue> defaultConfig_;
    static std::mutex schemaMutex_;
    static std::mutex defaultConfigMutex_;
};

} // namespace nlm