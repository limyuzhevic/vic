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

// Configuration source enumeration
// Defines where configuration values originate
enum class ConfigSource {
    Default,      // Built-in default values
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments
    Runtime       // Set at runtime via API
};

// Configuration entry structure
// Represents a single key-value pair with metadata
struct ConfigEntry {
    std::string key;          // Configuration key name
    ConfigValue value;        // Stored configuration value
    ConfigSource source;      // How this value was obtained
    std::string description;  // Human-readable description
    
    // Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Parameterized constructor
    // @param k Configuration key
    // @param v Configuration value
    // @param s Source of this configuration value
    // @param desc Optional description
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
// Provides key-value storage with type safety and multiple value sources
class Config {
public:
    // Constructor - creates empty configuration
    Config();
    
    // Destructor
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // ========== CONFIGURATION LOADING ==========
    
    // Load configuration from file
    // Supports simple key=value format (Phase 1) with plans for JSON/YAML
    // @param filepath Path to configuration file
    // @return bool true if file loaded successfully, false otherwise
    // @throws std::filesystem::filesystem_error if file cannot be accessed
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports both --key=value and -key value formats
    // @param argc Argument count (from main)
    // @param argv Argument vector (from main)
    // @return bool true if parsing completed, false on critical error
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Exports current configuration to persistent storage
    // @param filepath Path where configuration should be saved
    // @return bool true if save successful, false on error
    bool saveToFile(const std::string& filepath) const;
    
    // ========== CONFIGURATION ACCESS ==========
    
    // Get configuration value of specific type
    // Returns nullopt if key doesn't exist or type mismatch
    // @tparam T Expected type (must match ConfigValue variant member)
    // @param key Configuration key to retrieve
    // @return std::optional<T> Value if found and type matches, nullopt otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    // Returns default value if key doesn't exist or type mismatch
    // @tparam T Expected type (must match ConfigValue variant member)
    // @param key Configuration key to retrieve
    // @param defaultValue Value to return if key not found
    // @return T Value if key exists and type matches, otherwise defaultValue
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // ========== CONFIGURATION MODIFICATION ==========
    
    // Set configuration value (generic version)
    // Creates new configuration entry or updates existing one
    // @param key Configuration key
    // @param value Value to store
    // @param source Origin of this configuration value
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set configuration value (type-specific versions)
    // Convenience overloads for common types
    // @param key Configuration key
    // @param value Value to store
    // @param source Origin of this configuration value
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // ========== CONFIGURATION MANAGEMENT ==========
    
    // Check if configuration key exists
    // @param key Configuration key to check
    // @return bool true if key exists in configuration
    bool has(const std::string& key) const;
    
    // Remove configuration key
    // @param key Configuration key to remove
    void remove(const std::string& key);
    
    // Get all configuration keys
    // @return std::vector<std::string> List of all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // Get configuration summary
    // Provides overview of configuration state and sources
    // @return std::string Human-readable configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    // @param str String to trim
    // @return std::string String with leading/trailing whitespace removed
    static std::string trim(const std::string& str);
    
    // @param str String to convert
    // @return std::string Lowercase version of input string
    static std::string toLower(const std::string& str);
};

} // namespace nlm

// Template method implementations
// These would typically be in the .cpp file with proper documentation

// get() method implementation
// Provides type-safe access to configuration values
// 
// Usage examples:
//   auto value = config.get<int>("neuron_count");
//   if (value.has_value()) {
//       int neurons = value.value();
//   }
//   
// Note: This method will return nullopt for non-existent keys or type mismatches

// getOr() method implementation  
// Provides safe access with default values
// 
// Usage examples:
//   int neurons = config.getOr<int>("neuron_count", 1000);
//   bool debug = config.getOr<bool>("debug_mode", false);
//   std::string name = config.getOr<std::string>("name", "DefaultBrain");

// set() method implementations
// Support multiple configuration value types
// 
// Usage examples:
//   config.set("learning_rate", 0.01);
//   config.set("max_epochs", 100);
//   config.set("use_gpu", true);
//   config.set("model_name", std::string("NeuralNet"));
