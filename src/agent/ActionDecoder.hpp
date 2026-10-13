#pragma once

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <random>

namespace nlm {

/**
 * ActionDecoder - Decodes neural activity into motor commands
 * Extracted from AgentBrain to improve separation of concerns
 */
class ActionDecoder {
public:
    ActionDecoder(std::shared_ptr<Brain> brain);
    ~ActionDecoder() = default;
    
    // Disable copying, enable moving
    ActionDecoder(const ActionDecoder&) = delete;
    ActionDecoder& operator=(const ActionDecoder&) = delete;
    ActionDecoder(ActionDecoder&&) noexcept;
    ActionDecoder& operator=(ActionDecoder&&) noexcept;
    
    // Decode motor command from neural activity
    MotorCommand decodeMotorCommand();
    
    // Apply curiosity-based exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel);
    
    // Get motor neuron distributions
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    // Configuration
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setRandomExploration(bool enabled) { randomExplorationEnabled_ = enabled; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    
    // State management
    void reset() {
        motorActivity_.clear();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron distributions
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Neural activity tracking
    std::unordered_map<MotorCommand, float> motorActivity_;
    
    // Configuration
    float activityThreshold_ = 0.5f;
    bool randomExplorationEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Random number generator
    std::mt19937 rng_;
};

/**
 * NeuromodulationManager - Manages neuromodulatory signals and effects
 * Extracted from AgentBrain to improve separation of concerns
 */
class NeuromodulationManager {
public:
    NeuromodulationManager(std::shared_ptr<Brain> brain);
    ~NeuromodulationManager() = default;
    
    // Disable copying, enable moving
    NeuromodulationManager(const NeuromodulationManager&) = delete;
    NeuromodulationManager& operator=(const NeuromodulationManager&) = delete;
    NeuromodulationManager(NeuromodulationManager&&) noexcept;
    NeuromodulationManager& operator=(NeuromodulationManager&&) noexcept;
    
    // Apply reward modulation based on prediction error
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development effects
    void updateDevelopment(double timestep);
    
    // Get neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Configuration
    void setDevelopmentEnabled(bool enabled) { developmentEnabled_ = enabled; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    void setStructuralPlasticityEnabled(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    
    // State management
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0f;
        plasticityModifier_ = 1.0f;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation levels
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    float developmentalAge_ = 0.0f;
    float plasticityModifier_ = 1.0f;
    
    // Configuration flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    
    // Novelty decay factor
    float noveltyDecayFactor_ = 0.99f;
};

/**
 * AgentState - Maintains agent state and configuration
 * Extracted from AgentBrain to improve separation of concerns
 */
class AgentState {
public:
    AgentState();
    ~AgentState() = default;
    
    // Disable copying, enable moving
    AgentState(const AgentState&) = delete;
    AgentState& operator=(const AgentState&) = delete;
    AgentState(AgentState&&) noexcept;
    AgentState& operator=(AgentState&&) noexcept;
    
    // Get sensory input size
    size_t getSensoryInputSize() const {
        // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
        return 256 + 8 + 4 + 6;
    }
    
    // Get motor output size
    size_t getMotorOutputSize() const {
        // One motor neuron per action
        return 6;
    }
    
    // Configuration
    void setSensoryNoveltyDecay(float decay) { sensoryNoveltyDecay_ = decay; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    void setDevelopmentEnabled(bool enabled) { developmentEnabled_ = enabled; }
    void setStructuralPlasticityEnabled(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    
    // State management
    void reset() {
        developmentalAge_ = 0.0;
        plasticityModifier_ = 1.0f;
        curiosityEnabled_ = true;
        developmentEnabled_ = true;
        structuralPlasticityEnabled_ = true;
        rewardModulationEnabled_ = true;
    }
    
    // Get current configuration
    float getSensoryNoveltyDecay() const { return sensoryNoveltyDecay_; }
    bool getCuriosityEnabled() const { return curiosityEnabled_; }
    bool getDevelopmentEnabled() const { return developmentEnabled_; }
    bool getStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool getRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
private:
    // Agent configuration
    float sensoryNoveltyDecay_ = 0.99f;
    bool curiosityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool rewardModulationEnabled_ = true;
    
    // Development state
    double developmentalAge_ = 0.0;
    float plasticityModifier_ = 1.0f;
};

} // namespace nlm