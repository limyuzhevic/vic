#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Experiment configuration base class
class ExperimentConfig {
public:
    virtual ~ExperimentConfig() = default;
    
    virtual const std::string& getType() const = 0;
    virtual bool isEnabled() const = 0;
    
    // Common configuration parameters
    virtual uint64_t getNeuronCount() const { return 1000; }
    virtual uint64_t getRegionCount() const { return 1; }
    virtual float getConnectionProbability() const { return 0.1f; }
    virtual double getSimulationTimestep() const { return 0.001; }
    virtual uint64_t getMaxSteps() const { return 1000; }
    
    // Get configuration value
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        auto it = customValues.find(key);
        if (it != customValues.end()) {
            return std::any_cast<T>(it->second);
        }
        return defaultValue;
    }
    
    // Set configuration value
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        customValues[key] = value;
    }
    
    // Configuration validation
    virtual bool validate() const { return true; }
    
    virtual std::string getDescription() const { return "Base experiment configuration"; }
    
protected:
    std::unordered_map<std::string, std::any> customValues;
};

// Learning experiment configuration
class LearningExperimentConfig : public ExperimentConfig {
public:
    const std::string& getType() const override { static const std::string type = "learning"; return type; }
    bool isEnabled() const override { return enableLearning; }
    
    // Learning-specific parameters
    bool getEnableHebbian() const { return enableHebbian; }
    bool getEnableSTDP() const { return enableSTDP; }
    bool getEnableStructuralPlasticity() const { return enableStructuralPlasticity; }
    
    uint64_t getMaxSteps() const override { return learningSteps; }
    float getConnectionProbability() const override { return connectionProbability; }
    
    // Learning experiment parameters
    double getInputPatternDuration() const { return inputPatternDuration; }
    double getInterPatternInterval() const { return interPatternInterval; }
    uint64_t getPatternCount() const { return patternCount; }
    float getInputCurrentStrength() const { return inputCurrentStrength; }
    float getLearningRate() const { return learningRate; }
    
    bool validate() const override {
        return enableHebbian || enableSTDP || enableStructuralPlasticity;
    }
    
    std::string getDescription() const override {
        return "Learning experiment testing synaptic modification through experience";
    }
    
public:
    // Configuration
    bool enableLearning = true;
    bool enableHebbian = true;
    bool enableSTDP = true;
    bool enableStructuralPlasticity = false;
    uint64_t learningSteps = 2000;
    float connectionProbability = 0.15f;
    double inputPatternDuration = 0.1;
    double interPatternInterval = 0.05;
    uint64_t patternCount = 10;
    float inputCurrentStrength = 30.0f;
    float learningRate = 0.01f;
    std::string learningTask = "pattern_association";
    float noveltyThreshold = 0.5f;
    bool enableCuriosity = true;
};

// Adaptation experiment configuration
class AdaptationExperimentConfig : public ExperimentConfig {
public:
    const std::string& getType() const override { static const std::string type = "adaptation"; return type; }
    bool isEnabled() const override { return enableAdaptation; }
    
    // Adaptation-specific parameters
    bool getEnableEnvironmentChange() const { return enableEnvironmentChange; }
    bool getEnableRewardLearning() const { return enableRewardLearning; }
    bool getEnableBehavioralFlexibility() const { return enableBehavioralFlexibility; }
    
    uint64_t getMaxSteps() const override { return adaptationSteps; }
    double getSimulationTimestep() const override { return timestep; }
    
    // Adaptation parameters
    double getEnvironmentChangeInterval() const { return environmentChangeInterval; }
    float getRewardScale() const { return rewardScale; }
    float getPunishmentScale() const { return punishmentScale; }
    uint64_t getRewardFrequency() const { return rewardFrequency; }
    uint64_t getPunishmentFrequency() const { return punishmentFrequency; }
    
    bool validate() const override {
        return enableEnvironmentChange || enableRewardLearning || enableBehavioralFlexibility;
    }
    
    std::string getDescription() const override {
        return "Adaptation experiment testing response to changing environments";
    }
    
public:
    // Configuration
    bool enableAdaptation = true;
    bool enableEnvironmentChange = true;
    bool enableRewardLearning = true;
    bool enableBehavioralFlexibility = true;
    uint64_t adaptationSteps = 3000;
    double timestep = 0.001;
    double environmentChangeInterval = 500.0;
    float rewardScale = 1.0f;
    float punishmentScale = -0.5f;
    uint64_t rewardFrequency = 100;
    uint64_t punishmentFrequency = 150;
    std::string environmentType = "dynamic_stimuli";
    float adaptationRate = 0.05f;
    bool enableSustainedAttention = true;
    float attentionThreshold = 0.7f;
};

// Social learning experiment configuration
class SocialLearningExperimentConfig : public ExperimentConfig {
public:
    const std::string& getType() const override { static const std::string type = "social"; return type; }
    bool isEnabled() const override { return enableSocialLearning; }
    
    // Social-specific parameters
    bool getEnableObservation() const { return enableObservation; }
    bool getEnableImitation() const { return enableImitation; }
    bool getEnableCooperation() const { return enableCooperation; }
    
    uint64_t getMaxSteps() const override { return socialSteps; }
    
    // Social parameters
    uint64_t getNumberOfAgents() const { return numberOfAgents; }
    double getSocialInteractionInterval() const { return socialInteractionInterval; }
    float getImitationStrength() const { return imitationStrength; }
    float getCooperationThreshold() const { return cooperationThreshold; }
    float getTrustDecayRate() const { return trustDecayRate; }
    bool getEnableSocialMemory() const { return enableSocialMemory; }
    std::string getSocialStructure() const { return socialStructure; }
    
    bool validate() const override {
        return enableObservation || enableImitation || enableCooperation;
    }
    
    std::string getDescription() const override {
        return "Social learning experiment testing observation, imitation, and cooperation";
    }
    
public:
    // Configuration
    bool enableSocialLearning = true;
    bool enableObservation = true;
    bool enableImitation = true;
    bool enableCooperation = false;
    uint64_t socialSteps = 2500;
    uint64_t numberOfAgents = 5;
    double socialInteractionInterval = 0.5;
    float imitationStrength = 0.8f;
    float cooperationThreshold = 0.6f;
    float trustDecayRate = 0.01f;
    bool enableSocialMemory = true;
    std::string socialStructure = "hierarchical";
    float observationalLearningRate = 0.03f;
    float sharedExperienceStrength = 0.7f;
    bool enableSocialNormCompliance = true;
};

// Memory experiment configuration
class MemoryExperimentConfig : public ExperimentConfig {
public:
    const std::string& getType() const override { static const std::string type = "memory"; return type; }
    bool isEnabled() const override { return enableMemoryExperiment; }
    
    // Memory-specific parameters
    bool getEnableEpisodicMemory() const { return enableEpisodicMemory; }
    bool getEnableWorkingMemory() const { return enableWorkingMemory; }
    bool getEnableSemanticMemory() const { return enableSemanticMemory; }
    bool getEnableProceduralMemory() const { return enableProceduralMemory; }
    bool getEnableAssociativeMemory() const { return enableAssociativeMemory; }
    
    uint64_t getMaxSteps() const override { return memorySteps; }
    
    // Memory parameters
    uint64_t getMaxEpisodicEpisodes() const { return maxEpisodicEpisodes; }
    uint64_t getWorkingMemoryCapacity() const { return workingMemoryCapacity; }
    double getMemoryReplayInterval() const { return memoryReplayInterval; }
    float getMemoryConsolidationStrength() const { return memoryConsolidationStrength; }
    bool getEnableMemoryInterference() const { return enableMemoryInterference; }
    std::string getMemoryTask() const { return memoryTask; }
    bool getEnableForgetting() const { return enableForgetting; }
    double getForgettingRate() const { return forgettingRate; }
    
    bool validate() const override {
        return enableEpisodicMemory || enableWorkingMemory || 
               enableSemanticMemory || enableProceduralMemory || enableAssociativeMemory;
    }
    
    std::string getDescription() const override {
        return "Memory experiment testing formation, storage, and retrieval of memories";
    }
    
public:
    // Configuration
    bool enableMemoryExperiment = true;
    bool enableEpisodicMemory = true;
    bool enableWorkingMemory = true;
    bool enableSemanticMemory = false;
    bool enableProceduralMemory = false;
    bool enableAssociativeMemory = true;
    uint64_t memorySteps = 2000;
    uint64_t maxEpisodicEpisodes = 1000;
    uint64_t workingMemoryCapacity = 100;
    double memoryReplayInterval = 100.0;
    float memoryConsolidationStrength = 0.3f;
    bool enableMemoryInterference = true;
    std::string memoryTask = "sequential_task_learning";
    bool enableForgetting = true;
    double forgettingRate = 0.001;
    bool enableMemoryCompression = true;
    float compressionRatio = 0.5f;
    bool enableMemoryReconstruction = true;
};

// Comprehensive experiment configuration
class ComprehensiveExperimentConfig : public ExperimentConfig {
public:
    const std::string& getType() const override { static const std::string type = "comprehensive"; return type; }
    bool isEnabled() const override { return enableComprehensive; }
    
    // Enable all experiment types
    const LearningExperimentConfig& getLearningConfig() const { return learningConfig; }
    const AdaptationExperimentConfig& getAdaptationConfig() const { return adaptationConfig; }
    const SocialLearningExperimentConfig& getSocialConfig() const { return socialConfig; }
    const MemoryExperimentConfig& getMemoryConfig() const { return memoryConfig; }
    
    uint64_t getMaxSteps() const override { return comprehensiveSteps; }
    
    // Comprehensive parameters
    bool validate() const override {
        return learningConfig.isEnabled() || adaptationConfig.isEnabled() ||
               socialConfig.isEnabled() || memoryConfig.isEnabled();
    }
    
    std::string getDescription() const override {
        return "Comprehensive experiment integrating learning, adaptation, social learning, and memory";
    }
    
public:
    // Configuration
    bool enableComprehensive = true;
    uint64_t comprehensiveSteps = 5000;
    double globalTimestep = 0.001;
    float globalLearningRate = 0.01f;
    bool enableCrossSystemIntegration = true;
    bool enableEmergentBehaviors = true;
    std::string integrationMode = "sequential";
    float crossSystemCoupling = 0.5f;
    
    LearningExperimentConfig learningConfig;
    AdaptationExperimentConfig adaptationConfig;
    SocialLearningExperimentConfig socialConfig;
    MemoryExperimentConfig memoryConfig;
};

// Experiment configuration factory
class ExperimentConfigFactory {
public:
    static std::unique_ptr<ExperimentConfig> createConfig(const std::string& type) {
        std::string lowerType = type;
        std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
        
        if (lowerType == "learning") {
            return std::make_unique<LearningExperimentConfig>();
        } else if (lowerType == "adaptation") {
            return std::make_unique<AdaptationExperimentConfig>();
        } else if (lowerType == "social") {
            return std::make_unique<SocialLearningExperimentConfig>();
        } else if (lowerType == "memory") {
            return std::make_unique<MemoryExperimentConfig>();
        } else if (lowerType == "comprehensive") {
            return std::make_unique<ComprehensiveExperimentConfig>();
        } else if (lowerType == "default") {
            return std::make_unique<LearningExperimentConfig>();
        }
        
        // Return a default config if type is unknown
        return std::make_unique<LearningExperimentConfig>();
    }
    
    static std::vector<std::string> getAvailableExperimentTypes() {
        return {"learning", "adaptation", "social", "memory", "comprehensive", "default"};
    }
    
    static std::string getExperimentDescription(const std::string& type) {
        auto config = createConfig(type);
        return config->getDescription();
    }
};

// Global experiment configuration access
ExperimentConfig& getExperimentConfig();
void setExperimentConfig(std::unique_ptr<ExperimentConfig> config);
std::string getCurrentExperimentType();

// Convenience macros for experiment configuration
#define NLM_SET_EXPERIMENT_TYPE(type) nlm::setExperimentConfig(nlm::ExperimentConfigFactory::createConfig(type))
#define NLM_GET_EXPERIMENT_TYPE() nlm::getCurrentExperimentType()
#define NLM_GET_EXPERIMENT_DESCRIPTION() nlm::getExperimentConfig().getDescription()

// Experiment-specific macros
#define NLM_ENABLE_LEARNING(enable) nlm::getExperimentConfig().setValue("enable_learning", enable)
#define NLM_ENABLE_ADAPTATION(enable) nlm::getExperimentConfig().setValue("enable_adaptation", enable)
#define NLM_ENABLE_SOCIAL(enable) nlm::getExperimentConfig().setValue("enable_social", enable)
#define NLM_ENABLE_MEMORY(enable) nlm::getExperimentConfig().setValue("enable_memory", enable)

// Parameter macros
#define NLM_SET_NEURONS(count) nlm::getExperimentConfig().setValue("neuron_count", count)
#define NLM_SET_STEPS(count) nlm::getExperimentConfig().setValue("max_steps", count)
#define NLM_SET_TIMESTEP(time) nlm::getExperimentConfig().setValue("timestep", time)
#define NLM_SET_LEARNING_RATE(rate) nlm::getExperimentConfig().setValue("learning_rate", rate)

} // namespace nlm
