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
// The AgentBrain class serves as the critical interface between the internal
// neural processing of the NLM brain and the external environment. It converts
// between the brain's neural activity patterns and the agent's observable actions
// and sensory experiences, enabling embodied cognition and interaction with the world.
//
// Key Functions:
// - Sensory transduction: Converts neural activity to sensory percepts
// - Motor decoding: Interprets neural activity as motor commands
// - Action execution: Translates motor commands into world effects
// - Neuromodulation: Applies reward and learning signals to influence brain behavior
// - Development: Supports adaptive development and plasticity
//
// The agent operates in a closed loop with the world:
// 1. Sensor receives environmental input
// 2. Sensory percept converts to neural activity patterns
// 3. Brain processes and generates motor commands
// 4. Motor decoder interprets neural activity as actions
// 5. World executes actions and provides new sensory feedback
// 6. Loop continues for continuous interaction
//
// Usage example:
// @code
// // Initialize agent with brain and world
// auto brain = std::make_shared<Brain>(config);
// auto world = std::make_shared<SimpleWorld>();
// auto agentBrain = std::make_unique<AgentBrain>(brain);
// agentBrain->initialize(*world);
// 
// while (agentAlive && !episodeComplete) {
//     // Get current sensory percept from world
//     auto percept = world->getSensoryPercept();
//     
//     // Convert to neural activity and process with brain
//     agentBrain->processSensoryInput(percept);
//     brain->step(currentStep);
//     
//     // Generate action and execute in world
//     auto action = agentBrain->decodeMotorCommand();
//     auto result = world->applyMotorCommand(action, currentTime);
//     
//     // Apply neuromodulation based on performance
//     agentBrain->applyRewardModulation(result.reward, result.predictedReward);
//     
//     // Update for next timestep
//     currentStep++;
//     currentTime += timestep;
// }
// @endcode
class AgentBrain {
public:
    /**
     * Constructor - creates agent-brain interface
     * 
     * Establishes the connection between the neural brain and the agent's environment.
     * The agent brain manages the bidirectional flow of information between
     * neural processing and world interaction.
     * 
     * @param brain Shared pointer to the neural brain that provides processing
     * @throws May throw std::bad_alloc if memory allocation fails
     */
    AgentBrain(std::shared_ptr<Brain> brain);

    /**
     * Destructor - cleans up agent resources
     */
    ~AgentBrain();

    // Configuration and initialization

    /**
     * Initialize agent with world environment
     * 
     * Sets up the agent brain for operation with a specific world.
     * Establishes connections between brain neurons and world sensory/motor systems.
     * Configures integration mechanisms for perception and action.
     * 
     * @param world Reference to the world environment
     * @see processSensoryInput()
     * @see decodeMotorCommand()
     */
    void initialize(const SimpleWorld& world);

    /**
     * Get sensory input size expected by brain
     * 
     * Returns the dimensionality of the neural input space expected by the brain.
     * This determines the format and length of sensory percepts.
     * 
     * @return Number of sensory input units (neurons) expected
     */
    size_t getSensoryInputSize() const;

    /**
     * Get motor output size expected
     * 
     * Returns the dimensionality of motor output space that the brain can produce.
     * This determines the format and length of motor commands that can be generated.
     * 
     * @return Number of motor output units (neurons) available
     */
    size_t getMotorOutputSize() const;

    // Core processing functions

    /**
     * Process sensory percept and inject into brain
     * 
     * Performs the critical sensory transduction function: converts environmental
     * sensory data into neural activity patterns that can be processed by the brain.
     * This is the input pathway from the world to the brain.
     * 
     * @param percept Sensory percept containing environmental information
     * @see getSensoryInputSize()
     * @see getSensoryInputSize()
     */
    void processSensoryInput(const SensoryPercept& percept);

    /**
     * Decode brain motor activity into motor command
     * 
     * Performs motor decoding: interprets neural activity patterns as actionable
     * motor commands for interaction with the environment. This is the output
     * pathway from the brain to the world.
     * 
     * @return MotorCommand representing the selected action
     * 
     * @see getMotorOutputSize()
     * @see applyMotorCommand()  // World-side execution
     */
    MotorCommand decodeMotorCommand();

    // Learning and adaptation

    /**
     * Apply neuromodulation based on reward
     * 
     * Integrates reward signals into the brain's learning processes.
     * Implements reinforcement learning by modulating neural plasticity
     * based on prediction errors and actual outcomes.
     * 
     * @param reward Actual reward received from world interaction
     * @param predictedReward Expected reward (prediction error)
     * 
     * @see applyNeuromodulation()  // Brain-side application
     */
    void applyRewardModulation(float reward, float predictedReward);

    /**
     * Update development system
     * 
     * Drives adaptive development in response to experience.
     * Modifies neural properties, connectivity, and learning rates based on
     * developmental stage and environmental interaction.
     * 
     * @param timestep Time step increment for development
     */
    void updateDevelopment(double timestep);

    // State and monitoring

    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * Get neuromodulation level
     * 
     * Returns the current level of neuromodulatory influence on brain processing.
     * Reflects the integrated effects of reward, curiosity, novelty, and prediction error.
     * 
     * @return Neuromodulation level (0.0 to 1.0)
     */
    float getNeuromodulationLevel() const;

    /**
     * Get curiosity level
     * 
     * Represents the agent's drive to explore novel and challenging experiences.
     * Higher levels increase exploratory behavior and attention to new inputs.
     * 
     * @return Curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;

    /**
     * Get novelty level
     * 
     * Measures the surprisal of current sensory input compared to history.
     * High novelty signals indicate unexpected or interesting events.
     * 
     * @return Novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;

    /**
     * Get prediction error
     * 
     * Computes the discrepancy between predicted and actual sensory states.
     * This is the fundamental learning signal for predictive coding.
     * 
     * @return Prediction error magnitude (0.0 = perfect prediction)
     */
    float getPredictionError() const;

    /**
     * Reset agent for new episode
     * 
     * Resets agent state to initial conditions for a new learning episode.
     * Clears memory traces, resets neuromodulatory signals, and prepares
     * for fresh experience and learning.
     * 
     * @see reset()  // Agent-wide reset
     */
    void reset();

    // Brain access

    /**
     * Get brain pointer
     * 
     * Provides direct access to the underlying neural brain for specialized tasks
     * or testing. The brain is owned by this AgentBrain instance.
     * 
     * @return Pointer to brain, or nullptr if not initialized
     */
    Brain* getBrain() { return brain_.get(); }

    // Configuration

    /**
     * Enable reward modulation
     * 
     * Turns on/off reward-based learning and plasticity modulation.
     * Can be disabled for testing or specific experimental conditions.
     * 
     * @param enable true to enable reward modulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }

    /**
     * Enable structural plasticity
     * 
     * Turns on/off long-term structural changes (synaptogenesis, pruning).
     * Structural plasticity affects network topology over time.
     * 
     * @param enable true to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }

    /**
     * Enable development
     * 
     * Turns on/off developmental processes and plasticity modulation.
     * Development affects neural properties and learning rates over time.
     * 
     * @param enable true to enable development
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }

    /**
     * Enable curiosity
     * 
     * Turns on/off curiosity-driven exploration and learning.
     * Curiosity increases interest in novel and challenging experiences.
     * 
     * @param enable true to enable curiosity
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

    // Configuration status

    /**
     * Check if reward modulation is enabled
     * 
     * @return true if reward modulation is active
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }

    /**
     * Check if structural plasticity is enabled
     * 
     * @return true if structural plasticity is active
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }

    /**
     * Check if development is enabled
     * 
     * @return true if development processes are active
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }

    /**
     * Check if curiosity is enabled
     * 
     * @return true if curiosity-driven processes are active
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    // Core components
    std::shared_ptr<Brain> brain_;  ///< Neural brain providing processing

    // Motor neuron groups (for decoding)
    std::vector<Neuron*> motorForward_;     ///< Forward motion neurons
    std::vector<Neuron*> motorBackward_;   ///< Backward motion neurons
    std::vector<Neuron*> motorTurnLeft_;   ///< Left turn neurons
    std::vector<Neuron*> motorTurnRight_;  ///< Right turn neurons
    std::vector<Neuron*> motorInteract_;   ///< Interaction/action neurons
    std::vector<Neuron*> motorWait_;       ///< Wait/idle neurons

    // Sensory neuron groups (for transduction)
    std::vector<Neuron*> sensoryVision_;      ///< Vision-related neurons
    std::vector<Neuron*> sensoryTouch_;      ///< Touch/proximity neurons
    std::vector<Neuron*> sensoryInternal_;   ///< Internal state neurons
    std::vector<Neuron*> sensoryProprioception_; ///< Proprioception neurons

    // Neuromodulation state
    float dopamineLevel_;      ///< Dopamine concentration (reward signal)
    float noveltyLevel_;       ///< Novelty detection level
    float curiosityLevel_;    ///< Curiosity drive level
    float predictionError_;   ///< Prediction error signal
    float expectedReward_;    ///< Expected reward from predictions

    // Development state
    double developmentalAge_;       ///< Age of agent in simulation time
    float plasticityModifier_;     ///< Modifier for plasticity based on development

    // Configuration flags
    bool rewardModulationEnabled_;    ///< Whether reward modulation is active
    bool structuralPlasticityEnabled_;///< Whether structural plasticity is active
    bool developmentEnabled_;       ///< Whether development processes are active
    bool curiosityEnabled_;         ///< Whether curiosity-driven processes are active

    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;    ///< Previous vision input for change detection
    float sensoryNoveltyDecay_;            ///< Decay rate for novelty memory

    // Private methods for internal processing

    /**
     * Motor decoding: convert neural activity to motor command
     * 
     * Implements the motor decoding function: interprets patterns of neural
     * activity as specific motor commands for world interaction.
     * Uses population coding where different combinations of neurons represent
     * different actions.
     * 
     * @return MotorCommand decoded from current neural activity
     */
    MotorCommand decodeFromMotorNeurons();

    /**
     * Motor command selection with curiosity/exploration
     * 
     * Integrates curiosity-driven exploration with default motor responses.
     * When curiosity is high, increases probability of novel actions and
     * exploration of less-frequent motor patterns.
     * 
     * @param defaultCmd Default motor command based on neural activity
     * @return MotorCommand possibly modified for exploration
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
};

} // namespace nlm
