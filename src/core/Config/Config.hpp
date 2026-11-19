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

// Configuration categories for better organization
enum class ConfigCategory {
    Brain,
    Plasticity,
    Neuromodulation,
    Memory,
    Prediction,
    Cognition,
    Development,
    Sensory,
    Motor,
    Environment,
    Simulation,
    Logging,
    Visualization
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool requiresSource = false;  // Must be set from default or file
};

// Configuration metadata for better documentation
struct ConfigMetadata {
    std::string key;
    std::string description;
    std::string category;
    std::string type;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    bool isRequired = false;
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading from file (JSON/YAML support)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string saveToJSON() const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    
    void clear();
    
    // Enhanced summary with categories
    std::string summary() const;
    std::string summaryByCategory() const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidationRule(const ValidationRule& rule);
    void setUpDefaults();  // Apply comprehensive default values
    
    // Brain-specific convenience methods
    void setBrainDefaults();
    void setPlasticityDefaults();
    void setNeuromodulationDefaults();
    void setMemoryDefaults();
    void setPredictionDefaults();
    void setCognitionDefaults();
    void setDevelopmentDefaults();
    void setSensoryDefaults();
    void setMotorDefaults();
    void setEnvironmentDefaults();
    void setSimulationDefaults();
    void setLoggingDefaults();
    void setVisualizationDefaults();
    
    // Utility functions
    static std::string categoryToString(ConfigCategory category);
    static ConfigCategory stringToCategory(const std::string& categoryStr);
    static bool isValidCategory(const std::string& categoryStr);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static bool validateIntRange(const ConfigValue& value, int min, int max, std::string& errorMsg);
    static bool validateDoubleRange(const ConfigValue& value, double min, double max, std::string& errorMsg);
    static bool validateBool(const ConfigValue& value, std::string& errorMsg);
    static bool validateString(const ConfigValue& value, size_t minLen, size_t maxLen, std::string& errorMsg);
    
    // Default configuration setup
    void setupBrainDefaults();
    void setupPlasticityDefaults();
    void setupNeuromodulationDefaults();
    void setupMemoryDefaults();
    void setupPredictionDefaults();
    void setupCognitionDefaults();
    void setupDevelopmentDefaults();
    void setupSensoryDefaults();
    void setupMotorDefaults();
    void setupEnvironmentDefaults();
    void setupSimulationDefaults();
    void setupLoggingDefaults();
    void setupVisualizationDefaults();
};

} // namespace nlm
