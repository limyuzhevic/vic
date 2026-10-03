// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Agent-Brain Interface
//
// The AgentBrain class provides the crucial interface between the artificial neural
// substrate (Brain) and the external environment (World). It handles sensory transduction,
// motor decoding, and integrates neuromodulation signals to enable adaptive behavior.
//
// Key Functions:
// - Sensory Processing: Converts world observations to neural inputs
// - Motor Output: Translates neural activity to executable actions
// - Neuromodulation: Applies reward and prediction error signals
// - Development: Manages developmental stages and plasticity modifiers
// - Curiosity & Exploration: Drives discovery and novel behavior

#pragma once

/**
 * @file AgentBrain.hpp
 * @brief AgentBrain class - interface connecting NLM brain to world
 * @author NLM Project
 * @date Phase 6 Implementation
 * @version 0.1.0
 * 
 * The AgentBrain class serves as the bridge between the neural processing substrate
 * (Brain) and the external simulation environment (World). It implements the essential
 * perception-action cycle:
 * 
 * 1. Sensory Input: World observations are converted to neural signals
 * 2. Motor Output: Neural activity is decoded into motor commands
 * 3. Action Execution: Motor commands are applied to the world
 * 4. Reward Learning: Prediction errors and rewards modulate learning
 * 
 * This class encapsulates the interaction between the artificial brain and its
 * environment, providing a clean abstraction for agent-based behavior in NLM.
 */

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief Low-level motor command for neural control
 * 
 * Motor commands represent the output of motor neurons and control
 * the agent's interaction with the environment. Each command corresponds
 * to a specific type of action the agent can perform.
 */
enum class MotorCommand {
    MoveForward,   /**< Move forward in the current direction */
    MoveBackward,  /**< Move backward */
    TurnLeft,      /**< Rotate counterclockwise */
    TurnRight,     /**< Rotate clockwise */
    LookLeft,      /**< Turn head/viewport left */
    LookRight,     /**< Turn head/viewport right */
    Interact,      /**< Interact with objects in the environment */
    Wait,          /**< Do nothing (maintain current state) */
};

/**
 * @brief Action result from world interaction
 * 
 * Represents the outcome of applying a motor command to the world,
 * including reward, success status, and any relevant feedback.
 */
struct ActionResult {
    float reward;      /**< Reward value from the action (-1.0 to 1.0) */
    bool success;      /**< Whether the action was successful */
    std::string message; /**< Optional message about the action result */
    
    ActionResult() : reward(0.0f), success(false), message() {}
    ActionResult(float r, bool s, const std::string& m = "")
        : reward(r), success(s), message(m) {}
};

/**
 * @brief AgentBrain - connects NLM brain to the world
 * 
 * The AgentBrain class implements the essential interface between the artificial
 * neural brain and its simulation environment. It handles:
 * 
 * - **Sensory Processing**: Converts world observations (vision, touch, internal)
 *   into neural current injections for sensory neurons
 * - **Motor Decoding**: Interprets motor neuron activity to generate appropriate
 *   actions for the environment
 * - **Neuromodulation**: Applies reward signals, prediction errors, and other
 *   neuromodulatory influences to guide learning and behavior
 * - **Development**: Manages developmental stages and plasticity modifiers that
 *   affect learning throughout the agent's lifetime
 * - **Curiosity & Exploration**: Generates exploratory behavior based on
 *   novelty detection and unexpected prediction errors
 * 
 * The AgentBrain enables the NLM brain to act in and learn from an environment,
 * implementing the closed-loop control system essential for adaptive behavior.
 * 
 * Key Design Principles:
 * - **Biologically Inspired**: Neuromodulation mechanisms mirror biological systems
 * - **Modular**: Each subsystem (sensory, motor, neuromodulation) can be independently configured
 * - **Extensible**: New sensory inputs, motor outputs, and neuromodulators can be added
 * - **Developmentally Plausible**: Behavioral changes occur through developmental processes
 * 
 * Example Usage:
 * @code
 * // Create brain and agent
 * auto config = pynlm.createDefaultConfig()
 * auto brain = pynlm.createBrain(config)
 * auto agent = pynlm.createAgentBrain(brain)
 * 
 * // Create world and initialize agent
 * auto world = pynlm.createSimpleWorld()
 * world.configure(20, 20, 8, 8)
 * agent.initialize(world)
 * 
 * // Enable adaptive subsystems
 * agent.enableRewardModulation(True)
 * agent.enableCuriosity(True)
 * agent.enableDevelopment(True)
 * 
 * // Run simulation loop
 * for step in range(1000):
 *     # 1. Update world
 *     world.update(0.1)
 *     
 *     # 2. Get sensory input
 *     percept = world.getSensoryPercept()
 *     agent.processSensoryInput(percept)
 *     
 *     # 3. Brain computation
 *     brain.step(step)
 *     
 *     # 4. Generate action
 *     action = agent.decodeMotorCommand()
 *     world.applyMotorCommand(action, world.getSimulationTime())
 *     
 *     # 5. Learn from consequences
 *     reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
 *     agent.applyRewardModulation(reward, 0.0)
 * @endcode
 */
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    /**
     * @brief Initialize agent with world interface
     * 
     * Sets up the agent's internal state and neuron groups for processing
     * sensory inputs and producing motor outputs. Must be called before
     * the agent can process sensory information or produce actions.
     * 
     * @param world Reference to the simulation world
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected size of sensory input data
     * 
     * Returns the dimensionality of sensory data that this agent expects
     * from the world. This includes vision, touch, internal signals, and
     * proprioception data.
     * 
     * @return Number of expected sensory input values
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the size of motor output commands
     * 
     * Returns the number of possible motor commands this agent can produce.
     * This determines the dimensionality of action space.
     * 
     * @return Number of possible motor output values
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * Converts world observations into neural current injections for the
     * brain's sensory neurons. Updates internal state including novelty
     * detection and curiosity levels.
     * 
     * @param percept Sensory data from the world (vision, touch, internal signals)
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * Interprets the firing patterns in motor neurons to produce a specific
     * action for the environment. Includes curiosity-based exploration
     * and novelty-driven behavior selection.
     * 
     * @return Motor command to execute in the world
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply reward-based neuromodulation
     * 
     * Applies reward signals and prediction errors to guide learning.
     * Updates dopamine levels, eligibility traces, and plasticity factors
     * based on received rewards and expected outcomes.
     * 
     * @param reward Actual reward received from the world
     * @param predictedReward Expected reward before the action
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * Advances the agent's developmental stage and updates plasticity
     * modifiers based on accumulated experience. Development affects
     * learning rates, memory consolidation, and behavioral flexibility.
     * 
     * @param timestep Time step for the current update
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level
     * 
     * Returns the current level of neuromodulatory influence (primarily dopamine).
     * This affects neural excitability and plasticity.
     * 
     * @return Neuromodulation level (typically -1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get curiosity level
     * 
     * Returns the agent's current drive for exploration and novelty seeking.
     * Curiosity increases exploration of novel situations and drives learning
     * about uncertain environments.
     * 
     * @return Curiosity level (typically 0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get novelty level
     * 
     * Returns the current level of novelty detection. Novelty signals
     * highlight unexpected or surprising sensory input, driving curiosity
     * and attention.
     * 
     * @return Novelty level (typically 0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * Returns the difference between expected and actual rewards. Prediction
     * errors are crucial for learning and updating internal models of the environment.
     * 
     * @return Prediction error (can be positive or negative)
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Resets the agent's internal state to initial conditions, preparing for
     * a new simulation episode. This includes resetting neuromodulation levels,
     * developmental state, and memory traces.
     */
    void reset();
    
    /**
     * @brief Get the underlying brain
     * 
     * Provides access to the brain object for advanced operations and debugging.
     * 
     * @return Pointer to the brain's configuration object
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Configuration methods for enabling/disabling subsystems
     * 
     * Controls which adaptive systems are active in the agent:
     * - Reward modulation enables learning from consequences
     * - Structural plasticity allows growth and pruning of connections
     * - Development enables age-dependent changes
     * - Curiosity drives exploration and novelty seeking
     * 
     * @param enable Whether to enable/disable the respective subsystem
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Decode motor command from motor neuron activity
     * 
     * Internal method that converts patterns of motor neuron firing into
     * specific motor commands. Uses population coding where different
     * groups of motor neurons represent different actions.
     * 
     * @return Decoded motor command based on neural activity patterns
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Select motor command with curiosity-based exploration
     * 
     * Adds exploration to action selection when curiosity is high. The
     * probability of selecting random actions increases with curiosity level,
     * promoting discovery of novel behaviors and solutions.
     * 
     * @param defaultCmd The normally selected motor command
     * @return Either the default command or a random exploratory action
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    /**
     * @brief Motor neuron groups for action selection
     * 
     * Each action type is represented by a population of motor neurons.
     * The agent selects actions based on which population has the highest
     * average firing rate.
     */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    /**
     * @brief Sensory neuron groups for input processing
     * 
     * Different sensory modalities are represented by distinct populations:
     * - Vision: Processed visual input for spatial awareness
     * - Touch: Processed tactile input for object interaction
     * - Internal: Processed internal state signals for homeostasis
     * - Proprioception: Processed body position/orientation signals
     */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    /**
     * @brief Neuromodulation state
     * 
     * Current levels of neuromodulators that influence learning and behavior:
     * - Dopamine: Reward prediction and reinforcement learning
     * - Novelty: Detection of unexpected stimuli
     * - Curiosity: Drive for exploration and information seeking
     * - Prediction error: Mismatch between expected and actual outcomes
     */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    /**
     * @brief Development state
     * 
     * Tracks the agent's developmental progress and its effects on behavior:
     * - developmentalAge_: Accumulated experience (biological age)
     * - plasticityModifier_: Modulates learning rates throughout development
     */
    double developmentalAge_;
    float plasticityModifier_;
    
    /**
     * @brief Configuration flags for adaptive systems
     * 
     * Controls which learning and adaptive mechanisms are active:
     * - rewardModulationEnabled_: Whether reward-based learning is active
     * - structuralPlasticityEnabled_: Whether connection growth/pruning is active
     * - developmentEnabled_: Whether developmental changes occur
     * - curiosityEnabled_: Whether curiosity-driven exploration is active
     */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    /**
     * @brief Previous sensory state for novelty detection
     * 
     * Stores the previous vision input to compute novelty as the difference
     * between current and previous sensory observations. This drives curiosity
     * and exploration behavior.
     */
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
} // namespace nlm
