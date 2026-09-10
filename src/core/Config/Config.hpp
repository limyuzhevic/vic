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

// Cognitive system configuration
namespace CognitiveConfig {
    // NeuralPlanner configuration
    constexpr const char* NEURAL_PLANNER_DEPTH = "cognitive.neural_planner.planning_depth";
    constexpr const char* NEURAL_PLANNER_CONFIDENCE = "cognitive.neural_planner.planning_confidence";
    constexpr const char* NEURAL_PLANNER_MAX_CACHE_SIZE = "cognitive.neural_planner.max_cache_size";
    
    // ConceptFormation configuration
    constexpr const char* CONCEPT_FORMATION_MIN_PATTERN_SIZE = "cognitive.concept_formation.min_pattern_size";
    constexpr const char* CONCEPT_FORMATION_MAX_PATTERN_SIZE = "cognitive.concept_formation.max_pattern_size";
    constexpr const char* CONCEPT_FORMATION_SIMILARITY_THRESHOLD = "cognitive.concept_formation.similarity_threshold";
    constexpr const char* CONCEPT_FORMATION_MIN_REINFORCEMENT = "cognitive.concept_formation.min_reinforcement";
    
    // SelfModel configuration
    constexpr const char* SELF_MODEL_CAPABILITY_BASELINE = "cognitive.self_model.capability_baseline";
    constexpr const char* SELF_MODEL_BODY_AWARENESS_THRESHOLD = "cognitive.self_model.body_awareness_threshold";
    constexpr const char* SELF_MODEL_PREDICTION_ERROR_THRESHOLD = "cognitive.self_model.prediction_error_threshold";
    
    // SocialLearning configuration
    constexpr const char* SOCIAL_LEARNING_MAX_OBSERVATIONS = "cognitive.social_learning.max_observations";
    constexpr const char* SOCIAL_LEARNING_IMITATION_CONFIDENCE = "cognitive.social_learning.imitation_confidence";
    constexpr const char* SOCIAL_LEARNING_COMMUNICATION_ENABLED = "cognitive.social_learning.communication_enabled";
}

// Memory system configuration
namespace MemoryConfig {
    // WorkingMemory configuration
    constexpr const char* WORKING_MEMORY_CAPACITY = "memory.working_memory.capacity";
    constexpr const char* WORKING_MEMORY_DECAY_RATE = "memory.working_memory.decay_rate";
    constexpr const char* WORKING_MEMORY_REINFORCEMENT_FACTOR = "memory.working_memory.reinforcement_factor";
    constexpr const char* WORKING_MEMORY_COMPETITION_STRENGTH = "memory.working_memory.competition_strength";
    
    // EpisodicMemory configuration
    constexpr const char* EPISODIC_MEMORY_MAX_EPISODES = "memory.episodic_memory.max_episodes";
    constexpr const char* EPISODIC_MEMORY_REPLAY_ENABLED = "memory.episodic_memory.replay_enabled";
    constexpr const char* EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD = "memory.episodic_memory.consolidation_threshold";
    constexpr const char* EPISODIC_MEMORY_SIMILARITY_THRESHOLD = "memory.episodic_memory.similarity_threshold";
    
    // AssociativeMemory configuration
    constexpr const char* ASSOCIATIVE_MEMORY_MAX_ASSOCIATIONS = "memory.associative_memory.max_associations";
    constexpr const char* ASSOCIATIVE_MEMORY_MAX_PATTERNS = "memory.associative_memory.max_patterns";
    constexpr const char* ASSOCIATIVE_MEMORY_SPREAD_STEPS = "memory.associative_memory.spread_steps";
    constexpr const char* ASSOCIATIVE_MEMORY_DECAY_RATE = "memory.associative_memory.decay_rate";
}

// Prediction system configuration
namespace PredictionConfig {
    constexpr const char* PREDICTION_MODEL_TYPE = "prediction.model_type";
    constexpr const char* PREDICTION_UPDATE_RATE = "prediction.update_rate";
    constexpr const char* PREDICTION_CONSISTENCY_THRESHOLD = "prediction.consistency_threshold";
    constexpr const char* PREDICTION_CONFIDENCE_THRESHOLD = "prediction.confidence_threshold";
    constexpr const char* PREDICTION_MAX_HISTORY = "prediction.max_history";
    constexpr const char* PREDICTION_LEARNING_RATE = "prediction.learning_rate";
}

// Default configuration values
namespace DefaultConfig {
    // Cognitive system defaults
    constexpr int DEFAULT_NEURAL_PLANNER_DEPTH = 3;
    constexpr float DEFAULT_NEURAL_PLANNER_CONFIDENCE = 0.8f;
    constexpr size_t DEFAULT_NEURAL_PLANNER_MAX_CACHE_SIZE = 100;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_MIN_PATTERN_SIZE = 3;
    constexpr size_t DEFAULT_CONCEPT_FORMATION_MAX_PATTERN_SIZE = 10;
    constexpr float DEFAULT_CONCEPT_FORMATION_SIMILARITY_THRESHOLD = 0.7f;
    constexpr float DEFAULT_CONCEPT_FORMATION_MIN_REINFORCEMENT = 0.1f;
    constexpr float DEFAULT_SELF_MODEL_CAPABILITY_BASELINE = 0.5f;
    constexpr float DEFAULT_SELF_MODEL_BODY_AWARENESS_THRESHOLD = 0.6f;
    constexpr float DEFAULT_SELF_MODEL_PREDICTION_ERROR_THRESHOLD = 0.3f;
    constexpr size_t DEFAULT_SOCIAL_LEARNING_MAX_OBSERVATIONS = 20;
    constexpr float DEFAULT_SOCIAL_LEARNING_IMITATION_CONFIDENCE = 0.7f;
    constexpr bool DEFAULT_SOCIAL_LEARNING_COMMUNICATION_ENABLED = true;
    
    // Memory system defaults
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    constexpr float DEFAULT_WORKING_MEMORY_DECAY_RATE = 0.05f;
    constexpr float DEFAULT_WORKING_MEMORY_REINFORCEMENT_FACTOR = 1.5f;
    constexpr float DEFAULT_WORKING_MEMORY_COMPETITION_STRENGTH = 1.0f;
    constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;
    constexpr bool DEFAULT_EPISODIC_MEMORY_REPLAY_ENABLED = true;
    constexpr float DEFAULT_EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD = 0.3f;
    constexpr float DEFAULT_EPISODIC_MEMORY_SIMILARITY_THRESHOLD = 0.7f;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_MAX_ASSOCIATIONS = 5000;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_MAX_PATTERNS = 1000;
    constexpr size_t DEFAULT_ASSOCIATIVE_MEMORY_SPREAD_STEPS = 3;
    constexpr float DEFAULT_ASSOCIATIVE_MEMORY_DECAY_RATE = 0.01f;
    
    // Prediction system defaults
    constexpr const char* DEFAULT_PREDICTION_MODEL_TYPE = "bayesian";
    constexpr float DEFAULT_PREDICTION_UPDATE_RATE = 0.1f;
    constexpr float DEFAULT_PREDICTION_CONSISTENCY_THRESHOLD = 0.8f;
    constexpr float DEFAULT_PREDICTION_CONFIDENCE_THRESHOLD = 0.6f;
    constexpr size_t DEFAULT_PREDICTION_MAX_HISTORY = 100;
    constexpr float DEFAULT_PREDICTION_LEARNING_RATE = 0.05f;
}

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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
