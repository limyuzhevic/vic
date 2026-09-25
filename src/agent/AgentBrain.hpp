#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Copy operations
    AgentBrain(const AgentBrain& other);
    AgentBrain& operator=(const AgentBrain& other);
    
    // Move operations
    AgentBrain(AgentBrain&& other) noexcept;
    AgentBrain& operator=(AgentBrain&& other) noexcept;
    
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
    
    // Enhanced initialization with distribution system
    void initializeNeuronGroups();
    
    // Helper methods for sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    
    // Configuration constants for better maintainability
    static constexpr float DEFAULT_SENORY_NOVELTY_DECAY = 0.99f;
    static constexpr float DEFAULT_SENORY_SCALE_VISION = 5.0f;
    static constexpr float DEFAULT_SENORY_SCALE_TOUCH = 8.0f;
    static constexpr float DEFAULT_SENORY_SCALE_INTERNAL = 5.0f;
    static constexpr float DEFAULT_SENORY_SCALE_PROPRIO = 3.0f;
    
    // Constants for motor decoding
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float MOTOR_EXPLORATION_CHANCE = 0.3f;
    static constexpr float MOTOR_EXPLORATION_CURIOUSNESS = 0.5f;
    
    // Constants for neuromodulation
    static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    static constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;
    static constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced neuron distribution using smart pointers
    void distributeMotorNeurons(NeuralPopulation* population);
    void distributeSensoryNeurons(NeuralPopulation* population);
    
    // Optimized neuron activity calculation
    float calculateNeuronGroupActivity(const std::vector<Neuron*>& neurons) const;
    
    // Enhanced novelty detection with exponential moving average
    void updateNovelty(const std::vector<float>& currentVision);
    
    // Scale input values to neuron-appropriate ranges
    float scaleVisionInput(float value) const;
    float scaleTouchInput(float value) const;
    float scaleInternalInput(float value) const;
    float scaleProprioceptionInput(float value) const;
    
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
    
    // Random generator for exploration behavior
    std::mt19937 randomGenerator_;
};
