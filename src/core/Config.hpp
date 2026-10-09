// ============================================================================
// Configurable System for NLM
// ============================================================================

#pragma once

#include <map>
#include <string>
#include <any>
#include <memory>
#include <string>
#include <sstream>

namespace nlm {

class Config {
public:
    Config();
    ~Config();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from string
    bool loadFromString(const std::string& configString);
    
    // Get value with type conversion
    template<typename T>
    T get(const std::string& key) const;
    
    // Get value with default
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Set value
    template<typename T>
    void set(const std::string& key, T value);
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration as string
    std::string toString() const;
    
    // Merge with another config
    void merge(const Config& other);
    
    // Clear configuration
    void clear();
    
    // Save configuration
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration
    bool validate() const;
    
    // Get configuration version
    std::string getVersion() const { return "2.0"; }
    
    // Get configuration description
    std::string getDescription() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// Configuration Templates for NLM Components
// ============================================================================

// Neuromodulator configuration
struct NeuromodulatorConfig {
    std::string name;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float inhibitionFactor;
    float facilitationFactor;
    float plasticityFactor;
    
    NeuromodulatorConfig()
        : name("Neuromodulator")
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , inhibitionFactor(0.0f)
        , facilitationFactor(0.0f)
        , plasticityFactor(1.0f) {}
};

// Memory system configuration
struct MemoryConfig {
    size_t capacity;
    float decayRate;
    size_t maxEpisodicEpisodes;
    float consolidationThreshold;
    size_t replayInterval;
    size_t consolidationInterval;
    
    MemoryConfig()
        : capacity(1000)
        , decayRate(0.95f)
        , maxEpisodicEpisodes(1000)
        , consolidationThreshold(0.5f)
        , replayInterval(100)
        , consolidationInterval(1000) {}
};

// Reward system configuration
struct RewardConfig {
    float explorationBonus;
    float intrinsicWeight;
    float extrinsicWeight;
    float predictionErrorBaseline;
    float decayRate;
    size_t historySize;
    
    RewardConfig()
        : explorationBonus(0.1f)
        , intrinsicWeight(0.5f)
        , extrinsicWeight(0.5f)
        , predictionErrorBaseline(0.0f)
        , decayRate(0.99f)
        , historySize(1000) {}
};

// Plasticity system configuration
struct PlasticityConfig {
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float hebbianLearningRate;
    float bcmLearningRate;
    float synaptogenesisRate;
    float pruningRate;
    
    PlasticityConfig()
        : stdpLtpWeight(0.01f)
        , stdpLtdWeight(0.012f)
        , stdpTau(20.0f)
        , hebbianLearningRate(0.1f)
        , bcmLearningRate(0.05f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f) {}
};

// ============================================================================
// Configuration Validation
// ============================================================================

// Configuration validation rules
class ConfigValidator {
public:
    static bool validate(const Config& config);
    static std::vector<std::string> getValidationErrors(const Config& config);
    
private:
    static bool validateNeuromodulator(const Config& config);
    static bool validateMemory(const Config& config);
    static bool validateReward(const Config& config);
    static bool validatePlasticity(const Config& config);
    static bool validateDynamics(const Config& config);
};

// ============================================================================
// Configuration Builder
// ============================================================================

class ConfigBuilder {
public:
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createResearchConfig();
    static Config createPerformanceConfig();
    static Config loadFromFile(const std::string& filepath);
    static Config parseString(const std::string& configString);
    
private:
    static void applyConfigToComponent(Config& config);
};

// ============================================================================
// Configuration Exceptions
// ============================================================================

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public ConfigError {
public:
    ValidationError(const std::string& message) : ConfigError(message) {}
};

class LoadError : public ConfigError {
public:
    LoadError(const std::string& message) : ConfigError(message) {}
};

} // namespace nlm

// ============================================================================
// End of Configurable System
// ============================================================================
