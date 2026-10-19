#pragma once

/**
 * @file AgentBrain.hpp
 * @brief AgentBrain: Connects NLM brain to the world
 * @author NLM Framework
 * @version 6.0
 * @date 2024
 * 
 * @details
 * The AgentBrain class serves as the interface between the NLM brain and the
 * simulated world, handling sensory transduction and motor decoding. This class
 * bridges the gap between the brain's internal neural representations and the
 * agent's interaction with the external environment.
 * 
 * This class is part of the Phase 6 integrated brain architecture, providing
 * the crucial link between internal neural processing and external action.
 * 
 * @section architecture Architecture Overview
 * AgentBrain implements the brain-world interface with the following components:
 * 
 * 1. **Sensory Processing**
 *    - Transduction of world state into neural signals
 *    - Integration of multiple sensory modalities (vision, touch, internal, proprioception)
 *    - Noise reduction and signal preprocessing
 *    - Novelty detection for exploration motivation
 * 
 * 2. **Motor Control**
 *    - Decoding neural activity into motor commands
 *    - Action selection with curiosity-based exploration
 *    - Movement execution in the world
 *    - Reward modulation based on prediction errors
 * 
 * 3. **Development**
 *    - Activity-dependent structural changes
 *    - Plasticity rate modulation through developmental stages
 *    - Behavioral maturation and learning
 * 
 * 4. **Neuromodulation**
 *    - Reward processing (dopamine)
 *    - Exploration motivation (curiosity)
 *    - Novelty detection
 *    - Prediction error signaling
 * 
 * @section sensory_transduction Sensory Transduction
 * AgentBrain converts external world state into neural-compatible sensory input:
 * - **Vision**: Ray-casting-based vision produces grayscale intensity values
 * - **Touch**: Proximity sensors detect nearby obstacles and objects
 * - **Internal**: Homeostatic signals track energy, health, movement, and turning
 * - **Proprioception**: Body position, velocity, and orientation signals
 * 
 * @section motor_decoding Motor Decoding
 * AgentBrain converts neural activity into motor actions:
 * - **Activity Analysis**: Motor neuron populations are analyzed for firing patterns
 * - **Action Selection**: Maximum activity determines action type
 * - **Exploration**: Curiosity-based random actions for exploration
 * - **Command Execution**: Motor commands are sent to the world
 * 
 * @section thread_safety Thread Safety
 * The AgentBrain class is **NOT thread-safe**. All access to agent state must be
 * synchronized by external synchronization mechanisms. The agent maintains
 * internal state that should not be modified concurrently during operation.
 * 
 * @section performance Performance Considerations
 * - Efficient neuron grouping for fast sensory/motor processing
 * - Constant-time activity calculation for motor decoding
 * - Exponential moving averages for smooth state estimation
 * - Minimal copying of sensory data between components
 * 
 * @section lifecycle Lifecycle
 * AgentBrain lifecycle:
 * 1. **Construction**: Create with brain reference
 * 2. **Initialization**: Configure with world dimensions and allocate neuron groups
 * 3. **Processing**: Continuously process sensory input and produce motor output
 * 4. **Development**: Update developmental state and plasticity rates
 * 5. **Reset**: Clear state for new episodes
 * 
 * @note This implementation requires C++11 or later for modern C++ features.
 */

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Interface between NLM brain and simulated world
 * @details
 * AgentBrain connects the internal neural processing of the brain to external
 * world interaction, handling the transduction of sensory signals and decoding
 * of motor commands. It serves as the essential bridge for embodied agents
 * in the NLM framework.
 * 
 * Key responsibilities:
 * - Transduce world state into neural-compatible sensory input
 * - Decode neural activity into motor actions for world interaction
 * - Maintain neuromodulation state for reward and exploration
 * - Track agent development and behavioral maturation
 * - Provide curiosity-based exploration for intrinsic motivation
 * - Handle reward prediction error and learning
 * 
 * The class maintains organized groups of neurons for efficient processing:
 * - Motor neurons: Grouped by action type (forward, backward, turn, etc.)
 * - Sensory neurons: Grouped by modality (vision, touch, internal, proprioception)
 * - Neuromodulation signals: Dopamine, curiosity, novelty, prediction error
 * 
 * @note AgentBrain works in conjunction with Brain::receiveSensoryInput() and
 * Brain::produceAction() to complete the brain-world feedback loop.
 */
class AgentBrain {
public:
    // Create agent brain with brain reference
    /**
     * @brief Constructor for AgentBrain class
     * @param brain Shared pointer to brain instance for neural processing
     * @details
     * Constructs the agent brain with a reference to the neural brain.
     * The brain provides the core neural processing capabilities, while
     * AgentBrain handles the interface to the external world.
     * 
     * The constructor initializes neuromodulation state and prepares
     * for sensory processing and motor decoding. It does not require
     * world configuration, which is provided during initialization.
     * 
     * @note AgentBrain takes ownership of the brain reference via shared pointer
     * to ensure proper lifetime management.
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * @brief Initialize agent brain with world
     * @param world Reference to the simple world instance
     * @details
     * Initializes the agent brain with world configuration and allocates
     * sensory neuron groups based on world dimensions.
     * 
     * This method performs the following initialization:
     * 1. **Sensory Buffer Setup**: Resizes vision buffer to match world vision
     * 2. **Development Reset**: Sets initial developmental age and plasticity
     * 3. **Neuron Group Information**: Logs population of sensory neurons by modality
     * 
     * The initialization establishes the agent's sensory capabilities and
     * developmental starting state.
     * 
     * @warning Must be called before first sensory processing.
     * @post Agent is ready to process sensory input and produce motor output.
     * 
     * @see AgentBrain::processSensoryInput() for subsequent processing
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get sensory input size expected by brain
     * @return size_t Total number of sensory signals expected
     * @details
     * Returns the dimensionality of the sensory space that the brain expects.
     * This is used by the world to generate appropriate sensory percepts.
     * 
     * The sensory space is organized as:
     * - Vision: 16x16 = 256 values (grayscale intensity)
     * - Touch: 8 values (proximity to 8 directions)
     * - Internal: 4 values (energy, health, movement, turning)
     * - Proprioception: 6 values (position, velocity, orientation)
     * 
     * @return Total sensory dimensionality (256 + 8 + 4 + 6 = 274)
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get motor output size expected
     * @return size_t Number of motor commands produced
     * @details
     * Returns the number of distinct motor actions that can be produced.
     * This determines the action space available to the agent.
     * 
     * The motor action space includes:
     * - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
     * - Orientation: TurnLeft, TurnRight
     * - Sensing: LookLeft, LookRight
     * - Interaction: Interact
     * - Special: Eat, Drink, Rest, Wait
     * - Customization: Custom (user-defined actions)
     * 
     * @return Motor action count (7 actions currently supported)
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * @param percept Sensory percept from world
     * @details
     * Processes the sensory percept from the world and injects the neural signals
     * into the brain for processing. This is the primary sensory transduction
     * pathway for the agent.
     * 
     * The processing includes:
     * 1. **Vision Processing**: Scale vision intensity and inject into vision neurons
     * 2. **Touch Processing**: Scale touch signals and inject into touch neurons
     * 3. **Internal Processing**: Center and scale internal signals
     * 4. **Proprioception**: Scale and inject proprioceptive signals
     * 5. **Novelty Detection**: Compute change from previous vision for exploration
     * 6. **Curiosity Update**: Update curiosity based on novelty and prediction error
     * 
     * This method enables the brain to perceive and understand the environment.
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * @return MotorCommand Selected motor command based on neural activity
     * @details
     * Decodes the current motor neuron activity into a specific motor command
     * for world interaction. This is the primary motor output pathway.
     * 
     * The decoding process:
     * 1. **Activity Calculation**: Compute average activity in each motor group
     * 2. **Selection**: Choose group with highest activity
     * 3. **Threshold Check**: Only act if activity exceeds threshold
     * 4. **Curiosity Override**: Apply curiosity-based exploration if appropriate
     * 
     * This method translates internal neural decisions into external actions.
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward
     * @param reward Actual reward received from world
     * @param predictedReward Expected reward based on prediction
     * @details
     * Applies reward-modulated plasticity based on the difference between
     * actual and predicted rewards (prediction error). This is the core
     * reinforcement learning mechanism.
     * 
     * The neuromodulation includes:
     * - **Prediction Error**: Compute difference (actual - predicted)
     * - **Dopamine Signal**: Convert to dopamine level (clamped to [-1, 1])
     * - **Eligibility Traces**: Apply reward to synapses with traces
     * - **Plasticity Modulation**: Scale STDP learning rates
     * - **Expected Reward**: Update moving average for future predictions
     * 
     * This enables learning from experience and adaptation to the environment.
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * @param timestep Current simulation timestep
     * @details
     * Updates the brain's development system based on current simulation time.
     * This drives activity-dependent behavioral maturation and plasticity changes.
     * 
     * The development process:
     * 1. **Age Increment**: Increase developmental age
     * 2. **Stage Determination**: Determine current developmental stage based on age
     * 3. **Plasticity Modulation**: Set plasticity rate based on stage
     * 4. **Structural Plasticity**: Update synaptogenesis and pruning rates
     * 
     * Development creates behavioral changes over time, from high plasticity
     * in youth to stable adult behavior.
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * @return DevelopmentalStage Current developmental stage
     * @details
     * Returns the brain's current developmental stage, which affects plasticity
     * rates and learning capabilities.
     * 
     * Developmental stages:
     * - **Initial**: High plasticity, rapid learning
     * - **CriticalPeriod**: Modulated plasticity, sensitive learning
     * - **Maturation**: Reduced plasticity, stable learning
     * - **Adult**: Low plasticity, stable behavior
     * - **Aging**: Decreased plasticity, reduced learning
     * 
     * @return DevelopmentalStage representing current brain maturity
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get neuromodulation level
     * @return float Current neuromodulation level (dopamine)
     * @details
     * Returns the current neuromodulation level, primarily dopamine, which
     * affects neural excitability and plasticity.
     * 
     * Higher dopamine levels increase neural excitability and plasticity,
     * promoting learning and exploration. Lower levels decrease these effects.
     * 
     * @return Neuromodulation level in range [-1.0, 1.0]
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * @return float Current curiosity level
     * @details
     * Returns the current curiosity level, which drives exploration and
     * intrinsic motivation. Curiosity is updated based on novelty and
     * prediction error.
     * 
     * Higher curiosity levels increase the likelihood of exploratory actions,
     * promoting discovery of new information and behaviors.
     * 
     * @return Curiosity level in range [0.0, 1.0]
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * @return float Current novelty level
     * @details
     * Returns the current novelty level, which detects changes in the environment
     * and drives exploration when novelty is high.
     * 
     * Novelty is computed as the average difference between current and
     * previous sensory input (primarily vision). High novelty indicates
     * new or unexpected environmental states.
     * 
     * @return Novelty level in range [0.0, 1.0]
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * @return float Current prediction error
     * @details
     * Returns the current prediction error, which is the difference between
     * expected and actual rewards. This is the core signal for reinforcement
     * learning.
     * 
     * Prediction error drives learning:
     * - Positive error: Better than expected, increase future expectations
     * - Negative error: Worse than expected, decrease future expectations
     * 
     * @return Prediction error (can be positive or negative)
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * @details
     * Resets all agent state for a new episode. This clears neuromodulation
     * signals, developmental state, and sensory history to ensure a fresh start.
     * 
     * Reset includes:
     * - Neuromodulation: Clear dopamine, curiosity, novelty, prediction error
     * - Development: Reset age and plasticity modifier
     * - Sensory: Clear vision buffer
     * 
     * This prepares the agent for learning in a new episode.
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * @return Brain* Pointer to underlying brain instance
     * @details
     * Provides direct access to the underlying brain for advanced usage
     * and debugging. This is not typically needed for normal operation.
     * 
     * @return Pointer to brain instance (must not be null after initialization)
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @name Configuration
     * Methods for enabling/disabling agent subsystems
     */
    ///@{ 
    
    /**
     * @brief Enable or disable reward modulation
     * @param enable True to enable reward modulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * @param enable True to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * @param enable True to enable development
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity
     * @param enable True to enable curiosity
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * @return bool True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * @return bool True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * @return bool True if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * @return bool True if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    ///@}
    
private:
    // Motor decoding: convert neural activity to motor command
    /**
     * @brief Internal motor decoding from neural activity
     * @return MotorCommand Selected motor command
     * @details
     * Private method that performs the core motor decoding from neural activity.
     * This calculates the average activity in each motor neuron group and selects
     * the command with the highest activity.
     * 
     * @see AgentBrain::decodeMotorCommand() for public interface
     */
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    /**
     * @brief Select motor command with curiosity-based exploration
     * @param defaultCmd Default motor command
     * @return MotorCommand Potentially different command for exploration
     * @details
     * Applies curiosity-based exploration to the default motor command.
     * When curiosity is high, there's a probability of selecting a random action
     * to promote exploration of the environment.
     * 
     * @param defaultCmd The command that would normally be selected
     * @return Either defaultCmd or a random exploratory command
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // ========== BRAIN INTEGRATION ==========
    /** Brain instance for neural processing */
    std::shared_ptr<Brain> brain_;
    
    // ========== MOTOR NEURON GROUPS ==========
    /** Motor neurons grouped by action type */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // ========== SENSORY NEURON GROUPS ==========
    /** Sensory neurons grouped by modality */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // ========== NEUROMODULATION STATE ==========
    /** Neuromodulation signals */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // ========== DEVELOPMENT STATE ==========
    /** Development and plasticity state */
    double developmentalAge_;
    float plasticityModifier_;
    
    // ========== CONFIGURATION ==========
    /** Subsystem enable flags */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // ========== SENSORY HISTORY ==========
    /** Previous vision state for novelty detection */
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};
