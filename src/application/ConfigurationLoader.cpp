#include "application/ConfigurationLoader.hpp"

namespace nlm {
    // ConfigurationLoader implementation
    std::shared_ptr<Config> ConfigurationLoader::load(const std::string& configFile, 
                                 const std::map<std::string, std::string>& customOptions) {
        auto config = std::make_shared<Config>();
        
        // Load from file
        if (config->loadFromFile(configFile)) {
            // std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            // std::cout << "Using default configuration." << std::endl;
        }
        
        // Apply default Phase 2 values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Apply custom options
        for (const auto& pair : customOptions) {
            config->set(pair.first, pair.second, ConfigSource::CommandLine);
        }
        
        // Validate configuration
        validateConfig(config);
        
        return config;
    }
    
    void ConfigurationLoader::validateConfig(const std::shared_ptr<Config>& config) {
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
        }
        
        if (!config->has("simulation_timestep") || config->getOr<double>("simulation_timestep", 0.001) <= 0.0) {
            config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        }
        
        if (!config->has("neuron_count") || config->getOr<int64_t>("neuron_count", 500) <= 0) {
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        }
        
        if (!config->has("region_count") || config->getOr<int64_t>("region_count", 1) <= 0) {
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        }
        
        if (!config->has("connection_probability") || 
            config->getOr<float>("connection_probability", 0.15f) < 0.0f || 
            config->getOr<float>("connection_probability", 0.15f) > 1.0f) {
            config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        }
    }
}