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
// Acts as the interface between neural processing and environmental interaction
// Integrates multiple learning systems (reward, curiosity, development, structural plasticity)
// 
// The AgentBrain is a crucial component that translates between:
// 1. External world (SensoryPercept objects)
// 2. Internal neural processing (Brain class)
// 3. Environmental action (MotorCommand objects)
// 
// It implements the brain loop described in the README:
// WORLD -> SENSORY INPUT -> NEURAL PROCESSING -> ACTION SELECTION -> MOTOR OUTPUT
// 
// AgentBrain implementation placeholder - Phase 6 integration will implement full dynamics

class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    // Parameters:
    //   world - Reference to the environment/world for sensory processing
    // Note: Establishes the initial connection between brain and world
    // Called at the start of each episode to reset world relationship
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    // Returns: Number of sensory channels the brain expects
    // Used for validating sensory percept dimensions
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    // Returns: Number of motor channels the brain produces
    // Used for validating motor command dimensions
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // Parameters:
    //   percept - Processed sensory information from the world
    // Converts world percepts to neural input (sensory transduction)
    // Updates brain's internal state based on environmental input
    // Triggers appropriate neural processing and plasticity
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // Returns: Motor command to send to world
    // Implements motor decoding: neural activity -> actionable output
    // Includes integration of learned behaviors and current state
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    // Parameters:
    //   reward - Actual reward received from world
    //   predictedReward - Expected reward based on current policy
    // Implements reward prediction error signaling
    // Updates neuromodulation levels (dopamine, curiosity, novelty)
    // Drives reward-modulated plasticity and learning
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    // Parameters:
    //   timestep - Time elapsed since last update (seconds)
    // Advances developmental state and plasticity rates
    // Implements age-dependent learning capabilities
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    // Returns: Current developmental stage
    // Determines which learning mechanisms are active
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    // Returns: Current neuromodulation level (typically dopamine)
    // Used for monitoring brain state and regulating plasticity
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // Returns: Current curiosity-driven exploration level
    // Higher curiosity = more exploration, better learning in novel environments
    float getCuriosityLevel() const;
    
    // Get novelty level
    // Returns: Current novelty detection level
    // Signals unexpected environmental changes
    float getNoveltyLevel() const;
    
    // Get prediction error
    // Returns: Current prediction error signal
    // Difference between expected and actual outcomes
    // Drives learning and adaptation
    float getPredictionError() const;
    
    // Reset agent for new episode
    // Resets all internal state for a fresh learning episode
    // Clears memory, neuromodulation, developmental state
    void reset();
    
    // Get brain pointer
    // Returns: Pointer to internal brain instance
    // Useful for direct brain access by other components
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    
    // Enable reward-modulated plasticity
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    // Enable structural plasticity
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    // Enable developmental regulation
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    // Enable curiosity-driven exploration
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Query configuration options
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    // Returns: Motor command derived from motor neuron activity
    // Implements the mapping from neural representations to actions
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    // Parameters:
    //   defaultCmd - Default motor command based on current policy
    // Returns: Motor command with curiosity-driven exploration
    // Implements exploration-exploitation tradeoff using curiosity signals
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;  // Pointer to internal brain instance
    
    // Motor neuron groups
    // These neurons represent different possible actions
    std::vector<Neuron*> motorForward_;    // Move forward action
    std::vector<Neuron*> motorBackward_;   // Move backward action
    std::vector<Neuron*> motorTurnLeft_;   // Turn left action
    std::vector<Neuron*> motorTurnRight_;  // Turn right action
    std::vector<Neuron*> motorInteract_;   // Interact with environment
    std::vector<Neuron*> motorWait_;       // Wait/idle action
    
    // Sensory neuron groups
    // These neurons represent different sensory modalities
    std::vector<Neuron*> sensoryVision_;           // Visual processing
    std::vector<Neuron*> sensoryTouch_;           // Touch/proprioception
    std::vector<Neuron*> sensoryInternal_;        // Internal state monitoring
    std::vector<Neuron*> sensoryProprioception_;  // Body position sensing
    
    // Neuromodulation state
    float dopamineLevel_;          // Reward prediction error signal
    float noveltyLevel_;           // Novelty detection signal
    float curiosityLevel_;         // Exploration motivation signal
    float predictionError_;        // Prediction error signal
    float expectedReward_;         // Expected reward prediction
    
    // Development state
    double developmentalAge_;      // Age of the system (simulation time)
    float plasticityModifier_;     // Current plasticity rate modifier
    
    // Configuration flags
    bool rewardModulationEnabled_;      // Whether reward modulation is active
    bool structuralPlasticityEnabled_; // Whether structural plasticity is active
    bool developmentEnabled_;           // Whether developmental regulation is active
    bool curiosityEnabled_;             // Whether curiosity-driven exploration is active
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;       // Previous visual input for novelty detection
    float sensoryNoveltyDecay_;                // Decay rate for novelty signals
};

} // namespace nlm
