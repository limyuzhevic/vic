// BrainBuilder.hpp - Factory pattern for creating NLM brain objects
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

class Config;
class Brain;
class AgentBrain;
class SimpleWorld;

class BrainBuilder {
public:
    BrainBuilder() = default;
    ~BrainBuilder() = default;
    
    // Copy and move operations
    BrainBuilder(const BrainBuilder&) = delete;
    BrainBuilder& operator=(const BrainBuilder&) = delete;
    BrainBuilder(BrainBuilder&&) = default;
    BrainBuilder& operator=(BrainBuilder&&) = default;
    
    // Configuration methods
    BrainBuilder& withConfig(std::shared_ptr<Config> config);
    BrainBuilder& withNeuronCount(size_t neuronCount);
    BrainBuilder& withRegionCount(size_t regionCount);
    BrainBuilder& withConnectionProbability(float probability);
    BrainBuilder& addCustomConfig(const std::string& key, const std::string& value);
    BrainBuilder& addCustomConfig(const std::string& key, int value);
    BrainBuilder& addCustomConfig(const std::string& key, float value);
    BrainBuilder& addCustomConfig(const std::string& key, double value);
    
    // World configuration
    BrainBuilder& withWorldSize(size_t width, size_t height);
    BrainBuilder& withVisionSize(size_t visionWidth, size_t visionHeight);
    
    // Development configuration
    BrainBuilder& withDevelopmentStage(const std::string& stage);
    BrainBuilder& enableRewardModulation(bool enable = true);
    BrainBuilder& enableStructuralPlasticity(bool enable = true);
    BrainBuilder& enableDevelopment(bool enable = true);
    BrainBuilder& enableCuriosity(bool enable = true);
    
    // Brain construction
    std::shared_ptr<Brain> buildBrain();
    std::shared_ptr<AgentBrain> buildAgentBrain(std::shared_ptr<SimpleWorld> world = nullptr);
    
    // Reset builder for reuse
    void reset();
    
    // Get current configuration summary
    std::string getConfigSummary() const;
    
private:
    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, std::string> customConfigs_;
    
    size_t neuronCount_ = 1000;
    size_t regionCount_ = 1;
    float connectionProbability_ = 0.1f;
    
    // World settings
    size_t worldWidth_ = 100;
    size_t worldHeight_ = 100;
    size_t visionWidth_ = 16;
    size_t visionHeight_ = 16;
    
    // Development flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
};

// Convenience functions for common use cases
std::shared_ptr<Brain> createDefaultBrain();
std::shared_ptr<Brain> createMinimalBrain(size_t neuronCount = 100);
std::shared_ptr<Brain> createDevelopmentBrain();
std::shared_ptr<Brain> createReinforcementBrain();
}