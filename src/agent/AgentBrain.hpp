#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding for real-world interaction
// Acts as the interface between neural processing and environmental interaction
class AgentBrain {
public:
    // Create agent brain with neural processing core
    // The agent brain provides the interface between neural computation and world interaction
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    ~AgentBrain();
    
    // Initialize agent with world model
    // Sets up sensory neuron groups and motor command mapping based on world dimensions
    void initialize(const SimpleWorld& world);
    
    // Sensory and motor system configuration
    // Returns the size of sensory input vector expected by the brain
    size_t getSensoryInputSize() const;
    
    // Returns the size of motor output vector produced by the brain
    size_t getMotorOutputSize() const;
    
    // Sensory processing
    // Transforms environmental percepts into neural currents
    // Updates working memory and novelty detection based on sensory input
    void processSensoryInput(const SensoryPercept& percept);
    
    // Motor decoding
    // Converts neural activity patterns into actionable motor commands
    // Applies curiosity-driven exploration when appropriate
    MotorCommand decodeMotorCommand();
    
    // Reward-based learning
    // Applies reward prediction error to neural plasticity
    // Updates dopamine levels and eligibility traces
    void applyRewardModulation(float reward, float predictedReward);
    
    // Developmental progression
    // Advances agent's developmental stage based on simulated age
    // Modulates plasticity rates according to developmental stage
    void updateDevelopment(double timestep);
    
    // State queries
    // Returns current developmental stage (Initial, CriticalPeriod, Maturation, Adult)
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Neuromodulation state
    // Returns current dopamine level (0.0 to 1.0 scale)
    float getNeuromodulationLevel() const;
    
    // Motivation state
    // Returns curiosity level (0.0 to 1.0 scale)
    float getCuriosityLevel() const;
    
    // Learning state
    // Returns novelty detection level (0.0 to 1.0 scale)
    float getNoveltyLevel() const;
    
    // Prediction error signal
    // Returns the difference between expected and actual reward
    float getPredictionError() const;
    
    // Reset agent state for new episode
    // Clears neuromodulation, novelty, and developmental state
    void reset();
    
    // Brain access
    // Returns pointer to underlying brain for direct access when needed
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Subsystem configuration
    // Enable/disable specific learning and adaptation subsystems
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Subsystem status
    // Query whether subsystems are enabled
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Neural processing core
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron group mapping
    // Each action has a dedicated group of motor neurons
    std::vector<Neuron*> motorForward_;    // Move forward
    std::vector<Neuron*> motorBackward_;  // Move backward
    std::vector<Neuron*> motorTurnLeft_;   // Rotate left
    std::vector<Neuron*> motorTurnRight_;  // Rotate right
    std::vector<Neuron*> motorInteract_;   // Interact with objects
    std::vector<Neuron*> motorWait_;       // Stay still
    
    // Sensory neuron group mapping
    // Different sensory modalities use separate neuron populations
    std::vector<Neuron*> sensoryVision_;          // Visual input (256 neurons)
    std::vector<Neuron*> sensoryTouch_;           // Touch input (8 neurons)
    std::vector<Neuron*> sensoryInternal_;        // Internal state (4 neurons)
    std::vector<Neuron*> sensoryProprioception_;  // Body position (6 neurons)
    
    // Neuromodulation state
    float dopamineLevel_;      // Reward prediction error signal
    float noveltyLevel_;       // Novelty detection
    float curiosityLevel_;    // Exploration motivation
    float predictionError_;   // Prediction error (reward - expected)
    float expectedReward_;    // Running estimate of expected reward
    
    // Development state
    double developmentalAge_;  // Simulated agent age in steps
    float plasticityModifier_; // Modulates plasticity rates by developmental stage
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // State persistence
    // Previous vision state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_; // Decay rate for novelty signals
};

} // namespace nlm
