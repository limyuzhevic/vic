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
// 
// This class serves as the interface layer between the neural simulation brain
// and the external environment. It translates between the brain's internal
// neural representation and the world's sensory and motor signals.
// 
// Key responsibilities:
// 1. Convert external sensory observations into brain-compatible input
// 2. Decode neural activity into actionable motor commands
// 3. Apply neuromodulatory signals based on environmental feedback
// 4. Manage developmental progression and plasticity modifiers
// 
// This class enables embodied cognition by grounding the brain in a simulated
// environment, providing sensory input, motor output, and reward signals.

class AgentBrain {
public:
    /**
     * Create agent brain interface with brain
     * @param brain Shared pointer to brain to interface with
     */
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * Destructor
     * Cleans up agent resources
     */
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * Initialize agent with world
     * @param world Reference to world to connect to
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * Get sensory input size expected by brain
     * @return Number of sensory input dimensions
     */
    size_t getSensoryInputSize() const;
    
    /**
     * Get motor output size expected
     * @return Number of motor output dimensions
     */
    size_t getMotorOutputSize() const;
    
    /**
     * Process sensory percept and inject into brain
     * @param percept Sensory perception data from world
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * Decode brain motor activity into motor command
     * @return Motor command based on current brain activity
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * Apply neuromodulation based on reward
     * @param reward Received reward value
     * @param predictedReward Expected reward value
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * Update development system
     * @param timestep Time step for development updates
     */
    void updateDevelopment(double timestep);
    
    /**
     * Get current developmental stage
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Get neuromodulation level
     * @return Current neuromodulation level (e.g., dopamine)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Get curiosity level
     * @return Current curiosity/exploration level
     */
    float getCuriosityLevel() const;
    
    /**
     * Get novelty level
     * @return Current novelty detection level
     */
    float getNoveltyLevel() const;
    
    /**
     * Get prediction error
     * @return Current prediction error signal
     */
    float getPredictionError() const;
    
    /**
     * Reset agent for new episode
     * Restores agent to initial state
     */
    void reset();
    
    /**
     * Get brain pointer
     * @return Pointer to underlying brain
     */
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    
    /**
     * Enable or disable reward modulation
     * @param enable Whether to enable reward-based learning
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * Enable or disable structural plasticity
     * @param enable Whether to enable structural changes
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * Enable or disable development
     * @param enable Whether to enable developmental progression
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * Enable or disable curiosity
     * @param enable Whether to enable curiosity-driven exploration
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * Check if reward modulation is enabled
     * @return True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * Check if structural plasticity is enabled
     * @return True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * Check if development is enabled
     * @return True if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * Check if curiosity is enabled
     * @return True if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * Motor decoding: convert neural activity to motor command
     * @return Motor command based on current brain motor neuron activity
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * Motor command selection with curiosity/exploration
     * @param defaultCmd Default motor command to select from
     * @return Motor command with curiosity-driven exploration bias
     */
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
