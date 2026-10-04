#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <fstream>
#include <sstream>

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
    
    // Advanced Brain Configuration
    void setBrainType(const std::string& type) { set("brain_type", type); }
    void setNeuronCount(size_t count) { set("neuron_count", count); }
    void setRegionCount(size_t count) { set("region_count", count); }
    void setConnectionProbability(float prob) { set("connection_probability", prob); }
    
    // Advanced Memory Configuration
    void setWorkingMemoryCapacity(size_t capacity) { set("working_memory_capacity", capacity); }
    void setWorkingMemoryDecayRate(float rate) { set("working_memory_decay_rate", rate); }
    void setEpisodicMemoryMaxEpisodes(size_t max) { set("episodic_memory_max_episodes", max); }
    void setEpisodicMemoryConsolidationThreshold(float threshold) { set("episodic_memory_consolidation_threshold", threshold); }
    
    // Advanced Prediction System Configuration
    void setPredictionDepth(int depth) { set("prediction_depth", depth); }
    void setPredictionConfidenceThreshold(float threshold) { set("prediction_confidence_threshold", threshold); }
    void setPredictionErrorSmoothing(float smoothing) { set("prediction_error_smoothing", smoothing); }
    
    // Advanced Cognition Configuration
    void setPlannerPlanningDepth(int depth) { set("planner_planning_depth", depth); }
    void setConceptFormationStabilityThreshold(float threshold) { set("concept_formation_stability_threshold", threshold); }
    void setAttentionInhibitionStrength(float strength) { set("attention_inhibition_strength", strength); }
    void setAttentionExcitationStrength(float strength) { set("attention_excitation_strength", strength); }
    
    // Advanced Neuromodulation Configuration
    void setDopaminePlasticityFactor(float factor) { set("dopamine_plasticity_factor", factor); }
    void setNoveltyDetectionThreshold(float threshold) { set("novelty_detection_threshold", threshold); }
    void setCuriosityExplorationWeight(float weight) { set("curiosity_exploration_weight", weight); }
    void setSerotoninMoodInfluence(float influence) { set("serotonin_mood_influence", influence); }
    void setNorepinephrineArousalThreshold(float threshold) { set("norepinephrine_arousal_threshold", threshold); }
    void setAcetylcholineAttentionGain(float gain) { set("acetylcholine_attention_gain", gain); }
    
    // Advanced Development Configuration
    void setDevelopmentStageTransitions(float transitionRate) { set("development_stage_transitions", transitionRate); }
    void setDevelopmentalPlasticityMultiplier(float multiplier) { set("developmental_plasticity_multiplier", multiplier); }
    void setMaturationRate(float rate) { set("maturation_rate", rate); }
    
    // Advanced Checkpoint Configuration
    void setCheckpointPath(const std::string& path) { set("checkpoint_path", path); }
    void setCheckpointCompressionLevel(int level) { set("checkpoint_compression_level", level); }
    void setCheckpointMaxFiles(size_t max) { set("checkpoint_max_files", max); }
    void setCheckpointAutoSave(bool autoSave) { set("checkpoint_auto_save", autoSave); }
    
    // Advanced Performance Configuration
    void setMemoryPoolSize(size_t size) { set("memory_pool_size", size); }
    void setEventQueueCapacity(size_t capacity) { set("event_queue_capacity", capacity); }
    void setParallelProcessingEnabled(bool enabled) { set("parallel_processing_enabled", enabled); }
    void setSIMDVectorizationEnabled(bool enabled) { set("simd_vectorization_enabled", enabled); }
    
    // Advanced Replay Configuration
    void setReplayMemoryCount(size_t count) { set("replay_memory_count", count); }
    void setReplayConfidenceThreshold(float threshold) { set("replay_confidence_threshold", threshold); }
    void setReplayTimeStepInterval(size_t interval) { set("replay_time_step_interval", interval); }
    
    // Advanced Rest Configuration
    void setRestPeriodDuration(size_t duration) { set("rest_period_duration", duration); }
    void setRestNeuromodulatorThresholds(float sero, float dopa, float norepi, float ach) {
        set("rest_serotonin_threshold", sero);
        set("rest_dopamine_threshold", dopa);
        set("rest_norepinephrine_threshold", norepi);
        set("rest_acetylcholine_threshold", ach);
    }
    void setRestDevelopmentAcceleration(bool accelerated) { set("rest_development_acceleration", accelerated); }
    
    // Advanced Event System Configuration
    void setEventQueueMaxSize(size_t size) { set("event_queue_max_size", size); }
    void setEventProcessingInterval(size_t interval) { set("event_processing_interval", interval); }
    void setEventPriorityLevel(size_t level) { set("event_priority_level", level); }
    
    // Advanced Memory Consolidation Configuration
    void setMemoryConsolidationThreshold(float threshold) { set("memory_consolidation_threshold", threshold); }
    void setMemoryConsolidationDecay(float decay) { set("memory_consolidation_decay", decay); }
    void setMemoryReplayStrength(float strength) { set("memory_replay_strength", strength); }
    
    // Advanced Configuration Methods
    bool hasAdvancedFeatures() const { return getOr<bool>("advanced_features_enabled", false); }
    std::string getAdvancedConfigSummary() const;
    std::vector<std::string> getAllAdvancedKeys() const;
    void loadAdvancedConfigFromFile(const std::string& filepath);
    void saveAdvancedConfigToFile(const std::string& filepath);
    
    // Enhanced getOr methods for advanced types
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        if (auto value = get<T>(key)) {
            return *value;
        }
        return defaultValue;
    }
    
    // Enhanced advanced configuration methods
    template<typename T>
    void setAdvanced(const std::string& key, const T& value) {
        set(key, value);
        // Trigger validation for advanced settings
        validateAdvancedSetting(key, value);
    }
    
    // Validate advanced settings
    bool validateAdvancedSetting(const std::string& key, const std::string& value) const;
    bool validateAdvancedSetting(const std::string& key, size_t value) const;
    bool validateAdvancedSetting(const std::string& key, float value) const;
    bool validateAdvancedSetting(const std::string& key, bool value) const;
    
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
    
    // Advanced validation mappings
    static std::map<std::string, std::function<bool(const std::string&, const ConfigValue&)>> getAdvancedValidators();
};

} // namespace nlm
