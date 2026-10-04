#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace nlm {

// Forward declarations
class Neuron;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Provides interface for agent to interact with world through sensory input and motor output
// Uses weak_ptr for neuron pointers to avoid circular dependencies and memory ownership issues

class AgentBrain {
public:
    // Constructor - creates an agent brain interface from a neural brain instance
    // @param brain Shared pointer to the NLM brain that will process sensory and motor signals
    // @note Uses weak references to brain neurons to avoid circular dependencies and memory ownership issues
    AgentBrain(std::shared_ptr<Brain> brain);
    
    // Destructor - cleans up agent brain resources
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world - sets up agent for world interaction
    // @param world Reference to the SimpleWorld this agent will operate in
    // @pre Must be called before processing sensory input or decoding motor commands
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    // @return Size of sensory vector expected by the brain's input layer
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    // @return Size of motor output vector expected by the agent's motor system
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // @param percept Sensory percept from the world containing vision, touch, internal signals
    // @post Current is injected into appropriate sensory neurons in the brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // @return MotorCommand representing the agent's intended action based on brain activity
    // @note Applies curiosity-based exploration when enabled and novelty is high
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    // @param reward Reward value from world interaction (positive for good outcomes, negative for bad)
    // @param predictedReward Expected reward based on agent's prediction
    // @post Updates dopamine levels and eligibility traces for reward-modulated learning
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    // @param timestep Time step duration for developmental changes
    // @post Updates developmental stage and plasticity modifier based on agent age
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    // @return DevelopmentalStage representing current maturation phase
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    // @return Current dopamine level (-1.0 to 1.0, negative = punishment, positive = reward)
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // @return Current curiosity level (0.0 to 1.0, drives exploration of novel stimuli)
    float getCuriosityLevel() const;
    
    // Get novelty level
    // @return Current novelty level (0.0 to 1.0, based on sensory change detection)
    float getNoveltyLevel() const;
    
    // Get prediction error
    // @return Prediction error (reward - expected_reward, drives learning)
    float getPredictionError() const;
    
    // Reset agent for new episode
    // @post Resets all neuromodulation states, curiosity, and internal sensory buffers
    void reset();
    
    // Get brain pointer
    // @return Pointer to underlying NLM brain (const version)
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    // Enable/disable neuromodulation based on reward signals
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
    // Analyzes activity in different motor neuron groups and selects appropriate action
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    // Occasionally chooses random action when curiosity is high for exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - each group represents a specific action
    // Uses weak_ptr to avoid circular dependencies and memory ownership issues
    std::vector<std::weak_ptr<Neuron>> motorForward_;      // Move forward action
    std::vector<std::weak_ptr<Neuron>> motorBackward_;     // Move backward action
    std::vector<std::weak_ptr<Neuron>> motorTurnLeft_;      // Turn left action
    std::vector<std::weak_ptr<Neuron>> motorTurnRight_;     // Turn right action
    std::vector<std::weak_ptr<Neuron>> motorInteract_;      // Interact with objects action
    std::vector<std::weak_ptr<Neuron>> motorWait_;          // No action (wait) 
    
    // Sensory neuron groups - each group processes different sensory modalities
    std::vector<std::weak_ptr<Neuron>> sensoryVision_;              // Visual input processing
    std::vector<std::weak_ptr<Neuron>> sensoryTouch_;               // Touch/collision sensing
    std::vector<std::weak_ptr<Neuron>> sensoryInternal_;            // Internal physiological signals
    std::vector<std::weak_ptr<Neuron>> sensoryProprioception_;       // Body position sensing
    
    // Neuromodulation state
    float dopamineLevel_;        // Reward prediction error signal (-1.0 to 1.0)
    float noveltyLevel_;          // Novelty detection signal (0.0 to 1.0)
    float curiosityLevel_;       // Exploration motivation (0.0 to 1.0)
    float predictionError_;      // Reward prediction error signal
    float expectedReward_;       // Expected reward (exponential moving average)
    
    // Development state
    double developmentalAge_;    // Agent age in simulation steps
    float plasticityModifier_;  // Plasticity scaling factor (decreases with age)
    
    // Configuration flags
    bool rewardModulationEnabled_;     // Whether reward-based learning is active
    bool structuralPlasticityEnabled_; // Whether structural synaptic changes are allowed
    bool developmentEnabled_;          // Whether developmental changes occur
    bool curiosityEnabled_;           // Whether curiosity-driven exploration is active
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;  // Previous vision frame for change detection
    float sensoryNoveltyDecay_;           // Decay factor for novelty signal
};

} // namespace nlm