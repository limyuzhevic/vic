#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

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

// CLI argument definition
struct CLIArgument {
    std::string name;
    std::string shortName;
    std::string description;
    std::string defaultValue;
    bool required;
    std::function<void(const std::string&)> callback;
    std::vector<std::string> valueTypes; // string, int, double, bool
};

// Help and version information
struct HelpInfo {
    std::string programName;
    std::string version;
    std::string description;
    std::string usage;
    std::vector<CLIArgument> arguments;
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, const HelpInfo& helpInfo = HelpInfo());
    
    // Load from command line arguments with validation
    bool loadFromArgsWithValidation(int argc, char** argv, 
                                  const std::map<std::string, CLIArgument>& arguments,
                                  const HelpInfo& helpInfo = HelpInfo());
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Help and validation methods
    static bool handleHelpFlag(int argc, char** argv, const HelpInfo& helpInfo = HelpInfo());
    static void printHelp(const HelpInfo& helpInfo);
    static void printVersion(const HelpInfo& helpInfo);
    static std::string validateArgument(const CLIArgument& arg, const std::string& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Argument parsing helpers
    static bool parseKeyValueFormat(const std::string& arg, Config& config, const std::map<std::string, CLIArgument>& arguments);
    static bool parseFlagFormat(const std::string& arg, Config& config);
};

} // namespace nlm
