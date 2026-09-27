#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
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
     * @brief Load configuration from a JSON file
     * @param filepath Path to JSON configuration file
     * @return true if loading succeeded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if loading succeeded, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file
     * @param filepath Path to output JSON file
     * @return true if saving succeeded, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type checking
     * @tparam T Type to retrieve (int, double, bool, string, or vector types)
     * @param key Configuration key (dot-separated path)
     * @return Optional containing the value if key exists, empty optional otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Type to retrieve (int, double, bool, string, or vector types)
     * @param key Configuration key (dot-separated path)
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value or default if key missing
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * @param key Configuration key (dot-separated path)
     * @param value Configuration value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from string
     * @param key Configuration key (dot-separated path)
     * @param value String representation of value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from integer
     * @param key Configuration key (dot-separated path)
     * @param value Integer value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from double
     * @param key Configuration key (dot-separated path)
     * @param value Double value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from boolean
     * @param key Configuration key (dot-separated path)
     * @param value Boolean value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return Vector of all configuration keys (dot-separated paths)
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * @return String containing human-readable summary of configuration
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
