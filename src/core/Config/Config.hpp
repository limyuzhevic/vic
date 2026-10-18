#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Advanced configuration data structures

// Neural Network Parameters
struct NeuralNetworkParams {
    // LIF Neuron Settings
    double v_rest = -65.0;        // Resting potential (mV)
    double v_reset = -70.0;       // Reset potential (mV)
    double v_thresh = -50.0;      // Threshold potential (mV)
    double tau_m = 20.0;          // Membrane time constant (ms)
    double tau_ref = 2.0;         // Refractory period (ms)
    
    // Synaptic Parameters
    double synapse_weight_max = 1.0;  // Maximum synaptic weight
    double synapse_weight_min = -1.0; // Minimum synaptic weight
    double synapse_decay = 0.01;      // Synaptic decay rate
    double connection_probability = 0.1; // Connection probability
    
    // Intracellular Dynamics
    double i_leak = 0.1;           // Leak current
    double capacitance = 1.0;      // Membrane capacitance
};

// Memory System Configuration
struct MemorySystemConfig {
    // Working Memory
    double working_memory_capacity = 100.0; // Capacity in "chunks"
    double working_memory_decay = 0.01;    // Decay rate
    double working_memory_theta = 0.05;    // Theta rhythm frequency (Hz)
    double working_memory_gamma = 40.0;    // Gamma rhythm frequency (Hz)
    
    // Episodic Memory
    double episodic_memory_capacity = 10000.0; // Number of episodes
    double episodic_memory_decay = 0.001;      // Decay rate
    double episodic_memory_compression = 0.8;  // Compression ratio
    
    // Associative Memory
    double associative_memory_strength = 0.5;   // Hebbian strength
    double associative_memory_threshold = 0.1;   // Activation threshold
    double associative_memory_noise = 0.01;     // Noise level
};

// Neuromodulation Configuration
struct NeuromodulationConfig {
    // Dopamine System
    double dopamine_baseline = 0.1;            // Baseline dopamine level
    double dopamine_gain = 1.0;                // Dopamine gain factor
    double dopamine_sensitivity = 0.5;         // Sensitivity to rewards
    double dopamine_decay = 0.05;              // Decay time constant
    
    // Curiosity System
    double curiosity_baseline = 0.1;           // Baseline curiosity
    double curiosity_gain = 1.0;               // Curiosity amplification
    double curiosity_sensitivity = 0.5;        // Sensitivity to novelty
    double curiosity_decay = 0.05;             // Decay time constant
    
    // Novelty Detection
    double novelty_threshold = 0.5;           // Novelty threshold
    double novelty_sensitivity = 1.0;          // Novelty detection sensitivity
    double novelty_memory = 0.01;              // Novelty memory decay
};

// Development System Configuration
struct DevelopmentSystemConfig {
    // Synaptogenesis
    double synaptogenesis_rate = 0.001;        // Synaptogenesis rate
    double synaptogenesis_peak_age = 10.0;     // Age at peak synaptogenesis
    double synaptogenesis_strength = 0.5;      // Synaptogenesis strength
    
    // Pruning
    double pruning_rate = 0.0001;             // Pruning rate
    double pruning_peak_age = 100.0;          // Age at peak pruning
    double pruning_selectivity = 0.8;         // Pruning selectivity
    
    // Myelination
    double myelination_rate = 0.0005;         // Myelination rate
    double myelination_efficiency = 0.9;       // Myelination efficiency
};

// Plasticity Configuration
struct PlasticityConfig {
    // STDP Parameters
    double stdp_ltp_weight = 0.01;           // LTP weight change
    double stdp_ltd_weight = 0.012;          // LTD weight change
    double stdp_time_constant = 20.0;        // STDP time constant
    double stdp_tau_pre = 10.0;              // Pre-synaptic time constant
    double stdp_tau_post = 20.0;             // Post-synaptic time constant
    
    // Hebbian Plasticity
    double hebbian_learning_rate = 0.01;     // Hebbian learning rate
    double hebbian_threshold = 0.1;          // Hebbian threshold
    double hebbian_decay = 0.001;            // Hebbian decay
    
    // Spike-Timing Dependent Plasticity
    double stp_ltp_window = 20.0;            // LTP time window (ms)
    double stp_ltd_window = 50.0;            // LTD time window (ms)
};

// Prediction System Configuration
struct PredictionSystemConfig {
    // Prediction Error
    double prediction_error_weight = 1.0;     // Prediction error weight
    double prediction_error_decay = 0.1;     // Prediction error decay
    double prediction_error_threshold = 0.5;  // Prediction error threshold
    
    // Confidence Tracking
    double confidence_baseline = 0.5;        // Baseline confidence
    double confidence_gain = 1.0;            // Confidence gain
    double confidence_update_rate = 0.01;    // Confidence update rate
    
    // Forward Models
    double forward_model_accuracy = 0.8;     // Forward model accuracy
    double forward_model_noise = 0.1;        // Forward model noise
    double forward_model_capacity = 100.0;   // Forward model memory capacity
};

// Cognition System Configuration
struct CognitionSystemConfig {
    // Neural Planner
    double planner_horizon = 10;             // Planning horizon (steps)
    double planner_exploration = 0.1;       // Exploration rate
    double planner_exploitation = 0.9;      // Exploitation rate
    double planner_confidence_threshold = 0.7; // Confidence threshold for planning
    
    // Concept Formation
    double concept_threshold = 0.5;         // Concept formation threshold
    double concept_decay = 0.001;           // Concept decay rate
    double concept_similarity = 0.8;        // Concept similarity measure
    double concept_merge_threshold = 0.9;   // Concept merge threshold
    
    // Attention System
    double attention_gain = 1.0;            // Attention gain
    double attention_decay = 0.1;           // Attention decay
    double attention_threshold = 0.1;       // Attention threshold
    double attention_saturation = 0.95;     // Attention saturation point
};

// Experiment Configuration
struct ExperimentConfig {
    // Basic Settings
    int max_steps = 10000;                   // Maximum simulation steps
    double time_limit = 0.0;                // Time limit (seconds, 0 = unlimited)
    double record_interval = 1.0;           // Recording interval (steps)
    double save_interval = 100.0;           // Save interval (steps)
    
    // Recording Options
    bool record_neurons = true;             // Record neuron states
    bool record_synapses = true;            // Record synapse states
    bool record_memories = true;            // Record memory states
    bool record_modulators = true;          // Record neuromodulator states
    
    // Analysis Options
    bool compute_metrics = true;            // Compute performance metrics
    bool track_rewards = true;              // Track rewards
    bool track_errors = true;               // Track prediction errors
};

// Visualization Configuration
struct VisualizationConfig {
    // Basic Visualization
    bool enabled = false;                    // Enable visualization
    double update_rate = 30.0;              // Update rate (Hz)
    std::string backend = "glfw";          // Visualization backend
    
    // Display Settings
    int window_width = 1920;                // Window width
    int window_height = 1080;               // Window height
    bool fullscreen = false;                // Fullscreen mode
    double point_size = 1.0;                // Neuron point size
    double line_width = 1.0;                // Synapse line width
    
    // Color Schemes
    std::string color_scheme = "default";  // Color scheme
    double opacity = 1.0;                   // Global opacity
    bool antialiasing = true;               // Enable antialiasing
};

// Performance Optimization Configuration
struct PerformanceOptimizationConfig {
    // SIMD Optimization
    bool enable_simd = true;                // Enable SIMD instructions
    int simd_width = 256;                   // SIMD vector width (bits)
    bool enable_avx2 = true;                // Enable AVX2 instructions
    bool enable_avx512 = false;             // Enable AVX-512 instructions
    
    // Parallelization
    int parallel_threads = 4;               // Number of parallel threads
    bool enable_openmp = true;              // Enable OpenMP parallelization
    bool enable_cuda = false;               // Enable CUDA acceleration
    int cuda_device = 0;                    // CUDA device ID
    
    // Memory Optimization
    bool enable_memory_pool = true;         // Enable memory pools
    int memory_pool_size = 1000000;        // Memory pool size
    bool enable_numa_optimization = false;   // Enable NUMA optimization
};

// Advanced Logging and Monitoring Configuration
struct AdvancedLoggingConfig {
    // Log Levels
    std::string log_level = "INFO";        // Log level (DEBUG, INFO, WARN, ERROR, FATAL)
    bool log_to_file = false;               // Log to file
    std::string log_filename = "nlm.log";  // Log filename
    bool log_timestamps = true;             // Include timestamps
    bool log_thread_id = true;              // Include thread ID
    
    // Performance Monitoring
    bool enable_performance_monitoring = true; // Enable performance monitoring
    double performance_sample_rate = 0.01;   // Performance sampling rate (Hz)
    bool enable_memory_monitoring = true;    // Enable memory monitoring
    bool enable_cpu_monitoring = true;       // Enable CPU monitoring
    
    // Custom Metrics
    std::vector<std::string> custom_metrics; // Custom metrics to track
    bool enable_custom_metrics = false;      // Enable custom metrics
};

// Configuration preset types
enum class ConfigPreset {
    RESEARCH,      // Research-focused configuration
    PERFORMANCE,   // Performance-optimized configuration
    EDUCATION,     // Education-focused configuration
    BALANCED,      // Balanced configuration
    CUSTOM         // Custom configuration
};

// Configuration value types - enhanced to support JSON types and advanced structures
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<ConfigValue>,  // JSON array support
    std::map<std::string, ConfigValue>,  // JSON object support
    // Advanced configuration structures
    NeuralNetworkParams,
    MemorySystemConfig,
    NeuromodulationConfig,
    DevelopmentSystemConfig,
    PlasticityConfig,
    PredictionSystemConfig,
    CognitionSystemConfig,
    ExperimentConfig,
    VisualizationConfig,
    PerformanceOptimizationConfig,
    AdvancedLoggingConfig,
    std::vector<NeuralNetworkParams>,
    std::vector<MemorySystemConfig>,
    std::vector<NeuromodulationConfig>,
    std::vector<DevelopmentSystemConfig>,
    std::vector<PlasticityConfig>,
    std::vector<PredictionSystemConfig>,
    std::vector<CognitionSystemConfig>,
    std::vector<ExperimentConfig>,
    std::vector<VisualizationConfig>,
    std::vector<PerformanceOptimizationConfig>,
    std::vector<AdvancedLoggingConfig>,
    std::map<std::string, NeuralNetworkParams>,
    std::map<std::string, MemorySystemConfig>,
    std::map<std::string, NeuromodulationConfig>,
    std::map<std::string, DevelopmentSystemConfig>,
    std::map<std::string, PlasticityConfig>,
    std::map<std::string, PredictionSystemConfig>,
    std::map<std::string, CognitionSystemConfig>,
    std::map<std::string, ExperimentConfig>,
    std::map<std::string, VisualizationConfig>,
    std::map<std::string, PerformanceOptimizationConfig>,
    std::map<std::string, AdvancedLoggingConfig>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    JSONFile
};

// Configuration entry with enhanced metadata for advanced options
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string schemaType;  // JSON schema type for validation
    std::string category;    // Configuration category (neural, memory, neuromodulation, etc.)
    std::string subcategory; // Subcategory for better organization
    bool advanced;           // Whether this is an advanced option
    std::string presetScope; // Preset scope (research, performance, education)
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), schemaType(), 
                   category(), subcategory(), advanced(false), presetScope() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const std::string& schema = "", const std::string& cat = "",
                const std::string& subcat = "", bool adv = false, const std::string& preset = "")
        : key(k), value(v), source(s), description(desc), schemaType(schema), category(cat), 
          subcategory(subcat), advanced(adv), presetScope(preset) {}
};

// Enhanced Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;  // json schema type (string, number, boolean, array, object)
    std::string description;
    bool required;  // Whether this key is required
    std::string defaultValue;  // Default value as string
    std::string category;      // Configuration category for organization
    std::string presetScope;   // Which presets include this option
    
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "", 
                 bool req = false, const std::string& def = "", const std::string& cat = "",
                 const std::string& preset = "")
        : key(k), type(t), description(desc), required(req), defaultValue(def), 
          category(cat), presetScope(preset) {}
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
    
    // Load from file (JSON or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file specifically
    bool loadFromJSONFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON or key=value format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON file specifically
    bool saveToJSONFile(const std::string& filepath) const;
    
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
    
// Configuration preset
    bool applyPreset(ConfigPreset preset);
    static std::vector<std::string> getPresetNames();
    static ConfigPreset stringToPreset(const std::string& presetName);
    static std::string presetToString(ConfigPreset preset);
    
    // Enhanced validation
    static bool validateAdvancedOptions(const Config& config);
    static bool validatePresetSpecific(const ConfigPreset preset, const Config& config);
    
    // Schema validation helpers
    static std::string getConfigValueType(const ConfigValue& value);
    static bool validateAdvancedSchemaType(const std::string& schemaType, const ConfigValue& value);
    
    // Configuration serialization
    static std::string configValueToDetailedString(const ConfigValue& value, int indent = 0) const;
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static bool jsonToConfigValue(const nlohmann::json& json, ConfigValue& value);
    
    // Preset configuration loading
    static bool loadPresetFromJSON(const std::string& presetName, Config& config);
    static bool savePresetToJSON(const std::string& presetName, const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File type detection
    static bool isJSONFile(const std::string& filepath);
    
    // Schema validation helpers
    static bool validateValueType(const std::string& schemaType, const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value);
    
    // Advanced option category detection
    static std::string getCategoryFromKey(const std::string& key);
    static std::string getSubcategoryFromKey(const std::string& key);
    
    // Preset application
    static bool applyResearchPreset(Config& config);
    static bool applyPerformancePreset(Config& config);
    static bool applyEducationPreset(Config& config);
    static bool applyBalancedPreset(Config& config);
};

} // namespace nlm
