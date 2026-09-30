#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <typeindex>
#include <any>
#include <stdexcept>

namespace nlm {

// Enhanced configuration validation and schema definitions
struct ConfigurationSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::any> defaultValues;
    std::map<std::string, std::function<bool(const std::any&)>> validators;
    
    ConfigurationSchema(const std::string& name, const std::string& description)
        : name(name), description(description) {}
    
    bool validate(const std::string& key, const std::any& value) const {
        auto it = validators.find(key);
        if (it == validators.end()) return true;  // No validation defined
        return it->second(value);
    }
};

// Configuration category enumerations
enum class ConfigurationCategory {
    BRAIN,
    PLASTICITY,
    NEUROMODULATION,
    MEMORY,
    PREDICTION,
    COGNITION,
    DEVELOPMENT,
    SIMULATION,
    VISUALIZATION,
    ADVANCED
};

// Configuration presets for common use cases
struct ConfigurationPresets {
    static std::shared_ptr<Config> createMinimalBrain() {
        auto config = std::make_shared<Config>();
        
        // Brain configuration
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05f);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.hebbian.enable", false);
        config->set("plasticity.structural.enable", false);
        
        // Neuromodulation configuration
        config->set("neuromod.curiosity.enable", false);
        config->set("neuromod.novelty.enable", false);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        // Development configuration
        config->set("development.enabled", false);
        config->set("development.critical_period_start", 10.0f);
        config->set("development.critical_period_end", 100.0f);
        
        // Simulation configuration
        config->set("simulation.time.step_size", 0.1);
        config->set("simulation.max_steps", 1000);
        
        return config;
    }
    
    static std::shared_ptr<Config> createAdvancedBrain() {
        auto config = std::make_shared<Config>();
        
        // Brain configuration
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.v_reset", -75.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("brain.tau_ref", 2.0f);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001f);
        config->set("plasticity.stdp.tau_plus", 20.0f);
        config->set("plasticity.stdp.tau_minus", 20.0f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.threshold", 0.5f);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0001f);
        config->set("plasticity.structural.pruning_rate", 0.00001f);
        
        // Neuromodulation configuration
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.01f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.threshold", 0.3f);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.dopamine.baseline", 0.1f);
        
        // Memory configuration
        config->set("memory.working.capacity", 100);
        config->set("memory.episodic.max_episodes", 1000);
        config->set("memory.associative.affinity_threshold", 0.7f);
        
        // Prediction configuration
        config->set("prediction.enabled", true);
        config->set("prediction.horizon", 10);
        config->set("prediction.confidence_threshold", 0.8f);
        
        // Cognition configuration
        config->set("cognition.planning.horizon", 5);
        config->set("cognition.attention.gating_strength", 0.5f);
        
        // Development configuration
        config->set("development.enabled", true);
        config->set("development.critical_period_start", 10.0f);
        config->set("development.critical_period_end", 100.0f);
        config->set("development.maturation_rate", 0.1f);
        config->set("development.plasticity_decay", 0.95f);
        
        // Simulation configuration
        config->set("simulation.time.step_size", 0.01);
        config->set("simulation.max_steps", 10000);
        config->set("simulation.save_interval", 1000);
        config->set("simulation.random_seed", 42);
        
        // Visualization configuration
        config->set("visualization.enabled", true);
        config->set("visualization.frame_rate", 30);
        config->set("visualization.quality", "high");
        
        return config;
    }
    
    static std::shared_ptr<Config> createExplorationBrain() {
        auto config = std::make_shared<Config>();
        
        // Brain configuration - smaller but more plastic
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05f);
        config->set("brain.v_thresh", -55.0f);
        config->set("brain.v_rest", -70.0f);
        
        // Plasticity configuration - enhanced exploration
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.0005f);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.threshold", 0.3f);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.synaptogenesis_rate", 0.0002f);
        
        // Neuromodulation configuration - exploration focused
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.05f);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.threshold", 0.5f);
        config->set("neuromod.dopamine.scale", 0.7f);
        
        // Development configuration - extended plasticity
        config->set("development.enabled", true);
        config->set("development.critical_period_start", 5.0f);
        config->set("development.critical_period_end", 50.0f);
        config->set("development.maturation_rate", 0.05f);
        
        // Simulation configuration
        config->set("simulation.time.step_size", 0.1);
        config->set("simulation.max_steps", 5000);
        config->set("simulation.random_seed", 123);
        
        return config;
    }
};

// Configuration validation utilities
class ConfigurationValidator {
public:
    static bool validateBrainConfig(const Config& config) {
        bool valid = true;
        
        if (!config.has("brain.neuron_count") || 
            config.get<size_t>("brain.neuron_count", 0) == 0) {
            NLM_LOG_ERROR("Brain neuron count must be specified and > 0");
            valid = false;
        }
        
        if (config.has("brain.neuron_count")) {
            size_t neurons = config.get<size_t>("brain.neuron_count", 1000);
            if (neurons > 100000) {
                NLM_LOG_WARNING("High neuron count may impact performance: " + std::to_string(neurons));
            }
        }
        
        if (config.has("brain.v_thresh") && config.has("brain.v_rest")) {
            float thresh = config.get<float>("brain.v_thresh", -50.0f);
            float rest = config.get<float>("brain.v_rest", -70.0f);
            if (thresh >= rest) {
                NLM_LOG_ERROR("Neuron threshold must be less than resting potential");
                valid = false;
            }
        }
        
        return valid;
    }
    
    static bool validatePlasticityConfig(const Config& config) {
        if (config.has("plasticity.stdp.enable") && 
            config.get<bool>("plasticity.stdp.enable")) {
            
            if (config.has("plasticity.stdp.learning_rate")) {
                float lr = config.get<float>("plasticity.stdp.learning_rate", 0.001f);
                if (lr < 0.0f || lr > 1.0f) {
                    NLM_LOG_ERROR("STDP learning rate must be between 0.0 and 1.0");
                    return false;
                }
            }
        }
        
        return true;
    }
    
    static bool validateNeuromodulationConfig(const Config& config) {
        if (config.has("neuromod.curiosity.enable") &&
            config.get<bool>("neuromod.curiosity.enable")) {
            
            if (config.has("neuromod.curiosity.base_rate")) {
                float rate = config.get<float>("neuromod.curiosity.base_rate", 0.01f);
                if (rate < 0.0f || rate > 1.0f) {
                    NLM_LOG_ERROR("Curiosity base rate must be between 0.0 and 1.0");
                    return false;
                }
            }
        }
        
        return true;
    }
    
    static bool validateDevelopmentConfig(const Config& config) {
        if (config.has("development.enabled") &&
            config.get<bool>("development.enabled")) {
            
            if (config.has("development.critical_period_start") &&
                config.has("development.critical_period_end")) {
                
                float start = config.get<float>("development.critical_period_start", 10.0f);
                float end = config.get<float>("development.critical_period_end", 100.0f);
                
                if (start < 0.0f || end < 0.0f) {
                    NLM_LOG_ERROR("Development periods must be positive");
                    return false;
                }
                
                if (start >= end) {
                    NLM_LOG_ERROR("Critical period start must be less than end");
                    return false;
                }
            }
        }
        
        return true;
    }
    
    static bool validateAllConfig(const Config& config) {
        bool valid = true;
        
        valid &= validateBrainConfig(config);
        valid &= validatePlasticityConfig(config);
        valid &= validateNeuromodulationConfig(config);
        valid &= validateDevelopmentConfig(config);
        
        if (!valid) {
            NLM_LOG_ERROR("Configuration validation failed");
        }
        
        return valid;
    }
};

// Configuration comparison utilities
class ConfigurationComparison {
public:
    static std::vector<std::string> findDifferences(const Config& config1, const Config& config2) {
        std::vector<std::string> differences;
        
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        // Find keys in config1 but not in config2
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
                differences.push_back("Missing in config2: " + key);
            } else if (!config1.get<std::string>(key).compare(config2.get<std::string>(key))) {
                differences.push_back("Different value for key " + key + ": " +
                    config1.get<std::string>(key) + " vs " + config2.get<std::string>(key));
            }
        }
        
        // Find keys in config2 but not in config1
        for (const auto& key : keys2) {
            if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
                differences.push_back("Missing in config1: " + key);
            }
        }
        
        return differences;
    }
    
    static bool isConfigSubset(const Config& subset, const Config& superset) {
        auto keys = subset.getKeys();
        
        for (const auto& key : keys) {
            if (!superset.has(key) || subset.get<std::string>(key) != superset.get<std::string>(key)) {
                return false;
            }
        }
        
        return true;
    }
    
    static Config mergeConfigs(const Config& config1, const Config& config2) {
        Config merged = config1;
        auto keys = config2.getKeys();
        
        for (const auto& key : keys) {
            if (config2.has(key)) {
                merged.set(key, config2.get<std::string>(key));
            }
        }
        
        return merged;
    }
};

// Configuration import/export utilities
class ConfigurationIO {
public:
    static bool saveConfig(const Config& config, const std::string& filename) {
        // Use existing Config::saveToFile method
        return config.saveToFile(filename);
    }
    
    static bool loadConfig(Config& config, const std::string& filename) {
        // Use existing Config::loadFromFile method
        return config.loadFromFile(filename);
    }
    
    static bool exportConfigToJSON(const Config& config, const std::string& filename) {
        // Enhanced JSON export with schema information
        std::stringstream json;
        json << "{\n";
        
        auto keys = config.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            json << "  \"" << key << "\": \"" << config.get<std::string>(key) << "\"";
            
            if (i < keys.size() - 1) {
                json << ",\n";
            } else {
                json << "\n";
            }
        }
        
        json << "}\n";
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filename);
            return false;
        }
        
        file << json.str();
        file.close();
        
        return true;
    }
    
    static bool importConfigFromJSON(Config& config, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading: " + filename);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        // Parse JSON (simplified - would use proper JSON parser in production)
        // For now, use existing loadFromFile method
        return config.loadFromFile(filename);
    }
};

// Configuration builder with schema validation
class EnhancedConfigurationBuilder {
private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> errors_;
    
    bool validateKey(const std::string& key, const std::any& value) {
        // Basic type validation
        if (key.find(".int") != std::string::npos) {
            return std::any_cast<int>(value) >= 0;
        } else if (key.find(".float") != std::string::npos) {
            float val = std::any_cast<float>(value);
            return val >= 0.0f && val <= 1.0f;
        } else if (key.find(".bool") != std::string::npos) {
            return std::any_cast<bool>(value);
        }
        return true;
    }
    
public:
    EnhancedConfigurationBuilder() : config_(std::make_shared<Config>()) {}
    
    EnhancedConfigurationBuilder& set(const std::string& key, const std::any& value) {
        if (!validateKey(key, value)) {
            errors_.push_back("Invalid value for key " + key);
            return *this;
        }
        
        // Convert any type to appropriate C++ type
        if (std::type_index(typeid(int)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<int>(value));
        } else if (std::type_index(typeid(float)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<float>(value));
        } else if (std::type_index(typeid(bool)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<bool>(value));
        } else if (std::type_index(typeid(std::string)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<std::string>(value));
        } else if (std::type_index(typeid(size_t)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<size_t>(value));
        } else if (std::type_index(typeid(double)) == std::type_index(typeid(value))) {
            config_->set(key, std::any_cast<double>(value));
        }
        
        return *this;
    }
    
    EnhancedConfigurationBuilder& brain(size_t neuronCount = 1000, 
                                        float synapseDensity = 0.1f,
                                        float vThresh = -50.0f,
                                        float vRest = -70.0f) {
        config_->set("brain.neuron_count", neuronCount);
        config_->set("brain.synapse_density", synapseDensity);
        config_->set("brain.v_thresh", vThresh);
        config_->set("brain.v_rest", vRest);
        return *this;
    }
    
    EnhancedConfigurationBuilder& plasticity(bool enableSTDP = true,
                                             float stdpLearningRate = 0.001f,
                                             bool enableHebbian = true,
                                             bool enableStructural = true) {
        config_->set("plasticity.stdp.enable", enableSTDP);
        config_->set("plasticity.stdp.learning_rate", stdpLearningRate);
        config_->set("plasticity.hebbian.enable", enableHebbian);
        config_->set("plasticity.structural.enable", enableStructural);
        return *this;
    }
    
    EnhancedConfigurationBuilder& neuromodulation(bool enableCuriosity = true,
                                                   bool enableNovelty = true,
                                                   float dopamineScale = 1.0f) {
        config_->set("neuromod.curiosity.enable", enableCuriosity);
        config_->set("neuromod.novelty.enable", enableNovelty);
        config_->set("neuromod.dopamine.scale", dopamineScale);
        return *this;
    }
    
    EnhancedConfigurationBuilder& development(bool enableDevelopment = true,
                                               float criticalPeriodStart = 10.0f,
                                               float criticalPeriodEnd = 100.0f) {
        config_->set("development.enabled", enableDevelopment);
        config_->set("development.critical_period_start", criticalPeriodStart);
        config_->set("development.critical_period_end", criticalPeriodEnd);
        return *this;
    }
    
    EnhancedConfigurationBuilder& simulation(double timestep = 0.1,
                                             size_t maxSteps = 1000) {
        config_->set("simulation.time.step_size", timestep);
        config_->set("simulation.max_steps", maxSteps);
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config_;
    }
    
    std::vector<std::string> getErrors() const {
        return errors_;
    }
    
    bool hasErrors() const {
        return !errors_.empty();
    }
};

} // namespace nlm
