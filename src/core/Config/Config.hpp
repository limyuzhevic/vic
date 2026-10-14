#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration profiles for different use cases
enum class ConfigProfile {
    Default,
    Research,
    Production,
    Demo,
    Development,
    Performance,
    MemoryIntensive
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
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
    
    // === USER-FRIENDLY ENHANCEMENTS ===
    
    // Apply a configuration profile
    bool applyProfile(ConfigProfile profile);
    
    // Get profile name
    static std::string profileToString(ConfigProfile profile);
    
    // Validate configuration against rules
    bool validate(const std::vector<ValidationRule>& rules) const;
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule);
    
    // Remove validation rule
    void removeValidationRule(const std::string& key);
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // === ADVANCED FEATURES ===
    
    // Save configuration with automatic backup
    bool saveWithBackup(const std::string& filepath, bool createBackup = true);
    
    // Merge another configuration (overwrites existing keys)
    void merge(const Config& other);
    
    // Get configuration difference
    Config getDifference(const Config& other) const;
    
    // === CONVENIENCE METHODS ===
    
    // Set brain-specific configuration
    void setBrainConfig(size_t neuronCount = 1000, size_t regionCount = 1, 
                       double connectionProbability = 0.1, double timestep = 0.001);
    
    // Set plasticity configuration
    void setPlasticityConfig(bool enableSTDP = true, bool enableHebbian = true, 
                            bool enableStructural = true, double learningRate = 0.001);
    
    // Set neuromodulation configuration
    void setNeuromodulationConfig(bool enableDopamine = true, bool enableCuriosity = true,
                                 bool enableNovelty = true, double dopamineScale = 1.0);
    
    // Set development configuration
    void setDevelopmentConfig(bool enableDevelopment = true, size_t developmentalStages = 5);
    
    // Set memory configuration
    void setMemoryConfig(bool enableWorkingMemory = true, bool enableEpisodicMemory = true,
                        bool enableAssociativeMemory = true, size_t memoryCapacity = 1000);
    
    // Set prediction configuration
    void setPredictionConfig(bool enablePrediction = true, size_t predictionHorizon = 10);
    
    // Set cognition configuration
    void setCognitionConfig(bool enablePlanner = true, bool enableConceptFormation = true,
                           bool enableAttention = true);
    
    // === UTILITY METHODS ===
    
    // Get configuration for specific domain
    std::unordered_map<std::string, ConfigValue> getDomainConfig(const std::string& domain) const;
    
    // Set configuration for specific domain
    void setDomainConfig(const std::string& domain, const std::unordered_map<std::string, ConfigValue>& values);
    
    // Get configuration statistics
    std::string getConfigStats() const;
    
    // Reset to specific profile
    bool resetToProfile(ConfigProfile profile);
    
    // Export configuration to JSON/YAML (placeholder)
    bool exportToJSON(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    
    // Import configuration from JSON/YAML (placeholder)
    bool importFromJSON(const std::string& filepath);
    bool importFromYAML(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation rules
    std::vector<ValidationRule> validationRules;
    mutable std::vector<std::string> validationErrors;
    
    // Apply profile-specific settings
    void applyProfileDefaults(ConfigProfile profile);
    void applyProfileResearch(ConfigProfile profile);
    void applyProfileProduction(ConfigProfile profile);
    void applyProfileDemo(ConfigProfile profile);
    void applyProfileDevelopment(ConfigProfile profile);
    void applyProfilePerformance(ConfigProfile profile);
    void applyProfileMemoryIntensive(ConfigProfile profile);
    
    // Validate single value
    bool validateValue(const ConfigValue& value, const ValidationRule& rule) const;
};

// Configuration value type helpers
namespace ConfigHelpers {
    // Convert string to specific type
    template<typename T>
    T toType(const std::string& str);
    
    // Convert specific type to string
    template<typename T>
    std::string toString(const T& value);
    
    // Check if value is within range
    template<typename T>
    bool isInRange(const T& value, const T& min, const T& max);
    
    // Clamp value to range
    template<typename T>
    T clamp(const T& value, const T& min, const T& max);
}

} // namespace nlm
