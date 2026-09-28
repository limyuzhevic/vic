#pragma once

/**
 * @file AgentBrain.hpp
 * @brief AgentBrain class: Connects NLM brain to the external world.
 * 
 * This header defines the AgentBrain class, which serves as the interface between
 * the internal NLM brain and the external environment. It handles sensory transduction
 * (converting external world inputs into brain-compatible signals) and motor decoding
 * (converting brain outputs into actionable motor commands).
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/agent/AgentBody.hpp
 * @see src/agent/SensoryPercept.hpp
 * @see src/brain/Brain.hpp
 * @see src/world/SimpleWorld.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see src/plasticity/StructuralPlasticity.hpp
 * @see src/core/Types/Types.hpp
 * @see src/brain/Neuron.hpp
 * @see TODO Complete agent integration (Phase 2)
 */

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * AgentBrain: Connects NLM brain to the world and handles sensory/motor interfaces.
 * 
 * The AgentBrain class serves as the bridge between the neural network brain and
 * the external environment. It performs two primary functions:
 * 
 * 1. **Sensory Processing**: Receives information from the world through sensory
 *    perceptions and converts it into compatible signals that can be injected into
 *    the brain for processing.
 * 
 * 2. **Motor Output**: Decodes neural activity patterns into motor commands that
 *    can control the agent's body or actions in the environment.
 * 
 * The AgentBrain also manages neuromodulation, curiosity-driven exploration,
 * developmental processes, and learning mechanisms that connect the agent's
 * internal neural dynamics to external rewards and environmental interactions.
 * 
 * @see AgentBody
 * @see SensoryPercept
 * @see Brain
 * @see SimpleWorld
 * @see PlasticityRule
 * @see StructuralPlasticity
 * @see Neuron
 * @see TODO Implement complete agent integration (Phase 2)
 */
class AgentBrain {
public:
    /**
     * Construct AgentBrain with the given brain.
     * 
     * @param brain Shared pointer to the brain that will be controlled
     * @see Brain
     * @see getBrain()
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * Destructor ensures proper cleanup of all resources.
     */
    ~AgentBrain();
    
    /**
     * @name Initialization
     * 
     * Methods for setting up the agent brain with world information.
     */
    /// @{
    
    /**
     * @brief Initialize agent brain with world information
     * 
     * @param world Reference to the world the agent will interact with
     * @note This method sets up sensory mapping, motor neuron groups, and
     *       initializes neuromodulation systems based on world characteristics.
     */
    void initialize(const SimpleWorld& world);
    /// @}
    
    /**
     * @name Sensory Interface
     * 
     * Methods for processing external sensory information.
     */
    /// @{
    
    /**
     * @brief Get the expected sensory input size for the brain
     * 
     * @return size_t Number of sensory input features expected by the brain
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size from the brain
     * 
     * @return size_t Number of motor output dimensions expected
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * @param percept SensoryPercept containing world-derived sensory information
     * @see SensoryPercept
     */
    void processSensoryInput(const SensoryPercept& percept);
    /// @}
    
    /**
     * @name Motor Output
     * 
     * Methods for converting brain activity to motor commands.
     */
    /// @{
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * @return MotorCommand Decoded motor command for agent action
     * @note This method analyzes neural activity patterns in motor neuron groups
     *       and produces appropriate motor commands.
     */
    MotorCommand decodeMotorCommand();
    /// @}
    
    /**
     * @name Neuromodulation
     * 
     * Methods for managing neuromodulatory systems and reward processing.
     */
    /// @{
    
    /**
     * @brief Apply neuromodulation based on reward prediction error
     * 
     * @param reward Actual received reward
     * @param predictedReward Expected reward based on current policy
     * @note This method updates neuromodulatory levels (dopamine, etc.) that
     *       influence plasticity and learning.
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Get current neuromodulation level
     * 
     * @return float Current neuromodulation level (typically dopamine)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * @return float Current curiosity/exploration level
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * @return float Current novelty detection level
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * @return float Current prediction error (reward - predicted reward)
     */
    float getPredictionError() const;
    /// @}
    
    /**
     * @name Development
     * 
     * Methods for managing developmental processes and lifespan changes.
     */
    /// @{
    
    /**
     * @brief Update development system
     * 
     * @param timestep Current simulation timestep
     * @note This method updates developmental stage, plasticity modifiers,
     *       and other age-dependent processes.
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    /// @}
    
    /**
     * @name Configuration and Control
     * 
     * Methods for enabling/disabling various agent capabilities.
     */
    /// @{
    
    /**
     * @brief Reset agent for new episode
     * 
     * @note This method resets all internal state, neuromodulation levels,
     *       and developmental timers for a fresh episode.
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Brain* Pointer to the managed brain
     * @see AgentBrain(std::shared_ptr<Brain> brain)
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable true to enable, false to disable
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable true to enable, false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * 
     * @param enable true to enable, false to disable
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable true to enable, false to disable
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return bool true if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return bool true if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return bool true if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return bool true if curiosity-driven exploration is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * The AgentBrain class integrates with multiple system components.
     */
    /// @{
    
    /**
     * @see Brain for internal neural processing
     * @see PlasticityRule for learning mechanisms
     * @see StructuralPlasticity for structural changes
     * @see Neuron for neural unit implementation
     * @see TODO Implement complete agent integration (Phase 2)
     */
    /// @}
    
private:
    /**
     * @name Motor Decoding Implementation
     * 
     * Private methods for motor output processing.
     */
    /// @{
    
    /**
     * @brief Decode motor activity from motor neuron groups
     * 
     * @return MotorCommand Decoded motor command
     * @note This method analyzes activity in motorForward_, motorBackward_,
     *       motorTurnLeft_, motorTurnRight_, motorInteract_, and motorWait_
     *       neuron groups.
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Select motor command with curiosity-driven exploration
     * 
     * @param defaultCmd Default motor command based on neural activity
     * @return MotorCommand Selected motor command (may include exploration)
     * @note This method adds random exploration based on curiosity level
     *       to promote discovery and learning.
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    /// @}
    
    /**
     * @name Brain and Network Management
     * 
     * Core brain management and neural network organization.
     */
    /// @{
    
    std::shared_ptr<Brain> brain_;  /**< Shared pointer to the managed brain */
    /// @}
    
    /**
     * @name Motor Neuron Organization
     * 
     * Organization of neurons for different motor functions.
     */
    /// @{
    
    std::vector<Neuron*> motorForward_;   /**< Neurons for forward movement */
    std::vector<Neuron*> motorBackward_;  /**< Neurons for backward movement */
    std::vector<Neuron*> motorTurnLeft_;  /**< Neurons for left turn */
    std::vector<Neuron*> motorTurnRight_; /**< Neurons for right turn */
    std::vector<Neuron*> motorInteract_;  /**< Neurons for interaction */
    std::vector<Neuron*> motorWait_;      /**< Neurons for waiting/idle */
    /// @}
    
    /**
     * @name Sensory Neuron Organization
     * 
     * Organization of neurons for different sensory modalities.
     */
    /// @{
    
    std::vector<Neuron*> sensoryVision_;        /**< Vision sensory neurons */
    std::vector<Neuron*> sensoryTouch_;        /**< Touch sensory neurons */
    std::vector<Neuron*> sensoryInternal_;     /**< Internal state neurons */
    std::vector<Neuron*> sensoryProprioception_; /**< Proprioception neurons */
    /// @}
    
    /**
     * @name Neuromodulation State
     * 
     * State variables for neuromodulatory systems.
     */
    /// @{
    
    float dopamineLevel_;      /**< Dopamine neuromodulator level */
    float noveltyLevel_;       /**< Novelty detection level */
    float curiosityLevel_;     /**< Curiosity/exploration level */
    float predictionError_;    /**< Reward prediction error */
    float expectedReward_;     /**< Expected reward based on current policy */
    /// @}
    
    /**
     * @name Development State
     * 
     * State variables for developmental processes.
     */
    /// @{
    
    double developmentalAge_;    /**< Current developmental age */
    float plasticityModifier_;   /**< Plasticity modification factor */
    /// @}
    
    /**
     * @name Configuration Flags
     * 
     * Boolean flags controlling agent capabilities.
     */
    /// @{
    
    bool rewardModulationEnabled_;     /**< Whether reward modulation is active */
    bool structuralPlasticityEnabled_; /**< Whether structural plasticity is active */
    bool developmentEnabled_;          /**< Whether development processes are active */
    bool curiosityEnabled_;            /**< Whether curiosity-driven exploration is active */
    /// @}
    
    /**
     * @name Previous State
     * 
     * State variables for change detection and novelty calculation.
     */
    /// @{
    
    std::vector<float> previousVision_;      /**< Previous vision state for novelty detection */
    float sensoryNoveltyDecay_;               /**< Decay rate for novelty detection */
    /// @}

}; // class AgentBrain

} // namespace nlm
