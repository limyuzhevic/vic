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

// Default configuration constants
namespace DefaultConfig {
    constexpr float STDP_LTP_WEIGHT = 0.01f;
    constexpr float STDP_LTD_WEIGHT = 0.012f;
    constexpr float STDP_TIME_CONSTANT = 20.0f;
    constexpr float SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float PRUNING_RATE = 0.00001f;
    constexpr float SIMULATION_TIMESTEP = 0.001f;
    constexpr float NEURON_COUNT = 1000.0f;
    constexpr float REGION_COUNT = 1.0f;
    constexpr float CONNECTION_PROBABILITY = 0.1f;
    constexpr uint64_t RANDOM_SEED = 42;
    constexpr size_t VISUALIZATION_WIDTH = 256;
    constexpr size_t VISUALIZATION_HEIGHT = 256;
    constexpr float NEURON_RESTING_POTENTIAL = -70.0f;
    constexpr float NEURON_THRESHOLD = -55.0f;
    constexpr float MAX_SPIKE_HISTORY = 100;
    constexpr float TIME_CONSTANT = 20.0f;
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    constexpr float ADAPTATION_DECAY = 0.95f;
    constexpr float ADAPTATION_INCREMENT = 1.0f;
    constexpr float REFRACTORY_PERIOD_MIN = 2;
    constexpr float REFRACTORY_PERIOD_MAX = 10;
    constexpr float ADAPTATION_STRENGTH = 0.01f;
    constexpr float ADAPTATION_RATE = 0.95f;
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
