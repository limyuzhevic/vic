#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <filesystem>
#include "../Error/Error.hpp"
#include "nlohmann/json.hpp"

namespace nlm {

// Forward declarations
class Config;

/**
 * Config class - Core configuration management for NLM
 * @brief Manages application configuration with multiple value types and persistence
 * @details This class provides comprehensive configuration management supporting:
 *         - Multiple value types (int, float, string, bool, vectors)
 *         - Multiple sources (default, file, command line, runtime)
 *         - JSON-like configuration file format
 *         - Validation and error handling
 *         - Serialization and persistence
 *         
 * Configuration values are stored as key-value pairs with source tracking:
 * - Source indicates how the value was set (priority: Runtime > CommandLine > File > Default)
 * - Default values provide initial configuration
 * - File values can be loaded from config files
 * - Command line values override file settings
 * - Runtime values provide dynamic configuration changes
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Configuration values should be validated before use
 * @note Configuration keys must be alphanumeric without spaces
 * @see Config.cpp for implementation details
 * @see nlm::Config for usage examples
 */
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <filesystem>
#include "../Error/Error.hpp"
#include "nlohmann/json.hpp"

namespace nlm {

// Forward declarations
class Config;

/**
 * Config class - Core configuration management for NLM
 * @brief Manages application configuration with multiple value types and persistence
 * @details This class provides comprehensive configuration management supporting:
 *         - Multiple value types (int, float, string, bool, vectors)
 *         - Multiple sources (default, file, command line, runtime)
 *         - JSON-like configuration file format (including full JSON support)
 *         - Schema validation and parameter range checking
 *         - Cross-parameter validation for consistency
 *         - Enhanced error handling with recovery suggestions
 *         - Serialization and persistence
 *         
 * Configuration values are stored as key-value pairs with source tracking:
 * - Source indicates how the value was set (priority: Runtime > CommandLine > File > Default)
 * - Default values provide initial configuration
 * - File values can be loaded from config files (JSON format supported)
 * - Command line values override file settings
 * - Runtime values provide dynamic configuration changes
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Configuration values should be validated before use
 * @note Configuration keys must be alphanumeric without spaces
 * @see Config.cpp for implementation details
 * @see nlm::Config for usage examples
 */
class Config {
public:
    /**
     * @brief Default constructor - creates empty configuration
     * @post Configuration object created; ready for loading or initialization
     * @note Initial configuration empty; all values undefined
     * @see loadFromFile() for file-based initialization
     * @see loadFromArgs() for command-line initialization
     */
    Config();
    
    /**
     * @brief Destructor - cleans up configuration resources
     * @post All configuration entries cleared; memory released
     */
    ~Config();
    
    /**
     * @brief Copy constructor - deleted to prevent copying
     * @details Configuration objects should not be copied to maintain consistency
     * @warning Attempting to copy will result in compilation error
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor - enables efficient configuration transfer
     * @param other Configuration to move from (will be invalidated)
     * @post this->pImpl = other->pImpl, other->pImpl = nullptr
     * @note Move semantics provide efficient transfer without deep copying
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient configuration reassignment
     * @param other Configuration to move from (will be invalidated)
     * @post this->pImpl = other->pImpl, other->pImpl = nullptr
     * @return Reference to this configuration
     * @note Move assignment provides efficient transfer without deep copying
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (supports JSON and key=value formats)
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false if failed
     * @post Configuration entries updated from file
     * @details File format supports:
     *         - JSON format files (with .json extension or auto-detected)
     *         - Legacy key=value format files
     *         - Automatic format detection based on file extension or content
     *         - Schema validation for JSON files
     *         - Enhanced error reporting for malformed files
     * 
     * @warning File permissions must allow reading
     * @warning Invalid format causes partial loading with error reporting
     * @throws NLMException if file cannot be opened
     * @see loadFromJsonFile() for JSON-specific loading
     * @see loadFromLegacyFile() for legacy format loading
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from JSON file
     * @param filepath Path to JSON configuration file
     * @return true if file loaded successfully, false if failed
     * @post Configuration entries updated from JSON file
     * @details Loads configuration from full JSON file with schema validation
     *         - Validates file against JSON schema
     *         - Validates parameter ranges and types
     *         - Performs cross-parameter validation
     *         - Provides detailed error messages for validation failures
     * 
     * @warning File permissions must allow reading
     * @warning Invalid JSON causes failure with detailed error messages
     * @warning Schema validation failures cause rejection of entire file
     * @throws NLMException if file cannot be opened or is invalid JSON
     * @see loadFromFile() for auto-detecting loader
     */
    bool loadFromJsonFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from legacy key=value format file
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false if failed
     * @post Configuration entries updated from legacy file
     * @details Backward-compatible loader for existing config files
     *         - Maintains support for existing key=value format
     *         - Supports comments (# and /)
     *         - Supports string values in quotes
     *         - Supports boolean (true/false) and numeric values
     *         - Supports vector values in brackets [item1, item2, ...]
     * 
     * @warning Legacy format lacks advanced validation
     * @warning Type conversion may be less robust than JSON format
     * @see loadFromFile() for auto-detecting loader
     * @see loadFromJsonFile() for JSON format
     */
    bool loadFromLegacyFile(const std::string& filepath);
    
    /**
     * @brief Save configuration to file (supports JSON and legacy formats)
     * @param filepath Path to output configuration file
     * @param format Output format (JSON or legacy)
     * @return true if saved successfully, false if failed
     * @post Configuration file written
     * @details File format supports:
     *         - JSON format (when format=Json)
     *         - Legacy key=value format (when format=Legacy)
     *         - Proper value formatting (strings in quotes, etc.)
     *         - Fixed precision for floating-point values
     *         - Proper line endings and formatting
     * 
     * @warning File permissions must allow writing
     * @warning Existing file will be overwritten
     * @see saveToJsonFile() for JSON-specific saving
     * @see saveToLegacyFile() for legacy format saving
     */
    bool saveToFile(const std::string& filepath, const std::string& format = "auto");
    
    /**
     * @brief Save configuration to JSON file
     * @param filepath Path to output JSON configuration file
     * @return true if saved successfully, false if failed
     * @post Configuration file written as JSON
     * @details Saves configuration in full JSON format with metadata
     *         - Includes schema version information
     *         - Includes creation timestamp
     *         - Properly formatted JSON with indentation
     *         - All configuration values included
     * 
     * @warning File permissions must allow writing
     * @warning Existing file will be overwritten
     * @see saveToFile() for auto-detecting saver
     * @see saveToLegacyFile() for legacy format
     */
    bool saveToJsonFile(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to legacy key=value format file
     * @param filepath Path to output configuration file
     * @return true if saved successfully, false if failed
     * @post Configuration file written in legacy format
     * @details Saves configuration in backward-compatible key=value format
     *         - Maintains compatibility with existing config files
     *         - Supports comments for each entry
     *         - Proper value formatting (strings in quotes, etc.)
     *         - Fixed precision for floating-point values
     * 
     * @warning Loses schema information and advanced metadata
     * @warning Some extended configuration may not serialize correctly
     * @see saveToFile() for auto-detecting saver
     * @see saveToJsonFile() for JSON format
     */
    bool saveToLegacyFile(const std::string& filepath) const;
    
    /**
     * @brief Validate configuration against schema
     * @param filepath Path to schema file
     * @return true if validation succeeds, false if validation fails
     * @post Configuration validated
     * @details Validates current configuration against provided schema
     *         - Checks for required fields and values
     *         - Validates parameter ranges
     *         - Performs cross-parameter validation
     *         - Reports detailed validation errors
     * 
     * @warning Validation may fail for current configuration
     * @warning Returns false instead of throwing on validation errors
     * @see validateConfig() for internal schema validation
     */
    bool validateAgainstSchema(const std::string& filepath) const;
    
    /**
     * @brief Validate configuration internally
     * @return true if configuration is valid, false otherwise
     * @post Configuration validated
     * @details Validates current configuration:
     *         - Checks for required fields
     *         - Validates parameter ranges and types
     *         - Performs cross-parameter validation
     *         - Provides detailed error messages
     * 
     * @warning May fail for current configuration depending on schema
     * @warning Returns false instead of throwing on validation errors
     * @see validateAgainstSchema() for external schema validation
     */
    bool validateConfig() const;
    
    /**
     * @brief Get typed configuration value
     * @tparam T Desired value type
     * @param key Configuration key
     * @return Optional value (std::nullopt if key not found or type mismatch)
     * @post Value returned; configuration unchanged
     * @details Type conversion uses std::variant type checking:
     *         - Exact type matches return the value
     *         - Type mismatches return std::nullopt with warning
     *         - Missing keys return std::nullopt
     * 
     * @warning Type mismatch is logged but doesn't throw
     * @warning Invalid keys return std::nullopt
     * @see getOr() for default values
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Desired value type
     * @param key Configuration key
     * @param defaultValue Value to return if key not found
     * @return Configuration value or default if key missing
     * @post None
     * @details Type-safe fallback mechanism:
     *         - Returns actual value if key exists and matches type
     *         - Returns default value if key missing
     *         - Type mismatch falls back to default with warning
     * 
     * @warning Type mismatch with default may indicate configuration issues
     * @see get() for exact value retrieval without default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value (generic version)
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (default: Runtime)
     * @post Value set; source recorded
     * @warning Key validation performed
     * @warning Key cannot contain spaces or special characters
     * @throws NLMException if key invalid
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration (default: Runtime)
     * @post Value set; source recorded
     * @warning Key and value validation performed
     * @warning Value cannot be empty string
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration (default: Runtime)
     * @post Value set; source recorded
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double version)
     * @param key Configuration key
     * @param value Double value
     * @param source Source of configuration (default: Runtime)
     * @post Value set; source recorded
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration (default: Runtime)
     * @post Value set; source recorded
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     * @post None
     * @details Efficient lookup using std::any_of:
     *         - O(1) for small configuration sets
     *         - O(n) for larger configuration sets
     *         - Case-sensitive key matching
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     * @post Key removed if found; source cleared
     * @warning Removing runtime keys may affect application behavior
     * @warning Removing file keys affects persistence
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return Vector of all configuration keys
     * @post None
     * @details Returns keys in insertion order (maintainability)
     *         - Keys sorted by insertion (preserves loading order)
     *         - No duplicates
     *         - Empty if no configuration loaded
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * @post All configuration entries removed; empty state
     * @warning Removes all configuration including defaults
     * @warning Clears source information
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * @return Formatted configuration summary string
     * @post None
     * @details Creates human-readable summary including:
     *         - Total entry count
     *         - All key-value pairs with sources
     *         - Proper formatting for readability
     *         - Useful for debugging and logging
     */
    std::string summary() const;
    
    /**
     * @brief Get current configuration as JSON object
     * @return JSON representation of current configuration
     * @post None
     * @details Converts current configuration to JSON format
     *         - Includes all configuration entries
     *         - Preserves value types
     *         - Includes metadata if available
     */
    nlohmann::json toJson() const;
    
    /**
     * @brief Load configuration from JSON string
     * @param jsonString JSON string to parse
     * @return true if loaded successfully, false if failed
     * @post Configuration loaded from JSON
     * @details Parses JSON string and loads configuration
     *         - Validates JSON format
     *         - Validates schema if available
     *         - Reports detailed error messages
     */
    bool loadFromJsonString(const std::string& jsonString);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static nlohmann::json parseLegacyToJson(const std::string& content);
    static bool isJsonFile(const std::string& filepath);
    
    // Validation helpers
    bool validateParameterRanges() const;
    bool validateParameterTypes() const;
    bool validateCrossParameterDependencies() const;
    bool validateRequiredFields() const;
    
    // JSON parsing helpers
    void mergeJsonConfig(const nlohmann::json& jsonConfig);
    std::string formatJsonValue(const nlohmann::json& value) const;
    
    // Legacy file helpers
    bool loadLegacyFile(const std::string& filepath);
    bool saveLegacyFile(const std::string& filepath) const;
    
    // Error reporting
    void reportValidationError(const std::string& message) const;
    void reportParseError(const std::string& message, const std::string& context = "") const;
};

} // namespace nlm
