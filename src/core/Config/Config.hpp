// Configuration System Header
// ===========================
// 
// This file defines the Config class and related types for managing application configuration.
// 
// The configuration system provides:
// - File-based configuration loading from simple key=value format
// - Command-line argument parsing (both --key=value and -k value formats)
// - Type-safe value storage supporting various primitive and container types
// - Index-based lookup system for efficient access
// - Configuration source tracking (Default, File, CommandLine, Runtime)
// - Serialization and deserialization capabilities
// 
// Configuration files use a simple key=value format with support for quoted strings
// and escaped characters. Values are automatically parsed and converted to the
// appropriate type based on their content.
// 
// The system is designed for performance and memory efficiency, with O(1) lookup
// for frequently accessed configuration values through an index-based system.
// 
// See Config.cpp for implementation details and algorithm explanations.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Config;

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
/**
 * @brief Represents a single configuration value with metadata.
 * 
 * Each configuration entry stores a key-value pair along with information about
 * where the value originated and how it should be interpreted.
 * 
 * @note This struct is not designed for direct instantiation by users. Use the
 *       Config class's set() methods instead to ensure proper validation and
 *       source tracking.
 */
struct ConfigEntry {
    std::string key;        ///< The configuration key (unique identifier)
    ConfigValue value;      ///< The configuration value (type-erased)
    ConfigSource source;    ///< The source from which this configuration was loaded
    std::string description; ///< Optional human-readable description of the configuration
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @brief The Config class provides a type-safe configuration management system.
 * 
 * This class implements a comprehensive configuration management system that supports
 * multiple configuration sources, efficient lookup, and serialization/deserialization.
 * The system uses a PIMPL pattern for optimal performance and memory efficiency.
 * 
 * Key features:
 * - File-based configuration loading from simple key=value format
 * - Command-line argument parsing (supports --key=value and -k value formats)
 * - Type-safe value storage with support for primitive and container types
 * - Index-based lookup system for O(1) access to configuration values
 * - Source tracking to determine configuration precedence (Default < File < CommandLine < Runtime)
 * - Automatic type conversion and validation
 * - Thread-safe operations for configuration access
 * 
 * The configuration system follows a layered approach where configuration sources have
 * different precedence levels. Values from higher precedence sources override those
 * from lower precedence sources. This allows for flexible configuration management
 * where command-line arguments can override file settings, which can be overridden
 * by runtime settings.
 * 
 * The implementation uses a doubly-linked structure (vector + hash map) to provide
 * both fast lookup by key and efficient iteration. When values are added or removed,
 * the index is maintained to ensure consistent performance.
 * 
 * @note The Config class is move-constructible and move-assignable but explicitly
 *       deletes copy operations to prevent unintended copies and ensure proper
 *       resource management.
 * 
 * @warning Configuration values are validated during parsing. Invalid values may
 *          cause parsing to skip or fail gracefully with appropriate error messages.
 * 
 * Performance characteristics:
 * - get() and has() operations: O(1) average case
 * - set() operations: O(1) average case
 * - remove() operations: O(n) worst case due to index maintenance
 * - File loading: O(n) where n is the number of lines in the configuration file
 * 
 * @see Config.cpp for detailed algorithm explanations and implementation details
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Constructs a new, empty configuration object.
     * 
     * This constructor initializes an empty configuration with all default values
     * and creates the internal implementation structure. The configuration starts
     * with no entries, ready to be loaded from various sources or populated directly.
     * 
     * The constructor performs no validation or parsing - it simply sets up the
     * internal data structures for efficient configuration management.
     */
    Config();
    
    /**
     * @brief Destructor that cleans up the configuration object.
     * 
     * This destructor properly cleans up the internal implementation structure,
     * releasing any allocated resources. Since the implementation uses a
     * unique_ptr for the pImpl pattern, the memory is automatically managed.
     * 
     * The destructor is implicitly defaulted for performance reasons, as the
     * pImpl pattern ensures proper cleanup without explicit resource management.
     */
    ~Config();
    
    /**
     * @brief Disables copy construction.
     * 
     * Copy construction is explicitly deleted to prevent multiple copies of the
     * configuration object, which could lead to resource management issues and
     * inconsistent state. Use move semantics instead.
     * 
     * @note This enforces the PIMPL pattern's requirement for unique ownership
     *       of the implementation pointer.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disables copy assignment.
     * 
     * Copy assignment is explicitly deleted for the same reasons as copy
     * construction - to maintain unique ownership of the implementation and
     * prevent resource management issues.
     * 
     * @note This ensures the configuration object remains safely move-only.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor with exception safety.
     * 
     * This constructor transfers ownership of the implementation from the source
     * configuration object to the newly created one. After the move operation,
     * the source object will have a null implementation pointer.
     * 
     * The noexcept specification ensures that this operation never throws,
     * making it safe to use in exception-critical contexts.
     * 
     * @param other The source configuration object to move from
     * @note The source object is left in a valid but empty state after the move
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator with exception safety.
     * 
     * This operator transfers ownership of the implementation from the source
     * configuration object to this object. It properly handles self-assignment
     * and ensures the source object is left in a valid state.
     * 
     * The noexcept specification guarantees no exceptions will be thrown during
     * this operation, maintaining the type's noexcept guarantees.
     * 
     * @param other The source configuration object to move from
     * @return Reference to this configuration object
     * @note After assignment, the source object is left with a null implementation
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from a file on disk.
     * 
     * This method reads and parses a configuration file using a simple key=value
     * format. The file is processed line by line, with support for:
     * - Comments (lines starting with # or /)
     * - Quoted strings with escape sequence support
     * - Automatic type conversion (string, integer, floating-point, boolean)
     * - Vector values in [value1,value2,value3] format
     * 
     * The loading process includes validation for:
     * - Line length limits (max 4096 characters)
     * - Proper key=value format
     * - Non-empty keys
     * - Well-formed values
     * 
     * If the file cannot be opened or parsed, an error message is printed to
     * stderr and the method returns false. Otherwise, returns true.
     * 
     * @param filepath Path to the configuration file to load
     * @return true if the file was successfully loaded and parsed, false otherwise
     * @throws May throw exceptions for malformed input during parsing
     * 
     * @note Configuration from files has lower precedence than command-line arguments
     *       but higher than default values.
     * @see loadFromArgs() for command-line loading, saveToFile() for serialization
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command-line arguments.
     * 
     * This method parses command-line arguments in two common formats:
     * - Long format: --key=value (e.g., --port=8080, --debug=true)
     * - Short format: -k value (e.g., -p 8080, -d true)
     * 
     * The parsing is case-sensitive for keys but supports case-insensitive
     * value comparison for boolean values. Arguments are processed in order,
     * with later arguments overriding earlier ones for the same key.
     * 
     * The method automatically skips malformed arguments with warning messages
     * printed to stderr, ensuring that one bad argument doesn't prevent valid
     * configuration from being loaded.
     * 
     * @param argc Number of command-line arguments (typically from main())
     * @param argv Array of command-line argument strings (typically from main())
     * @return Always returns true (argument parsing is forgiving)
     * 
     * @note Command-line arguments have higher precedence than file configuration
     *       but lower than runtime-set values.
     * @see loadFromFile() for file loading, set() for runtime configuration
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves the current configuration to a file.
     * 
     * This method serializes the current configuration to a file in the same
     * key=value format that loadFromFile() reads. The output includes:
     * - Value descriptions as comments above each key
     * - Proper escaping of strings with quotes
     * - Consistent formatting for all value types
     * 
     * The serialization preserves the original value types and formats them
     * appropriately for human readability while maintaining machine readability
     * for potential re-loading.
     * 
     * @param filepath Path to the file where configuration should be saved
     * @return true if the file was successfully written, false otherwise
     * @throws May throw exceptions for I/O errors during file writing
     * 
     * @note This method does not change the source of saved values - they retain
     *       their original ConfigSource designation.
     * @see loadFromFile() for loading from files
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Retrieves a configuration value with type conversion.
     * 
     * This method looks up a configuration value by key and attempts to convert
     * it to the requested type T. If the key doesn't exist or the type doesn't
     * match, returns std::nullopt.
     * 
     * The type conversion is performed through std::get<T> on the variant
     * value, which means only exact type matches are successful. For example,
     * attempting to get<int>("port") will fail if the stored value is a string.
     * 
     * This method uses the index-based lookup system for O(1) average case
     * performance.
     * 
     * @tparam T The type to convert the configuration value to (must be one of
     *           the supported types: int, int64_t, double, bool, std::string,
     *           std::vector<int>, std::vector<double>, std::vector<std::string>)
     * @param key The configuration key to look up
     * @return std::optional<T> The converted value if found and type matches,
     *         std::nullopt otherwise
     * 
     * @note The index-based lookup system provides O(1) average case performance
     *       for this operation. If the index is corrupted, a fallback linear
     *       search is performed.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Retrieves a configuration value with type conversion and default.
     * 
     * This method is a convenience wrapper around get() that provides a default
     * value when the key is not found or the type doesn't match. It eliminates
     * the need for manual checking of std::optional return values.
     * 
     * The type conversion and lookup behavior are the same as get(), but this
     * method always returns a value of type T, never std::nullopt.
     * 
     * @tparam T The type to convert the configuration value to (must be one of
     *           the supported types: int, int64_t, double, bool, std::string,
     *           std::vector<int>, std::vector<double>, std::vector<std::string>)
     * @param key The configuration key to look up
     * @param defaultValue The value to return if the key is not found or type
     *                     conversion fails
     * @return T The converted value if found and type matches, otherwise
     *         defaultValue
     * 
     * @note This method is more efficient than get() + has_value() checking when
     *       default values are needed for all lookups.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Sets a configuration value with type safety.
     * 
     * This overloaded method accepts a ConfigValue variant and stores it with
     * the specified source. The value is stored as-is without type conversion,
     * allowing for any of the supported variant types to be stored.
     * 
     * This method uses the index-based lookup system and updates both the index
     * and the entries vector to maintain O(1) lookup performance.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to store (as ConfigValue variant)
     * @param source The source of this configuration value (defaults to Runtime)
     * 
     * @note The set operation has the highest precedence in the configuration
     *       hierarchy, overriding any values from files or command-line arguments.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a configuration value from a string with automatic type conversion.
     * 
     * This method accepts a string value and attempts to parse it into the
     * appropriate type based on common patterns:
     * - "true" or "false" (case-insensitive) becomes bool
     * - Integer strings without decimal points become int
     * - Integer strings with decimal points become double
     * - Everything else is stored as string
     * 
     * This provides a convenient way to set configuration from string sources
     * like command-line arguments while maintaining type safety.
     * 
     * @param key The configuration key to set
     * @param value The string value to parse and store
     * @param source The source of this configuration value (defaults to Runtime)
     * 
     * @note This method delegates to the ConfigValue constructor for type conversion.
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a configuration value from an integer.
     * 
     * This method stores an integer value with the specified source. This is
     * the preferred way to set integer configuration values.
     * 
     * @param key The configuration key to set
     * @param value The integer value to store
     * @param source The source of this configuration value (defaults to Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a configuration value from a double.
     * 
     * This method stores a floating-point value with the specified source. This
     * is the preferred way to set double configuration values.
     * 
     * @param key The configuration key to set
     * @param value The double value to store
     * @param source The source of this configuration value (defaults to Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a configuration value from a boolean.
     * 
     * This method stores a boolean value with the specified source. This is
     * the preferred way to set boolean configuration values.
     * 
     * @param key The configuration key to set
     * @param value The boolean value to store
     * @param source The source of this configuration value (defaults to Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if a configuration key exists.
     * 
     * This method performs an O(1) lookup using the index-based system to check
     * if the specified key has been defined in the configuration.
     * 
     * @param key The configuration key to check for
     * @return true if the key exists in the configuration, false otherwise
     * 
     * @note This method always uses the index for lookup and does not perform
     *       fallback linear search. For corrupted index recovery, use get().
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key from the configuration.
     * 
     * This method removes the specified key and all associated metadata (source
     * and description) from the configuration. It also updates the index to
     * maintain the integrity of the index-based lookup system.
     * 
     * Removing a key can be useful for implementing conditional configuration
     * or cleaning up configuration state during runtime.
     * 
     * @param key The configuration key to remove
     * 
     * @note If the key doesn't exist, this method has no effect.
     * @warning Removal of configuration keys can break assumptions made by
     *          other parts of the system that depend on certain keys being present.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Retrieves all configuration keys.
     * 
     * This method returns a vector containing all keys currently defined in the
     * configuration. The order of keys is not guaranteed but is typically
     * insertion order.
     * 
     * @return std::vector<std::string> Vector containing all configuration keys
     * 
     * @note This operation is O(n) where n is the number of configuration entries.
     *       For frequent access to all keys, consider caching the result.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration entries.
     * 
     * This method removes all configuration entries and resets the index to an
     * empty state. After this operation, the configuration object will be empty
     * and any previously loaded configuration will be lost.
     * 
     * This is useful for resetting configuration state between different
     * operational modes or during testing.
     * 
     * @note This operation is O(n) where n is the number of configuration entries
     *       due to vector clearing.
     */
    void clear();
    
    /**
     * @brief Generates a human-readable summary of the configuration.
     * 
     * This method creates a formatted string representation of the current
     * configuration, including:
     * - Total number of entries
     * - Each key-value pair with proper formatting
     * - The source of each value (as integer enum)
     * - String values are properly escaped with quotes
     * 
     * The output is primarily intended for debugging and logging purposes.
     * 
     * @return std::string Formatted string representation of the configuration
     * 
     * @note This operation is O(n) where n is the number of configuration entries.
     */
    std::string summary() const;

    // ============================================================================
    // Batch Operations
    // ============================================================================
    
    /**
     * @brief Set multiple configuration values in a single operation.
     * 
     * This method efficiently sets multiple key-value pairs in one operation,
     * reducing the overhead of repeated individual set operations. All values
     * are stored with the specified source.
     * 
     * @param values Map of key-value pairs to set
     * @param source The source of all configuration values (defaults to Runtime)
     * @throws std::invalid_argument if any key is empty or value is of unsupported type
     */
    void batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Remove multiple configuration keys in a single operation.
     * 
     * This method efficiently removes multiple keys in one operation, reducing
     * the overhead of repeated individual remove operations. Only keys that
     * exist in the configuration are removed.
     * 
     * @param keys Vector of keys to remove
     */
    void batchRemove(const std::vector<std::string>& keys);
    
    /**
     * @brief Get multiple configuration values in a single operation.
     * 
     * This method efficiently retrieves multiple configuration values in one
     * operation. Returns only values that exist and have compatible types.
     * 
     * @tparam T The type to convert all values to
     * @param keys Vector of keys to retrieve
     * @return std::unordered_map<std::string, T> Map of key-value pairs found
     */
    template<typename T>
    std::unordered_map<std::string, T> batchGet(const std::vector<std::string>& keys) const;
    
    /**
     * @brief Update only specific keys, leaving others unchanged.
     * 
     * This method updates only the keys specified in the updates map, leaving
     * all other keys in the configuration unchanged. This is useful for partial
     * updates without clearing the entire configuration.
     * 
     * @param updates Map of keys to update with new values
     * @param source The source of updated configuration values (defaults to Runtime)
     * @throws std::invalid_argument if any key doesn't exist
     */
    void batchUpdate(const std::unordered_map<std::string, ConfigValue>& updates, ConfigSource source = ConfigSource::Runtime);

    // ============================================================================
    // Scripting Support
    // ============================================================================
    
    /**
     * @brief Execute a configuration script for manipulation.
     * 
     * This method accepts a script string that can manipulate configuration
     * values. Supported commands:
     * - set <key> <value> [type]
     * - get <key>
     * - remove <key>
     * - clear
     * - update <key> <value> [type]
     * 
     * Script variables can be referenced using ${var_name} syntax and are
     * replaced with their actual values before execution.
     * 
     * @param script The script string to execute
     * @return std::vector<std::string> Vector of output messages from script execution
     * @throws std::runtime_error if script syntax is invalid
     */
    std::vector<std::string> executeScript(const std::string& script);
    
    /**
     * @brief Substitute variables in a script string.
     * 
     * This method performs variable substitution in a script string, replacing
     * ${var_name} patterns with actual configuration values. If the variable
     * is not found in the configuration, it's replaced with an empty string.
     * 
     * @param script The script string to process for variable substitution
     * @return std::string The script with variables substituted
     */
    std::string scriptVar(const std::string& script) const;
    
    /**
     * @brief Evaluate a simple expression and return the result.
     * 
     * This method evaluates a simple expression string containing basic arithmetic
     * operations (+, -, *, /) and parentheses. Variables can be referenced using
     * ${var_name} syntax which are replaced with configuration values.
     * 
     * Supported types: integers, doubles, and strings (for concatenation).
     * Expression syntax follows standard arithmetic rules with operator precedence.
     * 
     * @param expression The expression string to evaluate
     * @return std::string The result of expression evaluation as a string
     * @throws std::runtime_error if expression syntax is invalid or evaluation fails
     */
    std::string scriptEval(const std::string& expression);

    // ============================================================================
    // Advanced Filtering
    // ============================================================================
    
    /**
     * @brief Get all keys from a specific configuration source.
     * 
     * This method filters and returns all keys that were loaded from the
     * specified source. The order is insertion order.
     * 
     * @param source The configuration source to filter by
     * @return std::vector<std::string> Vector of keys from the specified source
     */
    std::vector<std::string> filterBySource(ConfigSource source) const;
    
    /**
     * @brief Get all keys of a specific type.
     * 
     * This method filters and returns all keys whose values are of the specified
     * type. The order is insertion order.
     * 
     * @tparam T The type to filter by
     * @return std::vector<std::string> Vector of keys with values of the specified type
     */
    template<typename T>
    std::vector<std::string> filterByType() const;
    
    /**
     * @brief Find keys matching a wildcard pattern.
     * 
     * This method searches for keys that match the given wildcard pattern.
     * Supported wildcards:
     * - * matches any sequence of characters (except path separators)
     * - ? matches any single character
     * - [abc] matches any character in the set
     * - [!abc] matches any character not in the set
     * 
     * @param pattern The wildcard pattern to match against
     * @return std::vector<std::string> Vector of matching keys
     */
    std::vector<std::string> findKeys(const std::string& pattern) const;

    // ============================================================================
    // Import/Export Enhancements
    // ============================================================================
    
    /**
     * @brief Export configuration to a JSON string.
     * 
     * This method serializes the current configuration to a JSON string format.
     * The output includes all configuration values with their sources and types.
     * 
     * @return std::string JSON string representation of the configuration
     */
    std::string exportToJSON() const;
    
    /**
     * @brief Import configuration from a JSON string.
     * 
     * This method deserializes configuration from a JSON string format.
     * The JSON should contain key-value pairs with optional source information.
     * 
     * @param jsonString The JSON string to import
     * @return true if the configuration was successfully imported, false otherwise
     * @throws std::runtime_error if JSON parsing fails
     */
    bool importFromJSON(const std::string& jsonString);
    
    /**
     * @brief Export configuration to a YAML string.
     * 
     * This method serializes the current configuration to a YAML string format.
     * The output includes all configuration values with proper indentation and
     * type-specific formatting.
     * 
     * @return std::string YAML string representation of the configuration
     */
    std::string exportToYAML() const;
    
    /**
     * @brief Import configuration from a YAML string.
     * 
     * This method deserializes configuration from a YAML string format.
     * The YAML should contain key-value pairs with proper YAML syntax.
     * 
     * @param yamlString The YAML string to import
     * @return true if the configuration was successfully imported, false otherwise
     * @throws std::runtime_error if YAML parsing fails
     */
    bool importFromYAML(const std::string& yamlString);
    
    /**
     * @brief Compare two configurations and return differences.
     * 
     * This method compares this configuration with another configuration and
     * returns a detailed report of differences. Each difference includes:
     * - Key name
     * - Old value (if present)
     * - New value (if present)
     * - Source information for both sides
     * 
     * @param otherConfig The configuration to compare against
     * @return std::vector<std::string> Vector of difference descriptions
     */
    std::vector<std::string> diff(const Config& otherConfig) const;

    // ============================================================================
    // Configuration Templates
    // ============================================================================
    
    /**
     * @brief Apply a predefined template configuration.
     * 
     * This method applies a predefined template configuration. Common templates:
     * - "production": Production-ready configuration with security and performance settings
     * - "development": Development configuration with debug and verbose settings
     * - "testing": Test configuration with mock data and isolated settings
     * - "minimal": Minimal configuration with only essential settings
     * 
     * @param templateName The name of the template to apply
     * @return true if the template was applied successfully, false otherwise
     * @throws std::runtime_error if template doesn't exist
     */
    bool applyTemplate(const std::string& templateName);
    
    /**
     * @brief Save the current configuration as a template.
     * 
     * This method saves the current configuration as a named template that can
     * be applied later using applyTemplate(). Templates are stored in an internal
     * template registry and persist for the lifetime of the configuration system.
     * 
     * @param templateName The name for the new template
     * @param description Human-readable description of the template
     * @throws std::runtime_error if template already exists
     */
    void createTemplate(const std::string& templateName, const std::string& description = "");
    
    /**
     * @brief List all available configuration templates.
     * 
     * This method returns information about all available configuration templates,
     * including their names and descriptions.
     * 
     * @return std::unordered_map<std::string, std::string> Map of template names to descriptions
     */
    std::unordered_map<std::string, std::string> listTemplates() const;
    
    // ============================================================================
    // Helper Functions for Scripts
    // ============================================================================
    
    /**
     * @brief Check if a string matches a wildcard pattern.
     * 
     * This helper function checks if a string matches a wildcard pattern.
     * Supported wildcards:
     * - * matches any sequence of characters
     * - ? matches any single character
     * - [abc] matches any character in the set
     * - [!abc] matches any character not in the set
     * 
     * @param str The string to check
     * @param pattern The wildcard pattern
     * @return true if the string matches the pattern, false otherwise
     */
    static bool matchWildcard(const std::string& str, const std::string& pattern);
    
    /**
     * @brief Escape a string for safe inclusion in JSON.
     * 
     * This helper function escapes special characters in a string for safe
     * inclusion in JSON output. Handles:
     * - Backslashes (\\) -> \\
     * - Double quotes (\") -> \\\"
     * - Control characters -> \\uXXXX notation
     * - Newlines, carriage returns, tabs -> \\n, \\r, \\t
     * 
     * @param input The string to escape
     * @return std::string Escaped string safe for JSON inclusion
     */
    static std::string escapeJSON(const std::string& input);
    
    /**
     * @brief Parse a simple expression string.
     * 
     * This helper function parses a simple expression string into tokens for
     * evaluation. Supports basic arithmetic operations and parentheses.
     * 
     * @param expression The expression string to parse
     * @return std::vector<std::string> Vector of parsed tokens
     */
    static std::vector<std::string> parseExpression(const std::string& expression);
    
    /**
     * @brief Convert a string to a ConfigValue.
     * 
     * This helper function converts a string to a ConfigValue with automatic
     * type detection and conversion. Supports the same type detection logic
     * as the set() method from string.
     * 
     * @param value The string value to convert
     * @return ConfigValue The converted value
     */
    static ConfigValue stringToConfigValue(const std::string& value);
    
private:
    /**
     * @brief Internal implementation structure for the PIMPL pattern.
     * 
     * This structure contains the actual implementation details of the Config class,
     * including the index and entries vectors that provide the efficient lookup
     * capabilities of the configuration system.
     * 
     * The pImpl pattern is used to hide implementation details and minimize
     * compilation dependencies. Only header-only types are included in the
     * public interface.
     * 
     * @note The members of this struct are implementation details and should not
     *       be accessed directly by users of the Config class.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl; // ///< Pointer to internal implementation
    
    /**
     * @brief Trims whitespace from both ends of a string.
     * 
     * This helper method removes leading and trailing whitespace characters
     * (spaces, tabs, carriage returns, newlines) from the input string.
     * It's used primarily during configuration file parsing to normalize
     * key and value strings.
     * 
     * @param str The string to trim
     * @return std::string The trimmed string
     * 
     * @note This is a static helper method used internally by loadFromFile().
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Converts a string to lowercase.
     * 
     * This helper method performs a case-insensitive conversion by transforming
     * all uppercase characters in the input string to lowercase. This is useful
     * for case-insensitive comparisons during configuration parsing.
     * 
     * @param str The string to convert to lowercase
     * @return std::string The lowercase version of the input string
     * 
     * @note This is a static helper method used internally for boolean value
     *       parsing and comparison operations.
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Escapes special characters in a string.
     * 
     * This helper method processes a string and escapes special characters
     * to make it safe for inclusion in various contexts (e.g., configuration
     * file values). It handles:
     * - Backslashes (\\) -> \\
     * - Double quotes (\") -> \\\"
     * - Control characters (< 32 or > 126) -> \\xHH hex notation
     * - Newlines, carriage returns, tabs -> \\n, \\r, \\t
     * 
     * @param output Reference to the string that will receive the escaped result
     * @param input The string to escape
     * @return bool true if escaping completed successfully
     * 
     * @note This is a static helper method used during serialization to ensure
     *       that output strings are properly escaped.
     */
    static bool escapeString(std::string& output, const std::string& input);
    
    /**
     * @brief Formats a configuration value for output.
     * 
     * This helper method converts a ConfigValue variant to a string representation
     * suitable for output in configuration files or logs. The formatting is
     * type-specific:
     * - strings: wrapped in double quotes
     * - integer vectors: [1,2,3] format
     * - double vectors: [1.0,2.0,3.0] format
     * - string vectors: ["a","b","c"] format
     * - primitives: converted using operator<<
     * 
     * @param value The configuration value to format
     * @return std::string String representation of the value
     * 
     * @note This is a static helper method used by saveToFile() for serialization.
     */
    static std::string formatValue(const ConfigValue& value);
};;

} // namespace nlm
