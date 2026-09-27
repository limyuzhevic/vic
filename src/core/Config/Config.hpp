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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Sleep/rest cycle configuration
    bool getEnableSleepRestCycle() const;
    void setEnableSleepRestCycle(bool enable);
    
    size_t getSleepStageDuration() const;
    void setSleepStageDuration(size_t duration);
    
    size_t getSleepCycleLength() const;
    void setSleepCycleLength(size_t length);
    
    float getNREMToREMRatio() const;
    void setNREMToREMRatio(float ratio);
    
    size_t getMemoryConsolidationInterval() const;
    void setMemoryConsolidationInterval(size_t interval);
    
    float getMemoryConsolidationStrength() const;
    void setMemoryConsolidationStrength(float strength);
    
    bool getEnableReplayDuringSleep() const;
    void setEnableReplayDuringSleep(bool enable);
    
    size_t getReplayIntervalDuringSleep() const;
    void setReplayIntervalDuringSleep(size_t interval);
    
    size_t getReplayBatchSize() const;
    void setReplayBatchSize(size_t size);
    
    float getNeuromodulationSleepLow() const;
    void setNeuromodulationSleepLow(float low);
    
    float getNeuromodulationSleepHigh() const;
    void setNeuromodulationSleepHigh(float high);
    
    float getAttentionSleepSuppression() const;
    void setAttentionSleepSuppression(float suppression);
    
    bool getEnableDevelopmentDuringSleep() const;
    void setEnableDevelopmentDuringSleep(bool enable);
    
    float getDevelopmentRateSleep() const;
    void setDevelopmentRateSleep(float rate);
    
    bool getEnablePredictionUpdateSleep() const;
    void setEnablePredictionUpdateSleep(bool enable);
    
float getPredictionUpdateStrength() const;
    void setPredictionUpdateStrength(float strength);

    // Neuromodulator (ACh, NE, 5-HT) configuration
    float getNeuromodulationAChBaseline() const;
    void setNeuromodulationAChBaseline(float baseline);
    
    float getNeuromodulationAChPeak() const;
    void setNeuromodulationAChPeak(float peak);
    
    float getNeuromodulationAChDecayRate() const;
    void setNeuromodulationAChDecayRate(float decayRate);
    
    float getNeuromodulationNEActivationThreshold() const;
    void setNeuromodulationNEActivationThreshold(float threshold);
    
    float getNeuromodulationNESleepLow() const;
    void setNeuromodulationNESleepLow(float low);
    
    float getNeuromodulationNESleepHigh() const;
    void setNeuromodulationNESleepHigh(float high);
    
    float getNeuromodulation5HTBaseline() const;
    void setNeuromodulation5HTBaseline(float baseline);
    
    float getNeuromodulation5HTReleaseRate() const;
    void setNeuromodulation5HTReleaseRate(float releaseRate);
    
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
