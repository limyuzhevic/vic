#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief AgentBrain: Connects NLM brain to the world
 * 
 * This class serves as the interface between the NLM artificial brain and the external
 * world. It handles:
 * - Sensory transduction: Converts world observations into neural signals
 * - Motor decoding: Translates neural activity into actionable motor commands
 * - Neuromodulation: Applies reward-based learning signals to the brain
 * - Development: Manages brain maturation over time
 * 
 * The agent brain maintains organized groups of neurons for different functions:
 * - Motor neurons: Organized into action groups (forward, backward, turn left/right, etc.)
 * - Sensory neurons: Organized into modality groups (vision, touch, internal, proprioception)
 * - Neuromodulatory signals: Dopamine, curiosity, novelty, prediction error
 * 
 * This creates a closed-loop cognitive architecture where the brain can interact
 * with its environment, learn from experience, and adapt its behavior.
 */
class AgentBrain {
public:
    /**
     * @brief Construct a new AgentBrain instance
     * 
     * @param brain Shared pointer to the NLM brain this agent will control
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destroy the AgentBrain instance
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with world dimensions
     * 
     * This sets up the agent's sensory buffer and internal state for interaction
     * with the provided world simulation.
     * 
     * @param world Reference to the simple world the agent will operate in
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the sensory input size expected by the brain
     * 
     * Returns the total number of sensory channels the connected brain expects
     * for proper operation. This includes vision, touch, internal, and proprioception signals.
     * 
     * @return size_t Expected sensory input size
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the motor output size expected by the brain
     * 
     * Returns the number of distinct motor commands the agent can produce.
     * The connected brain uses this to determine how many action outputs are available.
     * 
     * @return size_t Expected motor output size
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * This method converts world observations into neural signals through:
     * - Vision: Maps pixel intensity to vision sensory neuron groups
     * - Touch: Detects collisions and converts to touch neuron activity
     * - Internal: Applies homeostatic regulation signals to internal neurons
     * - Proprioception: Provides body position/velocity feedback
     * 
     * Novelty detection and curiosity computation are also performed here.
     * 
     * @param percept The sensory percept containing all current observations
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * Analyzes the activity levels across all motor neuron groups to determine
     * the most appropriate action. Curiosity-based exploration is also applied
     * when the agent encounters novel stimuli.
     * 
     * @return MotorCommand The selected motor command
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward
     * 
     * Implements reward prediction error theory by:
     * - Computing prediction error (actual reward - expected reward)
     * - Updating expected reward using exponential moving average
     * - Applying dopamine-like signals to synaptic plasticity
     * - Modulating plasticity rates based on neuromodulatory state
     * 
     * This is the core learning mechanism that allows the agent to adapt
     * based on experience.
     * 
     * @param reward The actual reward received from the world
     * @param predictedReward The agent's predicted reward for this action
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * Manages brain development over time by:
     * - Tracking developmental age
     * - Applying age-dependent plasticity modifiers
     * - Updating structural plasticity rates based on developmental stage
     * - Setting appropriate developmental stage (Initial, CriticalPeriod, etc.)
     * 
     * Development modulates learning rates to enable sensitive periods
     * and gradual maturation.
     * 
     * @param timestep Time increment since last development update
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage The current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level
     * 
     * Returns the current dopamine level, which modulates neural excitability
     * and plasticity. Higher levels increase learning and neural responsiveness.
     * 
     * @return float Current neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * 
     * Returns the current curiosity signal, which drives exploration of novel
     * states and encourages discovery learning. Higher curiosity leads to
     * more exploratory behavior.
     * 
     * @return float Current curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * 
     * Returns the detected novelty signal, which indicates how different
     * current sensory input is from recent experience. This helps the agent
     * identify new learning opportunities.
     * 
     * @return float Current novelty level
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * Returns the reward prediction error signal, which compares expected
     * outcomes with actual results. This is a key component of reinforcement
     * learning.
     * 
     * @return float Current prediction error (-1.0 to 1.0)
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Clears all neuromodulatory state, novelty buffers, and developmental
     * counters in preparation for a new learning episode. This allows for
     * independent learning across multiple trials.
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Brain* Pointer to the connected NLM brain
     */
    Brain* getBrain() { return brain_.get(); }
    
    // ========== CONFIGURATION ==========
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable True to enable reward-based learning
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable True to enable structural changes (new connections, pruning)
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * 
     * @param enable True to enable developmental maturation
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable True to enable curiosity-based exploration
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return true If reward modulation is enabled
     * @return false If reward modulation is disabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return true If structural plasticity is enabled
     * @return false If structural plasticity is disabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return true If development is enabled
     * @return false If development is disabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return true If curiosity is enabled
     * @return false If curiosity is disabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // ========== MOTOR DECODING ==========
    
    /**
     * @brief DecodeFromMotorNeurons: Convert neural activity to motor command
     * 
     * Computes the average activity level across each motor neuron group
     * (forward, backward, turn left, turn right, interact, wait) and selects
     * the group with highest activity as the intended action.
     * 
     * @return MotorCommand The decoded motor command
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief SelectWithCuriosity: Motor command selection with exploration
     * 
     * Occasionally overrides the default action selection with random exploration
     * when curiosity is high. This allows the agent to discover novel behaviors
     * and escape local optima.
     * 
     * @param defaultCmd The default motor command from neural activity
     * @return MotorCommand The selected command (default or random)
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // ========== INTERNAL STATE ==========
    
    /**
     * @brief Connected NLM brain
     * 
     * The artificial brain this agent controls for perception and action.
     */
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups - organized by action type
    std::vector<Neuron*> motorForward_;     ///< Neurons for forward movement
    std::vector<Neuron*> motorBackward_;    ///< Neurons for backward movement
    std::vector<Neuron*> motorTurnLeft_;    ///< Neurons for turning left
    std::vector<Neuron*> motorTurnRight_;   ///< Neurons for turning right
    std::vector<Neuron*> motorInteract_;    ///< Neurons for interaction
    std::vector<Neuron*> motorWait_;        ///< Neurons for waiting/idle
    
    // Sensory neuron groups - organized by modality
    std::vector<Neuron*> sensoryVision_;        ///< Neurons for vision input (256 values)
    std::vector<Neuron*> sensoryTouch_;         ///< Neurons for touch/proximity (8 values)
    std::vector<Neuron*> sensoryInternal_;      ///< Neurons for internal state (4 values)
    std::vector<Neuron*> sensoryProprioception_; ///< Neurons for proprioception (6 values)
    
    // Neuromodulation state
    float dopamineLevel_;      ///< Dopamine signal for reward prediction error
    float noveltyLevel_;        ///< Novelty detection for novelty-driven learning
    float curiosityLevel_;     ///< Curiosity signal for exploration
    float predictionError_;    ///< Reward prediction error signal
    float expectedReward_;     ///< Expected reward (exponential moving average)
    
    // Development state
    double developmentalAge_;  ///< Current developmental age in simulation time
    float plasticityModifier_; ///< Modifies overall plasticity rate
    
    // Configuration flags
    bool rewardModulationEnabled_;     ///< Enable/disable reward-based learning
    bool structuralPlasticityEnabled_; ///< Enable/disable structural changes
    bool developmentEnabled_;          ///< Enable/disable developmental maturation
    bool curiosityEnabled_;           ///< Enable/disable curiosity-driven exploration
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;  ///< Last vision frame for novelty computation
    float sensoryNoveltyDecay_;          ///< Decay rate for novelty signal (0.0-1.0)
};
