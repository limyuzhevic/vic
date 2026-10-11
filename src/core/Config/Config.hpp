#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

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
    Runtime,
    Environment
};

// Configuration categories
enum class ConfigCategory {
    Basic,
    Neural,
    Advanced,
    Performance,
    Development,
    Experiment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    std::string validationPattern;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string defaultValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    category(ConfigCategory::Basic), validationPattern(), 
                    minValue(), maxValue(), defaultValue() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", ConfigCategory cat = ConfigCategory::Basic,
                const std::string& pattern = "", std::optional<double> min = std::nullopt,
                std::optional<double> max = std::nullopt, const std::string& def = "")
        : key(k), value(v), source(s), description(desc), category(cat), 
          validationPattern(pattern), minValue(min), maxValue(max), defaultValue(def) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "vector<int>", etc.
    std::string description;
    ConfigCategory category;
    std::string validationPattern;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    ConfigValue defaultValue;
    bool required;
    std::vector<std::string> validValues;
    nlohmann::json config; // Store schema info
};

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const ConfigEntry& entry) const = 0;
    virtual std::string getErrorMessage() const = 0;
};

// Configuration manager with categories and profiles
class ConfigManager {
public:
    enum class Profile {
        Debug,
        Release,
        Benchmark
    };
    
    ConfigManager();
    ~ConfigManager();
    
    // Profile management
    void setProfile(Profile profile);
    Profile getProfile() const;
    void applyProfileDefaults();
    
    // Configuration categories
    void addAdvancedConfig();
    void addPerformanceConfig();
    void addDevelopmentConfig();
    void addNeuralConfig();
    
    // Validation
    bool validateConfig(const ConfigEntry& entry) const;
    bool validateAllConfig() const;
    
    // JSON support
    static nlohmann::json serialize(const ConfigEntry& entry);
    static ConfigEntry deserialize(const nlohmann::json& data);
    
    // Backup and inheritance
    static bool backupConfigFile(const std::string& filepath);
    bool loadFromParentProfile(const ConfigManager& parent);
    
    // Configuration templates
    static nlohmann::json getDefaultTemplate(ConfigCategory category);
    
    // Get config by category
    std::vector<ConfigEntry> getConfigByCategory(ConfigCategory category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Default configuration providers
namespace DefaultConfig {
    // Basic defaults
    static constexpr int BASIC_NEURONS = 1000;
    static constexpr double BASIC_LEARNING_RATE = 0.01;
    static constexpr int BASIC_TIME_STEPS = 100;
    
    // Advanced defaults
    static constexpr double ADVANCED_STDP_LTP_WEIGHT = 0.1;
    static constexpr double ADVANCED_STDP_LTD_WEIGHT = 0.12;
    static constexpr double ADVANCED_STDP_TIME_CONST = 20.0;
    static constexpr double ADVANCED_LEARNING_RATE = 0.05;
    
    // Performance defaults
    static constexpr int PERFORMANCE_THREAD_COUNT = 4;
    static constexpr bool PERFORMANCE_SIMD_ENABLED = true;
    static constexpr size_t PERFORMANCE_MEMORY_POOL_SIZE = 65536;
    static constexpr bool PERFORMANCE_OPTIMIZATION_ENABLED = true;
    
    // Development defaults
    static constexpr int DEVELOPMENT_DEBUG_LEVEL = 1;
    static constexpr bool DEVELOPMENT_PROFILING_ENABLED = false;
    static constexpr bool DEVELOPMENT_VALIDATION_ENABLED = true;
    
    // Neural defaults
    static constexpr double NEURAL_SYNAPTIC_DENSITY = 0.1;
    static constexpr int NEURAL_POPULATION_SIZE = 100;
    static constexpr double NEURAL_REGION_SIZE = 1.0;
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
    
    // Validation methods
    static bool validateFilePath(const std::string& filepath);
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
