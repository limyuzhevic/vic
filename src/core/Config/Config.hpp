#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Configuration value types
// 
// The configuration system uses a variant type to store different kinds of
// values. This allows flexible configuration with support for:
// - Basic types: integers, floats, booleans, strings
// - Collections: integer arrays, double arrays, string arrays
// 
// The variant ensures type safety while maintaining flexibility for
// different configuration needs.
// 
// Usage:
//   ConfigValue val = 42;           // int
//   ConfigValue val = "hello";       // string
//   ConfigValue val = std::vector<int>{1,2,3}; // int array
// 
// When retrieving values, the template get() method safely extracts
// the appropriate type or returns nullopt if type mismatch.

using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
]>;

// Configuration source
// 
// Indicates the source of a configuration value, which affects its
// priority and how it should be interpreted:
// - Default: Built into the system
// - File: Loaded from configuration file
// - CommandLine: From command-line arguments
// - Runtime: Set programmatically at runtime

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

// Configuration entry
// 
// Represents a single configuration key-value pair with metadata about its
// origin and description. This structure is used internally to track
// configuration values and their sources.
// 
// Each configuration entry stores:
// - key: The configuration parameter name (e.g., "neuron_count")
// - value: The actual value (using ConfigValue variant type)
// - source: Where this value came from (Default, File, CommandLine, Runtime)
// - description: Human-readable explanation of the parameter's purpose
// 
// Configuration source hierarchy affects priority:
// Runtime > CommandLine > File > Default
// Later settings override earlier ones with same keys.
// 
// Example usage:
//   ConfigEntry entry("learning_rate", 0.01f, ConfigSource::Default, 
//                    "Neuronal learning rate (0.0-1.0)");

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
// 
// The central configuration manager for the NLM system. Provides a unified
// interface to set, get, and manage configuration parameters from multiple
// sources (default values, files, command line, runtime).
// 
// Configuration is stored as key-value pairs with metadata. The class supports
// multiple value types (int, float, string, arrays) and type-safe retrieval.
// 
// Key features:
// - Multiple configuration sources with priority resolution
// - Type-safe get() methods with optional<T> returns
// - File-based configuration (JSON/INI format)
// - Command-line argument parsing
// - Configuration validation and summary
// - Save/load capability
// 
// Typical workflow:
//   Config config;
//   config.loadFromFile("nlm.cfg");
//   config.loadFromArgs(argc, argv);
//   config.set("neuron_count", 1000);
//   
//   // Safe retrieval with defaults
//   auto count = config.get<size_t>("neuron_count");
//   size_t count = config.getOr<size_t>("neuron_count", 1000);
//   
//   // Check if parameter exists
//   if (config.has("learning_rate")) {
//       // Process learning_rate
//   }
// 
// Example configuration file format (simplified):
//   # Neuron system configuration
//   neuron_count = 1000
//   "region_count" = 2
//   learning_rate = 0.01
//   max_energy = 1.0
// 
// See also: nlm::ConfigSource for source priority rules

class Config {
public:
    // Create empty configuration
     // 
     // Constructor creates a new configuration object with default settings.
     // The configuration starts with no entries, ready to be populated from
     // default values, files, command line, or runtime.
     // 
     // Example:
     //   Config config;
     //   // Now ready to load configuration
     //   bool success = config.loadFromFile("nlm.cfg");
     //   if (!success) {
     //       std::cerr << "Failed to load config file" << std::endl;
     //   }

    Config();
    
    // Load from file (JSON format)
     // 
     // Loads configuration from a file. Supports simple key=value format
     // with optional comments (# or //) and quoted values.
     // 
     // Parameters:
     //   filepath: Path to the configuration file
     // 
     // Returns:
     //   bool true if file loaded successfully, false if file not found or parse error
     // 
     // File format example:
     //   # NLM Configuration
     //   neuron_count = 1000
     //   "learning_rate" = 0.01
     //   region_count = 2
     //   
     // Note: For complex configurations, consider using JSON/YAML format
     // in Phase 2. Currently implements Phase 1 simple parser.
     // 
     // Example:
     //   Config config;
     //   if (config.loadFromFile("nlm.cfg")) {
     //       std::cout << "Config loaded successfully" << std::endl;
     //   } else {
     //       std::cerr << "Failed to load config from nlm.cfg" << std::endl;
     //   }

    bool loadFromFile(const std::string& filepath);

    // Load from command line arguments
     // 
     // Parses command-line arguments to populate configuration.
     // Supports both --key=value and -k value formats.
     // 
     // Parameters:
     //   argc: Number of command-line arguments (from main())
     //   argv: Command-line argument array (from main())
     // 
     // Supported formats:
     //   --neuron_count=1000
     //   -r 2
     //   
     // Example in main():
     //   int main(int argc, char** argv) {
     //       Config config;
     //       config.loadFromArgs(argc, argv);
     //       // config now contains values from command line
     //   }
     // 
     // Returns:
     //   bool true if parsing succeeded, false otherwise

    bool loadFromArgs(int argc, char** argv);

    // Save to file
     // 
     // Saves current configuration to a file. The saved configuration
     // includes all current key-value pairs with their descriptions.
     // 
     // Parameters:
     //   filepath: Path where to save the configuration
     // 
     // Returns:
     //   bool true if save succeeded, false if file cannot be opened/written
     // 
     // The saved file can be reloaded with loadFromFile().
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   if (config.saveToFile("nlm.cfg")) {
     //       std::cout << "Configuration saved" << std::endl;
     //   }

    bool saveToFile(const std::string& filepath) const;
    
// Get values
     // 
     // Type-safe value retrieval with optional<T> return to handle missing keys.
     // The template method safely extracts values of different types from the
     // variant ConfigValue. If the stored value type doesn't match the requested
     // type, returns std::nullopt.
     // 
     // Parameters:
     //   key: Configuration parameter name to retrieve
     // 
     // Returns:
     //   std::optional<T> value if key exists and type matches, std::nullopt otherwise
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   auto count = config.get<size_t>("neuron_count");
     //   if (count) {
     //       std::cout << "Neuron count: " << *count << std::endl;
     //   } else {
     //       std::cout << "neuron_count not found or wrong type" << std::endl;
     //   }
     // 
     // Template examples:
     //   auto val = config.get<int>("timeout");
     //   auto val = config.get<double>("learning_rate");
     //   auto val = config.get<std::string>("name");
     //   auto val = config.get<std::vector<int>>("ids");

    template<typename T>
    std::optional<T> get(const std::string& key) const;

    // Get value with default fallback
     // 
     // Alternative to get() that returns a default value if the key doesn't exist
     // or has the wrong type. This is more convenient for required parameters.
     // 
     // Parameters:
     //   key: Configuration parameter name to retrieve
     //   defaultValue: Value to return if key not found or type mismatch
     // 
     // Returns:
     //   T value if key exists and type matches, otherwise defaultValue
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   size_t count = config.getOr<size_t>("neuron_count", 500);
     //   // Returns 1000 (from config), or 500 if key not found
     //   
     //   size_t missing = config.getOr<size_t>("missing_param", 100);
     //   // Returns 100 (default)

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check existence
     // 
     // Checks if a configuration parameter exists.
     // 
     // Parameters:
     //   key: Configuration parameter name to check
     // 
     // Returns:
     //   bool true if the key exists, false otherwise
     // 
     // Example:
     //   Config config;
     //   config.set("learning_rate", 0.01f);
     //   if (config.has("learning_rate")) {
     //       std::cout << "Configuration present" << std::endl;
     //   }

    bool has(const std::string& key) const;

    // Remove key
     // 
     // Removes a configuration parameter from the configuration.
     // 
     // Parameters:
     //   key: Configuration parameter name to remove
     // 
     // Example:
     //   Config config;
     //   config.set("temp_param", 42);
     //   config.remove("temp_param");
     //   // temp_param is now removed from config

    void remove(const std::string& key);

    // Get all keys
     // 
     // Returns:
     //   std::vector<std::string> All configuration parameter names
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   config.set("learning_rate", 0.01f);
     //   auto keys = config.getKeys();
     //   for (const auto& key : keys) {
     //       std::cout << key << std::endl;
     //   }
     //   // Output: learning_rate, neuron_count

    std::vector<std::string> getKeys() const;

    // Clear all
     // 
     // Removes all configuration entries.
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   config.clear();
     //   // config now has no entries

    void clear();

    // Get configuration summary
     // 
     // Returns a human-readable summary of the current configuration.
     // 
     // Returns:
     //   std::string Multi-line summary showing all entries with their values and sources
     // 
     // Example:
     //   Config config;
     //   config.set("neuron_count", 1000);
     //   config.set("learning_rate", 0.01f, ConfigSource::File);
     //   std::cout << config.summary();
     //   // Output:
     //   // Configuration (2 entries):
     //   //   learning_rate = "0.01" (1)
     //   //   neuron_count = [1000] (3)
     //   // 
     //   // Note: numbers represent ConfigSource enum values

    std::string summary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
