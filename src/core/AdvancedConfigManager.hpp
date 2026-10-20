#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "Config.hpp"

namespace nlm {

/**
 * Advanced Configuration Manager for NLM
 * 
 * Provides enhanced configuration management with:
 * - Preset profiles for different research scenarios
 * - Parameter validation and type checking
 * - Research scene management
 * - Configuration comparison and validation
 * - Import/export utilities
 */
class AdvancedConfigManager {
public:
    /**
     * Configuration profile for different research scenarios
     */
    enum class ResearchProfile {
        // Development and testing
        Development,
        
        // Performance and scalability
        SmallScale,
        MediumScale,
        LargeScale,
        
        // Research-specific
        CognitiveDevelopment,
        NeuromodulationStudy,
        MemoryIntegration,
        LearningDynamics,
        BehavioralModeling,
        
        // Custom
        Custom
    };

    /**
     * Configuration parameter metadata
     */
    struct ParameterMetadata {
        std::string name;
        std::string description;
        std::string category;  // e.g., "neuron", "plasticity", "memory"
        std::string type;      // "int", "float", "bool", "string"
        
        // Range constraints
        std::optional<double> minValue;
        std::optional<double> maxValue;
        
        // Default value
        std::string defaultValue;
        
        // Advanced users only
        bool advanced = false;
        
        ParameterMetadata() : minValue(std::nullopt), maxValue(std::nullopt), advanced(false) {}
    };

    /**
     * Research scene configuration
     */
    struct ResearchScene {
        std::string name;
        std::string description;
        ResearchProfile profile;
        
        // Custom parameters
        std::unordered_map<std::string, std::string> parameters;
        
        // Metadata
        std::string author;
        std::string date;
        std::string version;
        
        ResearchScene() : profile(ResearchProfile::Custom) {}
    };

    AdvancedConfigManager() = default;
    ~AdvancedConfigManager() = default;

    /**
     * Apply a research profile to a configuration
     */
    void applyProfile(std::shared_ptr<Config> config, ResearchProfile profile) {
        // Clear existing custom settings
        config->clear();
        
        // Apply profile-specific settings
        switch (profile) {
            case ResearchProfile::Development:
                applyDevelopmentProfile(config);
                break;
            case ResearchProfile::SmallScale:
                applySmallScaleProfile(config);
                break;
            case ResearchProfile::MediumScale:
                applyMediumScaleProfile(config);
                break;
            case ResearchProfile::LargeScale:
                applyLargeScaleProfile(config);
                break;
            case ResearchProfile::CognitiveDevelopment:
                applyCognitiveDevelopmentProfile(config);
                break;
            case ResearchProfile::NeuromodulationStudy:
                applyNeuromodulationStudyProfile(config);
                break;
            case ResearchProfile::MemoryIntegration:
                applyMemoryIntegrationProfile(config);
                break;
            case ResearchProfile::LearningDynamics:
                applyLearningDynamicsProfile(config);
                break;
            case ResearchProfile::BehavioralModeling:
                applyBehavioralModelingProfile(config);
                break;
            case ResearchProfile::Custom:
                // No changes
                break;
        }
    }

    /**
     * Get parameter metadata for a specific parameter
     */
    ParameterMetadata getParameterMetadata(const std::string& key) const {
        auto it = parameterMetadata_.find(key);
        if (it != parameterMetadata_.find(key)) {
            return it->second;
        }
        return ParameterMetadata{}; // Empty metadata
    }

    /**
     * Validate configuration against all constraints
     */
    bool validateConfiguration(std::shared_ptr<Config> config, std::vector<std::string>& errors) const {
        errors.clear();
        
        for (const auto& key : config->getKeys()) {
            auto metadata = getParameterMetadata(key);
            
            // Check type constraints
            if (metadata.minValue.has_value() || metadata.maxValue.has_value()) {
                std::optional<double> value = getNumericValue(config, key);
                if (value.has_value()) {
                    if (metadata.minValue.has_value() && value.value() < metadata.minValue.value()) {
                        errors.push_back("Parameter " + key + " value " + std::to_string(value.value()) + 
                                      " is below minimum " + std::to_string(metadata.minValue.value()));
                    }
                    if (metadata.maxValue.has_value() && value.value() > metadata.maxValue.value()) {
                        errors.push_back("Parameter " + key + " value " + std::to_string(value.value()) + 
                                      " is above maximum " + std::to_string(metadata.maxValue.value()));
                    }
                }
            }
        }
        
        return errors.empty();
    }

    /**
     * Save configuration as research scene
     */
    bool saveResearchScene(std::shared_ptr<Config> config, const std::string& filename, 
                         const std::string& name, const std::string& description,
                         const std::string& author = "", ResearchProfile profile = ResearchProfile::Custom) {
        ResearchScene scene;
        scene.name = name;
        scene.description = description;
        scene.profile = profile;
        scene.author = author;
        scene.date = getCurrentDate();
        scene.version = "1.0.0";
        
        // Copy all configuration parameters
        for (const auto& key : config->getKeys()) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                scene.parameters[key] = value.value();
            }
        }
        
        // Save to file (using JSON for simplicity)
        return saveResearchSceneToFile(scene, filename);
    }

    /**
     * Load configuration from research scene
     */
    bool loadResearchScene(std::shared_ptr<Config> config, const std::string& filename) {
        ResearchScene scene;
        if (!loadResearchSceneFromFile(scene, filename)) {
            return false;
        }
        
        // Clear existing configuration
        config->clear();
        
        // Apply loaded parameters
        for (const auto& param : scene.parameters) {
            // Try to determine parameter type and set accordingly
            if (param.second.find('.') != std::string::npos) {
                try {
                    double double_value = std::stod(param.second);
                    config->set(param.first, double_value);
                } catch (...) {
                    config->set(param.first, param.second);
                }
            } else {
                try {
                    int int_value = std::stoi(param.second);
                    config->set(param.first, int_value);
                } catch (...) {
                    if (param.second == "true" || param.second == "false") {
                        bool bool_value = (param.second == "true");
                        config->set(param.first, bool_value);
                    } else {
                        config->set(param.first, param.second);
                    }
                }
            }
        }
        
        return true;
    }

    /**
     * Compare two configurations and show differences
     */
    std::string compareConfigurations(std::shared_ptr<Config> config1, 
                                     std::shared_ptr<Config> config2) const {
        std::stringstream ss;
        ss << "=== Configuration Comparison ===" << std::endl;
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Find parameters in config1 not in config2
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
                ss << "Parameter present in config1, missing in config2: " << key << std::endl;
            } else {
                auto value1 = config1->get<std::string>(key);
                auto value2 = config2->get<std::string>(key);
                if (value1 != value2) {
                    ss << "Difference in parameter '" << key << "': "
                       << "config1 = " << (value1.has_value() ? value1.value() : "(none)") << ", "
                       << "config2 = " << (value2.has_value() ? value2.value() : "(none)") << std::endl;
                }
            }
        }
        
        // Find parameters in config2 not in config1
        for (const auto& key : keys2) {
            if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
                ss << "Parameter present in config2, missing in config1: " << key << std::endl;
            }
        }
        
        return ss.str();
    }

    /**
     * Get all available research scenes
     */
    std::vector<std::string> getAvailableResearchScenes() const {
        std::vector<std::string> scenes;
        
        // Add built-in scene files
        std::vector<std::string> built_in_scenes = {
            "scenes/development.scene",
            "scenes/cognitive_research.scene",
            "scenes/neuromodulation_study.scene",
            "scenes/memory_integration.scene",
            "scenes/learning_dynamics.scene"
        };
        
        for (const auto& scene_file : built_in_scenes) {
            scenes.push_back(scene_file);
        }
        
        return scenes;
    }

    /**
     * Export configuration to JSON format
     */
    std::string exportToJSON(std::shared_ptr<Config> config) const {
        std::stringstream ss;
        ss << "{" << std::endl;
        
        bool first = true;
        for (const auto& key : config->getKeys()) {
            if (!first) {
                ss << "," << std::endl;
            }
            first = false;
            
            ss << "  \"" << key << "\": \"" << config->get<std::string>(key).value() << "\"";
        }
        
        ss << std::endl << "}" << std::endl;
        return ss.str();
    }

    /**
     * Import configuration from JSON format
     */
    bool importFromJSON(std::shared_ptr<Config> config, const std::string& json_string) {
        // Simple JSON parsing for key-value pairs
        // In a real implementation, use a proper JSON parser
        size_t pos = 0;
        std::string key, value;
        
        while (pos < json_string.size()) {
            // Find key
            size_t key_start = json_string.find('"', pos);
            if (key_start == std::string::npos) break;
            size_t key_end = json_string.find('"', key_start + 1);
            if (key_end == std::string::npos) break;
            
            key = json_string.substr(key_start + 1, key_end - key_start - 1);
            pos = key_end + 1;
            
            // Skip whitespace and colon
            while (pos < json_string.size() && (std::isspace(json_string[pos]) || json_string[pos] == ':')) {
                pos++;
            }
            
            // Find value
            size_t value_start = json_string.find('"', pos);
            if (value_start == std::string::npos) break;
            size_t value_end = json_string.find('"', value_start + 1);
            if (value_end == std::string::npos) break;
            
            value = json_string.substr(value_start + 1, value_end - value_start - 1);
            pos = value_end + 1;
            
            // Set configuration value
            config->set(key, value);
            
            // Skip comma and whitespace
            while (pos < json_string.size() && (json_string[pos] == ',' || std::isspace(json_string[pos]))) {
                pos++;
            }
        }
        
        return true;
    }

private:
    /**
     * Apply development profile
     */
    void applyDevelopmentProfile(std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.05f);
        config->set("stdp_ltp_weight", 0.005f);
        config->set("stdp_ltd_weight", 0.004f);
        config->set("synaptogenesis_rate", 0.00005f);
        config->set("pruning_rate", 0.000005f);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
    }

    /**
     * Apply small scale profile
     */
    void applySmallScaleProfile(std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
    }

    /**
     * Apply medium scale profile
     */
    void applyMediumScaleProfile(std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.15f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0002f);
        config->set("pruning_rate", 0.00002f);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward_modulation", true);
        config->set("enable_prediction", true);
    }

    /**
     * Apply large scale profile
     */
    void applyLargeScaleProfile(std::shared_ptr<Config> config) {
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(5000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.2f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("synaptogenesis_rate", 0.0003f);
        config->set("pruning_rate", 0.00003f);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward_modulation", true);
        config->set("enable_prediction", true);
        config->set("enable_working_memory", true);
        config->set("enable_episodic_memory", true);
        config->set("enable_semantic_memory", true);
    }

    /**
     * Apply cognitive development profile
     */
    void applyCognitiveDevelopmentProfile(std::shared_ptr<Config> config) {
        applyMediumScaleProfile(config);
        config->set("enable_concept_formation", true);
        config->set("enable_planning", true);
        config->set("enable_attention", true);
        config->set("enable_social_learning", false);
        config->set("enable_continual_learning", true);
        config->set("critical_period_length", 100.0);
        config->set("plasticity_decay_rate", 0.001);
    }

    /**
     * Apply neuromodulation study profile
     */
    void applyNeuromodulationStudyProfile(std::shared_ptr<Config> config) {
        applySmallScaleProfile(config);
        config->set("enable_reward_modulation", true);
        config->set("enable_curiosity", true);
        config->set("enable_novelty_detection", true);
        config->set("dopamine_scale", 1.0f);
        config->set("curiosity_threshold", 0.3f);
        config->set("novelty_decay_rate", 0.95f);
        config->set("prediction_error_weight", 0.5f);
    }

    /**
     * Apply memory integration profile
     */
    void applyMemoryIntegrationProfile(std::shared_ptr<Config> config) {
        applyMediumScaleProfile(config);
        config->set("enable_working_memory", true);
        config->set("enable_episodic_memory", true);
        config->set("enable_semantic_memory", true);
        config->set("enable_procedural_memory", false);
        config->set("working_memory_capacity", 100);
        config->set("episodic_memory_capacity", 1000);
        config->set("memory_replay_rate", 0.1f);
        config->set("memory_consolidation_threshold", 0.7f);
    }

    /**
     * Apply learning dynamics profile
     */
    void applyLearningDynamicsProfile(std::shared_ptr<Config> config) {
        applyLargeScaleProfile(config);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_reward_modulation", true);
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.018f);
        config->set("learning_rate", 0.001);
        config->set("weight_decay", 0.0001f);
        config->set("eligibility_trace_decay", 0.9f);
    }

    /**
     * Apply behavioral modeling profile
     */
    void applyBehavioralModelingProfile(std::shared_ptr<Config> config) {
        applyLargeScaleProfile(config);
        config->set("enable_structural_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
        config->set("enable_reward_modulation", true);
        config->set("enable_prediction", true);
        config->set("action_selection_temperature", 1.0f);
        config->set("exploration_rate", 0.2f);
        config->set("exploitation_rate", 0.8f);
        config->set("behavioral_constraints_enabled", true);
    }

    /**
     * Get numeric value from configuration
     */
    std::optional<double> getNumericValue(std::shared_ptr<Config> config, const std::string& key) const {
        auto int_value = config->get<int64_t>(key);
        if (int_value.has_value()) {
            return static_cast<double>(int_value.value());
        }
        
        auto float_value = config->get<double>(key);
        if (float_value.has_value()) {
            return float_value.value();
        }
        
        return std::nullopt;
    }

    /**
     * Get current date as string
     */
    std::string getCurrentDate() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    /**
     * Save research scene to file
     */
    bool saveResearchSceneToFile(const ResearchScene& scene, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== Research Scene ===" << std::endl;
        file << "Name: " << scene.name << std::endl;
        file << "Description: " << scene.description << std::endl;
        file << "Profile: " << static_cast<int>(scene.profile) << std::endl;
        file << "Author: " << scene.author << std::endl;
        file << "Date: " << scene.date << std::endl;
        file << "Version: " << scene.version << std::endl;
        file << "Parameters:" << std::endl;
        
        for (const auto& param : scene.parameters) {
            file << "  " << param.first << " = " << param.second << std::endl;
        }
        
        file.close();
        return true;
    }

    /**
     * Load research scene from file
     */
    bool loadResearchSceneFromFile(ResearchScene& scene, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        bool in_parameters = false;
        
        while (std::getline(file, line)) {
            if (line.find("Name: ") == 0) {
                scene.name = line.substr(6);
            } else if (line.find("Description: ") == 0) {
                scene.description = line.substr(12);
            } else if (line.find("Profile: ") == 0) {
                try {
                    scene.profile = static_cast<ResearchProfile>(std::stoi(line.substr(9)));
                } catch (...) {
                    scene.profile = ResearchProfile::Custom;
                }
            } else if (line.find("Author: ") == 0) {
                scene.author = line.substr(7);
            } else if (line.find("Date: ") == 0) {
                scene.date = line.substr(6);
            } else if (line.find("Version: ") == 0) {
                scene.version = line.substr(8);
            } else if (line == "Parameters:") {
                in_parameters = true;
            } else if (in_parameters && !line.empty() && line.find("  ") == 0) {
                size_t pos = line.find(" = ");
                if (pos != std::string::npos) {
                    std::string key = line.substr(2, pos - 2);
                    std::string value = line.substr(pos + 3);
                    scene.parameters[key] = value;
                }
            }
        }
        
        file.close();
        return true;
    }

    // Initialize parameter metadata
    void initializeParameterMetadata() {
        // Neuron parameters
        parameterMetadata_["neuron_count"] = ParameterMetadata{
            "neuron_count", "Total number of neurons", "neuron", "int",
            std::nullopt, std::nullopt, "1000"
        };
        
        parameterMetadata_["region_count"] = ParameterMetadata{
            "region_count", "Number of neural regions", "neuron", "int",
            std::nullopt, std::nullopt, "1"
        };
        
        parameterMetadata_["connection_probability"] = ParameterMetadata{
            "connection_probability", "Probability of connection between neurons", "connectivity", "float",
            0.0, 1.0, "0.1"
        };
        
        // Plasticity parameters
        parameterMetadata_["stdp_ltp_weight"] = ParameterMetadata{
            "stdp_ltp_weight", "STDP long-term potentiation weight", "plasticity", "float",
            0.0, std::nullopt, "0.01"
        };
        
        parameterMetadata_["stdp_ltd_weight"] = ParameterMetadata{
            "stdp_ltd_weight", "STDP long-term depression weight", "plasticity", "float",
            0.0, std::nullopt, "0.012"
        };
        
        // Development parameters
        parameterMetadata_["synaptogenesis_rate"] = ParameterMetadata{
            "synaptogenesis_rate", "Rate of new synapse formation", "development", "float",
            0.0, std::nullopt, "0.0001"
        };
        
        parameterMetadata_["pruning_rate"] = ParameterMetadata{
            "pruning_rate", "Rate of synapse elimination", "development", "float",
            0.0, std::nullopt, "0.00001"
        };
        
        // Neuromodulation parameters
        parameterMetadata_["dopamine_scale"] = ParameterMetadata{
            "dopamine_scale", "Dopamine modulation scale", "neuromodulation", "float",
            0.0, std::nullopt, "1.0"
        };
        
        parameterMetadata_["curiosity_threshold"] = ParameterMetadata{
            "curiosity_threshold", "Curiosity-driven exploration threshold", "neuromodulation", "float",
            0.0, 1.0, "0.3"
        };
        
        // Memory parameters
        parameterMetadata_["working_memory_capacity"] = ParameterMetadata{
            "working_memory_capacity", "Working memory capacity in traces", "memory", "int",
            0, std::nullopt, "100"
        };
        
        parameterMetadata_["episodic_memory_capacity"] = ParameterMetadata{
            "episodic_memory_capacity", "Episodic memory capacity in episodes", "memory", "int",
            0, std::nullopt, "1000"
        };
        
        // Development stage parameters
        parameterMetadata_["critical_period_length"] = ParameterMetadata{
            "critical_period_length", "Length of critical period (time units)", "development", "float",
            0.0, std::nullopt, "100.0"
        };
        
        // System flags
        parameterMetadata_["enable_structural_plasticity"] = ParameterMetadata{
            "enable_structural_plasticity", "Enable structural plasticity (synaptogenesis/pruning)", "system", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_development"] = ParameterMetadata{
            "enable_development", "Enable developmental processes", "system", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_curiosity"] = ParameterMetadata{
            "enable_curiosity", "Enable curiosity-driven exploration", "system", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_reward_modulation"] = ParameterMetadata{
            "enable_reward_modulation", "Enable reward-based learning", "system", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_prediction"] = ParameterMetadata{
            "enable_prediction", "Enable prediction system", "system", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        // Advanced features
        parameterMetadata_["enable_working_memory"] = ParameterMetadata{
            "enable_working_memory", "Enable working memory system", "memory", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_episodic_memory"] = ParameterMetadata{
            "enable_episodic_memory", "Enable episodic memory system", "memory", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_semantic_memory"] = ParameterMetadata{
            "enable_semantic_memory", "Enable semantic memory system", "memory", "bool",
            std::nullopt, std::nullopt, "false"
        };
        
        parameterMetadata_["enable_planning"] = ParameterMetadata{
            "enable_planning", "Enable neural planning system", "cognition", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_attention"] = ParameterMetadata{
            "enable_attention", "Enable attention system", "cognition", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        parameterMetadata_["enable_concept_formation"] = ParameterMetadata{
            "enable_concept_formation", "Enable concept formation system", "cognition", "bool",
            std::nullopt, std::nullopt, "true"
        };
        
        // Performance parameters
        parameterMetadata_["simulation_timestep"] = ParameterMetadata{
            "simulation_timestep", "Simulation time step size", "performance", "float",
            0.0001, std::nullopt, "0.001"
        };
        
        parameterMetadata_["random_seed"] = ParameterMetadata{
            "random_seed", "Random seed for reproducibility", "performance", "int",
            std::nullopt, std::nullopt, "42"
        };
    }

    std::unordered_map<std::string, ParameterMetadata> parameterMetadata_;
};

} // namespace nlm
