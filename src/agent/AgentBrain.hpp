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
// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
/**
 * @class AgentBrain
 * @brief Interface between NLM brain and the world for sensory and motor processing.
 * 
 * The AgentBrain class serves as the crucial interface that connects the neural brain
 * system to the external world. It handles sensory transduction (converting world
 * data into neural inputs) and motor decoding (converting neural activity into actions).
 * This class enables the brain to interact with environments, receive sensory information,
 * and generate appropriate motor commands.
 * 
 * AgentBrain provides the bridge between high-level neural computation and concrete
 * action execution in the simulated world. It manages the conversion of sensory
 * percepts into neural current injections and the decoding of motor neuron activity
 * into executable actions.
 * 
 * @note AgentBrain works in conjunction with SimpleWorld for environment simulation
 *       and provides the interface through which the brain interacts with external systems.
 * 
 * @see SimpleWorld for the simulated environment
 * @see Brain for the neural computation core
 * @see SensoryPercept for sensory data format
 * @see MotorCommand for action format
 */

class AgentBrain {
public:
    /**
     * @brief Constructor
     * 
     * Creates an AgentBrain instance that will interface with the provided neural brain.
     * The AgentBrain takes ownership of the brain's functionality and provides the
     * interface for sensory and motor processing.
     * 
     * @param brain Shared pointer to the neural brain that this agent will control.
     *             The brain must remain valid throughout the AgentBrain's lifetime.
     * 
     * @note The AgentBrain assumes responsibility for managing the brain's interface
     *       with the world, including sensory transduction and motor decoding.
     * 
     * @see Brain for the neural computation core
     * @see SimpleWorld for environment simulation
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Default destructor
     * 
     * Cleans up AgentBrain resources and terminates any ongoing agent-world interactions.
     * The destructor ensures proper cleanup of neural connections and agent state.
     * 
     * @note Cleanup includes releasing neural resources and terminating any pending
     *       simulation steps or processing.
     */
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * @brief Initialize agent with world environment
     * 
     * Sets up the agent's sensory and motor systems in preparation for world interaction.
     * This method should be called once after creating the AgentBrain to establish
     * the connection between the brain and the environment.
     * 
     * @param world Reference to the world object that provides sensory input and
     *              executes motor actions. The world must remain valid throughout
     *              the agent's lifetime.
     * 
     * @note Initialization typically occurs after the Brain object has been created
     *       and configured. This method sets up the agent's neural connectivity to the
     *              world and configures sensory/motor interfaces.
     * 
     * @see Brain for neural system setup
     * @see SimpleWorld for environment interface
     * @see reset for re-initialization after reset
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get sensory input size expected by brain
     * 
     * Returns the number of sensory inputs that the connected brain expects per
     * simulation step. This information is useful for configuring sensory systems
     * and validating sensory data.
     * 
     * @return size_t Number of sensory inputs expected by the brain.
     *         This corresponds to the size of sensory percept arrays that should
     *         be provided to processSensoryInput().
     * 
     * @note The sensory input size is determined by the brain's configuration and
     *       the organization of its sensory neuron populations. It remains constant
     *       throughout the agent's lifetime.
     * 
     * @see processSensoryInput for providing sensory data
     * @see getMotorOutputSize for motor system information
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get motor output size expected
     * 
     * Returns the number of motor outputs that the connected brain produces per
     * simulation step. This information is useful for configuring motor systems
     * and interpreting motor command decoding.
     * 
     * @return size_t Number of motor outputs produced by the brain.
     *         This corresponds to the size of motor command arrays that can be
     *         decoded from motor neuron activity.
     * 
     * @note The motor output size is determined by the brain's configuration and
     *       the organization of its motor neuron populations. It remains constant
     *       throughout the agent's lifetime.
     * 
     * @see decodeMotorCommand for converting neural activity to actions
     * @see getSensoryInputSize for sensory system information
     */
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * Converts sensory percept data from the environment into neural current inputs
     * that are injected into the brain's sensory neurons. This is the transduction
     * process that brings external world information into the neural system.
     * 
     * @param percept Sensory percept containing environmental data including vision,
     *               touch, internal state, and other sensory modalities. The percept
     *               represents the current state of the world as perceived by the agent.
     * 
     * @note This method typically interfaces with SimpleWorld::getSensoryPercept()
     *       to obtain the current sensory data and converts it into neural current
     *               that can be processed by the brain's sensory neuron populations.
     * 
     * @warning Sensory input should be provided each simulation step to maintain
     *         consistent sensory input to the neural system. Missing input may lead
     *         to incorrect neural processing.
     * 
     * @see getSensoryInputSize for expected input size
     * @see Brain::receiveSensoryInput for neural injection
     * @see SimpleWorld::getSensoryPercept for source of percept data
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * Converts the brain's motor neuron activity into executable motor commands
     * that can be used by the agent or world simulation. This method performs
     * the final stage of the sensorimotor loop by translating neural activity
     * into actionable behavior.
     * 
     * @return MotorCommand Structured motor command containing action parameters
     *         such as movement direction, speed, and action type. Returns a default
     *         command if no significant motor activity is detected.
     * 
     * @note This method typically interfaces with SimpleWorld::applyMotorCommand()
     *       to execute the generated action in the simulated environment. The motor
     *       command is the output of the entire sensorimotor processing pipeline.
     * 
     * @see decodeFromMotorNeurons for low-level neural decoding
     * @see selectWithCuriosity for curiosity-modulated action selection
     * @see Brain::produceAction for brain-level action generation
     * @see SimpleWorld::applyMotorCommand for action execution
     * 
     * @example
     * @code
     * AgentBrain agent(brain);
     * agent.initialize(world);
     * 
     * for (step = 0; step < episodeLength; ++step) {
     *     // Process sensory input
     *     agent.processSensoryInput(world->getSensoryPercept());
     *     
     *     // Generate motor command
     *     MotorCommand cmd = agent.decodeMotorCommand();
     *     
     *     // Execute action
     *     world->applyMotorCommand(cmd, currentTime);
     * }
     * @endcode
     */
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    /**
     * @brief Apply neuromodulation based on reward
     * 
     * Applies reward-based neuromodulation to the brain's plasticity and learning systems.
     * This implements reward prediction error signaling, which is crucial for reinforcement
     * learning and adaptive behavior.
     * 
     * @param reward Actual reward received by the agent (e.g., resource found, damage avoided)
     * @param predictedReward Expected reward based on current policy/model
     * 
     * @note Reward modulation typically affects dopamine-related systems and reinforces
     *               neural pathways that led to successful outcomes. The difference between
     *               actual and predicted reward (prediction error) drives learning.
     * 
     * @see applyNeuromodulation for general neuromodulation
     * @see getDopamine for reward-related neuromodulator system
     * @see getPredictionError for prediction error signal
     * @see enableRewardModulation for configuration
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    /**
     * @brief Update development system
     * 
     * Advances the agent's developmental system, which manages age-dependent changes
     * to neural development, plasticity, and behavior. Development typically occurs
     * early in the agent's lifespan and shapes long-term neural architecture.
     * 
     * @param timestep Simulation timestep duration
     * 
     * @note Development influences synaptic pruning, myelination, connectivity changes,
     *               and other age-dependent processes. The developmental stage can be
     *               controlled externally via setDevelopmentalStage().
     * 
     * @see getDevelopmentalStage for current developmental stage
     * @see setDevelopmentalStage for external stage control
     * @see getDevelopmentSystem for developmental process management
     * @see enableDevelopment for configuration
     */
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the agent, which determines the
     * type and intensity of developmental processes occurring. Developmental stages
     * typically include periods of rapid growth, refinement, and specialization.
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @note Developmental stages affect neural development, plasticity, and behavior.
     *               Different stages support different types of learning and adaptation.
     * 
     * @see setDevelopmentalStage for setting developmental stage
     * @see updateDevelopment for developmental process management
     * @see getDevelopmentSystem for developmental system details
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    /**
     * @brief Get neuromodulation level
     * 
     * Returns the current overall neuromodulation level, which represents the combined
     * influence of various neuromodulators (dopamine, serotonin, acetylcholine, etc.)
     * on neural processing and plasticity.
     * 
     * @return float Current neuromodulation level (0.0 to 1.0, normalized).
     *         Higher values indicate stronger neuromodulatory influence.
     * 
     * @note Neuromodulation level affects neural excitability, plasticity, and learning.
     *               It's influenced by reward prediction errors, curiosity, and other factors.
     * 
     * @see applyNeuromodulation for neuromodulation application
     * @see getDopamine for dopamine-specific level
     * @see getCuriosity for curiosity level
     * @see getNovelty for novelty level
     * @see getPredictionError for prediction error level
     */
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    /**
     * @brief Get curiosity level
     * 
     * Returns the current curiosity level, which drives exploration behavior and
     * motivates the agent to seek novel information and experiences. Curiosity is
     * essential for efficient exploration and learning in unknown environments.
     * 
     * @return float Current curiosity level (0.0 to 1.0, normalized).
     *         Higher values indicate stronger exploration motivation.
     * 
     * @note Curiosity level is influenced by novelty detection and novelty decay.
     *               It modulates action selection to balance exploitation and exploration.
     * 
     * @see getNovelty for novelty detection level
     * @see selectWithCuriosity for curiosity-modulated action selection
     * @see enableCuriosity for configuration
     * @see getCuriosityLevel for method name consistency (deprecated)
     */
    float getCuriosityLevel() const;
    
    // Get novelty level
    /**
     * @brief Get novelty level
     * 
     * Returns the current novelty level, which represents how novel the current
     * sensory input is compared to previous experiences. Novelty detection is
     * crucial for curiosity-driven learning and efficient exploration.
     * 
     * @return float Current novelty level (0.0 to 1.0, normalized).
     *         Higher values indicate more novel sensory input.
     * 
     * @note Novelty level is computed by comparing current sensory input to
     *               previous percepts. It decays over time to prevent permanent
     *               novelty from driving behavior.
     * 
     * @see getCuriosityLevel for curiosity level (driven by novelty)
     * @see sensoryNoveltyDecay_ for novelty decay mechanism
     */
    float getNoveltyLevel() const;
    
    // Get prediction error
    /**
     * @brief Get prediction error
     * 
     * Returns the current prediction error signal, which represents the difference
     * between predicted and actual sensory input. Prediction error is a fundamental
     * learning signal in predictive coding and reinforcement learning.
     * 
     * @return float Current prediction error signal.
     *         Positive values indicate prediction failure, negative values indicate
     *         over-prediction. Magnitude represents prediction magnitude.
     * 
     * @note Prediction error drives learning in predictive coding systems and
     *               influences plasticity rules. It's used by applyRewardModulation()
     *               and other neuromodulatory systems.
     * 
     * @see applyRewardModulation for prediction error usage
     * @see getPredictionErrorSignal for detailed prediction error system
     * @see getPredictionSystem for prediction system details
     */
    float getPredictionError() const;
    
    // Reset agent for new episode
    /**
     * @brief Reset agent for new episode
     * 
     * Resets the agent's internal state in preparation for a new simulation episode.
     * This clears agent-specific state including neuromodulation levels, developmental
     * age, and previous sensory history while preserving the brain's learned state.
     * 
     * @note Reset prepares the agent for a new episode while maintaining the brain's
     *               learned neural configuration. This is different from brain reset(),
     *               which resets neural connectivity and synaptic weights.
     * 
     * @see Brain::reset for neural system reset
     * @see initialize for full re-initialization
     * @see getSensoryInputSize for verifying reset completeness
     */
    void reset();
    
    // Get brain pointer
    /**
     * @brief Get brain pointer
     * 
     * Returns a pointer to the neural brain that this agent controls. This method
     * provides direct access to the brain for advanced usage or integration.
     * 
     * @return Brain* Pointer to the neural brain object. Returns nullptr if no brain
     *         is assigned.
     * 
     * @retval nullptr No brain assigned to this agent
     * @retval non-null Pointer to the neural brain
     * 
     * @note This method provides low-level access to the brain for advanced users
     *               who need direct brain control. For typical usage, AgentBrain provides
     *               the complete interface through its public methods.
     * 
     * @see Brain for the neural computation core
     * @see AgentBrain constructor for brain assignment
     */
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
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
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
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
