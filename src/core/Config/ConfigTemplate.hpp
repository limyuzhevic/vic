// NLM Configuration Template System
// Template-based configuration management with validation and inheritance

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration template types
enum class ConfigTemplateType {
    Neuron,
    Synapse,
    Region,
    Brain,
    Simulation,
    Plasticity,
    Memory,
    Neuromodulation,
    Custom
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        if (!errors.empty()) {
            oss << "Validation Errors:\n";
            for (const auto& error : errors) {
                oss << "  - " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            if (!errors.empty()) oss << "\n";
            oss << "Validation Warnings:\n";
            for (const auto& warning : warnings) {
                oss << "  - " << warning << "\n";
            }
        }
        return oss.str();
    }
};

// Configuration template base class
class ConfigTemplate {
public:
    virtual ~ConfigTemplate() = default;
    
    virtual ConfigTemplateType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Apply template to configuration
    virtual bool applyToConfig(std::shared_ptr<Config> config) const = 0;
    
    // Validate configuration against template
    virtual ValidationResult validate(const std::shared_ptr<Config>& config) const = 0;
    
    // Get default values
    virtual std::map<std::string, ConfigValue> getDefaults() const = 0;
    
    // Get validation rules
    virtual std::vector<std::string> getValidationRules() const = 0;
    
    // Check if template can be applied
    virtual bool canApply(const std::shared_ptr<Config>& config) const = 0;
};

// Neuron configuration template
class NeuronConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Neuron; }
    std::string getName() const override { return "Neuron Configuration"; }
    std::string getDescription() const override {
        return "Template for configuring neuron parameters and behavior";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Set default neuron parameters
        config->set("neuron_resting_potential", -70.0, ConfigSource::Default);
        config->set("neuron_threshold", -55.0, ConfigSource::Default);
        config->set("neuron_refractory_period", 5, ConfigSource::Default);
        config->set("neuron_leak_conductance", 10.0, ConfigSource::Default);
        config->set("neuron_reset_potential", -70.0, ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Validate neuron parameters
        auto potential = config->getOr<double>("neuron_resting_potential", -70.0);
        if (potential < -100.0 || potential > -40.0) {
            result.addError("Invalid resting potential: " + std::to_string(potential) + ". Must be between -100mV and -40mV.");
        }
        
        auto threshold = config->getOr<double>("neuron_threshold", -55.0);
        if (threshold < potential) {
            result.addError("Threshold (-" + std::to_string(threshold) + "mV) must be greater than resting potential (-" + std::to_string(potential) + "mV)");
        }
        
        if (threshold > 50.0) {
            result.addError("Threshold too high: " + std::to_string(threshold) + ". Must be less than 50mV.");
        }
        
        auto refractory = config->getOr<int64_t>("neuron_refractory_period", 5);
        if (refractory < 1) {
            result.addError("Refractory period must be positive: " + std::to_string(refractory));
        }
        
        auto leak = config->getOr<double>("neuron_leak_conductance", 10.0);
        if (leak < 0.0) {
            result.addError("Leak conductance cannot be negative: " + std::to_string(leak));
        }
        
        // Warning for extreme values
        if (potential < -85.0) {
            result.addWarning("Very negative resting potential (-" + std::to_string(potential) + "mV) may cause instability");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"neuron_resting_potential", -70.0},
            {"neuron_threshold", -55.0},
            {"neuron_refractory_period", 5},
            {"neuron_leak_conductance", 10.0},
            {"neuron_reset_potential", -70.0}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "neuron_resting_potential: range [-100, -40] mV",
            "neuron_threshold: > resting potential, < 50 mV",
            "neuron_refractory_period: positive integer",
            "neuron_leak_conductance: non-negative"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return !config->has("brain_simulation_mode") || 
               config->getOr<bool>("brain_simulation_mode", false);
    }
};

// Synapse configuration template
class SynapseConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Synapse; }
    std::string getName() const override { return "Synapse Configuration"; }
    std::string getDescription() const override {
        return "Template for configuring synaptic connections and plasticity";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Set default synapse parameters
        config->set("synapse_delay", 1, ConfigSource::Default);
        config->set("synapse_efficacy", 1.0, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_time_constant", 20.0f, ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Validate synapse parameters
        auto delay = config->getOr<int64_t>("synapse_delay", 1);
        if (delay < 0) {
            result.addError("Synapse delay cannot be negative: " + std::to_string(delay));
        }
        
        auto efficacy = config->getOr<double>("synapse_efficacy", 1.0);
        if (efficacy < 0.0 || efficacy > 2.0) {
            result.addError("Synapse efficacy out of range [0, 2]: " + std::to_string(efficacy));
        }
        
        // Validate STDP parameters
        auto ltp = config->getOr<float>("stdp_ltp_weight", 0.02f);
        auto ltd = config->getOr<float>("stdp_ltd_weight", 0.015f);
        
        if (ltp < 0.0 || ltp > 0.1) {
            result.addError("LTP weight out of range [0, 0.1]: " + std::to_string(ltp));
        }
        
        if (ltd < 0.0 || ltd > 0.1) {
            result.addError("LTD weight out of range [0, 0.1]: " + std::to_string(ltd));
        }
        
        auto tau = config->getOr<float>("stdp_time_constant", 20.0f);
        if (tau < 1.0 || tau > 100.0) {
            result.addError("STDP time constant out of range [1, 100]: " + std::to_string(tau));
        }
        
        if (ltp <= ltd) {
            result.addError("LTP weight must be greater than LTD weight for stable learning");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"synapse_delay", 1},
            {"synapse_efficacy", 1.0},
            {"stdp_ltp_weight", 0.02f},
            {"stdp_ltd_weight", 0.015f},
            {"stdp_time_constant", 20.0f}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "synapse_delay: non-negative integer",
            "synapse_efficacy: range [0, 2]",
            "stdp_ltp_weight: range [0, 0.1]",
            "stdp_ltd_weight: range [0, 0.1]",
            "stdp_time_constant: range [1, 100]",
            "stdp_ltp_weight > stdp_ltd_weight"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return !config->has("brain_simulation_mode") || 
               config->getOr<bool>("brain_simulation_mode", false);
    }
};

// Region configuration template
class RegionConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Region; }
    std::string getName() const override { return "Region Configuration"; }
    std::string getDescription() const override {
        return "Template for configuring neural regions and connectivity";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Set default region parameters
        config->set("region_connection_probability", 0.15f, ConfigSource::Default);
        config->set("region_max_neurons", 100, ConfigSource::Default);
        config->set("region_excitatory_ratio", 0.8f, ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Validate region parameters
        auto connProb = config->getOr<float>("region_connection_probability", 0.15f);
        if (connProb < 0.0 || connProb > 1.0) {
            result.addError("Connection probability out of range [0, 1]: " + std::to_string(connProb));
        }
        
        auto maxNeurons = config->getOr<int64_t>("region_max_neurons", 100);
        if (maxNeurons < 1) {
            result.addError("Max neurons must be positive: " + std::to_string(maxNeurons));
        }
        
        auto excRatio = config->getOr<float>("region_excitatory_ratio", 0.8f);
        if (excRatio < 0.0 || excRatio > 1.0) {
            result.addError("Excitatory ratio out of range [0, 1]: " + std::to_string(excRatio));
        }
        
        // Check consistency
        if (connProb > 1.0 / maxNeurons) {
            result.addWarning("Connection probability too high for specified neuron count - may cause saturation");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"region_connection_probability", 0.15f},
            {"region_max_neurons", 100},
            {"region_excitatory_ratio", 0.8f}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "region_connection_probability: range [0, 1]",
            "region_max_neurons: positive integer",
            "region_excitatory_ratio: range [0, 1]"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return config->has("brain_simulation_mode") && 
               config->getOr<bool>("brain_simulation_mode", false);
    }
};

// Phase 5/6 Enhanced Configuration Templates

// Phase 5: Performance Optimization Template
class PerformanceConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Custom; }
    std::string getName() const override { return "Performance Optimization"; }
    std::string getDescription() const override {
        return "Template for optimizing neural simulation performance";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Enable performance optimizations
        config->set("neuron_use_simd", true, ConfigSource::Default);
        config->set("synapse_use_sparse_matrix", true, ConfigSource::Default);
        config->set("brain_use_memory_pool", true, ConfigSource::Default);
        config->set("brain_enable_vectorization", true, ConfigSource::Default);
        config->set("brain_parallel_processing_enabled", true, ConfigSource::Default);
        config->set("brain_thread_pool_size", 4, ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Validate performance parameters
        auto threadPoolSize = config->getOr<int64_t>("brain_thread_pool_size", 4);
        if (threadPoolSize < 1) {
            result.addError("Thread pool size must be positive: " + std::to_string(threadPoolSize));
        } else if (threadPoolSize > 32) {
            result.addWarning("Large thread pool size (" + std::to_string(threadPoolSize) + ") may cause overhead");
        }
        
        // Check if optimizations are enabled
        if (!config->getOr<bool>("neuron_use_simd", true)) {
            result.addWarning("SIMD optimizations are recommended for better performance");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"neuron_use_simd", true},
            {"synapse_use_sparse_matrix", true},
            {"brain_use_memory_pool", true},
            {"brain_enable_vectorization", true},
            {"brain_parallel_processing_enabled", true},
            {"brain_thread_pool_size", 4}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "brain_thread_pool_size: positive integer (recommended < 32)",
            "neuron_use_simd: enables SIMD optimizations",
            "synapse_use_sparse_matrix: uses sparse matrix representation",
            "brain_use_memory_pool: enables memory pool allocation"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return !config->has("simulation_mode") || 
               config->getOr<std::string>("simulation_mode", "") != "ultra_light";
    }
};

// Phase 6: Neuromodulation Integration Template
class NeuromodulationConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Custom; }
    std::string getName() const override { return "Neuromodulation Integration"; }
    std::string getDescription() const override {
        return "Template for comprehensive neuromodulation support";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Enable neuromodulation systems
        config->set("neuromodulation_dopamine_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_curiosity_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_novelty_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_reward_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_prediction_error_enabled", true, ConfigSource::Default);
        config->set("neuromodulation_integration_depth", "deep", ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Validate neuromodulation parameters
        auto integrationDepth = config->getOr<std::string>("neuromodulation_integration_depth", "deep");
        if (integrationDepth != "shallow" && integrationDepth != "deep" && integrationDepth != "comprehensive") {
            result.addError("Invalid integration depth: " + integrationDepth + ". Must be: shallow, deep, or comprehensive");
        }
        
        // Check consistency
        if (integrationDepth == "comprehensive" && !config->getOr<bool>("neuromodulation_dopamine_enabled", true)) {
            result.addWarning("Comprehensive integration recommends dopamine neuromodulation");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"neuromodulation_dopamine_enabled", true},
            {"neuromodulation_curiosity_enabled", true},
            {"neuromodulation_novelty_enabled", true},
            {"neuromodulation_reward_enabled", true},
            {"neuromodulation_prediction_error_enabled", true},
            {"neuromodulation_integration_depth", "deep"}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "neuromodulation_integration_depth: shallow, deep, or comprehensive",
            "neuromodulation_dopamine_enabled: enables dopamine system",
            "neuromodulation_curiosity_enabled: enables curiosity system",
            "neuromodulation_novelty_enabled: enables novelty detection"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return config->has("brain_simulation_mode") && 
               (config->getOr<std::string>("brain_simulation_mode", "") == "full" ||
                config->getOr<std::string>("brain_simulation_mode", "") == "advanced");
    }
};

// Phase 6: Memory System Integration Template
class MemoryConfigTemplate : public ConfigTemplate {
public:
    ConfigTemplateType getType() const override { return ConfigTemplateType::Custom; }
    std::string getName() const override { return "Memory System Integration"; }
    std::string getDescription() const override {
        return "Template for comprehensive memory system integration";
    }
    
    bool applyToConfig(std::shared_ptr<Config> config) const override {
        // Enable memory systems
        config->set("memory_working_memory_enabled", true, ConfigSource::Default);
        config->set("memory_episodic_memory_enabled", true, ConfigSource::Default);
        config->set("memory_associative_memory_enabled", true, ConfigSource::Default);
        config->set("memory_procedural_memory_enabled", true, ConfigSource::Default);
        config->set("memory_replay_enabled", true, ConfigSource::Default);
        config->set("memory_consolidation_enabled", true, ConfigSource::Default);
        return true;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const override {
        ValidationResult result;
        
        // Check memory system consistency
        auto replayEnabled = config->getOr<bool>("memory_replay_enabled", true);
        auto consolidationEnabled = config->getOr<bool>("memory_consolidation_enabled", true);
        
        if (replayEnabled && !consolidationEnabled) {
            result.addWarning("Replay system enabled without consolidation may reduce memory effectiveness");
        }
        
        // Validate memory capacity settings
        auto workingMemoryCapacity = config->getOr<int64_t>("memory_working_memory_capacity", 1000);
        if (workingMemoryCapacity < 10) {
            result.addError("Working memory capacity too small: " + std::to_string(workingMemoryCapacity) + ". Minimum recommended: 10");
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValue> getDefaults() const override {
        return {
            {"memory_working_memory_enabled", true},
            {"memory_episodic_memory_enabled", true},
            {"memory_associative_memory_enabled", true},
            {"memory_procedural_memory_enabled", true},
            {"memory_replay_enabled", true},
            {"memory_consolidation_enabled", true},
            {"memory_working_memory_capacity", 1000}
        };
    }
    
    std::vector<std::string> getValidationRules() const override {
        return {
            "memory_working_memory_capacity: positive integer, minimum 10",
            "memory_working_memory_enabled: enables working memory",
            "memory_episodic_memory_enabled: enables episodic memory",
            "memory_replay_enabled: enables memory replay mechanism"
        };
    }
    
    bool canApply(const std::shared_ptr<Config>& config) const override {
        return config->has("brain_simulation_mode") && 
               config->getOr<std::string>("brain_simulation_mode", "") == "full";
    }
};

// Configuration template manager
class ConfigTemplateManager {
public:
    static ConfigTemplateManager& getInstance() {
        static ConfigTemplateManager instance;
        return instance;
    }
    
    // Register a configuration template
    void registerTemplate(std::unique_ptr<ConfigTemplate> template);
    
    // Get a template by type
    std::shared_ptr<ConfigTemplate> getTemplate(ConfigTemplateType type) const;
    
    // Get all registered templates
    std::vector<std::shared_ptr<ConfigTemplate>> getAllTemplates() const;
    
    // Apply multiple templates to configuration
    ValidationResult applyTemplates(std::shared_ptr<Config> config, 
                                   const std::vector<ConfigTemplateType>& templateTypes) const;
    
    // Validate configuration against all applicable templates
    ValidationResult validateAgainstTemplates(const std::shared_ptr<Config>& config) const;
    
    // Create configuration from template
    std::shared_ptr<Config> createFromTemplate(const std::vector<ConfigTemplateType>& templateTypes) const;
    
private:
    ConfigTemplateManager() = default;
    ~ConfigTemplateManager() = default;
    ConfigTemplateManager(const ConfigTemplateManager&) = delete;
    ConfigTemplateManager& operator=(const ConfigTemplateManager&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Template registry functions
void registerDefaultTemplates();

// Convenience functions for template usage
ValidationResult validateConfig(const std::shared_ptr<Config>& config);

std::shared_ptr<Config> createConfigFromTemplates(const std::vector<ConfigTemplateType>& templateTypes);

ValidationResult applyConfigTemplates(std::shared_ptr<Config> config,
                                     const std::vector<ConfigTemplateType>& templateTypes);

std::string getTemplateConfigSummary(const std::shared_ptr<Config>& config);

} // namespace nlm