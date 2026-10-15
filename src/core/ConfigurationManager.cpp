// ConfigurationManager.cpp
#include "core/ConfigurationManager.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <vector>
#include <map>

namespace nlm {

ConfigurationManager::ConfigurationManager() 
    : config_(std::make_unique<Config>()), hasFileConfig_(false) {
    loadDefaults();
}

bool ConfigurationManager::loadConfiguration(const std::string& configFile,
                                            const std::unordered_map<std::string, std::string>& cliOverrides) {
    // Clear previous validation errors
    validationErrors_.clear();
    
    // Load configuration file
    hasFileConfig_ = config_->loadFromFile(configFile);
    configFilePath_ = configFile;
    
    if (hasFileConfig_) {
        NLM_LOG_INFO("Loaded configuration from file: " + configFile);
    }
    
    // Apply command line overrides
    for (const auto& override : cliOverrides) {
        config_->set(override.first, override.second, ConfigSource::CommandLine);
    }
    
    // Validate configuration
    if (!validateConfiguration()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : validationErrors_) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    return true;
}

bool ConfigurationManager::validateConfiguration() const {
    validationErrors_.clear();
    
    // Validate required fields
    if (!config_->has("random_seed")) {
        validationErrors_.push_back("Missing required configuration: random_seed");
    }
    
    if (!config_->has("simulation_timestep")) {
        validationErrors_.push_back("Missing required configuration: simulation_timestep");
    } else {
        double timestep = config_->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            validationErrors_.push_back("Invalid simulation_timestep: must be between 0 and 1.0");
        }
    }
    
    if (!config_->has("neuron_count")) {
        validationErrors_.push_back("Missing required configuration: neuron_count");
    } else {
        int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            validationErrors_.push_back("Invalid neuron_count: must be between 1 and 1,000,000");
        }
    }
    
    // Validate experiment-specific configuration
    validateExperimentConfiguration();
    
    // Validate GPU configuration if enabled
    if (config_->getOr<bool>("gpu_enabled", false)) {
        validateGPUConfiguration();
    }
    
    // Validate memory configuration
    validateMemoryConfiguration();
    
    // Validate advanced configuration options
    validateAdvancedConfiguration();
    
    return validationErrors_.empty();
}

template<typename T>
T ConfigurationManager::getConfigValue(const std::string& key, const T& defaultValue) const {
    return config_->getOr<T>(key, defaultValue);
}

template<typename T>
std::optional<T> ConfigurationManager::getConfigValue(const std::string& key) const {
    return config_->get<T>(key);
}

template<typename T>
void ConfigurationManager::setConfigValue(const std::string& key, const T& value,
                                         ConfigSource source) {
    config_->set(key, value, source);
}

ConfigSource ConfigurationManager::getConfigSource(const std::string& key) const {
    // This would require access to config entries - Config class doesn't expose this directly
    // For now, return Runtime as a placeholder
    return ConfigSource::Runtime;
}

std::unordered_map<std::string, std::pair<ConfigValue, ConfigSource>> ConfigurationManager::getConfigSummary() const {
    std::unordered_map<std::string, std::pair<ConfigValue, ConfigSource>> summary;
    
    for (const auto& key : config_->getKeys()) {
        ConfigValue value;
        ConfigSource source = ConfigSource::Runtime;
        
        if (auto intVal = config_->get<int>(key)) {
            value = *intVal;
        } else if (auto int64Val = config_->get<int64_t>(key)) {
            value = *int64Val;
        } else if (auto doubleVal = config_->get<double>(key)) {
            value = *doubleVal;
        } else if (auto boolVal = config_->get<bool>(key)) {
            value = *boolVal;
        } else if (auto stringVal = config_->get<std::string>(key)) {
            value = *stringVal;
        }
        
        summary[key] = std::make_pair(value, source);
    }
    
    return summary;
}

void ConfigurationManager::clear() {
    config_->clear();
    hasFileConfig_ = false;
    validationErrors_.clear();
    loadDefaults();
}

void ConfigurationManager::loadDefaults() {
    setDefaultValues();
}

void ConfigurationManager::loadPhase2Defaults() {
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void ConfigurationManager::loadPhase3Defaults() {
    config_->set("environment_width", 20, ConfigSource::Default);
    config_->set("environment_height", 20, ConfigSource::Default);
    config_->set("vision_width", 16, ConfigSource::Default);
    config_->set("vision_height", 16, ConfigSource::Default);
    config_->set("agent_energy", 100.0f, ConfigSource::Default);
    config_->set("agent_speed", 5.0f, ConfigSource::Default);
}

void ConfigurationManager::loadPhase4Defaults() {
    // Memory system defaults
    config_->set("working_memory_capacity", 100, ConfigSource::Default);
    config_->set("working_memory_decay", 0.01, ConfigSource::Default);
    config_->set("episodic_memory_capacity", 1000, ConfigSource::Default);
    config_->set("episodic_memory_strength", 0.1f, ConfigSource::Default);
    
    // Prediction system defaults
    config_->set("prediction_horizon", 10, ConfigSource::Default);
    config_->set("prediction_accuracy_threshold", 0.8f, ConfigSource::Default);
    
    // Development defaults
    config_->set("developmental_critical_period", 1000, ConfigSource::Default);
    config_->set("developmental_maturation_rate", 0.01, ConfigSource::Default);
}

void ConfigurationManager::loadPhase5Defaults() {
    // Multithreading defaults
    config_->set("enable_multithreading", true, ConfigSource::Default);
    config_->set("thread_count", 0, ConfigSource::Default); // Auto-detect
    config_->set("thread_pool_size", 4, ConfigSource::Default);
    
    // SIMD defaults
    config_->set("enable_simd", true, ConfigSource::Default);
    
    // Scaling defaults
    config_->set("enable_sparse_connectivity", true, ConfigSource::Default);
    config_->set("enable_event_driven", true, ConfigSource::Default);
    
    // Performance defaults
    config_->set("checkpoint_interval", 10000, ConfigSource::Default);
    config_->set("max_simulation_steps", 300000, ConfigSource::Default);
}

void ConfigurationManager::loadAdvancedDefaults() {
    // Performance optimization profiles
    config_->set("performance.profile", "balanced", ConfigSource::Default);
    config_->set("performance.optimization_level", 2, ConfigSource::Default);
    
    // Learning optimization
    config_->set("learning.plasticity.adaptive_rules", true, ConfigSource::Default);
    config_->set("learning.neuromodulator.dopamine_scale", 1.0, ConfigSource::Default);
    config_->set("learning.curiosity.weight", 0.5, ConfigSource::Default);
    config_->set("learning.novelty.sensitivity", 1.0, ConfigSource::Default);
    
    // Memory management
    config_->set("memory.pools.enable", true, ConfigSource::Default);
    config_->set("memory.consolidation.enabled", true, ConfigSource::Default);
    config_->set("memory.compression.enabled", false, ConfigSource::Default);
    
    // Development dynamics
    config_->set("development.stages.enabled", true, ConfigSource::Default);
    config_->set("development.plasticity_rates.dynamic", true, ConfigSource::Default);
    
    // Prediction systems
    config_->set("prediction.horizon.max", 100, ConfigSource::Default);
    config_->set("prediction.confidence.weight", 0.7, ConfigSource::Default);
    config_->set("prediction.error.scaling.dynamic", true, ConfigSource::Default);
    
    // Neuromodulation
    config_->set("neuromodulation.dopamine.baseline", 0.1, ConfigSource::Default);
    config_->set("neuromodulation.dopamine.exploration_weight", 0.5, ConfigSource::Default);
    config_->set("neuromodulation.dopamine.exploitation_weight", 0.5, ConfigSource::Default);
}

bool ConfigurationManager::validateGPUConfiguration() const {
    std::string gpuVendor = config_->getOr<std::string>("gpu_vendor", "unknown");
    if (gpuVendor == "unknown") {
        validationErrors_.push_back("GPU enabled but vendor not specified");
    }
    return true;
}

bool ConfigurationManager::validateExperimentConfiguration() const {
    std::string experimentName = config_->getOr<std::string>("experiment_name", "");
    if (experimentName.empty()) {
        validationErrors_.push_back("No experiment specified, using default");
    }
    return true;
}

bool ConfigurationManager::validateAdvancedConfiguration() const {
    // Validate performance profile
    std::string profile = config_->getOr<std::string>("performance.profile", "balanced");
    std::vector<std::string> validProfiles = {"balanced", "speed", "memory", "accuracy", "adaptive"};
    if (std::find(validProfiles.begin(), validProfiles.end(), profile) == validProfiles.end()) {
        validationErrors_.push_back("Invalid performance profile: " + profile);
    }
    
    // Validate memory settings
    int64_t workingMemory = config_->getOr<int64_t>("memory.working_memory.size", 100);
    if (workingMemory <= 0 || workingMemory > 10000) {
        validationErrors_.push_back("Invalid working_memory.size: must be between 1 and 10000");
    }
    
    int64_t episodicMemory = config_->getOr<int64_t>("memory.episodic_memory.capacity", 1000);
    if (episodicMemory <= 0 || episodicMemory > 100000) {
        validationErrors_.push_back("Invalid episodic_memory.capacity: must be between 1 and 100000");
    }
    
    // Validate learning parameters
    double curiosityWeight = config_->getOr<double>("learning.curiosity.weight", 0.5);
    if (curiosityWeight < 0.0 || curiosityWeight > 1.0) {
        validationErrors_.push_back("Invalid learning.curiosity.weight: must be between 0.0 and 1.0");
    }
    
    double noveltySensitivity = config_->getOr<double>("learning.novelty.sensitivity", 1.0);
    if (noveltySensitivity < 0.0) {
        validationErrors_.push_back("Invalid learning.novelty.sensitivity: must be >= 0.0");
    }
    
    // Validate development parameters
    double maturationRate = config_->getOr<double>("development.maturation.rate", 0.01);
    if (maturationRate < 0.0 || maturationRate > 1.0) {
        validationErrors_.push_back("Invalid development.maturation.rate: must be between 0.0 and 1.0");
    }
    
    // Validate prediction parameters
    int64_t predictionHorizon = config_->getOr<int64_t>("prediction.horizon", 10);
    if (predictionHorizon <= 0 || predictionHorizon > 1000) {
        validationErrors_.push_back("Invalid prediction.horizon: must be between 1 and 1000");
    }
    
    double confidenceThreshold = config_->getOr<double>("prediction.confidence_threshold", 0.8);
    if (confidenceThreshold < 0.0 || confidenceThreshold > 1.0) {
        validationErrors_.push_back("Invalid prediction.confidence_threshold: must be between 0.0 and 1.0");
    }
    
    return true;
}

void ConfigurationManager::setDefaultValues() {
    // Core configuration defaults
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters (Phase 2)
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Phase 2 specific defaults
    loadPhase2Defaults();
    
    // Logging defaults
    config_->set("log_level", "INFO", ConfigSource::Default);
    config_->set("log_to_file", false, ConfigSource::Default);
    config_->set("log_filename", "nlm.log", ConfigSource::Default);
    
    // Performance defaults
    config_->set("max_simulation_steps", 10000, ConfigSource::Default);
    config_->set("simulation_time_limit", 0.0, ConfigSource::Default);
    
    // Output defaults
    config_->set("visualization_enabled", false, ConfigSource::Default);
    config_->set("visualization_update_rate", 30.0, ConfigSource::Default);
    
    // Experiment defaults
    config_->set("experiment_name", "learning", ConfigSource::Default);
    config_->set("run_all_experiments", false, ConfigSource::Default);
    
    // Advanced features defaults
    config_->set("gpu_enabled", false, ConfigSource::Default);
    config_->set("profiling_enabled", false, ConfigSource::Default);
    config_->set("memory_tracking_enabled", false, ConfigSource::Default);
    config_->set("output_format", "txt", ConfigSource::Default);
    config_->set("profiling_granularity", 1000, ConfigSource::Default);
    config_->set("profile_output", "profile.txt", ConfigSource::Default);
    
    // Advanced configuration defaults
    loadAdvancedDefaults();
}

} // namespace nlm
