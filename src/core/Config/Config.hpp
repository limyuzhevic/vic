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

    // Set values with automatic type conversion
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe setters with validation
    template<typename T>
    void setValidated(const std::string& key, const T& value, 
                      std::function<bool(const T&)> validator = nullptr,
                      const std::string& errorMessage = "Validation failed",
                      ConfigSource source = ConfigSource::Runtime);
    
    // Get values with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key,
                                 std::function<bool(const T&)> validator = nullptr,
                                 const std::string& errorMessage = "Validation failed") const;
    
    // Convenience methods for common configuration patterns
    void setBrainParameters(const std::string& brainName = "default");
    void setPlasticityParameters(const std::string& plasticityMode = "stdp");
    void setNeuromodulationParameters(const std::string& neuromodulationMode = "basic");
    void setDevelopmentParameters(const std::string& developmentStage = "adult");
    
    // Get configuration summary with detailed statistics
    std::string detailedSummary() const;
    
    // Configuration validation
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Import/export configuration sections
    void importSection(const std::string& sectionName, const Config& source);
    void exportSection(std::string& sectionName, Config& destination) const;
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration snapshot and rollback
    std::string createSnapshot() const;
    bool restoreSnapshot(const std::string& snapshot);
    
    // Get configuration keys by pattern (regex support)
    std::vector<std::string> getKeysByPattern(const std::string& pattern) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::vector<ConfigSource> getSources() const;
    
    // Get values for specific sources
    template<typename T>
    std::vector<T> getValuesForSource(const std::string& key, ConfigSource source) const;
    
    // Configuration versioning
    std::string getVersion() const;
    void setVersion(const std::string& version);
    
    // Get configuration change history
    std::vector<std::pair<std::string, std::string>> getChangeHistory() const;
    void clearChangeHistory();
    
private:
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static ConfigValue stringToValue(const std::string& str);
    
    // Change tracking
    struct ChangeRecord {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<ChangeRecord> changeHistory;
    std::string version;
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
