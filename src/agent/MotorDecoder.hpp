#pragma once

#include "../brain/Brain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBrain.hpp"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

// Motor decoding strategy interface
class MotorDecodingStrategy {
public:
    virtual ~MotorDecodingStrategy() = default;
    virtual MotorCommand decode(const MotorDecodingContext& context) = 0;
    virtual std::string getName() const = 0;
};

// Motor decoding context
struct MotorDecodingContext {
    std::shared_ptr<Brain> brain;
    float forwardActivity;
    float backwardActivity;
    float leftActivity;
    float rightActivity;
    float interactActivity;
    float waitActivity;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    float dopamineLevel;
    float plasticityModifier;
    const SimpleWorld* world;
};

// Motor configuration structure
struct MotorConfig {
    // Threshold-based decoding
    float activityThreshold_ = 0.5f;
    bool useThresholdBased_ = true;
    
    // Weighted voting
    float weightForward_ = 1.0f;
    float weightBackward_ = 1.0f;
    float weightLeft_ = 1.0f;
    float weightRight_ = 1.0f;
    float weightInteract_ = 1.0f;
    float weightWait_ = 1.0f;
    bool useWeightedVoting_ = false;
    
    // Top-N decoding
    int topN_ = 3;
    bool useTopN_ = false;
    
    // Adaptive threshold
    float adaptiveThresholdBase_ = 0.5f;
    float adaptiveThresholdMod_ = 0.2f;
    bool useAdaptiveThreshold_ = false;
    
    // Exploration parameters
    float explorationThreshold_ = 0.3f;
    float curiosityWeight_ = 1.0f;
    bool enableExploration_ = true;
    
    // Action space
    int actionSpaceSize_ = 6;
    bool customActionSpace_ = false;
    
    // Validation parameters
    bool requirePositiveActivity_ = true;
    float minActivityDifference_ = 0.1f;
    
    // Debug
    bool enableDebugOutput_ = false;
};

// Motor decoder implementation
class MotorDecoder {
public:
    MotorDecoder(std::shared_ptr<Brain> brain);
    ~MotorDecoder() = default;
    
    // Configuration
    void configure(const MotorConfig& config);
    const MotorConfig& getConfig() const { return config_; }
    
    // Set sensory neuron groups
    void setSensoryNeuronGroups(
        const std::vector<Neuron*>& vision,
        const std::vector<Neuron*>& touch,
        const std::vector<Neuron*>& internal,
        const std::vector<Neuron*>& proprioception);
    
    // Set world reference
    void setWorld(const SimpleWorld* world) { world_ = world; }
    
    // Motor decoding
    MotorCommand decodeMotorCommand();
    
    // Component access
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Strategy management
    void setStrategy(std::unique_ptr<MotorDecodingStrategy> strategy);
    
    // Configuration updates
    void updateActivityWeights(float forward, float backward, float left, float right, 
                               float interact, float wait);
    
    // Exploration
    MotorCommand exploreWithCuriosity(MotorCommand defaultCmd);
    
    // Statistics
    float getAverageActivity() const;
    float getMaxActivity() const;
    size_t getActiveMotorNeurons() const;
    
    // Debug
    void enableDebug(bool enable) { config_.enableDebugOutput_ = enable; }
    
private:
    std::shared_ptr<Brain> brain_;
    const SimpleWorld* world_;
    MotorConfig config_;
    
    // Motor neuron groups (legacy support)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Activity statistics
    float currentForwardActivity_;
    float currentBackwardActivity_;
    float currentLeftActivity_;
    float currentRightActivity_;
    float currentInteractActivity_;
    float currentWaitActivity_;
    float lastMaxActivity_;
    
    // Strategy
    std::unique_ptr<MotorDecodingStrategy> strategy_;
    
    // Threshold-based strategy
    class ThresholdStrategy : public MotorDecodingStrategy {
    public:
        MotorCommand decode(const MotorDecodingContext& context) override;
        std::string getName() const override { return "ThresholdBased"; }
    };
    
    // Weighted voting strategy
    class WeightedVotingStrategy : public MotorDecodingStrategy {
    public:
        MotorCommand decode(const MotorDecodingContext& context) override;
        std::string getName() const override { return "WeightedVoting"; }
    };
    
    // Top-N strategy
    class TopNStrategy : public MotorDecodingStrategy {
    public:
        MotorCommand decode(const MotorDecodingContext& context) override;
        std::string getName() const override { return "TopN"; }
    };
    
    // Adaptive threshold strategy
    class AdaptiveThresholdStrategy : public MotorDecodingStrategy {
    public:
        MotorCommand decode(const MotorDecodingContext& context) override;
        std::string getName() const override { return "AdaptiveThreshold"; }
    };
};

} // namespace nlm
