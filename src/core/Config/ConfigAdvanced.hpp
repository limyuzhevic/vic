#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Default configuration values
struct DefaultConfig {
    // Neural system parameters
    double neuron_count = 1000.0;
    double connection_probability = 0.1;
    double initial_weight_mean = 0.5;
    double initial_weight_std = 0.1;
    double v_thresh = -50.0;
    double v_rest = -70.0;
    double v_reset = -75.0;
    double tau_mem = 20.0;
    double tau_ref = 2.0;
    
    // Plasticity parameters
    bool stdp_enable = true;
    double stdp_learning_rate = 0.001;
    double stdp_tau_plus = 20.0;
    double stdp_tau_minus = 20.0;
    bool hebbian_enable = true;
    bool structural_enable = true;
    double synaptogenesis_rate = 0.0001;
    double pruning_rate = 0.00001;
    
    // Neuromodulation
    double dopamine_scale = 1.0;
    bool curiosity_enable = true;
    bool novelty_enable = true;
    
    // Simulation parameters
    double simulation_timestep = 0.001;
    size_t replay_interval = 100;
    size_t consolidation_interval = 1000;
    
    // Development
    double developmental_initial_plasticity = 1.0;
    double developmental_critical_period_factor = 0.8;
    double developmental_maturation_factor = 0.5;
    double developmental_adult_factor = 0.2;
    
    // Memory
    size_t working_memory_capacity = 100;
    size_t episodic_memory_max_episodes = 1000;
    double episodic_memory_retention = 0.5;
    
    // Prediction
    bool temporal_prediction_enable = true;
    double prediction_confidence = 0.5;
    double prediction_error_decay = 0.95;
    
    // Cognition
    double attention_inhibition_strength = 0.5;
    double attention_excitation_strength = 1.5;
    double planner_planning_depth = 5.0;
    double concept_formation_threshold = 0.75;
    double self_model_adaptation_rate = 0.1;
    
    // World parameters
    double world_width = 20.0;
    double world_height = 20.0;
    double world_vision_width = 16;
    double world_vision_height = 16;
    double max_energy = 100.0;
    double energy_decay_rate = 0.01;
    double resource_energy_gain = 30.0;
    double hazard_damage = 15.0;
    
    // Logging
    std::string log_level = "INFO";
    bool log_colors = true;
    std::string log_file = "";
    
    // Performance
    size_t checkpoint_interval = 10000;
    std::string checkpoint_dir = "./checkpoints";
    bool checkpoint_compress = true;
    
    // Random seed
    uint64_t random_seed = 42;
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    
    // Add validation rule
    void addRule(const ValidationRule& rule);
    
    // Add default configuration
    void addDefaults(const DefaultConfig& defaults);
    
    // Validate configuration
    ValidationResult validate(const std::map<std::string, ConfigValue>& config) const;
    
    // Get validation rules
    const std::vector<ValidationRule>& getRules() const { return rules_; }
    
    // Get default configuration
    const DefaultConfig& getDefaults() const { return defaults_; }
    
    // Apply defaults to configuration
    void applyDefaults(std::map<std::string, ConfigValue>& config) const;
    
    // Convert config value to string for validation
    static std::string configValueToString(const ConfigValue& value);
    
    // Convert string to config value
    static ConfigValue stringToConfigValue(const std::string& str);
    
private:
    std::vector<ValidationRule> rules_;
    DefaultConfig defaults_;
};

// Configuration class with JSON support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (with validation)
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration
    bool validate();
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Set validator
    void setValidator(std::shared_ptr<ConfigValidator> validator) { validator_ = validator; }
    
    // Get validator
    std::shared_ptr<ConfigValidator> getValidator() const { return validator_; }
    
    // Apply defaults
    void applyDefaults();
    
    // Get configuration keys
    std::vector<std::string> getKeys() const;
    
    // Check if key exists
    bool has(const std::string& key) const;
    
    // Get configuration summary
    std::string summary() const;

    // Additional methods from original Config
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    void remove(const std::string& key);
    void clear();
    
    // Get the internal configuration map
    const std::map<std::string, ConfigValue>& getConfigMap() const { return config_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigValidator> validator_;
    std::map<std::string, ConfigValue> config_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON serialization/deserialization
    nlohmann::json toJson() const;
    bool fromJson(const nlohmann::json& json);
    
    // Merge configurations
    void merge(const std::map<std::string, ConfigValue>& other);
    
    // Helper for template methods
    template<typename T>
    std::optional<T> getImpl(const std::string& key) const;
};

} // namespace nlm

#endif // NLM_CONFIG_HPP
