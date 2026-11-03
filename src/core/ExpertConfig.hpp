#ifndef NLM_EXPERT_CONFIG_HPP
#define NLM_EXPERT_CONFIG_HPP

#include "../core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace nlm {
namespace expert {

// Advanced configuration for expert users
class ExpertConfig {
private:
    std::shared_ptr<Config> baseConfig;
    std::map<std::string, std::string> expertSettings;
    
public:
    ExpertConfig(std::shared_ptr<Config> config = nullptr) : baseConfig(config) {
        if (!baseConfig) {
            baseConfig = std::make_shared<Config>();
        }
        initializeExpertDefaults();
    }
    
    // Expert-level configuration options
    struct ExpertSettings {
        // Simulation parameters
        float simulationTimestep;
        uint64_t maxSteps;
        uint64_t replayInterval;
        uint64_t consolidationInterval;
        
        // Memory and plasticity
        size_t workingMemoryCapacity;
        size_t episodicMemoryMaxEpisodes;
        float synaptogenesisRate;
        float pruningRate;
        
        // Neuromodulation
        float dopamineLevel;
        float curiosityLevel;
        float noveltyThreshold;
        
        // Development
        float developmentalPlasticityMod;
        uint64_t criticalPeriodSteps;
        
        // Advanced features
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        bool enableSocialLearning;
        bool enableTextTeaching;
        
        // Performance
        bool enableSIMD;
        bool enableMultithreading;
        uint32_t threadCount;
        
        // Debugging
        bool enableDebugOutput;
        bool enablePerformanceProfiling;
        bool enableStepTracing;
        bool enableNeuronActivityLogging;
        
        // Custom patterns and behaviors
        std::vector<float> customResourcePattern;
        std::vector<float> customHazardPattern;
        std::vector<float> customSocialPattern;
        
        // Environment settings
        float worldWidth;
        float worldHeight;
        float visionWidth;
        float visionHeight;
        float maxEnergy;
        float energyDecayRate;
        
        // Behavior modification
        float actionSelectionTemperature;
        float rewardScalingFactor;
        float predictionErrorThreshold;
        float curiosityThreshold;
        
        ExpertSettings() {
            // Default expert settings
            simulationTimestep = 0.001f;
            maxSteps = 10000ULL;
            replayInterval = 100ULL;
            consolidationInterval = 1000ULL;
            
            workingMemoryCapacity = 200;
            episodicMemoryMaxEpisodes = 1000;
            synaptogenesisRate = 0.0002f;
            pruningRate = 0.00002f;
            
            dopamineLevel = 0.5f;
            curiosityLevel = 0.5f;
            noveltyThreshold = 0.1f;
            
            developmentalPlasticityMod = 0.8f;
            criticalPeriodSteps = 500ULL;
            
            enableCheckpointing = true;
            enableReplay = true;
            enableDevelopment = true;
            enableSocialLearning = false;
            enableTextTeaching = false;
            
            enableSIMD = true;
            enableMultithreading = false;
            threadCount = std::thread::hardware_concurrency();
            
            enableDebugOutput = false;
            enablePerformanceProfiling = false;
            enableStepTracing = false;
            enableNeuronActivityLogging = false;
            
            worldWidth = 20.0f;
            worldHeight = 20.0f;
            visionWidth = 16;
            visionHeight = 16;
            maxEnergy = 100.0f;
            energyDecayRate = 0.01f;
            
            actionSelectionTemperature = 1.0f;
            rewardScalingFactor = 1.0f;
            predictionErrorThreshold = 0.1f;
            curiosityThreshold = 0.05f;
        }
    };
    
    void initializeExpertDefaults() {
        // Set expert-level defaults
        baseConfig->set("expert_mode", "true", ConfigSource::Expert);
        baseConfig->set("simulation_timestep", expertSettings.simulationTimestep, ConfigSource::Expert);
        baseConfig->set("max_steps", expertSettings.maxSteps, ConfigSource::Expert);
        baseConfig->set("replay_interval", expertSettings.replayInterval, ConfigSource::Expert);
        baseConfig->set("consolidation_interval", expertSettings.consolidationInterval, ConfigSource::Expert);
        
        // Memory and plasticity settings
        baseConfig->set("working_memory_capacity", expertSettings.workingMemoryCapacity, ConfigSource::Expert);
        baseConfig->set("episodic_memory_max_episodes", expertSettings.episodicMemoryMaxEpisodes, ConfigSource::Expert);
        baseConfig->set("synaptogenesis_rate", expertSettings.synaptogenesisRate, ConfigSource::Expert);
        baseConfig->set("pruning_rate", expertSettings.pruningRate, ConfigSource::Expert);
        
        // Neuromodulation settings
        baseConfig->set("dopamine_level", expertSettings.dopamineLevel, ConfigSource::Expert);
        baseConfig->set("curiosity_level", expertSettings.curiosityLevel, ConfigSource::Expert);
        baseConfig->set("novelty_threshold", expertSettings.noveltyThreshold, ConfigSource::Expert);
        
        // Development settings
        baseConfig->set("developmental_plasticity_mod", expertSettings.developmentalPlasticityMod, ConfigSource::Expert);
        baseConfig->set("critical_period_steps", expertSettings.criticalPeriodSteps, ConfigSource::Expert);
        
        // Feature flags
        baseConfig->set("enable_checkpointing", expertSettings.enableCheckpointing, ConfigSource::Expert);
        baseConfig->set("enable_replay", expertSettings.enableReplay, ConfigSource::Expert);
        baseConfig->set("enable_development", expertSettings.enableDevelopment, ConfigSource::Expert);
        baseConfig->set("enable_social_learning", expertSettings.enableSocialLearning, ConfigSource::Expert);
        baseConfig->set("enable_text_teaching", expertSettings.enableTextTeaching, ConfigSource::Expert);
        
        // Performance settings
        baseConfig->set("enable_simd", expertSettings.enableSIMD, ConfigSource::Expert);
        baseConfig->set("enable_multithreading", expertSettings.enableMultithreading, ConfigSource::Expert);
        baseConfig->set("thread_count", expertSettings.threadCount, ConfigSource::Expert);
        
        // Debug settings
        baseConfig->set("enable_debug_output", expertSettings.enableDebugOutput, ConfigSource::Expert);
        baseConfig->set("enable_performance_profiling", expertSettings.enablePerformanceProfiling, ConfigSource::Expert);
        baseConfig->set("enable_step_tracing", expertSettings.enableStepTracing, ConfigSource::Expert);
        baseConfig->set("enable_neuron_activity_logging", expertSettings.enableNeuronActivityLogging, ConfigSource::Expert);
        
        // Environment settings
        baseConfig->set("environment_width", expertSettings.worldWidth, ConfigSource::Expert);
        baseConfig->set("environment_height", expertSettings.worldHeight, ConfigSource::Expert);
        baseConfig->set("vision_width", expertSettings.visionWidth, ConfigSource::Expert);
        baseConfig->set("vision_height", expertSettings.visionHeight, ConfigSource::Expert);
        baseConfig->set("max_energy", expertSettings.maxEnergy, ConfigSource::Expert);
        baseConfig->set("energy_decay_rate", expertSettings.energyDecayRate, ConfigSource::Expert);
        
        // Behavior settings
        baseConfig->set("action_selection_temperature", expertSettings.actionSelectionTemperature, ConfigSource::Expert);
        baseConfig->set("reward_scaling_factor", expertSettings.rewardScalingFactor, ConfigSource::Expert);
        baseConfig->set("prediction_error_threshold", expertSettings.predictionErrorThreshold, ConfigSource::Expert);
        baseConfig->set("curiosity_threshold", expertSettings.curiosityThreshold, ConfigSource::Expert);
    }
    
    void setExpertSetting(const std::string& key, const std::string& value) {
        expertSettings[key] = value;
        baseConfig->set(key, value, ConfigSource::Expert);
    }
    
    void setExpertSetting(const std::string& key, float value) {
        expertSettings[key] = std::to_string(value);
        baseConfig->set(key, value, ConfigSource::Expert);
    }
    
    void setExpertSetting(const std::string& key, uint64_t value) {
        expertSettings[key] = std::to_string(value);
        baseConfig->set(key, value, ConfigSource::Expert);
    }
    
    void setExpertSetting(const std::string& key, bool value) {
        expertSettings[key] = value ? "true" : "false";
        baseConfig->set(key, value, ConfigSource::Expert);
    }
    
    template<typename T>
    T getExpertSetting(const std::string& key) {
        if (expertSettings.find(key) != expertSettings.end()) {
            // Parse the string value
            return parseValue<T>(expertSettings[key]);
        }
        return getDefaultValue<T>(key);
    }
    
    // Configuration presets for different experiment types
    static ExpertConfig createSimpleLearningConfig() {
        ExpertConfig config;
        config.baseConfig->set("neuron_count", 500, ConfigSource::Expert);
        config.baseConfig->set("region_count", 2, ConfigSource::Expert);
        config.baseConfig->set("connection_probability", 0.1f, ConfigSource::Expert);
        config.setExpertSetting("enable_developmental", true);
        config.setExpertSetting("enable_reward_modulation", true);
        config.setExpertSetting("enable_curiosity", true);
        return config;
    }
    
    static ExpertConfig createAdvancedLearningConfig() {
        ExpertConfig config;
        config.baseConfig->set("neuron_count", 1000, ConfigSource::Expert);
        config.baseConfig->set("region_count", 3, ConfigSource::Expert);
        config.baseConfig->set("connection_probability", 0.15f, ConfigSource::Expert);
        config.setExpertSetting("enable_developmental", true);
        config.setExpertSetting("enable_reward_modulation", true);
        config.setExpertSetting("enable_curiosity", true);
        config.setExpertSetting("enable_social_learning", true);
        config.setExpertSetting("enable_text_teaching", true);
        config.setExpertSetting("enable_replay", true);
        config.setExpertSetting("enable_checkpointing", true);
        config.setExpertSetting("enable_performance_profiling", true);
        return config;
    }
    
    static ExpertConfig createResearchConfig() {
        ExpertConfig config;
        config.baseConfig->set("neuron_count", 2000, ConfigSource::Expert);
        config.baseConfig->set("region_count", 4, ConfigSource::Expert);
        config.baseConfig->set("connection_probability", 0.2f, ConfigSource::Expert);
        config.setExpertSetting("enable_developmental", true);
        config.setExpertSetting("enable_reward_modulation", true);
        config.setExpertSetting("enable_curiosity", true);
        config.setExpertSetting("enable_social_learning", true);
        config.setExpertSetting("enable_text_teaching", true);
        config.setExpertSetting("enable_replay", true);
        config.setExpertSetting("enable_checkpointing", true);
        config.setExpertSetting("enable_performance_profiling", true);
        config.setExpertSetting("enable_step_tracing", true);
        config.setExpertSetting("enable_neuron_activity_logging", true);
        return config;
    }
    
    std::shared_ptr<Config> getConfig() const {
        return baseConfig;
    }
    
    std::string toJson() const {
        // Convert to JSON string for serialization
        return "{\"expert_config\": {}}";  // Placeholder
    }
    
    static ExpertConfig fromJson(const std::string& json) {
        ExpertConfig config;
        // Parse JSON and load settings
        return config;
    }
    
private:
    template<typename T>
    T parseValue(const std::string& str) {
        std::istringstream iss(str);
        T value;
        iss >> value;
        return value;
    }
    
    template<typename T>
    T getDefaultValue(const std::string& key) {
        // Return default value for key
        if (key == "enable_developmental") return true;
        if (key == "enable_reward_modulation") return true;
        if (key == "enable_curiosity") return true;
        if (key == "enable_social_learning") return false;
        if (key == "enable_text_teaching") return false;
        if (key == "enable_replay") return false;
        if (key == "enable_checkpointing") return false;
        if (key == "enable_simd") return true;
        if (key == "enable_multithreading") return false;
        if (key == "enable_debug_output") return false;
        if (key == "enable_performance_profiling") return false;
        if (key == "enable_step_tracing") return false;
        if (key == "enable_neuron_activity_logging") return false;
        return T();
    }
};

} // namespace expert
} // namespace nlm

#endif // NLM_EXPERT_CONFIG_HPP