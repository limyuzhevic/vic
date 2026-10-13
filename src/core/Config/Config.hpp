#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * @class Config
 * @brief Configuration management class for NLM system
 * @details Provides a flexible configuration system supporting JSON, YAML, and simple key=value formats.
 *          Supports type-safe value storage with automatic type conversion.
 * 
 * The configuration system allows:
 * - Loading from JSON, YAML, or simple key=value files
 * - Setting configuration values from command line arguments
 * - Type-safe access to configuration values (int, double, string, bool, vectors)
 * - Configuration value validation and error handling
 * - Serialization to file formats for persistence
 * 
 * Example usage:
 * @code
 * nlm::Config config;
 * config.loadFromFile("config.json");
 * int timeout = config.getOr<int>("timeout", 1000);
 * config.set("verbose", true);
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Copy constructor deleted (non-copyable)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator deleted (non-copyable)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * @param other The Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other The Config object to move from
     * @return Reference to this Config object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false otherwise
     * @details Automatically detects file format based on extension:
     *          - JSON files (.json) - preferred format
     *          - YAML files (.yaml, .yml) - YAML format if available
     *          - Other files - simple key=value format
     * 
     * If no extension is specified, JSON format is assumed.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Number of command line arguments
     * @param argv Command line arguments array
     * @return true if arguments parsed successfully, false otherwise
     * @details Supports:
     *          - --key=value format (long options)
     *          - -key value format (short options)
     *          - Values are automatically converted to appropriate types
     * 
     * Examples:
     * @code
     * // Command line: myapp --port 8080 --debug=true
     * config.loadFromArgs(3, {"myapp", "--port", "8080", "--debug=true"});
     * int port = config.getOr<int>("port", 8080);
     * @endcode
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * @param filepath Path to output configuration file
     * @return true if file saved successfully, false otherwise
     * @details Automatically detects file format based on extension:
     *          - JSON files (.json) - preferred format with metadata
     *          - YAML files (.yaml, .yml) - YAML format if available
     *          - Other files - simple key=value format
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key
     * @tparam T Type of value to retrieve (int, double, string, bool, or vector types)
     * @param key Configuration key
     * @return Optional containing value if key exists and type matches, empty otherwise
     * @details Type-safe access with automatic type checking:
     *          - If key doesn't exist, returns empty optional
     *          - If value exists but type doesn't match, returns empty optional
     *          - Type conversion is handled by the configuration system
     * 
     * Example:
     * @code
     * auto timeout = config.get<int>("timeout");
     * if (timeout.has_value()) {
     *     std::cout << "Timeout: " << timeout.value() << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value to return if key doesn't exist
     * @return Value from configuration or default value
     * @details Type-safe access with default fallback:
     *          - If key exists and type matches, returns configured value
     *          - Otherwise, returns default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * @tparam T Type of value to set
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of the configuration (File, CommandLine, Runtime, etc.)
     * @details Overloads for different value types:
     *          - void set(const std::string&, const ConfigValue&, ConfigSource)
     *          - void set(const std::string&, const std::string&, ConfigSource)
     *          - void set(const std::string&, int, ConfigSource)
     *          - void set(const std::string&, double, ConfigSource)
     *          - void set(const std::string&, bool, ConfigSource)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return true if key exists in configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     * @details Removes the key and its value from configuration.
     *          Has no effect if key doesn't exist.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * @details Removes all key-value pairs from configuration.
     *          Configuration is reset to empty state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * @return String containing configuration summary with all entries
     * @details Generates human-readable summary showing:
     *          - Number of configuration entries
     *          - Each key-value pair and its source
     *          - Used for debugging and logging
     */
    std::string summary() const;
    
private:
    /**
     * @brief Private implementation structure for Config
     * @details Internal implementation details to reduce header size and improve compilation times
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Load configuration from JSON file
     * @param filepath Path to JSON configuration file
     * @return true if JSON parsed successfully, false otherwise
     */
    bool loadFromJSON(const std::string& filepath);
    
    /**
     * @brief Load configuration from YAML file
     * @param filepath Path to YAML configuration file
     * @return true if YAML parsed successfully, false otherwise
     */
    bool loadFromYAML(const std::string& filepath);
    
    /**
     * @brief Load configuration from simple key=value file
     * @param filepath Path to simple configuration file
     * @return true if file parsed successfully, false otherwise
     */
    bool loadFromSimpleFormat(const std::string& filepath);
    
    /**
     * @brief Convert ConfigValue to JSON format
     * @param value Configuration value to convert
     * @return JSON representation of configuration value
     * @tparam T Type of value to convert
     */
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    
    /**
     * @brief Convert JSON to ConfigValue
     * @param jsonValue JSON value to convert
     * @return Configuration value representation
     * @details Handles all supported JSON types and converts them to appropriate
     *          ConfigValue variants (string, int, double, bool, vectors)
     */
    static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
    
    /**
     * @brief Convert ConfigValue to string representation
     * @param value Configuration value to convert
     * @return String representation of configuration value
     */
    static std::string configValueToString(const ConfigValue& value);
    
    /**
     * @brief Validate configuration value type
     * @param value Configuration value to validate
     * @param key Configuration key being validated
     * @return true if value type is valid, false otherwise
     * @details Basic validation that can be extended for more sophisticated schema validation
     */
    static bool validateValueType(const ConfigValue& value, const std::string& key);
    
    /**
     * @brief Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str String to convert
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
