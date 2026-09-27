// NLM Advanced Configuration System
// Provides comprehensive configuration management with hierarchical structure,
// type-safe access, validation, and support for multiple formats.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <any>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <spdlog/spdlog.h>

namespace nlm {

// Forward declarations
class AdvancedConfig;
class ConfigTemplate;
class ConfigCategory;

// Configuration value types with enhanced capabilities
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>,
    std::map<std::string, std::any>
>;

// Configuration sources for tracking origin
enum class ConfigSource {
    Default,      // Built into the application
    Template,     // From a configuration template
    File,         // From a configuration file (JSON, YAML, INI)
    CommandLine,  // From command line arguments
    Runtime,      // Set programmatically at runtime
    Inherited    // Inherited from a parent configuration
};

// Configuration categories for organizing parameters
enum class ConfigCategory {
    BrainParameters,           // Neuron counts, connectivity, plasticity rules
    NeuromodulationParameters, // Dopamine, reward, novelty, curiosity dynamics
    MemoryParameters,          // Working memory capacity, episodic memory limits
    SensoryParameters,         // Vision, touch, internal signal processing
    MotorParameters,           // Action space, motor neuron distribution
    DevelopmentParameters,     // Growth stages, maturation rates
    SimulationParameters,      // Timestep, duration, performance settings
    DebugParameters,           // Debugging and monitoring configuration
    PerformanceParameters,     // Performance optimization settings
    ExperimentParameters        // Experiment configuration management
};

// Validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string category;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true), errorMessage(""), category("") {}
    ValidationResult(bool v, const std::string& error, const std::string& cat = "")
        : valid(v), errorMessage(error), category(cat) {}
};

// Configuration metadata
struct ConfigMetadata {
    std::string key;
    std::string description;
    ConfigCategory category;
    ConfigSource source;
    std::string typeName;
    std::vector<std::string> tags;
    bool required;
    std::any defaultValue;
    std::function<ValidationResult(const std::any&)> validator;
    
    ConfigMetadata() : category(ConfigCategory::BrainParameters), source(ConfigSource::Default), 
                      required(false) {}
};

// Configuration entry with full metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigMetadata metadata;
    std::vector<std::string> path;  // Hierarchical path (e.g., {"brain", "neurons", "count"})
    std::weak_ptr<AdvancedConfig> parentConfig;  // Parent for inheritance
    
    ConfigEntry() = default;
    ConfigEntry(const std::string& k, const ConfigValue& v, const ConfigMetadata& meta)
        : key(k), value(v), metadata(meta) {}
};

// Configuration template for different use cases
class ConfigTemplate {
public:
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    std::vector<std::string> tags;
    std::map<std::string, ConfigMetadata> templateDefinitions;
    bool isPreset;  // Whether this is a preset for different experience levels
    
    ConfigTemplate() : isPreset(false) {}
    
    void addParameter(const std::string& key, const ConfigMetadata& metadata);
    ConfigMetadata getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    std::vector<std::string> getParameterKeys() const;
};

// Advanced configuration class with hierarchical structure
class AdvancedConfig {
public:
    AdvancedConfig();
    explicit AdvancedConfig(const std::string& name);
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Configuration hierarchy
    void setParent(std::shared_ptr<AdvancedConfig> parent);
    std::shared_ptr<AdvancedConfig> getParent() const;
    
    // Load from files (JSON, YAML, INI formats)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool loadFromINI(const std::string& iniContent);
    
    // Save to files
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToINI(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from template
    bool loadFromTemplate(const ConfigTemplate& template);
    
    // Load from preset
    bool loadFromPreset(const std::string& presetName);
    
    // Get values with type-safe access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Hierarchical key access
    template<typename T>
    std::optional<T> get(const std::vector<std::string>& path) const;
    
    template<typename T>
    T getOr(const std::vector<std::string>& path, const T& defaultValue) const;
    
    // Configuration management
    bool has(const std::string& key) const;
    bool has(const std::vector<std::string>& path) const;
    void remove(const std::string& key);
    void remove(const std::vector<std::string>& path);
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys with path
    std::vector<std::vector<std::string>> getAllPaths() const;
    
    // Clear and reset
    void clear();
    void resetToDefaults();
    
    // Configuration snapshots
    std::string createSnapshot() const;
    bool restoreSnapshot(const std::string& snapshotId);
    
    // Random seed management
    void setRandomSeed(int64_t seed);
    int64_t getRandomSeed() const;
    int64_t generateRandomSeed();
    
    // Validation
    ValidationResult validate() const;
    ValidationResult validate(const std::string& key) const;
    
    // Configuration summary
    std::string summary() const;
    std::string getCategorySummary(ConfigCategory category) const;
    
    // Template management
    void registerTemplate(const ConfigTemplate& template);
    ConfigTemplate getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;
    
    // Preset management
    void registerPreset(const ConfigTemplate& preset);
    ConfigTemplate getPreset(const std::string& name) const;
    bool hasPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    
    // Experiment configuration management
    void createExperimentConfig(const std::string& experimentName);
    bool saveExperimentConfig(const std::string& experimentName) const;
    bool loadExperimentConfig(const std::string& experimentName);
    std::vector<std::string> getExperimentNames() const;
    
    // Performance tuning
    void optimizeForPerformance();
    void optimizeForAccuracy();
    void optimizeForSpeed();
    
    // Debug and monitoring
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    void logConfiguration() const;
    
    // Configuration inheritance
    void inheritFrom(const AdvancedConfig& parent);
    bool overrides(const std::string& key) const;
    
    // Utility functions
    static std::string normalizeKey(const std::string& key);
    static std::vector<std::string> splitPath(const std::string& path);
    static std::string joinPath(const std::vector<std::string>& path);
    
    // Constants for built-in templates
    static const std::vector<ConfigTemplate> getBuiltInTemplates();
    static const std::vector<ConfigTemplate> getBuiltInPresets();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    
    // Value conversion and type checking
    static bool isTypeCompatible(const ConfigValue& value, const std::string& typeName) const;
    static std::any getDefaultValueForType(const std::string& typeName) const;
    static ValidationResult validateValue(const ConfigValue& value, const ConfigMetadata& metadata) const;
};

// Template factory functions
ConfigTemplate createBeginnerTemplate();
ConfigTemplate createIntermediateTemplate();
ConfigTemplate createAdvancedTemplate();
ConfigTemplate createResearchTemplate();

// Preset factory functions
ConfigTemplate createPerformancePreset();
ConfigTemplate createAccuracyPreset();
ConfigTemplate createBalancedPreset();
ConfigTemplate createExplorationPreset();

// Predefined configuration categories
namespace ConfigCategories {
    const std::string BRAIN = "brain";
    const std::string NEUROMODULATION = "neuromodulation";
    const std::string MEMORY = "memory";
    const std::string SENSORY = "sensory";
    const std::string MOTOR = "motor";
    const std::string DEVELOPMENT = "development";
    const std::string SIMULATION = "simulation";
    const std::string DEBUG = "debug";
    const std::string PERFORMANCE = "performance";
    const std::string EXPERIMENT = "experiment";
}

// Predefined configuration keys
namespace ConfigKeys {
    // Brain parameters
    const std::string NEURON_COUNT = "brain.neurons.count";
    const std::string REGION_COUNT = "brain.regions.count";
    const std::string CONNECTION_PROBABILITY = "brain.connections.probability";
    const std::string LIF_THRESHOLD = "brain.neurons.lif.threshold";
    const std::string LIF_MEMBRANE_TIME_CONSTANT = "brain.neurons.lif.tau_membrane";
    const std::string LIF_REFACTORY_PERIOD = "brain.neurons.lif.refractory_period";
    
    // Neuromodulation parameters
    const std::string DOPAMINE_BASELINE = "neuromodulation.dopamine.baseline";
    const std::string REWARD_DISCOUNT_FACTOR = "neuromodulation.reward.discount_factor";
    const std::string NOVELTY_SENSITIVITY = "neuromodulation.novelty.sensitivity";
    const std::string CURIOSITY_STRENGTH = "neuromodulation.curiosity.strength";
    
    // Memory parameters
    const std::string WORKING_MEMORY_CAPACITY = "memory.working.capacity";
    const std::string EPISODIC_MEMORY_LIMIT = "memory.episodic.limit";
    
    // Sensory parameters
    const std::string VISION_RESOLUTION = "sensory.vision.resolution";
    const std::string TOUCH_SENSITIVITY = "sensory.touch.sensitivity";
    const std::string INTERNAL_SIGNAL_NOISE = "sensory.internal.signal_noise";
    
    // Motor parameters
    const std::string ACTION_SPACE_SIZE = "motor.actions.space_size";
    const std::string MOTOR_NEURON_DISTRIBUTION = "motor.neurons.distribution";
    
    // Development parameters
    const std::string DEVELOPMENT_STAGE = "development.stage";
    const std::string SYNAPTAGENESIS_RATE = "development.synaptogenesis.rate";
    const std::string PRUNING_RATE = "development.pruning.rate";
    
    // Simulation parameters
    const std::string SIMULATION_TIMESTEP = "simulation.timestep";
    const std::string SIMULATION_DURATION = "simulation.duration";
    const std::string SIMULATION_MAX_STEPS = "simulation.max_steps";
    const std::string PERFORMANCE_TARGET_FPS = "simulation.performance.target_fps";
    
    // Debug parameters
    const std::string DEBUG_MODE = "debug.mode.enabled";
    const std::string LOG_LEVEL = "debug.logging.level";
    const std::string LOG_TO_FILE = "debug.logging.to_file";
    
    // Performance parameters
    const std::string PERFORMANCE_OPTIMIZATION_LEVEL = "performance.optimization.level";
    const std::string SIMD_ENABLED = "performance.simd.enabled";
    
    // Experiment parameters
    const std::string EXPERIMENT_NAME = "experiment.name";
    const std::string EXPERIMENT_DESCRIPTION = "experiment.description";
}

} // namespace nlm