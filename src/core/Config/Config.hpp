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

// Configuration options for Phase 6 cognitive systems
namespace ConfigKeys {
    // Prediction System Configuration
    const std::string PREDICTION_ENABLED = "prediction_system.enabled";
    const std::string PREDICTION_LEARNING_RATE = "prediction_system.learning_rate";
    const std::string PREDICTION_CONFIDENCE_THRESHOLD = "prediction_system.confidence_threshold";
    const std::string PREDICTION_HISTORY_SIZE = "prediction_system.history_size";
    
    // Concept Formation Configuration
    const std::string CONCEPT_ENABLED = "concept_formation.enabled";
    const std::string CONCEPT_FORMATION_THRESHOLD = "concept_formation.formation_threshold";
    const std::string CONCEPT_STABILITY_THRESHOLD = "concept_formation.stability_threshold";
    const std::string CONCEPT_STABILITY_WINDOW = "concept_formation.stability_window";
    const std::string CONCEPT_MAX_CONCEPTS = "concept_formation.max_concepts";
    const std::string CONCEPT_GENERALIZATION_THRESHOLD = "concept_formation.generalization_threshold";
    
    // Working Memory Configuration
    const std::string WORKING_MEMORY_ENABLED = "working_memory.enabled";
    const std::string WORKING_MEMORY_CAPACITY = "working_memory.capacity";
    const std::string WORKING_MEMORY_DECAY_RATE = "working_memory.decay_rate";
    const std::string WORKING_MEMORY_REINFORCEMENT_STRENGTH = "working_memory.reinforcement_strength";
    
    // Episodic Memory Configuration
    const std::string EPISODIC_MEMORY_ENABLED = "episodic_memory.enabled";
    const std::string EPISODIC_MEMORY_MAX_EPISODES = "episodic_memory.max_episodes";
    const std::string EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD = "episodic_memory.consolidation_threshold";
    const std::string EPISODIC_MEMORY_REPLAY_INTERVAL = "episodic_memory.replay_interval";
    
    // Attention System Configuration
    const std::string ATTENTION_ENABLED = "attention.enabled";
    const std::string ATTENTION_INHIBITION_STRENGTH = "attention.inhibition_strength";
    const std::string ATTENTION_EXCITATION_STRENGTH = "attention.excitation_strength";
    const std::string ATTENTION_NOISE_LEVEL = "attention.noise_level";
    
    // Planner Configuration
    const std::string PLANNER_ENABLED = "planner.enabled";
    const std::string PLANNER_PLANNING_DEPTH = "planner.planning_depth";
    const std::string PLANNER_ACTION_DIVERSITY = "planner.action_diversity";
    const std::string PLANNER_EXPLORATION_RATE = "planner.exploration_rate";
    const std::string PLANNER_CONSERVATION_BIAS = "planner.conservation_bias";
    
    // Neuromodulation Configuration
    const std::string NEUROMODULATION_ENABLED = "neuromodulation.enabled";
    const std::string DOPAMINE_BASELINE = "neuromodulation.dopamine_baseline";
    const std::string DOPAMINE_RELEASE_RATE = "neuromodulation.dopamine_release_rate";
    const std::string DOPAMINE_DECAY_RATE = "neuromodulation.dopamine_decay_rate";
    const std::string CURIOUSITY_BASELINE = "neuromodulation.curiosity_baseline";
    const std::string NOVELTY_SENSITIVITY = "neuromodulation.novelty_sensitivity";
    
    // Development System Configuration
    const std::string DEVELOPMENT_ENABLED = "development.enabled";
    const std::string DEVELOPMENT_INITIAL_PLATONICITY = "development.initial_plasticity";
    const std::string DEVELOPMENT_MATURATION_RATE = "development.maturation_rate";
    const std::string DEVELOPMENT_CRITICAL_PERIOD = "development.critical_period_duration";
    
    // Performance Configuration
    const std::string VERBOSE_LOGGING = "performance.verbose_logging";
    const std::string ENABLE_PERFORMANCE_METRICS = "performance.enable_metrics";
    const std::string METRICS_EXPORT_INTERVAL = "performance.metrics_interval";
    
    // Integration Configuration
    const std::string ENABLE_BRAIN_LOOP_LOGGING = "integration.enable_brain_loop_logging";
    const std::string BRAIN_LOOP_TIMEOUT = "integration.brain_loop_timeout_ms";
    const std::string ENABLE_EXPERIMENT_MODE = "integration.enable_experiment_mode";
    
    // Default configuration values
    const std::string DEFAULT_PREDICTION_LEARNING_RATE = "0.01";
    const std::string DEFAULT_PREDICTION_CONFIDENCE_THRESHOLD = "0.7";
    const std::string DEFAULT_PREDICTION_HISTORY_SIZE = "1000";
    
    const std::string DEFAULT_CONCEPT_FORMATION_THRESHOLD = "0.75";
    const std::string DEFAULT_CONCEPT_STABILITY_THRESHOLD = "0.7";
    const std::string DEFAULT_CONCEPT_STABILITY_WINDOW = "5";
    const std::string DEFAULT_CONCEPT_MAX_CONCEPTS = "100";
    const std::string DEFAULT_CONCEPT_GENERALIZATION_THRESHOLD = "0.8";
    
    const std::string DEFAULT_WORKING_MEMORY_CAPACITY = "200";
    const std::string DEFAULT_WORKING_MEMORY_DECAY_RATE = "0.01";
    const std::string DEFAULT_WORKING_MEMORY_REINFORCEMENT_STRENGTH = "0.5";
    
    const std::string DEFAULT_Episodic_MEMORY_MAX_EPISODES = "10000";
    const std::string DEFAULT_Episodic_MEMORY_CONSOLIDATION_THRESHOLD = "0.3";
    const std::string DEFAULT_Episodic_MEMORY_REPLAY_INTERVAL = "100";
    
    const std::string DEFAULT_ATTENTION_INHIBITION_STRENGTH = "0.5";
    const std::string DEFAULT_ATTENTION_EXCITATION_STRENGTH = "1.5";
    const std::string DEFAULT_ATTENTION_NOISE_LEVEL = "0.1";
    
    const std::string DEFAULT_PLANNER_PLANNING_DEPTH = "3";
    const std::string DEFAULT_PLANNER_ACTION_DIVERSITY = "10";
    const std::string DEFAULT_PLANNER_EXPLORATION_RATE = "0.3";
    const std::string DEFAULT_PLANNER_CONSERVATION_BIAS = "0.7";
    
    const std::string DEFAULT_DOPAMINE_BASELINE = "0.1";
    const std::string DEFAULT_DOPAMINE_RELEASE_RATE = "1.0";
    const std::string DEFAULT_DOPAMINE_DECAY_RATE = "0.1";
    
    const std::string DEFAULT_DEVELOPMENT_INITIAL_PLATONICITY = "1.0";
    const std::string DEFAULT_DEVELOPMENT_MATURATION_RATE = "0.01";
} // namespace ConfigKeys

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
    
    // Initialize with Phase 6 defaults
    void initializePhase6Defaults();
    
    // Get cognitive system specific getters
    bool getPredictionEnabled() const;
    float getPredictionLearningRate() const;
    float getPredictionConfidenceThreshold() const;
    int getPredictionHistorySize() const;
    
    bool getConceptFormationEnabled() const;
    float getConceptFormationThreshold() const;
    float getConceptStabilityThreshold() const;
    size_t getConceptStabilityWindow() const;
    size_t getConceptMaxConcepts() const;
    
    bool getWorkingMemoryEnabled() const;
    size_t getWorkingMemoryCapacity() const;
    float getWorkingMemoryDecayRate() const;
    
    bool getEpisodicMemoryEnabled() const;
    size_t getEpisodicMemoryMaxEpisodes() const;
    float getEpisodicMemoryConsolidationThreshold() const;
    size_t getEpisodicMemoryReplayInterval() const;
    
    bool getAttentionEnabled() const;
    float getAttentionInhibitionStrength() const;
    float getAttentionExcitationStrength() const;
    
    bool getPlannerEnabled() const;
    int getPlannerPlanningDepth() const;
    size_t getPlannerActionDiversity() const;
    
    bool getNeuromodulationEnabled() const;
    float getDopamineBaseline() const;
    
    bool getDevelopmentEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
