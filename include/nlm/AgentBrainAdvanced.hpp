#pragma once

#include "AgentBrain.hpp"
#include "../../core/Config/Config.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

// Agent configuration structure
struct AgentConfig {
    // Behavioral parameters
    float curiosityThreshold = 0.3f;
    float noveltyDecay = 0.99f;
    float rewardWeight = 1.0f;
    float explorationProbability = 0.1f;
    
    // Developmental parameters
    float developmentalAge = 0.0;
    float plasticityModifier = 1.0f;
    float structuralPlasticityRate = 0.0001;
    float pruningRate = 0.00001;
    
    // Neuromodulation parameters
    float dopamineSensitivity = 1.0f;
    float curiosityBaseline = 0.1f;
    float noveltyThreshold = 0.2f;
    
    // Memory parameters
    size_t workingMemoryCapacity = 100;
    size_t episodicMemoryLimit = 1000;
    float memoryRetentionRate = 0.95f;
    
    // Sensory parameters
    float visionSensitivity = 1.0f;
    float touchSensitivity = 1.0f;
    float internalSensitivity = 1.0f;
    float proprioceptionSensitivity = 1.0f;
    
    // Motor parameters
    float motorNoise = 0.05f;
    float actionSelectionTemperature = 1.0f;
    float rewardHistoryLength = 100.0f;
    
    // Development parameters
    float developmentalStageSpeed = 1.0f;
    float criticalPeriodDuration = 60.0f;
    float maturationThreshold = 300.0f;
    
    // Social parameters
    float imitationProbability = 0.3f;
    float observationWeight = 0.5f;
    float socialLearningRate = 0.1f;
};

// Advanced AgentBrain with configuration support
class AgentBrainAdvanced : public AgentBrain {
public:
    AgentBrainAdvanced(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config = nullptr);
    ~AgentBrainAdvanced() override;
    
    // Configuration management
    void loadConfiguration(const std::string& configFile);
    void loadFromConfig(std::shared_ptr<Config> config);
    void applyConfiguration(const AgentConfig& agentConfig);
    
    // Configuration access
    const AgentConfig& getConfiguration() const { return agentConfig_; }
    AgentConfig& getMutableConfiguration() { return agentConfig_; }
    
    // Behavioral controls
    void enableBehavioralFeature(const std::string& feature, bool enable);
    bool isBehavioralFeatureEnabled(const std::string& feature) const;
    
    // Configuration validation
    std::vector<std::string> validateConfiguration() const;
    
    // Configuration export/import
    std::string exportConfigurationToJSON() const;
    bool importConfigurationFromJSON(const std::string& json);
    
    // Configuration statistics
    std::string getConfigurationStatistics() const;
    
    // Advanced behavioral methods
    void applyAdvancedCuriosity(const SensoryPercept& percept);
    void applyAdvancedRewardModulation(float reward, float predictedReward, const std::string& context);
    void applyAdvancedDevelopment(double timestep, const std::string& environmentalFactors);
    
    // Configuration helpers
    void setCuriosityParameters(float baseline, float threshold, float decay);
    void setRewardParameters(float weight, float sensitivity);
    void setDevelopmentParameters(float stageSpeed, float criticalPeriod, float maturation);
    void setMemoryParameters(size_t workingCapacity, size_t episodicLimit, float retention);
    void setSensoryParameters(float vision, float touch, float internal, float proprioception);
    void setMotorParameters(float noise, float temperature, float historyLength);
    void setSocialParameters(float imitation, float observation, float learningRate);
    
    // Configuration presets
    void loadPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Configuration templates
    static AgentConfig getDefaultConfiguration();
    static AgentConfig getOptimizedForLearning();
    static AgentConfig getOptimizedForSpeed();
    static AgentConfig getOptimizedForMemory();
    static AgentConfig getOptimizedForSocialLearning();
    
private:
    AgentConfig agentConfig_;
    
    // Behavioral feature flags
    std::map<std::string, bool> behavioralFeatures_;
    
    // Configuration state
    std::map<std::string, float> parameterHistory_;
    double lastConfigurationUpdate_;
    
    // Helper methods
    void updateConfigurationHistory(const std::string& parameter, float value);
    float getHistoricalAverage(const std::string& parameter, size_t windowSize) const;
    bool isParameterInRange(float value, float min, float max) const;
    std::string parameterTypeToString(float value) const;
};

} // namespace nlm
