#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string message;
    
    ValidationResult(bool v = true, const std::string& msg = "") : valid(v), message(msg) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    std::function<ValidationResult(const ConfigValue&)> validator;
    bool required;
    
    ConfigSchemaEntry() : required(false) {}
    
    ConfigSchemaEntry(const std::string& k, const ConfigValue& defaultVal,
                     const std::string& desc, bool req = false)
        : key(k), defaultValue(defaultVal), description(desc), validator(nullptr), required(req) {}
    
    ConfigSchemaEntry(const std::string& k, const ConfigValue& defaultVal,
                     const std::string& desc,
                     std::function<ValidationResult(const ConfigValue&)> valid,
                     bool req = false)
        : key(k), defaultValue(defaultVal), description(desc), validator(valid), required(req) {}
};

// Configuration validation result
struct ValidationError {
    std::string key;
    std::string message;
    
    ValidationError(const std::string& k, const std::string& msg) : key(k), message(msg) {}
};

// Configuration summary with metadata
struct ConfigSummary {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    std::vector<std::string> overriddenKeys;
    
    ConfigSummary() : totalEntries(0), fileEntries(0), commandLineEntries(0), runtimeEntries(0) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Validate current configuration against schema
    std::vector<ValidationError> validate() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Get validation errors
    std::vector<ValidationError> getValidationErrors() const;
    
    // Get configuration summary with metadata
    ConfigSummary getSummary() const;
    
    // Get configuration summary as string (backward compatibility)
    std::string summary() const;
    
    // Apply schema (set defaults and add to schema)
    void applySchema(const std::vector<ConfigSchemaEntry>& schema);
    
    // Get schema entry by key
    std::optional<ConfigSchemaEntry> getSchemaEntry(const std::string& key) const;
    
    // Check if key is in schema
    bool hasSchema(const std::string& key) const;
    
    // Get default value for key
    template<typename T>
    T getDefaultOr(const std::string& key, const T& defaultValue) const;
    
    // Get schema keys
    std::vector<std::string> getSchemaKeys() const;
    
    // Add schema entry
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
    // Check if configuration was changed from default
    bool isModified(const std::string& key) const;
    
    // Get configuration source for key
    ConfigSource getSource(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal schema
    std::vector<ConfigSchemaEntry> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    void applyDefault(const ConfigSchemaEntry& entry);
    
    // Schema helpers
    std::optional<ConfigSchemaEntry> findSchemaEntry(const std::string& key) const;
    bool isDefaultSource(ConfigSource source) const;
};

} // namespace nlm

// Template function implementations
#include "Config.tpp"

// Common schemas
namespace nlm {
namespace schemas {
    // Default configuration schema
    const std::vector<ConfigSchemaEntry> defaultConfigSchema = {
        // Brain configuration
        ConfigSchemaEntry("brain.neuron_count", static_cast<int64_t>(1000),
                         "Total number of neurons", true),
        ConfigSchemaEntry("brain.synapse_density", 0.1f,
                         "Synapse connectivity density (0.0 to 1.0)", true),
        ConfigSchemaEntry("brain.connection_probability", 0.05f,
                         "Probability of connection between neurons (0.0 to 1.0)", true),
        ConfigSchemaEntry("brain.initial_weight_mean", 0.5f,
                         "Mean initial synaptic weight"),
        ConfigSchemaEntry("brain.initial_weight_std", 0.1f,
                         "Standard deviation of initial weights"),
        ConfigSchemaEntry("brain.v_thresh", -50.0f,
                         "Neuron firing threshold potential (mV)", true),
        ConfigSchemaEntry("brain.v_rest", -70.0f,
                         "Resting potential (mV)", true),
        ConfigSchemaEntry("brain.v_reset", -75.0f,
                         "Reset potential after spike (mV)", true),
        ConfigSchemaEntry("brain.tau_mem", 20.0f,
                         "Membrane time constant (ms)", true),
        ConfigSchemaEntry("brain.tau_ref", 2.0f,
                         "Refractory period (ms)", true),
        
        // Plasticity configuration
        ConfigSchemaEntry("plasticity.stdp.enable", true,
                         "Enable Spike-Timing-Dependent Plasticity", true),
        ConfigSchemaEntry("plasticity.stdp.learning_rate", 0.001f,
                         "STDP learning rate"),
        ConfigSchemaEntry("plasticity.stdp.tau_plus", 20.0f,
                         "STDP LTP time constant (ms)"),
        ConfigSchemaEntry("plasticity.stdp.tau_minus", 20.0f,
                         "STDP LTD time constant (ms)"),
        ConfigSchemaEntry("plasticity.hebbian.enable", true,
                         "Enable Hebbian learning", true),
        ConfigSchemaEntry("plasticity.structural.enable", true,
                         "Enable structural plasticity", true),
        
        // Neuromodulation configuration
        ConfigSchemaEntry("neuromod.dopamine.scale", 1.0f,
                         "Dopamine modulation scale"),
        ConfigSchemaEntry("neuromod.curiosity.enable", true,
                         "Enable curiosity-driven exploration", true),
        ConfigSchemaEntry("neuromod.novelty.enable", true,
                         "Enable novelty detection", true),
        
        // Simulation configuration
        ConfigSchemaEntry("simulation.timestep", 0.001,
                         "Simulation timestep (s)", true),
        ConfigSchemaEntry("random_seed", static_cast<int64_t>(42),
                         "Random seed for reproducibility", true),
        ConfigSchemaEntry("checkpoint_dir", std::string("./checkpoints"),
                         "Checkpoint directory"),
        ConfigSchemaEntry("replay_interval", static_cast<int64_t>(100),
                         "Replay interval in steps"),
        ConfigSchemaEntry("consolidation_interval", static_cast<int64_t>(1000),
                         "Memory consolidation interval in steps"),
    };
}

// Convenience functions for common configuration sets
namespace config {
    // Create development configuration
    std::shared_ptr<Config> createDevConfig();
    
    // Create production configuration
    std::shared_ptr<Config> createProductionConfig();
    
    // Create testing configuration
    std::shared_ptr<Config> createTestConfig();
    
    // Create minimal configuration
    std::shared_ptr<Config> createMinimalConfig();
    
    // Create small configuration for quick demos
    std::shared_ptr<Config> createDemoConfig();
}

} // namespace nlm

} // namespace nlm
