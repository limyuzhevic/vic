#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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
    // Configuration sections for organization
    enum class Section {
        Brain,
        Plasticity,
        Neuromodulation,
        Memory,
        Development,
        Simulation,
        Performance,
        Debug
    };
    
    // Configuration validation
    struct ValidationError {
        std::string key;
        std::string message;
        ConfigSource source;
        
        ValidationError(const std::string& k, const std::string& m, ConfigSource s)
            : key(k), message(m), source(s) {}
    };
    
    // Configuration defaults
    struct DefaultValues {
        static constexpr int64_t DEFAULT_NEURON_COUNT = 1000;
        static constexpr int64_t DEFAULT_REGION_COUNT = 1;
        static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
        static constexpr float DEFAULT_RANDOM_SEED = 42;
        static constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
        static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
        static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
        static constexpr float DEFAULT_STDP_TAU = 20.0f;
        static constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;
        static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
        static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
        static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
        static constexpr int64_t DEFAULT_CHECKPOINT_MAX_SIZE = 10000;
        static constexpr int DEFAULT_CHECKPOINT_HISTORY = 5;
        static constexpr bool DEFAULT_CHECKPOINT_COMPRESS = true;
    };
    
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
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience methods for setting values
    void setBrainParameters(const std::string& name = "");
    void setPlasticityParameters();
    void setNeuromodulationParameters();
    void setMemoryParameters();
    void setDevelopmentParameters();
    void setSimulationParameters();
    void setPerformanceParameters();
    void setDebugParameters();
    
    // Validation and error handling
    std::vector<ValidationError> validate() const;
    bool validate(const std::string& key, std::string& error) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by section
    std::vector<std::string> getKeysBySection(Section section) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration by section
    std::string summaryBySection(Section section) const;
    
    // Validate required configuration
    bool validateRequired() const;
    
    // Get configuration statistics
    struct Stats {
        size_t totalEntries;
        size_t bySource[3]; // Default, File, CommandLine
        size_t bySection[8];
    };
    
    Stats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
