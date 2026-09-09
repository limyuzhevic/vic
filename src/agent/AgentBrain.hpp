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
// Provides interface for agent behavior and interaction with environment
// Includes neuromodulation, reward processing, and developmental mechanisms
class AgentBrain {
public:
    // Create agent brain with underlying C++ brain
    // Validates brain pointer to ensure safe operation
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying (brain management is complex), enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // ========== INITIALIZATION ==========
    // Initialize agent brain with world configuration
    // Sets up neuron mapping and prepares for sensory processing
    void initialize(const SimpleWorld& world);
    
    // ========== INPUT/OUTPUT SIZES ==========
    // Get expected sensory input dimensions from brain
    // Used for creating appropriately sized perceptual structures
    size_t getSensoryInputSize() const;
    
    // Get expected motor output dimensions from brain
    // Used for creating action structures with correct parameter count
    size_t getMotorOutputSize() const;
    
    // ========== SENSORY PROCESSING ==========
    // Process sensory percept and inject into brain
    // Converts high-level percept into neural current injection
    void processSensoryInput(const SensoryPercept& percept);
    
    // ========== MOTOR CONTROL ==========
    // Decode brain motor activity into motor command
    // Translates population-level neural activity into discrete actions
    MotorCommand decodeMotorCommand();
    
    // ========== NEUROMODULATION ==========
    // Apply reward-based neuromodulation
    // Implements reward prediction error and dopamine signaling
    void applyRewardModulation(float reward, float predictedReward);
    
    // ========== DEVELOPMENT ==========
    // Update development system
    // Handles developmental stage progression and plasticity changes
    void updateDevelopment(double timestep);
    
    // ========== STATE ACCESS ==========
    // Get current developmental stage
    // Determines which behavioral and plasticity regimes are active
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get current neuromodulation level (dopamine)
    // Reflects prediction error and motivation state
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // Drives exploration and novelty seeking behavior
    float getCuriosityLevel() const;
    
    // Get novelty level
    // Detects unexpected environmental changes
    float getNoveltyLevel() const;
    
    // Get prediction error
    // Difference between expected and actual reward
    float getPredictionError() const;
    
    // ========== RESET ==========
    // Reset agent for new episode
    // Clears persistent state and prepares for fresh start
    void reset();
    
    // ========== BRAIN ACCESS ==========
    // Get pointer to underlying brain
    // Provides direct access to neural system for advanced use
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // ========== CONFIGURATION ==========
    // Enable or disable specific neuromodulation systems
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    // Check if neuromodulation systems are enabled
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
private:
    // ========== MOTOR DECODING ==========
    // Convert neural activity patterns into motor commands
    MotorCommand decodeFromMotorNeurons();
    
    // ========== EXPLORATION ==========
    // Select motor command with curiosity-driven exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // ========== BRAIN POINTER ==========
    std::shared_ptr<Brain> brain_;
    
    // ========== NEURON MAPPINGS ==========
    // Motor neuron groups for action selection
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups for input processing
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // ========== NEUROMODULATION STATE ==========
    float dopamineLevel_;      // Reward prediction error signal
    float noveltyLevel_;       // Environmental novelty detection
    float curiosityLevel_;     // Exploration motivation
    float predictionError_;    // Reward prediction error
    float expectedReward_;     // Expected value estimate
    
    // ========== DEVELOPMENTAL STATE ==========
    double developmentalAge_;         // Age in simulation steps
    float plasticityModifier_;       // Modulates learning rates
    
    // ========== CONFIGURATION FLAGS ==========
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // ========== SENSORY HISTORY ==========
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;  // Rate of novelty decay
};

} // namespace nlm
