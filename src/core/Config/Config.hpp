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
    
    // Validation and error handling
    bool validate() const;
    std::string validateWithDetails() const;
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
    // Configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createFastConfig();
    
    // Configuration merging
    void merge(const Config& other);
    void override(const Config& other);
    
    // Configuration analysis
    std::vector<std::string> getRequiredKeys() const;
    std::map<std::string, ConfigValue> getConfigAsMap() const;
    
    // Configuration copying
    void copyTo(Config& other) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration hashing
    size_t hash() const;
    
    // Configuration serialization
    std::string toJSON() const;
    std::string toINI() const;
    
    // Configuration parsing
    bool parseFromJSON(const std::string& json);
    bool parseFromINI(const std::string& ini);
    
    // Configuration batch operations
    void batchSet(const std::map<std::string, ConfigValue>& values);
    void batchGet(std::map<std::string, ConfigValue>& values) const;
    
    // Configuration history
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    std::vector<ConfigEntry> getHistory(const std::string& key) const;
    std::vector<ConfigEntry> getAllHistory() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    std::map<std::string, size_t> getKeyUsageCount() const;
    
    // Configuration backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // Configuration cleanup
    void cleanupInvalidEntries();
    void removeEmptyEntries();
    
    // Configuration validation rules
    struct ValidationRules {
        bool requireBrainNeurons = true;
        bool requireRegionCount = true;
        bool requireConnectionProbability = true;
        bool requireSimulationTimestep = true;
        bool requireRandomSeed = false;
        double minSynapseDensity = 0.0;
        double maxSynapseDensity = 1.0;
        int minNeuronCount = 1;
        int maxNeuronCount = 100000;
        int minRegionCount = 1;
        int maxRegionCount = 100;
        float minConnectionProbability = 0.0;
        float maxConnectionProbability = 1.0;
        float minSimulationTimestep = 0.0;
        float maxSimulationTimestep = 1.0;
        
        bool isValid() const;
        std::string getValidationError() const;
    };
    
    ValidationRules validationRules;
    
    // Configuration observers
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void onConfigChanged(const Config& config, const std::string& key) = 0;
    };
    
    void addObserver(std::shared_ptr<Observer> observer);
    void removeObserver(std::shared_ptr<Observer> observer);
    void notifyObservers(const std::string& key);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    static bool validateConfiguration(const Config& config, const ValidationRules& rules);
    
    // Serialization helpers
    static std::string configValueToString(const ConfigValue& value);
    static ConfigValue stringToConfigValue(const std::string& str);
    
    // Copy helpers
    static void copyEntries(const std::vector<ConfigEntry>& from, std::vector<ConfigEntry>& to);
    
    // Hashing helpers
    static size_t hashConfigEntry(const ConfigEntry& entry);
    
    // JSON parsing helpers
    static bool parseJSONObject(const std::string& json, size_t& pos, std::map<std::string, ConfigValue>& result);
    static bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& result);
    
    // INI parsing helpers
    static bool parseINI(const std::string& ini, std::map<std::string, ConfigValue>& result);
};

} // namespace nlm
