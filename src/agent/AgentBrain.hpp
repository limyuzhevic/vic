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
    /**
     * @brief Construct an AgentBrain with a given brain instance
     * 
     * @param brain Shared pointer to the NLM brain that this agent will control.
     * The brain must be properly initialized with neural populations before using
     * the agent. The AgentBrain will extract motor and sensory neuron groups from
     * the brain's regions for interfacing with the world.
     * 
     * @note The brain pointer is stored as a shared_ptr to prevent premature
     * deallocation. The brain object must outlive this AgentBrain instance.
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destroy the AgentBrain instance
     * 
     * Cleans up any internal resources and logs shutdown.
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with a world instance
     * 
     * This method must be called before starting the agent's interaction loop.
     * It sets up internal state based on world properties and prepares the
     * agent for sensory processing and motor command execution.
     * 
     * @param world Reference to the SimpleWorld instance that contains the
     * agent's physical environment, including vision dimensions and other
     * sensory inputs.
     * 
     * @note This method should be called once per agent instance, typically
     * after the brain has been fully developed and the agent has been placed
     * in the world.
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size required by the brain
     * 
     * Returns the total number of sensory signal values that will be fed into
     * the brain during processing. This includes vision, touch, internal, and
     * proprioception signals.
     * 
     * @return size_t Total number of expected sensory input values
     * 
     * @note The agent's sensory processing will produce this many values per
     * timestep, matching the brain's input requirements.
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size required by the brain
     * 
     * Returns the total number of distinct motor commands that can be generated
     * by this agent. Each motor command corresponds to a different action group
     * (forward, backward, turn left, turn right, interact, wait).
     * 
     * @return size_t Number of available motor commands (typically 6)
     * 
     * @note This determines the granularity of the agent's motor control and
     * should match the brain's motor output requirements.
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * This is the core sensory processing method that converts world percepts
     * into neural activity. The agent extracts low-level sensory signals from
     * the percept and injects them into appropriate sensory neuron groups in
     * the brain.
     * 
     * @param perception The SensoryPercept containing all sensory inputs
     * including vision, touch, internal state signals, and proprioception.
     * 
     * @note This method should be called once per simulation timestep after
     * updating the world state. It handles novelty detection and curiosity
     * level calculation internally.
     * 
     * @warning The perception parameter must not be null. Null pointers will
     * trigger warnings and skip processing.
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * This method analyzes current neural activity in motor neuron groups and
     * produces a discrete motor command. It incorporates curiosity-driven
     * exploration when enabled and enabled.
     * 
     * @return MotorCommand The selected motor command (e.g., MoveForward,
     * TurnLeft, Interact, etc.)
     * 
     * @note This method should be called once per simulation timestep to
     * determine the agent's action for the current timestep.
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward prediction error
     * 
     * This method implements reward-modulated plasticity by computing the
     * difference between received reward and predicted reward (prediction error)
     * and applying appropriate neural weight changes to the brain.
     * 
     * @param reward The actual reward received from the environment for the
     * most recent action.
     * @param predictedReward The brain's prediction of the expected reward for
     * the action that was taken.
     * 
     * @note This is the core reinforcement learning mechanism. When enabled,
     * it modulates synaptic plasticity based on dopamine-like signals.
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system based on elapsed time
     * 
     * Advances the agent's developmental stage and associated plasticity
     * changes according to the elapsed simulation time. This models biological
     * development where learning capabilities change over time.
     * 
     * @param timestep The duration of time to advance (in simulation units,
     * typically seconds or milliseconds).
     * 
     * @note Development affects plasticity rates, synaptic pruning, and other
     * learning-related processes. Different developmental stages have
     * different plasticity characteristics.
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get the current developmental stage
     * 
     * Returns the agent's current developmental stage, which affects learning
     * capabilities and behavior.
     * 
     * @return DevelopmentalStage Current developmental stage (Initial,
     * CriticalPeriod, Maturation, or Adult)
     * 
     * @note Developmental stages progress from Initial (high plasticity) to
     * Adult (reduced plasticity) over time.
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level (dopamine)
     * 
     * Returns the current level of neuromodulation signal, typically dopamine,
     * which affects neural excitability and plasticity.
     * 
     * @return float Current neuromodulation level, typically -1.0 to 1.0,
     * where positive values enhance plasticity and negative values reduce it.
     * 
     * @note This level is updated by reward prediction error signals and
     * affects learning speed and direction.
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * Returns the agent's current curiosity level, which drives exploration
     * and novel behavior seeking.
     * 
     * @return float Current curiosity level, typically 0.0 to 1.0, where
     * higher values indicate greater motivation to explore novel stimuli.
     * 
     * @note Curiosity is influenced by novelty detection and prediction error,
     * and affects motor command selection.
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * Returns the current level of novelty detection, which measures how
     * different the current sensory input is from previous inputs.
     * 
     * @return float Current novelty level, typically 0.0 to 1.0, where
     * higher values indicate greater sensory change.
     * 
     * @note Novelty is computed from vision input differences and is used
     * to drive curiosity and exploration behavior.
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get current prediction error
     * 
     * Returns the brain's prediction error signal, which is the difference
     * between received reward and expected reward.
     * 
     * @return float Current prediction error, where positive values indicate
     * better-than-expected performance and negative values indicate worse-than-expected.
     * 
     * @note This is a key signal for reinforcement learning and is used to
     * modulate synaptic plasticity.
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Resets all internal state variables to their initial values, preparing
     * the agent for a new episode or simulation run.
     * 
     * @note This method should be called when starting a new episode, such as
     * after the agent dies or when testing different scenarios.
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * Returns a raw pointer to the brain controlled by this agent. This is
     * primarily useful for direct access to brain internals, though direct
     * brain access should generally be avoided in favor of the agent interface.
     * 
     * @return Brain* Pointer to the brain instance, or nullptr if no brain
     * is set (should not happen with proper constructor usage).
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Configuration setters and getters for agent behavior
     * 
     * These methods allow fine-grained control over which learning and behavioral
     * mechanisms are enabled or disabled.
     */
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable true to enable reward-modulated plasticity, false to disable
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable true to enable structural changes (synaptogenesis, pruning),
     * false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development system
     * 
     * @param enable true to enable developmental stage progression,
     * false to disable
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable true to enable curiosity-based random exploration,
     * false to disable
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return bool true if reward modulation is enabled, false otherwise
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return bool true if structural plasticity is enabled, false otherwise
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return bool true if development is enabled, false otherwise
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return bool true if curiosity-driven exploration is enabled, false otherwise
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Decode motor command from neural activity in motor neuron groups
     * 
     * This private method implements the core motor decoding logic, analyzing
     * the average activity across different motor neuron groups (forward, backward,
     * turn left, turn right, interact, wait) to determine the most appropriate
     * action.
     * 
     * @return MotorCommand The selected motor command based on neural activity
     * 
     * @note This method is called by decodeMotorCommand() after which it may
     * apply curiosity-based exploration if enabled.
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Apply curiosity-based exploration to motor command selection
     * 
     * This method may override the default motor command with a random one
     * when curiosity levels are high, promoting exploration of novel behaviors.
     * 
     * @param defaultCmd The motor command that would be selected based on neural
     * activity alone.
     * @return MotorCommand Either the default command or a random exploration
     * command, depending on curiosity level and random chance.
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
