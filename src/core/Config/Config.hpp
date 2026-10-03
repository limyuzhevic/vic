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
    std::vector<std::string>,
    std::vector<float>,
    std::vector<std::vector<double>>,
    std::map<std::string, double>,
    std::map<std::string, std::string>
>;

// Configuration validation callback
using ValidationCallback = std::function<bool(const std::string&, const ConfigValue&)>;

// Configuration type
enum class ConfigType {
    Int,
    Int64,
    Float,
    Double,
    Bool,
    String,
    StringList,
    FloatList,
    IntList,
    Matrix,
    DictDouble,
    DictString
};

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

// Neural network configuration schemas
struct NeuralNetworkConfig {
    int neuronCount;
    int regionCount;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float synaptogenesisRate;
    float pruningRate;
    bool enableRewardModulation;
    bool enableStructuralPlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    int randomSeed;
    double simulationTimestep;
};

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    ConfigSource source;
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
    
    // Load from file (YAML format)
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYAML(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& category = "general",
             const ValidationCallback& validator = nullptr);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<float>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::vector<double>>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::map<std::string, double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::map<std::string, std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods
    void setInt(const std::string& key, int64_t value, const std::string& description = "");
    void setFloat(const std::string& key, float value, const std::string& description = "");
    void setDouble(const std::string& key, double value, const std::string& description = "");
    void setString(const std::string& key, const std::string& value, const std::string& description = "");
    void setBool(const std::string& key, bool value, const std::string& description = "");
    void setVector(const std::string& key, const std::vector<double>& value, const std::string& description = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Get entry details
    const ConfigEntry* getEntry(const std::string& key) const;
    std::optional<ConfigSource> getSource(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryByCategory(const std::string& category) const;
    
    // Configuration schema and validation
    void registerTypeHint(const std::string& key, ConfigType type);
    void registerValidator(const std::string& key, const ValidationCallback& validator);
    void registerDescription(const std::string& key, const std::string& description);
    void registerCategory(const std::string& key, const std::string& category);
    bool validate() const;
    std::vector<ValidationError> getValidationErrors() const;
    
    // Neural network configuration helpers
    bool configureNeuralNetwork(const NeuralNetworkConfig& config);
    NeuralNetworkConfig getNeuralNetworkConfig() const;
    void registerNeuralNetworkSchema();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diff(const Config& other) const;
    
    // Configuration merging
    void merge(const Config& other, bool overwrite = true);
    
    // Configuration cloning
    std::shared_ptr<Config> clone() const;
    
    // Get/set strict mode
    void setStrictMode(bool strict) { pImpl->strictMode = strict; }
    bool getStrictMode() const { return pImpl->strictMode; }
    
    // Get/set JSON mode
    void setJSONMode(bool json) { pImpl->jsonMode = json; }
    bool getJSONMode() const { return pImpl->jsonMode; }
    
    // Configuration search
    std::vector<std::string> search(const std::string& pattern) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringMatchesPattern(const std::string& str, const std::string& pattern);
    
    // Validation helpers
    static bool validateType(const std::string& key, const ConfigValue& value, ConfigType type);
    static std::optional<ValidationError> validateValue(const std::string& key, const ConfigValue& value);
};

} // namespace nlm
