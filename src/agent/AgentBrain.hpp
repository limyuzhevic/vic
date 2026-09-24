#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * Motor command configuration for customizing motor neuron categorization
 */
struct MotorCommandConfig {
    // Number of motor action groups
    int numMotorActions = 6;
    
    // Action names for better debugging and logging
    std::vector<std::string> actionNames;
    
    // Custom action distribution ratios (sum to 1.0)
    std::vector<float> actionRatios;
    
    // Enable/disable specific actions
    bool enableForward = true;
    bool enableBackward = true;
    bool enableTurnLeft = true;
    bool enableTurnRight = true;
    bool enableInteract = true;
    bool enableWait = true;
    
    // Custom action mapping from neuron indices
    // If empty, use default round-robin distribution
    std::vector<int> customActionMapping;
    
    MotorCommandConfig() {
        // Default action names
        actionNames = {"Forward", "Backward", "TurnLeft", "TurnRight", "Interact", "Wait"};
        
        // Default equal distribution
        actionRatios = {0.167f, 0.167f, 0.167f, 0.167f, 0.167f, 0.167f};
        
        // Validate
        if (actionNames.size() != numMotorActions) {
            actionNames.resize(numMotorActions, "Action");
        }
        if (actionRatios.size() != numMotorActions) {
            actionRatios.resize(numMotorActions, 1.0f / numMotorActions);
        }
        
        // Normalize ratios to sum to 1.0
        float total = 0.0f;
        for (float ratio : actionRatios) {
            total += ratio;
        }
        if (total > 0.0f) {
            for (float& ratio : actionRatios) {
                ratio /= total;
            }
        } else {
            for (size_t i = 0; i < actionRatios.size(); ++i) {
                actionRatios[i] = 1.0f / numMotorActions;
            }
        }
    }
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Motor configuration
    void setMotorConfig(const MotorCommandConfig& config) {
        motorConfig_ = config;
        applyMotorConfiguration();
    }
    
    const MotorCommandConfig& getMotorConfig() const { return motorConfig_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Motor command categorization settings
    MotorCommandConfig motorConfig_;
    
    // Apply motor configuration
    void applyMotorConfiguration();
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
