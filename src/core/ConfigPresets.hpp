#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace nlm {

// Configuration presets and helper functions
namespace config {
    
    // Enhanced configuration presets
    struct Preset {
        std::string name;
        std::string description;
        std::map<std::string, std::string> values;
        
        Preset(const std::string& name, const std::string& description, 
               const std::map<std::string, std::string>& values)
            : name(name), description(description), values(values) {}
    };
    
    // Predefined presets
    static const std::vector<Preset> PRESETS = {
        Preset("basic", 
               "Basic configuration for simple neural simulations",
               {
                   {"neuron_count", "100"},
                   {"region_count", "1"},
                   {"simulation_timestep", "0.001"},
                   {"random_seed", "42"},
                   {"connection_probability", "0.15"},
                   {"stdp_ltp_weight", "0.02"},
                   {"stdp_ltd_weight", "0.015"},
                   {"stdp_tau", "20.0"},
                   {"synaptogenesis_rate", "0.0001"},
                   {"pruning_rate", "0.00001"}
               }),
        
        Preset("learning", 
               "Enhanced configuration for learning-focused simulations",
               {
                   {"neuron_count", "500"},
                   {"region_count", "2"},
                   {"simulation_timestep", "0.001"},
                   {"random_seed", "42"},
                   {"connection_probability", "0.2"},
                   {"stdp_ltp_weight", "0.03"},
                   {"stdp_ltd_weight", "0.02"},
                   {"stdp_tau", "25.0"},
                   {"synaptogenesis_rate", "0.0002"},
                   {"pruning_rate", "0.00002"},
                   {"development_enabled", "true"},
                   {"curiosity_enabled", "true"},
                   {"novelty_enabled", "true"}
               }),
        
        Preset("fast", 
               "Fast simulation configuration for rapid testing",
               {
                   {"neuron_count", "50"},
                   {"region_count", "1"},
                   {"simulation_timestep", "0.002"},
                   {"random_seed", "42"},
                   {"connection_probability", "0.1"},
                   {"stdp_ltp_weight", "0.01"},
                   {"stdp_ltd_weight", "0.008"},
                   {"stdp_tau", "10.0"},
                   {"synaptogenesis_rate", "0.00005"},
                   {"pruning_rate", "0.000005"},
                   {"structural_plasticity_enabled", "false"},
                   {"development_enabled", "false"}
               }),
        
        Preset("research", 
               "Research-grade configuration with maximum detail",
               {
                   {"neuron_count", "2000"},
                   {"region_count", "4"},
                   {"simulation_timestep", "0.0005"},
                   {"random_seed", "42"},
                   {"connection_probability", "0.3"},
                   {"stdp_ltp_weight", "0.05"},
                   {"stdp_ltd_weight", "0.04"},
                   {"stdp_tau", "50.0"},
                   {"synaptogenesis_rate", "0.001"},
                   {"pruning_rate", "0.0001"},
                   {"development_enabled", "true"},
                   {"curiosity_enabled", "true"},
                   {"novelty_enabled", "true"},
                   {"prediction_enabled", "true"},
                   {"working_memory_size", "100"},
                   {"episodic_memory_size", "500"}
               }),
        
        Preset("demo", 
               "Demo configuration optimized for demonstration purposes",
               {
                   {"neuron_count", "20"},
                   {"region_count", "1"},
                   {"simulation_timestep", "0.01"},
                   {"random_seed", "42"},
                   {"connection_probability", "0.05"},
                   {"stdp_ltp_weight", "0.015"},
                   {"stdp_ltd_weight", "0.01"},
                   {"stdp_tau", "15.0"},
                   {"synaptogenesis_rate", "0.0001"},
                   {"pruning_rate", "0.00001"},
                   {"development_enabled", "false"},
                   {"curiosity_enabled", "false"},
                   {"novelty_enabled", "false"},
                   {"prediction_enabled", "false"},
                   {"visualization_enabled", "true"}
               })
    };
    
    // Get all available presets
    std::vector<std::string> getPresetNames() {
        std::vector<std::string> names;
        names.reserve(PRESETS.size());
        for (const auto& preset : PRESETS) {
            names.push_back(preset.name);
        }
        return names;
    }
    
    // Get preset description
    std::string getPresetDescription(const std::string& name) {
        for (const auto& preset : PRESETS) {
            if (preset.name == name) {
                return preset.description;
            }
        }
        return "Preset not found";
    }
    
    // Get preset values
    std::map<std::string, std::string> getPresetValues(const std::string& name) {
        for (const auto& preset : PRESETS) {
            if (preset.name == name) {
                return preset.values;
            }
        }
        return std::map<std::string, std::string>();
    }
    
    // Create configuration from preset
    std::shared_ptr<Config> createFromPreset(const std::string& name) {
        auto config = std::make_shared<Config>();
        
        // Get preset values
        auto presetValues = getPresetValues(name);
        
        // Apply values to configuration
        for (const auto& pair : presetValues) {
            // Convert string values to appropriate types based on key
            std::string key = pair.first;
            std::string strValue = pair.second;
            
            // Infer type from key name and value
            if (key.find("neuron") != std::string::npos || 
                key.find("region") != std::string::npos ||
                key.find("step") != std::string::npos ||
                key.find("count") != std::string::npos) {
                try {
                    int64_t intValue = std::stoll(strValue);
                    config->setInt(key, intValue);
                } catch (...) {
                    config->setString(key, strValue);
                }
            } else if (key.find("rate") != std::string::npos || 
                      key.find("weight") != std::string::npos ||
                      key.find("tau") != std::string::npos ||
                      key.find("time") != std::string::npos) {
                try {
                    float floatValue = std::stof(strValue);
                    config->setFloat(key, floatValue);
                } catch (...) {
                    config->setString(key, strValue);
                }
            } else if (key.find("enabled") != std::string::npos ||
                      key.find("active") != std::string::npos) {
                // Convert boolean strings
                std::string lowerValue = toLower(strValue);
                bool boolValue = (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1");
                config->setBool(key, boolValue);
            } else {
                // Default to string
                config->setString(key, strValue);
            }
        }
        
        return config;
    }
    
    // Validate preset name
    bool isValidPreset(const std::string& name) {
        for (const auto& preset : PRESETS) {
            if (preset.name == name) {
                return true;
            }
        }
        return false;
    }
    
    // Get preset with error handling
    std::shared_ptr<Config> getPreset(const std::string& name, bool throwOnError = true) {
        if (!isValidPreset(name)) {
            if (throwOnError) {
                throw std::runtime_error("Unknown preset: " + name);
            }
            return nullptr;
        }
        
        return createFromPreset(name);
    }
}

// Utility functions for common configuration tasks
namespace config_utils {
    
    // Create a simulation configuration with common settings
    std::shared_ptr<Config> createSimulationConfig(int neuronCount = 100,
                                                  int regionCount = 1,
                                                  float timestep = 0.001,
                                                  int seed = 42) {
        auto config = std::make_shared<Config>();
        
        config->setInt("neuron_count", neuronCount);
        config->setInt("region_count", regionCount);
        config->setFloat("simulation_timestep", timestep);
        config->setInt("random_seed", seed);
        config->setFloat("connection_probability", 0.15f);
        
        // Set default plasticity parameters
        config->setFloat("stdp_ltp_weight", 0.02f);
        config->setFloat("stdp_ltd_weight", 0.015f);
        config->setFloat("stdp_tau", 20.0f);
        config->setFloat("synaptogenesis_rate", 0.0001f);
        config->setFloat("pruning_rate", 0.00001f);
        
        return config;
    }
    
    // Create a learning-focused configuration
    std::shared_ptr<Config> createLearningConfig(int neuronCount = 500,
                                                int regionCount = 2) {
        auto config = createSimulationConfig(neuronCount, regionCount);
        
        // Enhance for learning
        config->setInt("neuron_count", neuronCount);
        config->setInt("region_count", regionCount);
        config->setFloat("stdp_ltp_weight", 0.03f);
        config->setFloat("stdp_ltd_weight", 0.02f);
        config->setFloat("stdp_tau", 25.0f);
        config->setFloat("synaptogenesis_rate", 0.0002f);
        config->setFloat("pruning_rate", 0.00002f);
        
        return config;
    }
    
    // Create a fast test configuration
    std::shared_ptr<Config> createFastConfig(int neuronCount = 50) {
        auto config = createSimulationConfig(neuronCount, 1, 0.002f);
        
        // Optimize for speed
        config->setInt("neuron_count", neuronCount);
        config->setFloat("stdp_ltp_weight", 0.01f);
        config->setFloat("stdp_ltd_weight", 0.008f);
        config->setFloat("stdp_tau", 10.0f);
        config->setFloat("synaptogenesis_rate", 0.00005f);
        config->setFloat("pruning_rate", 0.000005f);
        
        // Disable expensive features
        config->setBool("structural_plasticity_enabled", false);
        config->setBool("development_enabled", false);
        config->setBool("curiosity_enabled", false);
        config->setBool("novelty_enabled", false);
        
        return config;
    }
    
    // Create a research configuration
    std::shared_ptr<Config> createResearchConfig(int neuronCount = 2000,
                                                int regionCount = 4) {
        auto config = createSimulationConfig(neuronCount, regionCount, 0.0005f, 42);
        
        // Enhance for research
        config->setInt("neuron_count", neuronCount);
        config->setInt("region_count", regionCount);
        config->setFloat("stdp_ltp_weight", 0.05f);
        config->setFloat("stdp_ltd_weight", 0.04f);
        config->setFloat("stdp_tau", 50.0f);
        config->setFloat("synaptogenesis_rate", 0.001f);
        config->setFloat("pruning_rate", 0.0001f);
        
        // Enable advanced features
        config->setBool("development_enabled", true);
        config->setBool("curiosity_enabled", true);
        config->setBool("novelty_enabled", true);
        config->setBool("prediction_enabled", true);
        config->setInt("working_memory_size", 100);
        config->setInt("episodic_memory_size", 500);
        
        return config;
    }
    
    // Create a demo configuration
    std::shared_ptr<Config> createDemoConfig() {
        auto config = createFastConfig(20);
        
        // Optimize for demo
        config->setBool("visualization_enabled", true);
        config->setBool("development_enabled", false);
        config->setBool("curiosity_enabled", false);
        config->setBool("novelty_enabled", false);
        config->setBool("prediction_enabled", false);
        
        return config;
    }
    
    // Get configuration by type
    std::shared_ptr<Config> getConfig(const std::string& configType) {
        if (configType == "basic" || configType == "simple") {
            return createSimulationConfig();
        } else if (configType == "learning" || configType == "advanced") {
            return createLearningConfig();
        } else if (configType == "fast" || configType == "quick") {
            return createFastConfig();
        } else if (configType == "research" || configType == "detailed") {
            return createResearchConfig();
        } else if (configType == "demo" || configType == "example") {
            return createDemoConfig();
        } else {
            throw std::runtime_error("Unknown configuration type: " + configType);
        }
    }
    
    // Apply configuration from file with backup
    bool loadConfigWithBackup(Config& config, const std::string& filepath, 
                             const std::string& backupPath = "") {
        // Backup if requested
        if (!backupPath.empty()) {
            std::ifstream src(filepath, std::ios::binary);
            std::ofstream dst(backupPath, std::ios::binary);
            dst << src.rdbuf();
        }
        
        // Load new config
        return config.loadFromFile(filepath);
    }
    
    // Validate configuration
    bool validateConfig(const Config& config, std::vector<std::string>& errors) {
        errors.clear();
        
        auto validationResult = config.validate();
        if (!validationResult.valid) {
            errors = validationResult.errors;
            return false;
        }
        
        return true;
    }
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t validEntries;
        float validationScore;
        std::map<std::string, size_t> entriesBySource;
        std::vector<std::string> keys;
    };
    
    ConfigStats getConfigStats(const Config& config) {
        ConfigStats stats;
        stats.totalEntries = config.getEntryCount();
        stats.validEntries = config.getEntryCount(); // This should be validated
        stats.validationScore = config.getValidationScore();
        stats.keys = config.getKeys();
        
        // Count entries by source
        for (const auto& source : {nlm::ConfigSource::Default, nlm::ConfigSource::File,
                                  nlm::ConfigSource::CommandLine, nlm::ConfigSource::Runtime}) {
            size_t count = config.getEntryCount(source);
            if (count > 0) {
                stats.entriesBySource[std::to_string(static_cast<int>(source))] = count;
            }
        }
        
        return stats;
    }
}

} // namespace nlm