#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace nlm {

/**
 * ConfigurationManager - Centralized configuration management
 * Provides type-safe access to configuration parameters with validation
 */
class ConfigurationManager {
public:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept;
    
    // Set configuration from a Config object
    void loadConfig(std::shared_ptr<Config> config) {
        config_ = std::move(config);
        NLM_LOG_INFO("Configuration loaded");
    }
    
    // Get configuration with type safety and defaults
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const {
        if (config_) {
            auto valueOpt = config_->get<T>(key);
            if (valueOpt) {
                return *valueOpt;
            }
        }
        return defaultValue;
    }
    
    // Specialized getter methods for common parameter types
    uint64_t getRandomSeed() const { return get<uint64_t>("random_seed", 42); }
    float getSTDP LTPWeight() const { return get<float>("stdp_ltp_weight", 0.01f); }
    float getSTDP LTDWeight() const { return get<float>("stdp_ltd_weight", 0.012f); }
    float getSTDP Tau() const { return get<float>("stdp_tau", 20.0f); }
    float getSynaptogenesisRate() const { return get<float>("synaptogenesis_rate", 0.0001f); }
    float getPruningRate() const { return get<float>("pruning_rate", 0.00001f); }
    size_t getNeuronCount() const { return get<size_t>("neuron_count", 1000); }
    size_t getRegionCount() const { return get<size_t>("region_count", 1); }
    float getConnectionProbability() const { return get<float>("connection_probability", 0.1f); }
    size_t getReplayInterval() const { return get<size_t>("replay_interval", 100); }
    size_t getConsolidationInterval() const { return get<size_t>("consolidation_interval", 1000); }
    TimestepDuration getTimestep() const { return get<double>("simulation_timestep", 0.001); }
    bool getEnableCheckpointing() const { return get<bool>("enable_checkpointing", true); }
    bool getEnableReplay() const { return get<bool>("enable_replay", true); }
    bool getEnableDevelopment() const { return get<bool>("enable_development", true); }
    
    // Configuration validation
    bool validate() const {
        if (!config_) return false;
        
        // Check required parameters
        if (!config_->hasKey("neuron_count")) return false;
        if (!config_->hasKey("region_count")) return false;
        if (!config_->hasKey("connection_probability")) return false;
        
        // Validate parameter ranges
        size_t neuronCount = get<size_t>("neuron_count");
        if (neuronCount == 0 || neuronCount > 1000000) return false;
        
        float connectionProb = get<float>("connection_probability");
        if (connectionProb < 0.0f || connectionProb > 1.0f) return false;
        
        return true;
    }
    
    // Configuration presets
    static ConfigurationManager createDefaultConfig() {
        ConfigurationManager manager;
        // Create and set default config
        auto config = std::make_shared<Config>();
        // Add default parameters
        manager.loadConfig(std::move(config));
        return manager;
    }
    
    static ConfigurationManager createHighPerformanceConfig() {
        ConfigurationManager manager;
        auto config = std::make_shared<Config>();
        // Add high-performance parameters
        config->set("neuron_count", size_t(5000));
        config->set("region_count", size_t(2));
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.002);
        manager.loadConfig(std::move(config));
        return manager;
    }
    
    static ConfigurationManager createHighPlasticityConfig() {
        ConfigurationManager manager;
        auto config = std::make_shared<Config>();
        // Add high-plasticity parameters
        config->set("stdp_ltp_weight", 0.05f);
        config->set("stdp_ltd_weight", 0.06f);
        config->set("synaptogenesis_rate", 0.001f);
        config->set("pruning_rate", 0.0001f);
        manager.loadConfig(std::move(config));
        return manager;
    }
    
    // Configuration backup/restore
    bool saveToFile(const std::string& filepath) const {
        if (!config_) return false;
        // Implementation would depend on Config class interface
        NLM_LOG_WARNING("saveToFile not implemented");
        return false;
    }
    
    bool loadFromFile(const std::string& filepath) {
        // Implementation would depend on Config class interface
        NLM_LOG_WARNING("loadFromFile not implemented");
        return false;
    }
    
private:
    std::shared_ptr<Config> config_;
};

} // namespace nlm