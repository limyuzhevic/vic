#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Exceptions.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Error checking utility functions for AgentBrain operations
namespace AgentBrainValidation {
    // Input validation for agent brain operations
    inline void validatePercept(const SensoryPercept& percept, const std::string& operation = "") {
        // Basic validation - can be enhanced based on specific requirements
        if (percept.getVisionWidth() == 0 || percept.getVisionHeight() == 0) {
            NLM_THROW_AND_LOG(NLMValidationException,
                             "Invalid sensory percept dimensions: " + 
                             std::to_string(percept.getVisionWidth()) + "x" + 
                             std::to_string(percept.getVisionHeight()),
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMValidationException::ValidationError::INVALID_FORMAT),
                             "agentbrain_percept_validation");
        }
    }
    
    inline void validateReward(float reward, float predictedReward, const std::string& operation = "") {
        if (std::isnan(reward) || std::isinf(reward)) {
            NLM_THROW_AND_LOG(NLMValidationException,
                             "Invalid reward value: " + std::to_string(reward),
                             NLMException::Severity::WARNING,
                             static_cast<int>(NLMValidationException::ValidationError::INVALID_TYPE),
                             "agentbrain_reward_validation");
        }
    }
    
    inline void validateTimestep(double timestep) {
        if (timestep <= 0.0) {
            NLM_THROW_AND_LOG(NLMValidationException,
                             "Timestep must be positive, got: " + std::to_string(timestep),
                             NLMException::Severity::ERROR,
                             static_cast<int>(NLMValidationException::ValidationError::VALUE_OUT_OF_RANGE),
                             "agentbrain_timestep_validation");
        }
    }
}

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
