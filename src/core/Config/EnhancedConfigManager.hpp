/// Enhanced Configuration Manager for NLM
/// Provides robust configuration handling with validation and error reporting

#include "configs/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

class EnhancedConfigManager {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    static ValidationResult validateConfig(const Config& config) {
        ValidationResult result;
        
        // Validate required fields
        if (!config.hasKey("neuron_count")) {
            result.errors.push_back("Missing required field: neuron_count");
        } else {
            auto neuronCount = config.getValue<int64_t>("neuron_count");
            if (neuronCount <= 0) {
                result.errors.push_back("Invalid neuron_count: must be positive");
            }
        }
        
        if (!config.hasKey("region_count")) {
            result.warnings.push_back("Missing optional field: region_count");
        }
        
        if (!config.hasKey("connection_probability")) {
            result.warnings.push_back("Missing optional field: connection_probability");
        } else {
            auto prob = config.getValue<double>("connection_probability");
            if (prob < 0.0 || prob > 1.0) {
                result.errors.push_back("Invalid connection_probability: must be 0.0-1.0");
            }
        }
        
        // Validate float ranges for neuromodulation
        if (config.hasKey("dopamine_baseline")) {
            auto dopamine = config.getValue<double>("dopamine_baseline");
            if (dopamine < 0.0) {
                result.errors.push_back("Invalid dopamine_baseline: must be non-negative");
            }
        }
        
        // Validate simulation parameters
        if (config.hasKey("max_simulation_steps")) {
            auto steps = config.getValue<int64_t>("max_simulation_steps");
            if (steps <= 0) {
                result.errors.push_back("Invalid max_simulation_steps: must be positive");
            }
        }
        
        if (config.hasKey("simulation_timestep")) {
            auto timestep = config.getValue<double>("simulation_timestep");
            if (timestep <= 0.0) {
                result.errors.push_back("Invalid simulation_timestep: must be positive");
            }
        }
        
        return result;
    }
    
    static std::shared_ptr<Config> createValidatedConfig(const std::string& configName) {
        auto config = std::make_shared<Config>();
        
        // Load from file if exists
        std::string filename = configName + ".cfg";
        std::ifstream file(filename);
        if (file.good()) {
            config->loadFromFile(filename);
        } else {
            // Create default config
            config->set("neuron_count", 100, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.01, ConfigSource::Default);
            config->set("dopamine_baseline", 0.1, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("max_simulation_steps", 1000, ConfigSource::Default);
        }
        
        // Apply defaults for missing critical fields
        if (!config->hasKey("dopamine_baseline")) {
            config->set("dopamine_baseline", 0.1, ConfigSource::Default);
        }
        
        // Validate the config
        auto validation = validateConfig(*config);
        if (!validation.errors.empty()) {
            NLM_LOG_ERROR("Configuration validation failed for " + configName + ":");
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("  ERROR: " + error);
            }
        }
        
        for (const auto& warning : validation.warnings) {
            NLM_LOG_WARNING("Configuration warning for " + configName + ": " + warning);
        }
        
        return config;
    }
};

} // namespace nlm
