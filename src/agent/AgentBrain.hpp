#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for internal implementation classes
class MotorCommandSelector;
class NoveltyDetector;
class CuriosityEngine;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding using modular components
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world - sets up sensory and motor neuron mappings
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain - returns dimensions based on world config
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected - returns number of motor commands available
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain - extracts features and excites relevant neurons
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command - evaluates motor neuron activity and selects action
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward - implements reward-modulated learning
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system - progresses developmental stage based on age and plasticity
    void updateDevelopment(double timestep);
    
    // Get current developmental stage - returns neural development phase
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level - returns current dopamine-like signal strength
    float getNeuromodulationLevel() const;
    
    // Get curiosity level - returns exploration drive based on novelty and prediction error
    float getCuriosityLevel() const;
    
    // Get novelty level - returns measure of environmental change detection
    float getNoveltyLevel() const;
    
    // Get prediction error - returns reward prediction error for learning
    float getPredictionError() const;
    
    // Reset agent for new episode - clears neuromodulation state and resets novelty tracking
    void reset();
    
    // Get brain pointer - returns underlying brain reference for direct access
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods - enable/disable behavioral subsystems
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
    // Analyzes activity patterns across motor neuron groups
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    // Adds exploration behavior when curiosity is high
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Component objects for modularity
    MotorCommandSelector* motorSelector_;
    NoveltyDetector* noveltyDetector_;
    CuriosityEngine* curiosityEngine_;
    
    // Motor neuron groups - organized by action type
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups - organized by sensory modality
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state variables
    float dopamineLevel_;           // Reward prediction error signal
    float noveltyLevel_;            // Novelty detection output
    float curiosityLevel_;          // Exploration drive
    float predictionError_;         // Reward prediction error
    float expectedReward_;          // Expected reward for prediction
    
    // Development state
    double developmentalAge_;       // Neural development age in simulation time
    float plasticityModifier_;      // Plasticity modulation based on development
    
    // Configuration flags for behavioral subsystems
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;      // Decay rate for novelty signals
};

} // namespace nlm