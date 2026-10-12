#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

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
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
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
    
    // Novelty detection decay rate (initialized in constructor)
    float sensoryNoveltyDecay_;
    
    // Constants for better maintainability
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float VISION_SCALE_FACTOR = 5.0f;
    static constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    static constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    static constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    static constexpr float MIN_ACTIVITY_THRESHOLD = 0.3f;
    static constexpr float MAX_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    static constexpr float MIN_NOVELTY_THRESHOLD = 0.001f;
    static constexpr float MAX_NOVELTY_LEVEL = 1.0f;
    static constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
    static constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
    static constexpr float MAX_PLATIVITY_FACTOR = 2.0f;
    static constexpr float MIN_PLATIVITY_FACTOR = 0.1f;
    static constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    static constexpr float MIN_DEVELOPMENTAL_AGE = 60.0;
    static constexpr float MIN_CRITICAL_PERIOD_AGE = 300.0;
    static constexpr float MIN_MATURATION_AGE = 900.0;
    static constexpr float MAX_DEVELOPMENTAL_AGE = 900.0;
    static constexpr float MAX_Sensory_NOVELTY_DECAY_RATE = 1.0f;
    static constexpr float MIN_Sensory_NOVELTY_DECAY_RATE = 0.0f;
};

} // namespace nlm
