#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Connects NLM brain to the world and handles sensory-motor integration
 * 
 * The AgentBrain class serves as the bridge between the neural computation of the brain
 * and the physical world simulation. It handles:
 * - Sensory transduction: converting sensory input into neural signals
 * - Motor decoding: converting neural activity into motor commands
 * - Neuromodulation: applying reward and prediction error signals
 * - Development: tracking and managing developmental stages
 * 
 * This class is the primary interface for agent-environment interaction in the NLM framework.
 */
class AgentBrain {
public:
    /**
     * @brief Constructor for AgentBrain
     * 
     * @param brain Shared pointer to the brain instance that this agent will control
     */
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor
     */
    ~AgentBrain();

    /**
     * @brief Initialize the agent with a world reference
     * 
     * This method must be called before the agent can function. It sets up the
     * internal state required for sensory processing and motor control.
     * 
     * @param world Reference to the world simulation environment
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size for the brain
     * 
     * Returns the number of sensory input values that the brain expects to process
     * per simulation step. This is typically derived from the brain's neural architecture
     * and sensory system configuration.
     * 
     * @return Expected sensory input size
     */
    [[nodiscard]] size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size
     * 
     * Returns the number of motor outputs that the brain can produce. This corresponds
     * to the number of distinct motor commands the agent can execute.
     * 
     * @return Expected motor output size
     */
    [[nodiscard]] size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * This method converts external sensory data into neural signals that can be
     * processed by the brain. It handles all sensory modalities (vision, touch,
     * internal state, proprioception, etc.).
     * 
     * @param percept The sensory percept containing all sensory data for the current time step
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * This method reads the current neural activity in the brain's motor regions
     * and converts it into a discrete motor command. The decoding process incorporates
     * neuromodulatory signals like curiosity and novelty.
     * 
     * @return The selected motor command for the current time step
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward and prediction error
     * 
     * This method implements reward-modulated learning, where the difference between
     * predicted and actual rewards drives synaptic plasticity and learning.
     * 
     * @param reward The actual reward received from the environment
     * @param predictedReward The predicted reward based on prior expectations
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system with elapsed time
     * 
     * This method manages the developmental progression of the agent, including
     * changes in plasticity, behavioral strategies, and cognitive capabilities.
     * 
     * @param timestep Time elapsed since last development update (in simulation time)
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the agent, which determines
     * which behavioral and cognitive strategies are active.
     * 
     * @return Current developmental stage
     */
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level (dopamine-like signal)
     * 
     * Returns the current level of neuromodulatory influence on learning and behavior,
     * typically representing prediction error or reward prediction signals.
     * 
     * @return Current neuromodulation level (-1.0 to 1.0)
     */
    [[nodiscard]] float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * Returns the agent's curiosity-driven exploration motivation level.
     * Higher values encourage exploration of novel states and behaviors.
     * 
     * @return Current curiosity level (0.0 to 1.0)
     */
    [[nodiscard]] float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * Returns the level of environmental novelty detected from recent sensory input.
     * Novelty drives curiosity and learning about new stimuli and situations.
     * 
     * @return Current novelty level (0.0 to 1.0)
     */
    [[nodiscard]] float getNoveltyLevel() const;
    
    /**
     * @brief Get current prediction error
     * 
     * Returns the difference between expected and actual rewards, which drives
     * learning through reward-modulated plasticity.
     * 
     * @return Current prediction error (-1.0 to 1.0)
     */
    [[nodiscard]] float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Resets all internal state variables to their initial values, preparing the
     * agent for a new simulation episode. This includes neuromodulation levels,
     * developmental age, sensory history, and plastic state.
     */
    void reset();
    
    /**
     * @brief Get pointer to the brain instance
     * 
     * Provides access to the underlying brain object for advanced operations
     * and statistics gathering.
     * 
     * @return Pointer to the brain instance
     */
    [[nodiscard]] Brain* getBrain() noexcept { return brain_.get(); }
    
    // ============================================================================
    // Configuration and Subsystem Control
    // ============================================================================
    
    /**
     * @brief Enable or disable reward-modulated learning
     * 
     * @param enable Whether to enable reward modulation
     */
    void enableRewardModulation(bool enable) noexcept { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable Whether to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) noexcept { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable developmental progression
     * 
     * @param enable Whether to enable development
     */
    void enableDevelopment(bool enable) noexcept { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable Whether to enable curiosity
     */
    void enableCuriosity(bool enable) noexcept { curiosityEnabled_ = enable; }
    
    // ============================================================================
    // Configuration Queries
    // ============================================================================
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return True if reward modulation is enabled
     */
    [[nodiscard]] bool isRewardModulationEnabled() const noexcept { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return True if structural plasticity is enabled
     */
    [[nodiscard]] bool isStructuralPlasticityEnabled() const noexcept { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return True if development is enabled
     */
    [[nodiscard]] bool isDevelopmentEnabled() const noexcept { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return True if curiosity is enabled
     */
    [[nodiscard]] bool isCuriosityEnabled() const noexcept { return curiosityEnabled_; }

private:
    /**
     * @brief Motor decoding: convert neural activity to motor command
     * 
     * Private method that performs the core motor decoding by analyzing activity
     * across different motor neuron groups. This is the fundamental motor control
     * algorithm.
     * 
     * @return The most active motor command based on neural activity
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Motor command selection with curiosity-driven exploration
     * 
     * Private method that optionally overrides the default motor command with a
     * random one when curiosity levels are high, promoting exploration.
     * 
     * @param defaultCmd The default motor command from neural decoding
     * @return The selected motor command (possibly modified by curiosity)
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // ============================================================================
    // State Management
    // ============================================================================
    
    std::shared_ptr<Brain> brain_;  // The brain instance this agent controls
    
    // ============================================================================
    // Motor Neuron Groups
    // ============================================================================
    std::vector<Neuron*> motorForward_;    //!< Neurons controlling forward movement
    std::vector<Neuron*> motorBackward_;   //!< Neurons controlling backward movement
    std::vector<Neuron*> motorTurnLeft_;   //!< Neurons controlling left turns
    std::vector<Neuron*> motorTurnRight_;  //!< Neurons controlling right turns
    std::vector<Neuron*> motorInteract_;   //!< Neurons controlling interaction
    std::vector<Neuron*> motorWait_;       //!< Neurons controlling waiting
    
    // ============================================================================
    // Sensory Neuron Groups
    // ============================================================================
    std::vector<Neuron*> sensoryVision_;       //!< Neurons processing visual input
    std::vector<Neuron*> sensoryTouch_;       //!< Neurons processing touch input
    std::vector<Neuron*> sensoryInternal_;    //!< Neurons processing internal state
    std::vector<Neuron*> sensoryProprioception_; //!< Neurons processing body position
    
    // ============================================================================
    // Neuromodulation State
    // ============================================================================
    float dopamineLevel_;       //!< Dopamine-like neuromodulation signal
    float noveltyLevel_;        //!< Novelty detection signal
    float curiosityLevel_;      //!< Curiosity-driven exploration motivation
    float predictionError_;     //!< Difference between predicted and actual reward
    float expectedReward_;      //!< Expected reward (exponentially weighted average)
    
    // ============================================================================
    // Development State
    // ============================================================================
    double developmentalAge_;   //!< Age of the agent in simulation time
    float plasticityModifier_;  //!< Current plasticity factor (affects learning rate)
    
    // ============================================================================
    // Configuration Flags
    // ============================================================================
    bool rewardModulationEnabled_;      //!< Whether reward-modulated learning is active
    bool structuralPlasticityEnabled_; //!< Whether structural changes are allowed
    bool developmentEnabled_;          //!< Whether developmental progression occurs
    bool curiosityEnabled_;            //!< Whether curiosity-driven exploration is enabled
    
    // ============================================================================
    // Novelty Detection
    // ============================================================================
    std::vector<float> previousVision_;    //!< Previous frame of visual input for novelty detection
    float sensoryNoveltyDecay_;             //!< Decay factor for novelty signals over time
};

} // namespace nlm
