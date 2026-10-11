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
    Default,      // Default from code
    File,         // Loaded from file
    CommandLine,  // Loaded from command line
    Runtime       // Set at runtime
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;                    // Configuration key
    ConfigValue value;                  // Configuration value
    ConfigSource source;                // How this value was set
    std::string description;            // Human-readable description
    std::string category;               // "learning", "network", "neuromodulation", "development", "memory", "prediction", "cognition", "performance", "general"
    bool editable;                      // Whether this can be changed at runtime
    std::string validationPattern;      // Regex pattern for validation
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), 
          category("general"), editable(true), validationPattern("") {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& cat = "general",
                bool editable = true, const std::string& validation = "")
        : key(k), value(v), source(s), description(desc), category(cat),
          editable(editable), validationPattern(validation) {}
};

// Advanced learning rate configuration
struct LearningRateConfig {
    double basalRate;           // Base learning rate for all synapses (default: 0.01)
    double stdpLTP;            // Spike-timing dependent plasticity LTP weight (default: 0.02)
    double stdpLTD;            // Spike-timing dependent plasticity LTD weight (default: 0.02)
    double hebbianStrength;    // Hebbian learning strength (default: 0.05)
    double homeostatic;       // Homeostatic plasticity rate (default: 0.001)
    double neuromodulation;    // Neuromodulation influence on plasticity (default: 0.1)
    
    // Learning rate ranges for validation
    static constexpr double MIN_RATE = 0.0;
    static constexpr double MAX_RATE = 1.0;
    static constexpr double MIN_STDP = 0.0;
    static constexpr double MAX_STDP = 0.1;
    
    LearningRateConfig() : basalRate(0.01), stdpLTP(0.02), stdpLTD(0.02),
                           hebbianStrength(0.05), homeostatic(0.001), neuromodulation(0.1) {}
    
    bool validate() const {
        return basalRate >= MIN_RATE && basalRate <= MAX_RATE &&
               stdpLTP >= MIN_STDP && stdpLTP <= MAX_STDP &&
               stdpLTD >= MIN_STDP && stdpLTD <= MAX_STDP &&
               hebbianStrength >= MIN_RATE && hebbianStrength <= MAX_RATE &&
               homeostatic >= MIN_RATE && homeostatic <= MAX_RATE &&
               neuromodulation >= MIN_RATE && neuromodulation <= MAX_RATE;
    }
};

// Network architecture configuration
struct NetworkArchitectureConfig {
    uint64_t neuronCount;           // Total number of neurons (default: 100)
    uint64_t regionCount;           // Number of neural regions (default: 1)
    double connectionProbability;   // Probability of forming connections (default: 0.1)
    std::string architecture;      // "layered", "modular", "dense", "hierarchical" (default: "modular")
    double structuralPlasticity;    // Rate of new synapse formation (default: 0.0001)
    double pruningRate;            // Rate of synapse elimination (default: 0.00001)
    
    // Network limits
    static constexpr uint64_t MIN_NEURONS = 10;
    static constexpr uint64_t MAX_NEURONS = 10000;
    static constexpr uint64_t MIN_REGIONS = 1;
    static constexpr uint64_t MAX_REGIONS = 10;
    static constexpr double MIN_CONN_PROB = 0.0;
    static constexpr double MAX_CONN_PROB = 1.0;
    static constexpr double MIN_PLASTICITY = 0.0;
    static constexpr double MAX_PLASTICITY = 0.1;
    
    NetworkArchitectureConfig() : neuronCount(100), regionCount(1), connectionProbability(0.1),
                                 architecture("modular"), structuralPlasticity(0.0001), pruningRate(0.00001) {}
    
    bool validate() const {
        return neuronCount >= MIN_NEURONS && neuronCount <= MAX_NEURONS &&
               regionCount >= MIN_REGIONS && regionCount <= MAX_REGIONS &&
               connectionProbability >= MIN_CONN_PROB && connectionProbability <= MAX_CONN_PROB &&
               structuralPlasticity >= MIN_PLASTICITY && structuralPlasticity <= MAX_PLASTICITY &&
               pruningRate >= MIN_PLASTICITY && pruningRate <= MAX_PLASTICITY &&
               (architecture == "layered" || architecture == "modular" || 
                architecture == "dense" || architecture == "hierarchical");
    }
};

// Neuromodulation system configuration
struct NeuromodulationConfig {
    bool dopamineEnabled;          // Enable dopamine system (default: true)
    bool serotoninEnabled;        // Enable serotonin system (default: true)
    bool acetylcholineEnabled;     // Enable acetylcholine system (default: true)
    bool noradrenalineEnabled;      // Enable noradrenaline system (default: true)
    
    double dopamineBaseline;       // Baseline dopamine level (default: 0.0)
    double dopamineMax;            // Maximum dopamine level (default: 1.0)
    double noveltySensitivity;     // Sensitivity to novel stimuli (default: 0.5)
    double rewardPredictionGain;   // Gain on reward prediction error (default: 1.0)
    
    NeuromodulationConfig() : dopamineEnabled(true), serotoninEnabled(true),
                             acetylcholineEnabled(true), noradrenalineEnabled(true),
                             dopamineBaseline(0.0), dopamineMax(1.0),
                             noveltySensitivity(0.5), rewardPredictionGain(1.0) {}
    
    bool validate() const {
        return dopamineBaseline >= -1.0 && dopamineBaseline <= 1.0 &&
               dopamineMax >= 0.0 && dopamineMax <= 10.0 &&
               noveltySensitivity >= 0.0 && noveltySensitivity <= 10.0 &&
               rewardPredictionGain >= 0.0 && rewardPredictionGain <= 10.0;
    }
};

// Development system configuration
struct DevelopmentConfig {
    bool developmentEnabled;        // Enable developmental processes (default: true)
    double maturationRate;          // Rate of neural maturation (default: 0.1)
    double criticalPeriod;          // Duration of critical learning period (default: 300.0)
    double plasticityDecay;         // Age-related plasticity reduction rate (default: 0.01)
    bool structuralDevelopment;     // Enable structural changes during development (default: true)
    
    DevelopmentConfig() : developmentEnabled(true), maturationRate(0.1), criticalPeriod(300.0),
                         plasticityDecay(0.01), structuralDevelopment(true) {}
    
    bool validate() const {
        return maturationRate >= 0.0 && maturationRate <= 10.0 &&
               criticalPeriod >= 0.0 && criticalPeriod <= 10000.0 &&
               plasticityDecay >= 0.0 && plasticityDecay <= 1.0;
    }
};

// Memory system configuration
struct MemoryConfig {
    bool workingMemoryEnabled;      // Enable working memory (default: true)
    bool episodicMemoryEnabled;     // Enable episodic memory (default: true)
    bool semanticMemoryEnabled;     // Enable semantic memory (default: true)
    bool proceduralMemoryEnabled;  // Enable procedural memory (default: true)
    
    double consolidationRate;       // Rate of memory consolidation (default: 0.9)
    double decayRate;               // Memory decay rate (default: 0.01)
    double capacity;                // Memory capacity limits (default: 1000.0)
    bool replayEnabled;             // Enable memory replay during sleep (default: true)
    
    MemoryConfig() : workingMemoryEnabled(true), episodicMemoryEnabled(true),
                     semanticMemoryEnabled(true), proceduralMemoryEnabled(true),
                     consolidationRate(0.9), decayRate(0.01), capacity(1000.0), replayEnabled(true) {}
    
    bool validate() const {
        return consolidationRate >= 0.0 && consolidationRate <= 1.0 &&
               decayRate >= 0.0 && decayRate <= 1.0 &&
               capacity > 0.0;
    }
};

// Prediction system configuration
struct PredictionConfig {
    bool predictionEnabled;         // Enable predictive processing (default: true)
    double predictionGain;          // Confidence in predictions (default: 0.5)
    double predictionErrorGain;    // Learning rate for prediction errors (default: 0.1)
    bool forwardModelEnabled;       // Enable forward models (default: true)
    bool inverseModelEnabled;       // Enable inverse models (default: true)
    
    PredictionConfig() : predictionEnabled(true), predictionGain(0.5), predictionErrorGain(0.1),
                        forwardModelEnabled(true), inverseModelEnabled(true) {}
    
    bool validate() const {
        return predictionGain >= 0.0 && predictionGain <= 1.0 &&
               predictionErrorGain >= 0.0 && predictionErrorGain <= 1.0;
    }
};

// Cognition system configuration
struct CognitionConfig {
    bool planningEnabled;           // Enable planning systems (default: true)
    bool attentionEnabled;          // Enable attention mechanisms (default: true)
    bool conceptFormationEnabled;    // Enable concept formation (default: true)
    bool socialLearningEnabled;     // Enable social learning capabilities (default: true)
    
    double planningHorizon;         // Look-ahead time for planning (default: 10.0)
    double attentionThreshold;      // Attention threshold (default: 0.5)
    double conceptFormationRate;    // Rate of concept formation (default: 0.1)
    double socialLearningRate;      // Rate of social learning (default: 0.3)
    
    CognitionConfig() : planningEnabled(true), attentionEnabled(true),
                       conceptFormationEnabled(true), socialLearningEnabled(true),
                       planningHorizon(10.0), attentionThreshold(0.5), conceptFormationRate(0.1),
                       socialLearningRate(0.3) {}
    
    bool validate() const {
        return planningHorizon >= 0.0 && planningHorizon <= 1000.0 &&
               attentionThreshold >= 0.0 && attentionThreshold <= 1.0 &&
               conceptFormationRate >= 0.0 && conceptFormationRate <= 1.0 &&
               socialLearningRate >= 0.0 && socialLearningRate <= 1.0;
    }
};

// Performance optimization configuration
struct PerformanceConfig {
    bool vectorizationEnabled;      // Enable SIMD vectorization (default: true)
    bool parallelizationEnabled;    // Enable parallel processing (default: true)
    bool memoryPoolEnabled;         // Enable memory pools (default: true)
    bool eventDrivenEnabled;        // Enable event-driven architecture (default: true)
    
    uint32_t threadCount;            // Number of worker threads (default: hardware_concurrency)
    size_t memoryPoolSize;           // Size of memory pools (default: 1MB)
    double simulationSpeed;         // Simulation speed multiplier (default: 1.0)
    bool checkpointingEnabled;      // Enable checkpoint saving (default: false)
    std::string checkpointPath;     // Checkpoint file path (default: "/tmp/nlm_checkpoints")
    
    PerformanceConfig() : vectorizationEnabled(true), parallelizationEnabled(true),
                          memoryPoolEnabled(true), eventDrivenEnabled(true),
                          threadCount(0), memoryPoolSize(1024 * 1024), simulationSpeed(1.0),
                          checkpointingEnabled(false), checkpointPath("/tmp/nlm_checkpoints") {}
    
    bool validate() const {
        return threadCount == 0 || (threadCount >= 1 && threadCount <= 64) &&
               memoryPoolSize >= 1024 && memoryPoolSize <= 1024 * 1024 * 1024 &&
               simulationSpeed > 0.0 && simulationSpeed <= 100.0;
    }
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
    
    // Advanced configuration methods
    bool setLearningRates(const LearningRateConfig& config);
    bool setNetworkArchitecture(const NetworkArchitectureConfig& config);
    bool setNeuromodulation(const NeuromodulationConfig& config);
    bool setDevelopment(const DevelopmentConfig& config);
    bool setMemory(const MemoryConfig& config);
    bool setPrediction(const PredictionConfig& config);
    bool setCognition(const CognitionConfig& config);
    bool setPerformance(const PerformanceConfig& config);
    
    // Get advanced configuration objects
    LearningRateConfig getLearningRates() const;
    NetworkArchitectureConfig getNetworkArchitecture() const;
    NeuromodulationConfig getNeuromodulation() const;
    DevelopmentConfig getDevelopment() const;
    MemoryConfig getMemory() const;
    PredictionConfig getPrediction() const;
    CognitionConfig getCognition() const;
    PerformanceConfig getPerformance() const;
    
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
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    void normalizeConfig();
    bool validateLearningRates() const;
    bool validateNetworkArchitecture() const;
    bool validateNeuromodulation() const;
    bool validateDevelopment() const;
    bool validateMemory() const;
    bool validatePrediction() const;
    bool validateCognition() const;
    bool validatePerformance() const;
};

} // namespace nlm
