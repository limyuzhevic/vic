#pragma once

/**
 * @file Config.hpp
 * @brief Configuration system for NLM neural simulation framework
 * 
 * This file defines the Config class, which provides a flexible configuration
 * system for the NLM brain. It supports multiple configuration formats,
 * including JSON, YAML, and key=value styles, with validation and type safety.
 * 
 * @details The configuration system is used to:
 * - Set brain parameters (neuron count, region count, connectivity)
 * - Configure plasticity mechanisms (STDP, Hebbian, structural)
 * - Set neuromodulation parameters (dopamine, curiosity, novelty)
 * - Configure simulation settings (timestep, checkpointing)
 * - Load configuration from files and command line arguments
 * - Save configuration to files for persistence
 * 
 * @note The configuration system uses std::variant to support multiple
 *       value types (int, double, bool, string, vectors) while maintaining
 *       type safety through template accessors.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <filesystem>

namespace nlm {

/**
 * @brief Configuration source enumeration
 * 
 * Tracks where configuration values come from, useful for determining
 * which settings are user-provided vs. defaults.
 */
enum class ConfigSource {
public:
    /** Configuration source types */
    enum Type {
        Default = 0,      ///< Default built-in configuration
        File = 1,         ///< Loaded from configuration file
        CommandLine = 2,  ///< Loaded from command line arguments
        Runtime = 3       ///< Set programmatically at runtime
    };
    
    ConfigSource() : type(Type::Default) {}
    explicit ConfigSource(Type t) : type(t) {}
    
    Type getType() const { return type; }
    bool isUserProvided() const { return type == Type::File || type == Type::CommandLine || type == Type::Runtime; }
    bool isDefault() const { return type == Type::Default; }
    
private:
    Type type;
};

/**
 * @brief Configuration value type supporting multiple data types
 * 
 * Uses std::variant to support heterogeneous configuration values while
 * maintaining type safety through template accessors. Supports common
 * configuration types including scalars, strings, and collections.
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
 * @brief Configuration entry with metadata
 * 
 * Represents a single configuration key-value pair with additional metadata
 * including the source of the value and a description for documentation.
 */
struct ConfigEntry {
    std::string key;                ///< Configuration key (e.g., "brain.neuron_count")
    ConfigValue value;              ///< Configuration value
    ConfigSource source;            ///< Source of the configuration value
    std::string description;        ///< Human-readable description of the setting
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of the setting
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    /**
     * @brief Check if entry has a specific key
     * 
     * @param k Key to check
     * @return true if this entry has the specified key
     */
    bool hasKey(const std::string& k) const { return key == k; }
    
    /**
     * @brief Get key as string
     */
    const std::string& getKey() const { return key; }
    
    /**
     * @brief Get value
     */
    const ConfigValue& getValue() const { return value; }
    
    /**
     * @brief Get source
     */
    const ConfigSource& getSource() const { return source; }
    
    /**
     * @brief Get description
     */
    const std::string& getDescription() const { return description; }
};

/**
 * @brief Main configuration class for NLM system
 * 
 * This class provides a comprehensive configuration system for the NLM brain,
 * supporting multiple input formats, type safety, validation, and persistence.
 * 
 * @details Key features:
 * - **Multiple formats**: JSON, YAML, TOML, key=value
 * - **Type safety**: Template accessors with compile-time type checking
 * - **Source tracking**: Distinguish between defaults, file, CLI, and runtime settings
 * - **Validation**: Range checking and parameter validation
 * - **Persistence**: Save/load with different formats
 * - **Documentation**: Descriptors for all configuration options
 * 
 * @note Uses Pimpl pattern to reduce compilation dependencies and improve
 *       build times. All heavy-weight parsing is implemented in .cpp file.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates configuration with built-in defaults. Use loadFromFile() or
     * loadFromArgs() to override defaults with user settings.
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disable copying
     * 
     * Configurations should not be copied - use move operations instead.
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Enable move assignment
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Supports multiple file formats (JSON, YAML, TOML, key=value) with
     * automatic format detection based on file extension. Overwrites existing
     * configuration values with file contents.
     * 
     * @param filepath Path to configuration file
     * @return true if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports multiple argument formats:
     * - `--key=value` (single argument)
     * - `-key value` (paired arguments)
     * 
     * Command line arguments take precedence over file configuration.
     * 
     * @param argc Argument count (from main)
     * @param argv Argument values (from main)
     * @return true if arguments were processed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Supports multiple output formats (JSON, YAML, TOML, key=value) based on
     * file extension. Only user-provided configuration (File, CommandLine,
     * Runtime sources) is saved - defaults are omitted.
     * 
     * @param filepath Path to output file
     * @param format Output format (auto-detected from extension if not specified)
     * @return true if file was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath, const std::string& format = "");
    
    /**
     * @brief Set configuration value
     * 
     * @tparam T Expected value type
     * @param key Configuration key
     * @param value Value to set
     * @param source Configuration source (default: Runtime)
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Set string configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Set integer configuration value
     */
    void set(const std::string& key, int value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Set 64-bit integer configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Set floating-point configuration value
     */
    void set(const std::string& key, double value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Set boolean configuration value
     */
    void set(const std::string& key, bool value, ConfigSource::Type source = ConfigSource::Type::Runtime);
    
    /**
     * @brief Check if configuration has a specific key
     * 
     * @param key Key to check
     * @return true if configuration contains the key
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Expected value type
     * @param key Configuration key
     * @return Configuration value if key exists and type matches, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default
     * 
     * @tparam T Expected value type
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Remove configuration entry
     */
    void remove(const std::string& key);
    
    /**
     * @brief Remove all configuration entries
     */
    void clear();
    
    /**
     * @brief Get all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Get configuration summary for display
     */
    std::string summary() const;
    
    /**
     * @brief Get configuration value as string
     * 
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return String representation of configuration value
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief Get configuration value as integer
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief Get configuration value as double
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief Get configuration value as boolean
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
private:
    /**
     * @brief Pimpl implementation structure
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Detect file format from extension
     */
    static std::string detectFormat(const std::string& filepath);
    
    /**
     * @brief Parse configuration file
     */
    bool parseFile(const std::string& filepath, const std::string& format);
    
    /**
     * @brief Write configuration to file
     */
    bool writeToFile(const std::string& filepath, const std::string& format);
};

} // namespace nlm