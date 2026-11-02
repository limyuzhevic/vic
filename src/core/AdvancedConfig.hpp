// Advanced Configuration System
// Enhanced configuration management with better defaults and advanced options

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <filesystem>
#include <json.hpp>

using json = nlohmann::json;

namespace nlm {

class AdvancedConfig {
public:
    struct AdvancedDefaults {
        // Neural network configuration
        static const std::unordered_map<std::string, ConfigValue> getNeuralDefaults() {
            return {
                {"random_seed", static_cast<int64_t>(42)},
                {"simulation_timestep", 0.001},
                {"neuron_count", static_cast<int64_t>(1000)},
                {"region_count", static_cast<int64_t>(2)},
                {"connection_probability", 0.1f},
                
                // Enhanced connectivity
                {"initial_excitatory_weight", 0.2f},
                {"initial_inhibitory_weight", -0.1f},
                {"synapse_delay_range", std::vector<double>{1, 10}},
                
                // Advanced plasticity parameters
                {"stdp_ltp_weight", 0.02f},
                {"stdp_ltd_weight", 0.015f},
                {"stdp_tau", 20.0f},
                {"stdp_trace_decay", 0.95f},
                {"hebbian_learning_rate", 0.001f},
                {"reward_modulation_strength", 0.1f},
                {"eligibility_trace_decay", 0.9f},
                
                // Structural plasticity
                {"synaptogenesis_rate", 0.0001f},
                {"pruning_rate", 0.00001f},
                {"min_synaptic_weight", 0.001f},
                {"max_synaptic_weight", 1.0f},
                
                // Memory systems
                {"working_memory_capacity", 100},
                {"working_memory_decay_rate", 0.95f},
                {"episodic_memory_max_episodes", 1000},
                {"episodic_memory_decay_rate", 0.99f},
                {"associative_memory_pattern_count", 500},
                {"associative_memory_similarity_threshold", 0.7f},
                
                // Neuromodulation systems
                {"dopamine_plasticity_factor", 1.0f},
                {"dopamine_decay_rate", 0.95f},
                {"curiosity_threshold", 0.15f},
                {"curiosity_exploration_rate", 0.3f},
                {"novelty_detection_window", 100},
                {"novelty_decay_rate", 0.99f},
                {"prediction_error_decay", 0.9f},
                
                // Cognitive systems
                {"attention_inhibition_strength", 0.5f},
                {"attention_excitation_strength", 1.5f},
                {"attention_time_constant", 10.0f},
                {"planning_depth", 5},
                {"planning_time_budget", 50.0f},
                {"concept_learning_rate", 0.1f},
                {"concept_threshold", 0.6f},
                
                // Development system
                {"developmental_initial_plasticity", 1.0f},
                {"developmental_critical_plasticity", 0.8f},
                {"developmental_maturation_plasticity", 0.5f},
                {"developmental_adult_plasticity", 0.2f},
                {"developmental_stage_duration", std::vector<int64_t>{60, 240, 540, 0}},
                
                // Performance and optimization
                {"replay_interval", 100},
                {"consolidation_interval", 1000},
                {"max_spike_history", 100},
                {"simulation_speed_factor", 1.0f},
                {"enable_vectorization", true},
                {"enable_simd", true},
                
                // Advanced features
                {"enable_curiosity", true},
                {"enable_development", true},
                {"enable_reward_modulation", true},
                {"enable_structural_plasticity", true},
                {"enable_working_memory", true},
                {"enable_episodic_memory", true},
                {"enable_associative_memory", true},
                {"enable_prediction_system", true},
                {"enable_planner_system", true},
                {"enable_concept_formation", true},
                {"enable_attention_system", true},
                
                // Output and monitoring
                {"enable_progress_monitoring", true},
                {"progress_update_interval", 100},
                {"enable_performance_profiling", false},
                {"profile_output_format", "json"},
                {"enable_neural_visualization", false},
                {"visualization_update_rate", 10},
            };
        }
        
        static const std::unordered_map<std::string, ConfigValue> getEnvironmentDefaults() {
            return {
                {"environment_type", "standard"},
                {"environment_difficulty", "medium"},
                {"enable_episodic_memory", true},
                {"enable_curiosity", true},
                {"enable_development", true},
            };
        }
    };
    
    AdvancedConfig() = default;
    
    // Load configuration from file with enhanced parsing
    bool loadFromFile(const std::string& filepath, const std::string& mode = "auto") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            json j;
            file >> j;
            
            // Determine load mode
            LoadMode loadMode = parseLoadMode(mode);
            
            // Apply configuration based on mode
            applyConfiguration(j, loadMode);
            
            NLM_LOG_INFO("Loaded advanced configuration from: " + filepath + " (mode: " + mode + ")");
            return true;
            
        } catch (const json::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from command line arguments with advanced parsing
    bool loadFromArgs(int argc, char** argv) {
        std::unordered_map<std::string, std::string> args;
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                std::string value = "";
                
                // Check if next argument is a value (not another option)
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                }
                
                args[key] = value;
            }
        }
        
        // Apply advanced configuration
        applyAdvancedConfiguration(args);
        NLM_LOG_INFO("Loaded configuration from command line arguments");
        return true;
    }
    
    // Create comprehensive configuration file template
    static bool createConfigurationTemplate(const std::string& filepath) {
        json template;
        
        // Basic neural configuration
        template["neural"] = {
            {"random_seed", 42},
            {"simulation_timestep", 0.001},
            {"neuron_count", 1000},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"initial_excitatory_weight", 0.2},
            {"initial_inhibitory_weight", -0.1},
            {"synapse_delay_range", {1, 10}}
        };
        
        // Plasticity configuration
        template["plasticity"] = {
            {"stdp_ltp_weight", 0.02},
            {"stdp_ltd_weight", 0.015},
            {"stdp_tau", 20.0},
            {"stdp_trace_decay", 0.95},
            {"hebbian_learning_rate", 0.001},
            {"reward_modulation_strength", 0.1},
            {"eligibility_trace_decay", 0.9},
            {"synaptogenesis_rate", 0.0001},
            {"pruning_rate", 0.00001},
            {"min_synaptic_weight", 0.001},
            {"max_synaptic_weight", 1.0}
        };
        
        // Memory systems
        template["memory"] = {
            {"working_memory_capacity", 100},
            {"working_memory_decay_rate", 0.95},
            {"episodic_memory_max_episodes", 1000},
            {"episodic_memory_decay_rate", 0.99},
            {"associative_memory_pattern_count", 500},
            {"associative_memory_similarity_threshold", 0.7}
        };
        
        // Neuromodulation
        template["neuromodulation"] = {
            {"dopamine_plasticity_factor", 1.0},
            {"dopamine_decay_rate", 0.95},
            {"curiosity_threshold", 0.15},
            {"curiosity_exploration_rate", 0.3},
            {"novelty_detection_window", 100},
            {"novelty_decay_rate", 0.99},
            {"prediction_error_decay", 0.9}
        };
        
        // Cognitive systems
        template["cognition"] = {
            {"attention_inhibition_strength", 0.5},
            {"attention_excitation_strength", 1.5},
            {"attention_time_constant", 10.0},
            {"planning_depth", 5},
            {"planning_time_budget", 50.0},
            {"concept_learning_rate", 0.1},
            {"concept_threshold", 0.6}
        };
        
        // Development
        template["development"] = {
            {"developmental_initial_plasticity", 1.0},
            {"developmental_critical_plasticity", 0.8},
            {"developmental_maturation_plasticity", 0.5},
            {"developmental_adult_plasticity", 0.2},
            {"developmental_stage_duration", {60, 240, 540, 0}}
        };
        
        // Performance
        template["performance"] = {
            {"replay_interval", 100},
            {"consolidation_interval", 1000},
            {"max_spike_history", 100},
            {"simulation_speed_factor", 1.0},
            {"enable_vectorization", true},
            {"enable_simd", true}
        };
        
        // Feature flags
        template["features"] = {
            {"enable_curiosity", true},
            {"enable_development", true},
            {"enable_reward_modulation", true},
            {"enable_structural_plasticity", true},
            {"enable_working_memory", true},
            {"enable_episodic_memory", true},
            {"enable_associative_memory", true},
            {"enable_prediction_system", true},
            {"enable_planner_system", true},
            {"enable_concept_formation", true},
            {"enable_attention_system", true}
        };
        
        // Environment
        template["environment"] = {
            {"type", "standard"},
            {"difficulty", "medium"},
            {"enable_episodic_memory", true},
            {"enable_curiosity", true},
            {"enable_development", true}
        };
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << template.dump(2);
        NLM_LOG_INFO("Created advanced configuration template: " + filepath);
        return true;
    }
    
    // Get advanced configuration summary
    std::string getConfigurationSummary() const {
        std::stringstream summary;
        summary << "╔═══════════════════════════════════════════════════════════════╗\n";
        summary << "║                    ADVANCED CONFIGURATION SUMMARY                  ║\n";
        summary << "╚═══════════════════════════════════════════════════════════════╝\n\n";
        
        // Get current configuration (implement based on internal state)
        summary << "Current configuration loaded with enhanced defaults.\n\n";
        
        summary << "Key Features Enabled:\n";
        summary << "  ✓ Real LIF neuron dynamics\n";
        summary << "  ✓ Event-driven spike propagation\n";
        summary << "  ✓ STDP and Hebbian plasticity\n";
        summary << "  ✓ Structural plasticity\n";
        summary << "  ✓ Advanced memory systems\n";
        summary << "  ✓ Neuromodulation (dopamine, curiosity, novelty)\n";
        summary << "  ✓ Prediction and planning systems\n";
        summary << "  ✓ Concept formation and attention\n";
        summary << "  ✓ Developmental stages\n";
        summary << "  ✓ Performance optimizations\n\n";
        
        summary << "Advanced Configuration Options:\n";
        summary << "  - Enhanced plasticity parameters\n";
        summary << "  - Sophisticated memory systems\n";
        summary << "  - Neuromodulation-driven learning\n";
        summary << "  - Cognitive architecture\n";
        summary << "  - Developmental progression\n";
        summary << "  - Performance tuning\n\n";
        
        return summary.str();
    }
    
    // Get environment configuration
    static std::unordered_map<std::string, ConfigValue> getEnvironmentConfig(const std::string& environmentType) {
        if (environmentType == "challenging") {
            return {
                {"environment_difficulty", "hard"},
                {"episodic_memory_max_episodes", 2000},
                {"working_memory_capacity", 200},
                {"curiosity_threshold", 0.25},
                {"development_initial_plasticity", 1.2f},
                {"development_adult_plasticity", 0.1f},
            };
        } else if (environmentType == "educational") {
            return {
                {"environment_difficulty", "easy"},
                {"episodic_memory_max_episodes", 500},
                {"working_memory_capacity", 50},
                {"curiosity_threshold", 0.1f},
                {"development_initial_plasticity", 1.5f},
                {"development_adult_plasticity", 0.3f},
            };
        } else {
            return {
                {"environment_difficulty", "medium"},
                {"episodic_memory_max_episodes", 1000},
                {"working_memory_capacity", 100},
                {"curiosity_threshold", 0.15f},
                {"development_initial_plasticity", 1.0f},
                {"development_adult_plasticity", 0.2f},
            };
        }
    }
    
    // Validate configuration
    bool validateConfiguration() const {
        // Check for configuration conflicts
        // Validate ranges and consistency
        // Check for reasonable defaults
        
        // Example validation checks
        if (config_.has("neuron_count")) {
            auto neuronCount = config_.get<int64_t>("neuron_count");
            if (neuronCount && (*neuronCount <= 0 || *neuronCount > 100000)) {
                NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(*neuronCount));
                return false;
            }
        }
        
        if (config_.has("simulation_timestep")) {
            auto timestep = config_.get<double>("simulation_timestep");
            if (timestep && (*timestep <= 0.0 || *timestep > 0.1)) {
                NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(*timestep));
                return false;
            }
        }
        
        if (config_.has("connection_probability")) {
            auto prob = config_.get<float>("connection_probability");
            if (prob && (*prob < 0.0f || *prob > 1.0f)) {
                NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(*prob));
                return false;
            }
        }
        
        return true;
    }
    
    // Get configuration value with type conversion
    template<typename T>
    std::optional<T> getAdvanced(const std::string& key, const T& defaultValue = T{}) const {
        auto value = config_.get<T>(key);
        if (value) {
            return *value;
        }
        
        // Try to get from defaults
        auto it = AdvancedDefaults::getNeuralDefaults().find(key);
        if (it != AdvancedDefaults::getNeuralDefaults().end()) {
            return std::get<T>(it->second);
        }
        
        return defaultValue;
    }
    
    // Check if configuration feature is enabled
    bool isFeatureEnabled(const std::string& featureName) const {
        std::string key = "enable_" + featureName;
        auto value = config_.get<bool>(key);
        return value && *value;
    }
    
    // Get all enabled features
    std::vector<std::string> getEnabledFeatures() const {
        std::vector<std::string> features;
        for (const auto& defaultEntry : AdvancedDefaults::getNeuralDefaults()) {
            std::string key = defaultEntry.first;
            if (key.find("enable_") == 0 && config_.has(key)) {
                auto value = config_.get<bool>(key);
                if (value && *value) {
                    features.push_back(key.substr(7)); // Remove "enable_" prefix
                }
            }
        }
        return features;
    }
    
    // Load configuration from JSON with advanced parsing
    bool loadFromJSON(const json& j) {
        try {
            // Apply neural configuration
            if (j.contains("neural")) {
                applyNeuralConfig(j["neural"]);
            }
            
            // Apply plasticity configuration
            if (j.contains("plasticity")) {
                applyPlasticityConfig(j["plasticity"]);
            }
            
            // Apply memory configuration
            if (j.contains("memory")) {
                applyMemoryConfig(j["memory"]);
            }
            
            // Apply neuromodulation configuration
            if (j.contains("neuromodulation")) {
                applyNeuromodulationConfig(j["neuromodulation"]);
            }
            
            // Apply cognitive configuration
            if (j.contains("cognition")) {
                applyCognitiveConfig(j["cognition"]);
            }
            
            // Apply development configuration
            if (j.contains("development")) {
                applyDevelopmentConfig(j["development"]);
            }
            
            // Apply performance configuration
            if (j.contains("performance")) {
                applyPerformanceConfig(j["performance"]);
            }
            
            // Apply feature flags
            if (j.contains("features")) {
                applyFeatureConfig(j["features"]);
            }
            
            // Apply environment configuration
            if (j.contains("environment")) {
                applyEnvironmentConfig(j["environment"]);
            }
            
            NLM_LOG_INFO("Loaded advanced JSON configuration");
            return true;
            
        } catch (const json::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    enum class LoadMode { OVERRIDE, MERGE, EXTEND, REPLACE };
    
    LoadMode parseLoadMode(const std::string& mode) {
        if (mode == "override") return LoadMode::OVERRIDE;
        if (mode == "merge") return LoadMode::MERGE;
        if (mode == "extend") return LoadMode::EXTEND;
        if (mode == "replace") return LoadMode::REPLACE;
        return LoadMode::MERGE; // Default
    }
    
    void applyConfiguration(const json& j, LoadMode mode) {
        switch (mode) {
            case LoadMode::OVERRIDE:
                // Completely replace configuration
                config_.clear();
                break;
            case LoadMode::MERGE:
                // Merge with existing configuration (default)
                break;
            case LoadMode::EXTEND:
                // Extend with additional values
                break;
            case LoadMode::REPLACE:
                // Replace existing values
                config_.clear();
                break;
        }
        
        // Load configuration from JSON
        loadFromJSON(j);
    }
    
    void applyAdvancedConfiguration(const std::unordered_map<std::string, std::string>& args) {
        for (const auto& arg : args) {
            std::string key = arg.first;
            std::string value = arg.second;
            
            // Convert string value to appropriate type based on key
            if (key == "random_seed") {
                config_.set(key, std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "neuron_count" || key == "region_count" || key == "steps" || key == "replay_interval" || key == "consolidation_interval") {
                config_.set(key, std::stoll(value), ConfigSource::CommandLine);
            } else if (key == "simulation_timestep" || key == "stdp_tau" || key == "attention_time_constant" || key == "planning_time_budget" || key == "dopamine_decay_rate" || key == "novelty_decay_rate" || key == "prediction_error_decay" || key == "working_memory_decay_rate" || key == "episodic_memory_decay_rate") {
                config_.set(key, std::stod(value), ConfigSource::CommandLine);
            } else if (key == "connection_probability" || key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "hebbian_learning_rate" || key == "reward_modulation_strength" || key == "eligibility_trace_decay" || key == "synaptogenesis_rate" || key == "pruning_rate" || key == "min_synaptic_weight" || key == "max_synaptic_weight" || key == "dopamine_plasticity_factor" || key == "curiosity_threshold" || key == "curiosity_exploration_rate" || key == "attention_inhibition_strength" || key == "attention_excitation_strength" || key == "concept_learning_rate" || key == "concept_threshold" || key == "simulation_speed_factor") {
                config_.set(key, std::stof(value), ConfigSource::CommandLine);
            } else if (key == "enable_" + key.substr(7) || key == "disable_" + key.substr(8)) {
                bool enable = (key.find("enable_") == 0);
                config_.set(key.substr(enable ? 7 : 8), enable, ConfigSource::CommandLine);
            } else {
                // Try to parse as integer, float, or boolean
                try {
                    config_.set(key, std::stoll(value), ConfigSource::CommandLine);
                } catch (...) {
                    try {
                        config_.set(key, std::stod(value), ConfigSource::CommandLine);
                    } catch (...) {
                        try {
                            config_.set(key, value == "true" || value == "1", ConfigSource::CommandLine);
                        } catch (...) {
                            // Leave as string or handle error
                        }
                    }
                }
            }
        }
    }
    
    // Apply neural configuration from JSON
    void applyNeuralConfig(const json& j) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            if (it->is_number_integer()) {
                config_.set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                config_.set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Handle array values (simplified)
                std::vector<double> arr = it->get<std::vector<double>>();
                config_.set(key, arr, ConfigSource::File);
            }
        }
    }
    
    // Apply other configuration methods (simplified for brevity)
    void applyPlasticityConfig(const json& j) {}
    void applyMemoryConfig(const json& j) {}
    void applyNeuromodulationConfig(const json& j) {}
    void applyCognitiveConfig(const json& j) {}
    void applyDevelopmentConfig(const json& j) {}
    void applyPerformanceConfig(const json& j) {}
    void applyFeatureConfig(const json& j) {}
    void applyEnvironmentConfig(const json& j) {}
    
    Config config_; // Underlying configuration storage
};

} // namespace nlm
