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
     * @brief Constructor that initializes the AgentBrain with a reference to the NLM brain
     * 
     * Creates an AgentBrain instance that will handle sensory processing, motor decoding,
     * and neuromodulation for the given brain. The brain pointer must remain valid
     * throughout the lifetime of this AgentBrain instance.
     * 
     * @param brain Shared pointer to the Brain instance that provides neural computation
     * capabilities. The brain handles real spiking neural computation, memory systems,
     * prediction, cognition, and neuromodulation. This parameter cannot be null.
     * 
     * @details The constructor automatically partitions the brain's neural populations
     * into motor and sensory groups for efficient processing. Motor neurons (6 groups)
     * are mapped to: MoveForward, MoveBackward, TurnLeft, TurnRight, Interact, Wait.
     * Sensory neurons (4 groups) are mapped to: Vision, Touch, Internal, Proprioception.
     * 
     * @note The brain_ pointer is stored as a shared_ptr to maintain proper ownership
     * and reference counting. If the brain is destroyed while this AgentBrain exists,
     * all operations will fail gracefully.
     * 
     * @example 
     * ```cpp
     * std::shared_ptr<Brain> brain = std::make_shared<Brain>(config);
     * AgentBrain agentBrain(brain);
     * ```
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor that cleans up AgentBrain resources
     * 
     * Performs cleanup of any allocated resources. The default implementation
     * properly handles the destruction of the AgentBrain instance and its internal
     * state without external dependencies.
     * 
     * @details Since all internal members are either primitive types or managed
     * through smart pointers, no explicit cleanup is required. The destructor
     * is marked default to allow for proper move semantics.
     */
    ~AgentBrain();
    
    /**
     * @brief Initializes the AgentBrain with a reference to the world
     * 
     * Prepares the AgentBrain for operation by setting up internal state based on
     * the world's configuration. This must be called before any processing occurs.
     * 
     * @param world Constant reference to the SimpleWorld instance that provides
     * the sensory experience and environment. The world contains agent body state,
     * objects, and generates sensory percepts.
     * 
     * @details This method:
     * 1. Resizes the previous vision buffer to match world vision dimensions
     * 2. Resets the developmental age to zero
     * 3. Resets the plasticity modifier to 1.0f (maximum plasticity)
     * 4. Logs initialization statistics including the number of sensory neurons
     *    in each modality (vision, touch, internal, etc.)
     * 
     * @note Must be called before any other AgentBrain methods that depend on
     * world state or sensory processing. Typically called once at agent startup.
     * 
     * @example
     * ```cpp
     * AgentBrain agentBrain(brain);
     * SimpleWorld world;
     * world.configure(100.0f, 100.0f, 16, 16);
     * agentBrain.initialize(world);
     * ```
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Gets the expected sensory input size for the brain
     * 
     * Returns the number of sensory features the brain expects to receive
     * during each processing step. This defines the dimensionality of the
     * input signal vector.
     * 
     * @return size_t The expected sensory input size, which is the sum of:
     *         - Vision: 256 values (16×16 grid)
     *         - Touch: 8 values (binary collision signals)
     *         - Internal: 4 values (homeostatic signals)
     *         - Proprioception: 6 values (body position/velocity)
     *         Total: 274 values
     * 
     * @details The sensory input is provided by the brain's sensory processing
     * pipeline and corresponds to the concatenated output of all sensory modalities
     * from the agent's perception. The SensoryPercept class provides this data.
     * 
     * @note This value is determined by the brain's architecture and must match
     * the brain's sensory input expectations. Check the brain implementation
     * to understand the exact mapping to neural populations.
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Gets the expected motor output size for the brain
     * 
     * Returns the number of motor signals the brain produces during each processing
     * step. This defines the dimensionality of the output motor command vector.
     * 
     * @return size_t The expected motor output size, which is:
     *         - 6 motor neuron groups: MoveForward, MoveBackward, TurnLeft,
     *           TurnRight, Interact, Wait
     *         - Each group corresponds to one motor action command
     
     * @details The motor output is produced by the brain's motor neuron populations
     * and is decoded into high-level MotorCommand enum values. Each motor neuron
     * group corresponds to one possible motor action that can be executed by the agent.
     * 
     * @note This method helps in configuring the agent's motor output interface
     * and understanding the brain's motor command space dimensionality.
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Processes sensory input from the agent and injects into the brain
     * 
     * Converts the agent's sensory percept (sensory experience) into neural current
     * injections that drive the brain's neural populations. This is the primary
     * sensory transduction pathway from world perception to neural computation.
     * 
     * @param percept Constant reference to the SensoryPercept instance containing
     * the agent's current sensory experience. Includes vision, touch, internal,
     * proprioception, and audio signals. Cannot be null.
     * 
     * @details This method performs several key operations:
     * 1. **Vision Processing**: Scales 16×16 vision grid (256 values) to vision neurons
     * 2. **Touch Processing**: Scales 8 touch values to touch neurons  
     * 3. **Internal Processing**: Centers and scales internal homeostatic signals
     * 4. **Proprioception**: Centers and scales body position/velocity signals
     * 5. **Novelty Detection**: Computes visual change from previous frame to compute
     *    novelty level for curiosity-based exploration
     * 6. **Curiosity Update**: Calculates curiosity level based on novelty and prediction
     *    error to drive exploration behavior
     * 
     * @note Must be called after world.update() and before decodeMotorCommand().
     * This maintains the proper sensory-to-motor processing loop.
     * 
     * @warning The brain_ pointer must be valid and initialized. If not, this
     * method does nothing silently.
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decodes brain motor activity into actionable motor commands
     * 
     * Converts neural activity from motor neuron populations into high-level
     * MotorCommand enum values that can be executed by the agent in the world.
     * This is the motor decoding pathway from neural computation to action.
     * 
     * @return MotorCommand The decoded motor command. Returns MotorCommand::Wait
     * if:
     *         - Brain pointer is null or brain not initialized
     *         - No motor group shows significant activity (<0.5 threshold)
     *         - Multiple groups have equal activity (prefers 'Wait')
     
 * @details The decoding process:
     * 1. Calculates average activity level for each of 6 motor groups using
     *    membrane potential deviation from resting potential
     * 2. Compares activities to select the highest-activity group
     * 3. Applies a threshold (0.5) to filter out weak/noisy activity
     * 4. Applies curiosity-based exploration if curiosity is high (>0.3)
     * 5. Returns the selected MotorCommand for world execution
     * 
     * @note Curiosity exploration adds stochasticity: when curiosity > 0.5,
     * there's a chance (curiosity × 0.3) to randomly select a motor command
     * instead of the highest-activity one, promoting exploration.
     * 
     * @example
     * ```cpp
     * MotorCommand cmd = agentBrain.decodeMotorCommand();
     * switch (cmd) {
     *     case MotorCommand::MoveForward: applyForce(forward); break;
     *     case MotorCommand::TurnLeft: rotate(-angularSpeed); break;
     *     // ... handle other commands
     * }
     * ```
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Applies neuromodulation based on reward prediction error
     * 
     * Implements reward-modulated synaptic plasticity and dopamine signaling
     * using the difference between received reward and predicted reward. This
     * drives reinforcement learning and behavior adaptation.
     * 
     * @param reward The actual reward received from the environment (typically
     * between -1.0 and +1.0). Positive rewards indicate beneficial actions,
     * negative rewards indicate harmful actions.
     * 
     * @param predictedReward The expected reward predicted by the brain's
     * prediction system. This comes from forward models or value estimates.
     * 
     * @details Neuromodulation operations:
     * 1. Computes prediction error (delta = reward - predictedReward)
     * 2. Updates expected reward using exponential moving average (0.95 weight
     *    on history, 0.05 on current reward)
     * 3. Sets dopamineLevel_ to prediction error (clamped -1.0 to +1.0)
     * 4. Applies reward-modulated weight changes to all synapses with eligibility
     *    traces (delta * eligibility * plasticityModifier)
     * 5. Updates STDP plasticity parameters based on dopamine level
     * 6. Decays eligibility traces (0.1 per reward event)
     * 
     * @note This is the core learning mechanism in the agent. The neuromodulation
     * enables the brain to learn from experience through spike-timing dependent
     * plasticity (STDP) modulated by reward prediction error.
     * 
     * @warning Reward modulation is only active if rewardModulationEnabled_ is
     * true. Check enableRewardModulation() to enable/disable this learning mode.
     * 
     * @example
     * ```cpp
     * // During episode execution
     * float reward = world.applyAction(command);
 * agentBrain.applyRewardModulation(reward, predictedReward);
     * ```
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Updates the agent's developmental system
     * 
     * Manages developmental changes in brain plasticity, connectivity, and
     * behavior based on developmental age. This enables lifelong learning
     * and adaptation throughout the agent's lifetime.
     * 
     * @param timestep The time increment since the last developmental update
     * (typically the simulation timestep). Should be in consistent units with
     * developmental age tracking.
     * 
     * @details Developmental stages and effects:
     * 1. **Initial Stage** (age < 60s): High plasticity (1.0), initial learning
     * 2. **Critical Period** (60s ≤ age < 300s): Moderate plasticity (0.8), sensitive
     *    to environmental input
     * 3. **Maturation** (300s ≤ age < 900s): Reduced plasticity (0.5), refined
     *    connections
     * 4. **Adult** (age ≥ 900s): Low plasticity (0.2), stable performance
     * 
     * Structural plasticity effects:
     * - Synaptogenesis rate scales with plasticityModifier_
     * - Pruning rate scales with (2.0 - plasticityModifier_)
     * - Higher plasticity = more connection formation, less pruning
     * 
     * @note Development is only active if developmentEnabled_ is true.
     * This enables biological-like developmental trajectories and learning
     * curves that adapt to the agent's experience.
     * 
     * @warning Repeated calls without proper reset() between episodes can
     * lead to developmental age accumulating indefinitely, potentially causing
     * unintended behavioral changes.
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Gets the current developmental stage of the agent
     * 
     * Returns the developmental stage that represents the agent's current
     * life phase and associated behavioral characteristics.
     * 
     * @return DevelopmentalStage The current developmental stage:
     *         - Initial: High plasticity, rapid learning (age < 60s)
     *         - CriticalPeriod: Sensitive to environment (60s ≤ age < 300s)
     *         - Maturation: Connection refinement (300s ≤ age < 900s)
     *         - Adult: Stable, optimized behavior (age ≥ 900s)
     * 
     * @details The developmental stage influences:
     * 1. **Plasticity**: Determines how quickly synapses can change
     * 2. **Learning Rate**: Affects responsiveness to new experiences
    3. **Exploration**: Modulates curiosity and random behavior
     * 4. **Behavioral Flexibility**: Controls adaptation to new tasks
     * 
     * @note This stage is maintained by the underlying Brain instance and
     * reflects the agent's internal developmental state. It can be modified
     * by development events or external interventions.
     * 
     * @see updateDevelopment(double) for developmental progression logic
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Gets the current neuromodulation level (dopamine)
     * 
     * Returns the current dopamine level which reflects the prediction error
     * signal driving reward-modulated plasticity and motivation.
     * 
     * @return float The neuromodulation level in range [-1.0, 1.0]:
     *         - Positive values (>0): phasic excitation, learning from rewards
     *         - Negative values (<0): phasic inhibition, learning from punishments
     *         - Near zero: expected outcome, little learning
     * 
     * @details The dopamine level is updated by applyRewardModulation() based
     * on the difference between received reward and expected reward. It drives:
     * 1. **Motivation**: Higher levels increase drive for goal-directed behavior
     * 2. **Learning**: Modulates synaptic plasticity magnitude and direction
     * 3. **Attention**: Focuses processing on reward-relevant inputs
     * 
     * @note This is a key neuromodulatory signal that integrates environmental
     * feedback with internal predictions to guide learning and behavior.
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Gets the current curiosity level driving exploration
     * 
     * Returns the curiosity level that represents the agent's drive to explore
     * novel or uncertain states, promoting generalization and discovery.
     * 
     * @return float The curiosity level in range [0.0, 1.0]:
     *         - Low (<0.3): Exploits known behaviors, follows learned policies
     *         - Medium (0.3-0.5): Balanced exploration and exploitation
     *         - High (>0.5): Emphasizes exploration, tries new actions
     *         
     * @details Curiosity is computed from two sources:
     * 1. **Sensory Novelty**: Difference between current and previous visual input
     * 2. **Prediction Error**: Discrepancy between expected and actual outcomes
     * Formula: curiosity = novelty × 2.0 + |predictionError| × 0.5
     * 
     * @note Curiosity is only active if curiosityEnabled_ is true. When enabled,
     * it directly influences motor command selection via decodeMotorCommand(),
     * increasing random action selection when curiosity is high.
     * 
     * @see getNoveltyLevel() for sensory novelty computation
     * @see getPredictionError() for prediction error signal
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Gets the current novelty level indicating sensory change
     * 
     * Returns the novelty level that represents how novel the current sensory
     * input is compared to recent history, driving curiosity-based exploration.
     * 
     * @return float The novelty level in range [0.0, 1.0]:
     *         - Low (~0.0): Familiar sensory input, predictable environment
     *         - High (~1.0): Unpredictable sensory input, novel experiences
     * 
     * @details Novelty is computed as the average absolute difference between:
     * 1. Current vision percept (16×16 grid)
     * 2. Previous vision percept (stored from last step)
     * 
     * The novelty signal is exponentially decayed (decay factor stored as
     * sensoryNoveltyDecay_) to give more weight to recent changes while
     * maintaining temporal continuity.
     * 
     * @note Novelty drives curiosity, which in turn drives exploration behavior.
     * This forms a fundamental mechanism for agents to discover and adapt to
     * their environment.
     * 
     * @see getCuriosityLevel() for curiosity computation
     * @see processSensoryInput() for novelty calculation
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Gets the current prediction error signal
     * 
     * Returns the prediction error that represents the discrepancy between
     * expected and actual rewards, driving learning and adaptation.
     * 
     * @return float The prediction error in range [-1.0, 1.0]:
     *         - Positive (>0): Received reward higher than expected (surprise)
     *         - Negative (<0): Received reward lower than expected (disappointment)
     *         - Near zero: Met expectations, no learning signal
     * 
     * @details Prediction error is computed as:
     *   predictionError_ = reward - expectedReward_
     * 
     * This signal is central to reinforcement learning and drives:
     * 1. **Dopamine release**: Sets neuromodulation level
     * 2. **Synaptic plasticity**: Updates weights based on prediction error
     * 3. **Behavioral adaptation**: Adjusts future action selection
     * 
     * @note Prediction error is updated by applyRewardModulation() and reset
     * by reset(). It's a key component of temporal difference learning.
     */
    float getPredictionError() const;
    
    /**
     * @brief Resets the agent for a new episode
     * 
     * Performs complete state reset to prepare the agent for a new episode
     * or trial. This clears all neuromodulatory signals, developmental state,
     * and sensory history while maintaining configuration settings.
     * 
     * @details Reset operations include:
     * 1. Neuromodulation: Resets dopamine, novelty, curiosity, prediction error
     * 2. Development: Resets developmental age and plasticity modifier
     * 3. Sensory: Clears previous vision buffer
     * 4. Configuration: Maintains enabled/disabled flags
     * 
     * @note This should be called at the end of each episode to ensure a clean
     * slate for the next episode. Configuration settings (reward modulation,
     * development, curiosity, structural plasticity) persist across resets.
     * 
     * @example
     * ```cpp
     * while (episodes < maxEpisodes) {
     *     // Run episode...
     *     episodeBrain.reset();
     *     episodes++;
     * }
     * ```
     */
    void reset();
    
    /**
     * @brief Gets the underlying Brain pointer
     * 
     * Returns a raw pointer to the Brain instance managed by this AgentBrain.
     * This provides direct access to the brain's neural computation capabilities.
     * 
     * @return Brain* Pointer to the brain instance. May be null if brain was
     * not properly initialized during construction.
     * 
     * @note This method provides access to the brain's advanced features like
     * memory systems, prediction, and cognition. However, it's generally better
     * to use the AgentBrain interface methods for typical agent operations.
     * 
     * @warning The returned pointer is non-owning (raw pointer). Do not delete
     * it as it's owned by the shared_ptr brain_ member.
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enables or disables reward modulation
     * 
     * Sets the rewardModulationEnabled_ flag that controls whether reward
     * prediction error signals are processed and applied to synaptic plasticity.
     * 
     * @param enable If true, reward modulation is enabled (default: true).
     *        When enabled, applyRewardModulation() will update synapses
     *        based on reward prediction error.
     *        
     * @details Reward modulation is a key learning mechanism that:
     * 1. Connects environmental feedback with neural plasticity
     * 2. Enables reinforcement learning through dopamine signaling
     * 3. Updates synaptic weights based on prediction errors
     * 
     * @note Default behavior: Reward modulation is enabled.
     * Set to false to disable learning (useful for testing or evaluation).
     * 
     * @see applyRewardModulation(float, float) for the learning mechanism
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enables or disables structural plasticity
     * 
     * Sets the structuralPlasticityEnabled_ flag that controls whether the
     * agent's brain can form or prune synaptic connections during development.
     * 
     * @param enable If true, structural plasticity is enabled (default: true).
     *        When enabled, updateDevelopment() can modify connectivity patterns.
     *        
     * @details Structural plasticity affects:
     * 1. **Synaptogenesis**: Formation of new synaptic connections
     * 2. **Pruning**: Elimination of unused or weak connections
     * 3. **Development**: Age-dependent changes in connectivity
     * 
     * @note Default behavior: Structural plasticity is enabled.
     * This enables biological-like developmental trajectories.
     * 
     * @see updateDevelopment(double) for structural plasticity application
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enables or disables developmental processes
     * 
     * Sets the developmentEnabled_ flag that controls whether the agent
     * undergoes developmental changes in plasticity and behavior over time.
     * 
     * @param enable If true, development is enabled (default: true).
     *        When enabled, updateDevelopment() modifies plasticity modifier
     *        based on developmental age.
     *        
     * @details Development processes:
     * 1. **Age-dependent plasticity**: Plasticity modifier decreases with age
     * 2. **Behavioral maturation**: Changes in response to experience
     * 3. **Critical periods**: Sensitive phases for learning
     * 
     * @note Default behavior: Development is enabled.
     * Disable to keep agent at adult developmental stage (high stability).
     * 
     * @see updateDevelopment(double) for developmental updates
     * @see getDevelopmentalStage() for current developmental phase
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enables or disables curiosity-driven exploration
     * 
     * Sets the curiosityEnabled_ flag that controls whether curiosity-based
     * exploration influences motor command selection and behavior.
     * 
     * @param enable If true, curiosity is enabled (default: true).
     *        When enabled, decodeMotorCommand() can select random actions
     *        based on curiosity level.
 *        
     * @details Curiosity effects:
     * 1. **Exploration**: Random action selection when curiosity is high
     * 2. **Discovery**: Seeking novel sensory inputs
     * 3. **Generalization**: Trying new behaviors to understand environment
     * 
     * @note Default behavior: Curiosity is enabled.
     * This promotes exploration and discovery in novel environments.
     * 
     * @see decodeMotorCommand() for curiosity application
     * @see getCuriosityLevel() for curiosity computation
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Checks if reward modulation is currently enabled
     * 
     * Returns the current setting of the rewardModulationEnabled_ flag.
     * 
     * @return bool True if reward modulation is enabled, false otherwise.
     * 
     * @note This corresponds to the last enable/disable call. Default is true.
     * 
     * @see enableRewardModulation(bool) for enabling/disabling
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Checks if structural plasticity is currently enabled
     * 
     * Returns the current setting of the structuralPlasticityEnabled_ flag.
     * 
     * @return bool True if structural plasticity is enabled, false otherwise.
     * 
     * @note This corresponds to the last enable/disable call. Default is true.
     * 
     * @see enableStructuralPlasticity(bool) for enabling/disabling
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Checks if developmental processes are currently enabled
     * 
     * Returns the current setting of the developmentEnabled_ flag.
     * 
     * @return bool True if development is enabled, false otherwise.
     * 
     * @note This corresponds to the last enable/disable call. Default is true.
     * 
     * @see enableDevelopment(bool) for enabling/disabling
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Checks if curiosity-driven exploration is currently enabled
     * 
     * Returns the current setting of the curiosityEnabled_ flag.
     * 
     * @return bool True if curiosity is enabled, false otherwise.
     * 
     * @note This corresponds to the last enable/disable call. Default is true.
     * 
     * @see enableCuriosity(bool) for enabling/disabling
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Decodes motor commands from motor neuron activity (private implementation)
     * 
     * Internal method that converts neural activity patterns from motor neuron groups
     * into specific motor commands. This is the low-level decoding before curiosity
     * exploration is applied.
     * 
     * @return MotorCommand The highest-activity motor command, or MotorCommand::Wait
     * if all activities are below threshold.
     * 
     * @details This private method performs the actual neural decoding by:
     * 1. Calculating average activity for each of the 6 motor neuron groups
     * 2. Using membrane potential deviation as activity measure
     * 3. Selecting the group with maximum activity
     * 4. Applying a threshold filter (0.5) to ignore weak signals
     * 
     * @note This is called by decodeMotorCommand() after initialization and
     * before curiosity exploration is applied.
     * 
     * @see decodeMotorCommand() for the public interface
     * @see selectWithCuriosity(MotorCommand) for curiosity exploration
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Selects motor commands with curiosity-based exploration (private implementation)
     * 
     * Internal method that adds stochastic exploration to motor command selection
     * based on the agent's curiosity level. This promotes exploration of novel behaviors.
     * 
     * @param defaultCmd The motor command selected by standard decoding (from
     * decodeFromMotorNeurons()).
     * 
     * @return MotorCommand The selected motor command, which may be random if
     * curiosity is high enough.
     * 
     * @details Exploration logic:
     * 1. If curiosity > 0.5, there's a chance to select a random action
     * 2. Random chance = curiosity × 0.3 (up to 30% probability)
     * 3. Random selection uses brain's random generator
     * 4. Otherwise, returns the default command
     * 
     * @note This implements the exploration-exploitation tradeoff fundamental
     * to intelligent behavior. Higher curiosity = more exploration.
     * 
     * @see decodeMotorCommand() for the main interface
     * @see selectWithCuriosity(MotorCommand) for the public interface
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
