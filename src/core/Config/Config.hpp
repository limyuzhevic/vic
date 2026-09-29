#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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
    Runtime,
    SchemaValidated
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool validated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", bool val = false)
        : key(k), value(v), source(s), description(desc), validated(val) {}
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    ValidationResult(const std::vector<std::string>& err, const std::vector<std::string>& warn = {})
        : valid(err.empty()), errors(err), warnings(warn) {}
    
    bool isValid() const { return valid; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "ValidationResult:\n";
        ss << "  Valid: " << (valid ? "true" : "false") << "\n";
        if (!errors.empty()) {
            ss << "  Errors:\n";
            for (const auto& error : errors) {
                ss << "    - " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            ss << "  Warnings:\n";
            for (const auto& warning : warnings) {
                ss << "    - " << warning << "\n";
            }
        }
        return ss.str();
    }
};

// Configuration schema for validation
class ConfigSchema {
public:
    std::string name;
    std::unordered_map<std::string, std::string> fieldTypes;
    std::unordered_map<std::string, std::string> fieldDescriptions;
    std::unordered_map<std::string, std::string> fieldRanges;
    bool validateRequired;
    double minVersion;
    double maxVersion;
    
    ConfigSchema(const std::string& n, bool required = true, 
                 double minV = 0.0, double maxV = 999.0)
        : name(n), validateRequired(required), minVersion(minV), maxVersion(maxV) {}
    
    void addField(const std::string& key, const std::string& type, const std::string& description = "", 
                  const std::string& range = "") {
        fieldTypes[key] = type;
        fieldDescriptions[key] = description;
        fieldRanges[key] = range;
    }
    
    bool hasField(const std::string& key) const {
        return fieldTypes.find(key) != fieldTypes.end();
    }
    
    std::string getFieldType(const std::string& key) const {
        auto it = fieldTypes.find(key);
        return it != fieldTypes.end() ? it->second : "unknown";
    }
    
    std::string getFieldDescription(const std::string& key) const {
        auto it = fieldDescriptions.find(key);
        return it != fieldDescriptions.end() ? it->second : "";
    }
    
    std::string getFieldRange(const std::string& key) const {
        auto it = fieldRanges.find(key);
        return it != fieldRanges.end() ? it->second : "";
    }
    
    ValidationResult validate(const std::vector<ConfigEntry>& entries) const {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (validateRequired) {
            for (const auto& [key, type] : fieldTypes) {
                bool found = false;
                for (const auto& entry : entries) {
                    if (entry.key == key) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    errors.push_back("Missing required field: " + key);
                }
            }
        }
        
        // Check for unknown fields
        for (const auto& entry : entries) {
            if (!hasField(entry.key) && entry.source != ConfigSource::Default) {
                warnings.push_back("Unknown field in schema: " + entry.key);
            }
        }
        
        return ValidationResult(errors, warnings);
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
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlString);
    
    // Load from YAML file
    bool loadFromYamlFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJson() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set value with validation against schema
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::SchemaValidated);
    
    // Nested key support (e.g., "brain.neuron_count", "plasticity.stdp.learning_rate")
    template<typename T>
    std::optional<T> getNested(const std::vector<std::string>& keys) const;
    
    template<typename T>
    bool setNested(const std::vector<std::string>& keys, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    ValidationResult validateSchema(const ConfigSchema& schema) const;
    bool validateField(const std::string& key, const ConfigSchema& schema) const;
    
    // Field type checking
    bool isFieldOfType(const std::string& key, const std::string& typeName) const;
    std::string getFieldType(const std::string& key) const;
    
    // Range checking
    bool isFieldInRange(const std::string& key, double min, double max) const;
    
    // Get entry for a key
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Configuration inheritance and merging
    bool mergeFrom(const Config& other, bool override = true);
    bool inheritFrom(const Config& parent, const std::vector<std::string>& allowedKeys = {});
    
    // JSON/YAML utilities
    static std::string toJsonString(const ConfigValue& value);
    static ConfigValue fromJsonValue(const nlohmann::json& jsonValue);
    static nlohmann::json toNlohmannJson(const ConfigValue& value);
    static std::string configToJson(const std::vector<ConfigEntry>& entries);
    static std::vector<ConfigEntry> jsonToConfig(const std::string& jsonString, ConfigSource source = ConfigSource::File);
    static std::vector<ConfigEntry> yamlToConfig(const std::string& yamlString, ConfigSource source = ConfigSource::File);
    
    // Schema loading/saving
    static std::string schemaToJson(const ConfigSchema& schema);
    static ConfigSchema schemaFromJson(const std::string& jsonString);
    
    // Error handling
    static std::string lastError;
    static std::string getLastError();
    static void clearLastError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Key parsing for nested access
    static std::vector<std::string> parseKey(const std::string& key) const;
    static std::string joinKey(const std::vector<std::string>& keys) const;
    
    // Error handling
    static void setLastError(const std::string& error);
    
    // Type conversion helpers
    template<typename T>
    static std::string getTypeName();
    
    template<typename T>
    static bool valueMatchesType(const ConfigValue& value);
    
    template<typename T>
    static T getValueFromVariant(const ConfigValue& value);
    
    // JSON/YAML conversion helpers
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
    static YAML::Node configValueToYaml(const ConfigValue& value);
    static ConfigValue yamlToConfigValue(const YAML::Node& yamlNode);
    
    // Validation helpers
    static std::vector<std::string> validateFieldType(const ConfigEntry& entry) const;
    static std::vector<std::string> validateFieldRange(const ConfigEntry& entry) const;
};

// Template implementations for type checking and conversion
namespace nlm {

template<typename T>
inline std::string Config::getTypeName() {
    if (std::is_same_v<T, int>) return "int";
    if (std::is_same_v<T, int64_t>) return "int64_t";
    if (std::is_same_v<T, double>) return "double";
    if (std::is_same_v<T, bool>) return "bool";
    if (std::is_same_v<T, std::string>) return "string";
    if (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    if (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    if (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    return "unknown";
}

template<typename T>
inline bool Config::valueMatchesType(const ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) return std::holds_alternative<int>(value);
    if constexpr (std::is_same_v<T, int64_t>) return std::holds_alternative<int64_t>(value);
    if constexpr (std::is_same_v<T, double>) return std::holds_alternative<double>(value);
    if constexpr (std::is_same_v<T, bool>) return std::holds_alternative<bool>(value);
    if constexpr (std::is_same_v<T, std::string>) return std::holds_alternative<std::string>(value);
    if constexpr (std::is_same_v<T, std::vector<int>>) return std::holds_alternative<std::vector<int>>(value);
    if constexpr (std::is_same_v<T, std::vector<double>>) return std::holds_alternative<std::vector<double>>(value);
    if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::holds_alternative<std::vector<std::string>>(value);
    return false;
}

template<typename T>
inline T Config::getValueFromVariant(const ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) return std::get<int>(value);
    if constexpr (std::is_same_v<T, int64_t>) return std::get<int64_t>(value);
    if constexpr (std::is_same_v<T, double>) return std::get<double>(value);
    if constexpr (std::is_same_v<T, bool>) return std::get<bool>(value);
    if constexpr (std::is_same_v<T, std::string>) return std::get<std::string>(value);
    if constexpr (std::is_same_v<T, std::vector<int>>) return std::get<std::vector<int>>(value);
    if constexpr (std::is_same_v<T, std::vector<double>>) return std::get<std::vector<double>>(value);
    if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::get<std::vector<std::string>>(value);
    throw std::runtime_error("Unsupported type conversion");
}

} // namespace nlm

// Advanced configuration sections
namespace nlm {

// Advanced learning configuration
namespace AdvancedLearning {
    const std::string ADAPTIVE_DIFFICULTY = "advanced.learning.adaptive_difficulty";
    const std::string PROGRESSIVE_COMPLEXITY = "advanced.learning.progressive_complexity";
    const std::string CONVERGENCE_RATE = "advanced.learning.convergence_rate";
    const std::string LEARNING_RATE = "advanced.learning.learning_rate";
    const std::string CONVERGENCE_THRESHOLD = "advanced.learning.convergence_threshold";
    const std::string CURRICULUM_TASKS = "advanced.learning.curriculum_tasks";
    const std::string PROGRESS_FILE = "advanced.learning.progress_file";
    const std::string METRICS_FILE = "advanced.learning.metrics_file";
}

// Meta-learning configuration
namespace MetaLearning {
    const std::string TRANSFER_LEARNING = "advanced.meta_learning.transfer_learning";
    const std::string FEATURE_BASED_TRANSFER = "advanced.meta_learning.feature_based_transfer";
    const std::string TRANSFER_THRESHOLD = "advanced.meta_learning.transfer_threshold";
    const std::string OVERALL_EFFECTIVENESS = "advanced.meta_learning.overall_effectiveness";
    const std::string STATE_FILE = "advanced.meta_learning.state_file";
}

// Multi-objective optimization configuration
namespace MultiObjectiveOptimization {
    const std::string POPULATION_SIZE = "advanced.moo.population_size";
    const std::string CONVERGENCE_THRESHOLD = "advanced.moo.convergence_threshold";
    const std::string MAX_GENERATIONS = "advanced.moo.max_generations";
    const std::string MUTATION_RATE = "advanced.moo.mutation_rate";
    const std::string CROSSOVER_RATE = "advanced.moo.crossover_rate";
    const std::string SIGMA = "advanced.moo.sigma";
    const std::string RESULTS_FILE = "advanced.moo.results_file";
}

// Reinforcement learning configuration
namespace ReinforcementLearning {
    const std::string GAMMA = "advanced.rl.gamma";
    const std::string GAE_LAMBDA = "advanced.rl.gae_lambda";
    const std::string ENTROPY_COEFF = "advanced.rl.entropy_coeff";
    const std::string VALUE_LOSS_COEFF = "advanced.rl.value_loss_coeff";
    const std::string MODEL_FILE = "advanced.rl.model_file";
}

// Dynamic environment configuration
namespace DynamicEnvironment {
    const std::string NUM_SIMULATIONS = "environment.num_simulations";
    const std::string TIME_STEP = "environment.time_step";
    const std::string STABILITY_THRESHOLD = "environment.stability_threshold";
    const std::string EVOLUTION_SPEED = "environment.evolution_speed";
    const std::string ADAPTATION_THRESHOLD = "environment.adaptation_threshold";
    const std::string INITIAL_PARAMETERS = "environment.initial_parameters";
    const std::string STATE_FILE = "environment.state_file";
}

// Distributed computing configuration
namespace DistributedComputing {
    const std::string NUM_NODES = "distributed.num_nodes";
    const std::string LOAD_BALANCING_THRESHOLD = "distributed.load_balancing_threshold";
    const std::string FAILURE_TOLERANCE = "distributed.failure_tolerance";
    const std::string NODE_COMPUTE_RESOURCES = "distributed.node.compute_resources";
    const std::string NODE_MEMORY_RESOURCES = "distributed.node.memory_resources";
    const std::string NODE_NETWORK_RESOURCES = "distributed.node.network_resources";
}

// Statistical analysis configuration
namespace StatisticalAnalysis {
    const std::string CONFIDENCE_LEVEL = "stats.confidence_level";
    const std::string SIGNIFICANCE_LEVEL = "stats.significance_level";
    const std::string MAX_DATASETS = "stats.max_datasets";
}

// Neuroscientific analysis configuration
namespace NeuroscientificAnalysis {
    const std::string CONFIDENCE_LEVEL = "neuro.confidence_level";
    const std::string EEG_SAMPLING_RATE = "neuro.eeg.sampling_rate";
    const std::string FMRI_TR = "neuro.fmri.tr";
    const std::string FMRI_VOXEL_SIZE = "neuro.fmri.voxel_size";
}

// Debugging configuration
namespace Debugging {
    const std::string SNAPSHOT_INTERVAL = "debug.snapshot_interval";
    const std::string AUTO_LOGGING = "debug.auto_logging";
    const std::string LOG_FILE = "debug.log_file";
    const std::string CONSOLE_OUTPUT = "debug.console_output";
    const std::string FILE_OUTPUT = "debug.file_output";
    const std::string ENABLED_MODULES = "debug.enabled_modules";
}

// Model interpretability configuration
namespace ModelInterpretability {
    const std::string LIME_SAMPLE_SIZE = "interpretability.lime.sample_size";
    const std::string LIME_NEIGHBORHOOD_RADIUS = "interpretability.lime.neighborhood_radius";
    const std::string LIME_PERTURBATION_STEP = "interpretability.lime.perturbation_step";
    const std::string ANCHOR_COVERAGE_THRESHOLD = "interpretability.anchor.coverage_threshold";
    const std::string ANCHOR_PRECISION_THRESHOLD = "interpretability.anchor.precision_threshold";
    const std::string MAX_CANDIDATES = "interpretability.anchor.max_candidates";
}

// Research configuration
namespace Research {
    const std::string AUTOSAVE = "research.autosave";
    const std::string CHECKPOINT_FILE = "research.checkpoint_file";
    const std::string OUTPUT_DIRECTORY = "research.output_directory";
    const std::string MAX_EXPERIMENTS = "research.max_experiments";
    const std::string REPRODUCIBILITY_TOLERANCE = "research.reproducibility_tolerance";
}

// Feature importance calculation
namespace FeatureImportance {
    const std::string METHOD = "feature_importance.method";
    const std::string TOP_K = "feature_importance.top_k";
    const std::string NORMALIZATION = "feature_importance.normalization";
}

// Partial dependence
namespace PartialDependence {
    const std::string BANDWIDTH = "partial_dependence.bandwidth";
    const std::string SAMPLING_STRATEGY = "partial_dependence.sampling_strategy";
}

// Research workflow
namespace ResearchWorkflow {
    const std::string DATA_PREPARATION_ENABLED = "workflow.data_preparation";
    const std::string MODEL_TRAINING_ENABLED = "workflow.model_training";
    const std::string EVALUATION_ENABLED = "workflow.evaluation";
    const std::string VALIDATION_ENABLED = "workflow.validation";
    const std::string REPORTING_ENABLED = "workflow.reporting";
}

} // namespace nlm

