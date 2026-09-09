#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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
    std::string category;
    std::string subcategory;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category("general"), subcategory("") {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general"), subcategory("") {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, const std::string& cat, const std::string& subcat = "")
        : key(k), value(v), source(s), description(desc), category(cat), subcategory(subcat) {}
};

// Configuration categories
namespace ConfigCategory {
    constexpr const char* General = "general";
    constexpr const char* Performance = "performance";
    constexpr const char* Neural = "neural";
    constexpr const char* Learning = "learning";
    constexpr const char* Memory = "memory";
    constexpr const char* Development = "development";
    constexpr const char* Neuromodulation = "neuromodulation";
    constexpr const char* Experimental = "experimental";
    constexpr const char* Debugging = "debugging";
    constexpr const char* Session = "session";
    constexpr const char* Input = "input";
    constexpr const char* Output = "output";
}

// Configuration subcategories
namespace ConfigSubcategory {
    constexpr const char* Basic = "basic";
    constexpr const char* Advanced = "advanced";
    constexpr const char* Optimization = "optimization";
    constexpr const char* Timestep = "timestep";
    constexpr const char* Connectivity = "connectivity";
    constexpr const char* Plasticity = "plasticity";
    constexpr const char* STDP = "stdp";
    constexpr const char* Hebbian = "hebbian";
    constexpr const char* Structural = "structural";
    constexpr const char* MemorySystems = "memory_systems";
    constexpr const char* Development = "development";
    constexpr const char* Neuromodulation = "neuromodulation";
    constexpr const char* Experiment = "experiment";
    constexpr const char* Debug = "debug";
    constexpr const char* Verbosity = "verbosity";
    constexpr const char* Checkpoint = "checkpoint";
    constexpr const char* Seed = "seed";
    constexpr const char* Logging = "logging";
}

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
    bool loadFromArgs(int argc, char** argv);
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
