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
    std::vector<std::string>,
    std::vector<float>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,      // Built-in default values
    File,         // Loaded from configuration file
    CommandLine,  // Provided via command line
    Runtime       // Set at runtime during execution
};

// Detailed configuration metadata for better management
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;  // For organizing configuration options
    bool isAdvanced;       // Hide in simple UIs
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), category("General"), isAdvanced(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "General",
                bool adv = false)
        : key(k), value(v), source(s), description(desc), category(cat), isAdvanced(adv) {}
};

// Type-safe getters for common configuration values
class ConfigValueAccessor {
public:
    // Get int values
    static int asInt(const ConfigValue& value, int defaultValue = 0);
    
    // Get float values  
    static float asFloat(const ConfigValue& value, float defaultValue = 0.0f);
    
    // Get string values
    static std::string asString(const ConfigValue& value, const std::string& defaultValue = "");
    
    // Get boolean values
    static bool asBool(const ConfigValue& value, bool defaultValue = false);
    
    // Get vector values
    template<typename T>
    static std::vector<T> asVector(const ConfigValue& value, const std::vector<T>& defaultValue = {});
    
    // Type checking
    static bool isType(const ConfigValue& value, const std::string& typeName);
    
    // Type name for debugging
    static std::string getTypeName(const ConfigValue& value);
};

// Type definitions for common configuration patterns
struct BrainConfig {
    int neuronCount;
    int regionCount;
    float connectionProbability;
    float initialWeightMean;
    float initialWeightStd;
    float v_thresh;      // Neuron threshold potential
    float v_rest;        // Resting potential  
    float v_reset;       // Reset potential
    float tau_mem;       // Membrane time constant
    float tau_ref;       // Refractory period
    
    BrainConfig() : neuronCount(1000), regionCount(1), connectionProbability(0.1f),
                   initialWeightMean(0.5f), initialWeightStd(0.1f),
                   v_thresh(-50.0f), v_rest(-70.0f), v_reset(-75.0f),
                   tau_mem(20.0f), tau_ref(2.0f) {}
};

struct PlasticityConfig {
    bool stdpEnable;
    bool hebbianEnable;
    bool structuralEnable;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float hebbianLearningRate;
    float synaptogenesisRate;
    float pruningRate;
    
    PlasticityConfig() : stdpEnable(true), hebbianEnable(true), structuralEnable(true),
                        stdpLtpWeight(0.01f), stdpLtdWeight(0.012f), stdpTau(20.0f),
                        hebbianLearningRate(0.01f), synaptogenesisRate(0.0001f),
                        pruningRate(0.00001f) {}
};

struct NeuromodulationConfig {
    float dopamineScale;
    bool curiosityEnable;
    bool noveltyEnable;
    float dopamineLearningRate;
    float curiosityDecayRate;
    
    NeuromodulationConfig() : dopamineScale(1.0f), curiosityEnable(true), noveltyEnable(true),
                             dopamineLearningRate(0.001f), curiosityDecayRate(0.01f) {}
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
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Convenience getters for common configuration sections
    BrainConfig getBrainConfig() const;
    PlasticityConfig getPlasticityConfig() const;
    NeuromodulationConfig getNeuromodulationConfig() const;
    
    // Set values with type safety and validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "General",
             bool isAdvanced = false);
    
    // Specialized setters for better API
    void setBrainConfig(const BrainConfig& config);
    void setPlasticityConfig(const PlasticityConfig& config);
    void setNeuromodulationConfig(const NeuromodulationConfig& config);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration as structured data
    std::map<std::string, std::string> toMap() const;
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Get default value for key
    std::optional<ConfigValue> getDefault(const std::string& key) const;
    
    // Check if value differs from default
    bool isModified(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toCamelCase(const std::string& str);
    
    // Initialize with default values
    void initializeDefaults();
    
    // Validation helpers
    static bool validateInt(const std::string& key, int value, int min, int max);
    static bool validateFloat(const std::string& key, float value, float min, float max);
    static bool validateString(const std::string& key, const std::string& value, size_t maxLength);
};

} // namespace nlm

// Convenience template implementations
namespace nlm {
    
    // Template specialization for vector types
    template<typename T>
    inline std::optional<T> Config::get(const std::string& key) const {
        auto value = get<ConfigValue>(key);
        if (!value) return std::nullopt;
        
        try {
            return std::get<T>(*value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    inline T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value ? *value : defaultValue;
    }
    
    template<typename T>
    inline void Config::set(const std::string& key, const T& value, ConfigSource source,
                           const std::string& description, const std::string& category,
                           bool isAdvanced) {
        // Validate based on type
        bool valid = true;
        if constexpr (std::is_same_v<T, int>) {
            valid = validateInt(key, value, std::numeric_limits<int>::min(), 
                               std::numeric_limits<int>::max());
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            valid = validateFloat(key, static_cast<float>(value), std::numeric_limits<float>::lowest(),
                                 std::numeric_limits<float>::max());
        } else if constexpr (std::is_same_v<T, std::string>) {
            valid = validateString(key, value, 1024);
        }
        
        if (valid) {
            set<ConfigValue>(key, value, source);
            // Could also store metadata here
        }
    }
    
} // namespace nlm

// Required includes for template implementations
#include <limits>
#include <stdexcept>
