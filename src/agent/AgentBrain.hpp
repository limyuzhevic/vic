#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Manages neuromodulation, development, and curiosity-driven exploration
class AgentBrain {
public:
    // Constructor: Initializes brain connection with null state
    // @param brain Shared pointer to the neural brain instance
    AgentBrain(std::shared_ptr<Brain> brain);
    
    // Destructor: Cleans up brain connection
    ~AgentBrain();
    
    // Initialize with world context
    // Sets up sensory buffers and initializes developmental state
    // @param world Reference to the simulation world for context
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    // Returns total number of sensory values expected by the brain
    // @return Number of sensory inputs (vision + touch + internal + proprioception)
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    // Returns number of motor neuron groups this brain produces
    // @return Number of motor outputs (typically 6: forward, backward, left, right, interact, wait)
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // Converts sensory data into neural currents and updates internal state
    // @param percept Sensory percept containing vision, touch, internal, and proprioception data
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // Determines best motor command based on neural activity levels
    // Applies curiosity-based exploration when enabled
    // @return Selected motor command for current timestep
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    // Updates dopamine levels and applies reward prediction error
    // @param reward Actual reward received from world
    // @param predictedReward Expected reward based on prediction
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    // Advances developmental stage based on elapsed time
    // Adjusts plasticity and structural plasticity rates
    // @param timestep Time elapsed since last update (seconds)
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    // Returns the current developmental stage of the agent
    // @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    // Returns the current dopamine/neuromodulation level
    // @return Neuromodulation level (-1.0 to 1.0)
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // Returns the current curiosity-driven exploration motivation
    // @return Curiosity level (0.0 to 1.0)
    float getCuriosityLevel() const;
    
    // Get novelty level
    // Returns the current novelty detection signal
    // @return Novelty level (0.0 to 1.0)
    float getNoveltyLevel() const;
    
    // Get prediction error
    // Returns the reward prediction error signal
    // @return Prediction error (-1.0 to 1.0)
    float getPredictionError() const;
    
    // Get current action sequence (list of action types in order)
    // Returns vector of ActionType values representing the agent's current plan/behavior sequence
    // @return Vector of ActionType values (empty if not implemented)
    std::vector<ActionType> getActionSequence() const;
    
    // Get motor activity level for a specific motor command
    // Returns the average activity level (0.0-1.0) for the specified motor command
    // @param cmd Motor command to query activity for
    // @return Activity level (0.0 = no activity, 1.0 = maximum activity)
    float getMotorActivityLevel(MotorCommand cmd) const;
    
    // Get motor activity vector for all commands
    // Returns vector of activity levels for each motor command in standard order
    // @return Vector of activity levels for each MotorCommand enum value
    std::vector<float> getMotorActivityLevels() const;
    
    // Reset agent for new episode
    // Clears all internal state and resets neuromodulation levels
    void reset();
    
    // Get brain pointer
    // @return Pointer to the underlying Brain instance
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    // Enable/disable reward modulation (default: true)
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    // Enable/disable structural plasticity (default: true)
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    // Enable/disable development (default: true)
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    // Enable/disable curiosity-driven exploration (default: true)
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Check if reward modulation is enabled
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    // Check if structural plasticity is enabled
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Check if development is enabled
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Check if curiosity is enabled
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Motor command selection with curiosity/exploration
    // Implements curiosity-driven exploration strategies
    // @param defaultCmd Default motor command from neural decoding
    // @return Final motor command (may be random if curiosity-driven)
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced exploration strategies
    MotorCommand exploreWithTemperature(MotorCommand defaultCmd, float temperature);
    MotorCommand exploreByNovelty(MotorCommand defaultCmd);
    MotorCommand exploreByPredictionError(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups for different actions
    std::vector<Neuron*> motorForward_;      // Forward propulsion neurons
    std::vector<Neuron*> motorBackward_;     // Backward propulsion neurons
    std::vector<Neuron*> motorTurnLeft_;     // Left turn neurons
    std::vector<Neuron*> motorTurnRight_;    // Right turn neurons
    std::vector<Neuron*> motorInteract_;     // Interaction neurons
    std::vector<Neuron*> motorWait_;         // Idle/waiting neurons
    
    // Sensory neuron groups for different modalities
    std::vector<Neuron*> sensoryVision_;         // Visual processing neurons
    std::vector<Neuron*> sensoryTouch_;          // Tactile/proximity neurons
    std::vector<Neuron*> sensoryInternal_;       // Internal state neurons
    std::vector<Neuron*> sensoryProprioception_;  // Body position neurons
    
    // Neuromodulation state for learning and adaptation
    float dopamineLevel_;      // Dopamine signal level (-1.0 to 1.0)
    float noveltyLevel_;       // Novelty detection signal (0.0 to 1.0)
    float curiosityLevel_;     // Curiosity motivation signal (0.0 to 1.0)
    float predictionError_;    // Reward prediction error (-1.0 to 1.0)
    float expectedReward_;     // Expected reward (moving average)
    
    // Development state for behavioral maturation
    double developmentalAge_;  // Age in simulation seconds
    float plasticityModifier_; // Plasticity scaling factor (0.1 to 2.0)
    
    // Configuration flags for system behavior
    bool rewardModulationEnabled_;       // Enable reward-based learning
    bool structuralPlasticityEnabled_;   // Enable structural changes
    bool developmentEnabled_;            // Enable developmental stages
    bool curiosityEnabled_;              // Enable curiosity-driven exploration
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;  // Previous vision input for novelty
    float sensoryNoveltyDecay_;           // Decay rate for novelty (0.0 to 1.0)
};

} // namespace nlm
