#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "AgentBrainUtils.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Connects NLM brain to the world - handles sensory transduction and motor decoding
 * 
 * AgentBrain serves as the interface layer between the internal neural processing
 * of the Brain and the external world. It translates environmental percepts into
 * neural signals and neural activity into motor commands, enabling the brain to
 * interact with its environment and produce goal-directed behavior.
 * 
 * Key functions:
 * - **Sensory Transduction**: Converts environmental percepts (vision, touch, etc.)
 *   into neural current injection into sensory neurons
 * - **Motor Decoding**: Interprets neural motor activity as actionable motor commands
 * - **Neuromodulation**: Applies reward signals and other neuromodulators to influence learning
 * - **Development**: Manages developmental stage progression and associated plasticity
 * - **Attention**: Selects relevant sensory inputs for processing
 * - **Planning**: Uses learned predictions to select optimal actions
 * 
 * The AgentBrain abstracts away the complexity of neural simulation while
 * maintaining close coupling to the brain's internal representation, ensuring
 * that behavior is grounded in neural dynamics rather than high-level algorithms.
 * 
 * @warning The AgentBrain depends on a properly initialized Brain instance
 * and configured SimpleWorld for operation. Ensure both are set up before use.
 * 
 * @see AgentBody Physical properties and state of the agent
 * @see SimpleWorld Environment in which the agent operates
 * @see SensoryPercept Environmental input captured by sensors
 * @see MotorCommand Low-level motor commands for agent movement
 * 
 * @par Example Usage:
 * @code
 * // Create components
 * auto brain = std::make_shared<Brain>(config);
 * auto agentBrain = std::make_shared<AgentBrain>(brain, config);
 * auto agentBody = std::make_shared<AgentBody>();
 * auto world = std::make_shared<SimpleWorld>();
 * 
 * // Initialize
 * agentBrain->initialize(*world);
 * 
 * // Simulation loop
 * while (simulationRunning) {
 *     // Get sensory input from world
 *     auto percept = world->getSensoryPercept();
 *     
 *     // Process and inject into brain
 *     agentBrain->processSensoryInput(percept);
 *     
 *     // Run brain simulation
 *     brain->step(currentStep);
 *     
 *     // Decode motor command
 *     auto motorCommand = agentBrain->decodeMotorCommand();
 *     
 *     // Apply to world
 *     world->applyMotorCommand(motorCommand);
 *     
 *     // Update agent state
 *     agentBody->update(motorCommand, world->getResult());
 * }
 * @endcode
 */
class AgentBrain {
public:
    /**
     * @brief Constructs an AgentBrain instance with brain and configuration
     * 
     * Creates an agent-brain interface that bridges the internal neural processing
     * of the Brain with external world interaction. The AgentBrain establishes
     * the connection between neural activity and agent behavior.
     * 
     * @param brain Shared pointer to the brain instance this agent will control
     * @param config Optional shared configuration for agent parameters and settings
     * @note The brain pointer is stored as a shared pointer and must remain valid
     * throughout the agent's lifetime. The brain takes ownership of the agentBrain
     * through the shared pointer mechanism.
     * 
     * @warning If brain is nullptr, the agent will not be able to process
     * sensory input or produce motor output. Ensure brain is properly initialized
     * before constructing the AgentBrain.
     * 
     * @see Brain Core neural processing system
     * @see Config AgentBrain-specific configuration
     * @see initialize() Initialize with world environment
     */
    AgentBrain(std::shared_ptr<Brain> brain, std::shared_ptr<const Config> config = nullptr);
    
    /**
     * @brief Gets the size of sensory input expected by the brain
     * 
     * Returns the number of sensory inputs the brain expects, allowing the agent
     * to prepare appropriate sensory percepts of the correct size.
     * 
     * @return Number of sensory inputs expected by the brain
     * @note This value is determined by the brain's configuration during initialization.
     * Use this to allocate memory for sensory percepts before processing.
     * 
     * @see processSensoryInput() Process actual sensory input
     * @see Brain.getSensoryInputSize() Brain-side documentation
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Gets the size of motor output expected by the brain
     * 
     * Returns the number of motor outputs the brain produces, allowing the agent
     * to prepare appropriate motor command vectors of the correct size.
     * 
     * @return Number of motor outputs expected by the brain
     * @note This value is determined by the brain's motor neuron configuration.
     * Use this to ensure motor commands match what the brain expects to decode.
     * 
     * @see decodeMotorCommand() Decode brain motor activity into motor command
     * @see MotorCommand Motor command type definition
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Processes sensory percept and injects it into the brain
     * 
     * Converts environmental percepts (vision, touch, proprioception, internal signals)
     * into neural current that is injected into the appropriate sensory neurons. This
     * is the primary transduction step that allows the brain to perceive the environment.
     * 
     * @param percept Environmental sensory percept containing processed sensor data
     * @note The percept contains pre-processed sensory information that has been
     * extracted and normalized by the agent's sensory systems. The AgentBrain handles
     * the final step of converting this into neural current.
     * 
     * @warning The percept should match the brain's expected sensory input size.
     * Mismatched sizes will cause undefined behavior or processing errors.
     * 
     * @see getSensoryInputSize() Get expected input size
     * @see Brain.receiveSensoryInput() Brain-side processing
     * @see MotorCommand Decoding process that follows this step
     * 
     * @par Implementation Notes:
     * @code
     * This method typically distributes percept values across multiple sensory
     * neuron populations based on their receptive fields and modality.
     * Each sensory neuron's membrane potential is increased proportionally
     * to the corresponding sensor reading.
     * @endcode
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decodes brain motor activity into a motor command
     * 
     * Interprets neural activity from motor neuron populations and converts it
     * into a concrete motor command that the agent can execute. This is the
     * output phase that translates neural processing into behavioral action.
     * 
     * @return MotorCommand The motor command to execute (e.g., MoveForward, TurnLeft)
     * @note This method analyzes the firing rates and activity patterns of motor
     * neuron populations to determine the most appropriate action. It may implement
     * competitive selection between different motor options.
     * 
     * @warning The decoded motor command may be invalid (e.g., no clear winner)
     * in which case the agent should select a default action (typically Wait).
     * 
     * @see getMotorOutputSize() Get expected output size for validation
     * @see MotorCommand Enum of possible motor commands
     * @see applyRewardModulation() Neuromodulation may influence decoding
     * 
     * @par Implementation Notes:
     * @code
     * This method typically computes the weighted sum of motor neuron activities
     * for each action type and selects the action with the highest total activity.
     * It may include thresholding to prevent indecisive actions.
     * @endcode
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Applies neuromodulatory signals to influence learning and plasticity
     * 
     * Applies reward prediction error and other neuromodulatory signals to the brain,
     * influencing synaptic plasticity and neuromodulatory system activity. This is
     * the core reinforcement learning mechanism in the NLM framework.
     * 
     * @param reward Actual reward received from the environment
     * @param predictedReward Expected reward predicted by the brain's prediction system
     * @note The difference between actual and predicted reward (prediction error)
     * is used to modulate plasticity and attention. Positive prediction error
     * indicates better-than-expected outcomes, while negative error indicates
     * worse-than-expected outcomes.
     * 
     * @warning Both reward and predictedReward should be valid floating-point values.
     * Negative predicted rewards are allowed and represent expected costs.
     * 
     * @see getPredictionError() Access current prediction error value
     * @see getNeuromodulationLevel() Check overall neuromodulation level
     * @see Brain.applyNeuromodulation() Brain-side neuromodulation application
     * 
     * @par Implementation Notes:
     * @code
     * This method typically computes the prediction error as (reward - predictedReward),
     * applies it to the eligibility trace system, and uses it to modulate plasticity
     * rates and neuromodulatory system activity.
     * @endcode
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Updates developmental system and plasticity modulation
     * 
     * Advances the agent's developmental stage and updates plasticity rates
     * based on developmental age. This controls how plastic the brain remains
     * as the agent matures, transitioning from high plasticity (learning) to
     * lower plasticity (stabilization).
     * 
     * @param timestep Time elapsed since last development update
     * @note Development affects critical periods, plasticity rates, and learning
     * capabilities. Different developmental stages have different characteristics
     * and constraints on learning and behavior.
     * 
     * @warning The timestep should be positive and consistent with simulation timing.
     * Inconsistent timesteps may lead to incorrect developmental progression.
     * 
     * @see getDevelopmentalStage() Get current developmental stage
     * @see setDevelopmentalStage() Manually set developmental stage
     * @see DevelopmentSystem Developmental system implementation
     * 
     * @par Implementation Notes:
     * @code
     * This method typically checks developmental age against stage durations,
     * advances stages when thresholds are reached, and updates plasticity modifiers
     * based on the current stage.
     * @endcode
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Gets current developmental stage of the agent
     * 
     * Returns the agent's current developmental stage, which affects learning
     * capabilities, plasticity rates, and behavioral characteristics.
     * 
     * @return Current developmental stage
     * @note Developmental stages typically include: Initial (high plasticity),
     * Critical (rapid learning), Maturation (fine-tuning), and Adult (stable).
     * Each stage has different constraints and optimization goals.
     * 
     * @see setDevelopmentalStage() Set developmental stage
     * @see updateDevelopment() Update based on elapsed time
     * @see DevelopmentalStage Enum of developmental stages
     * 
     * @warning The stage affects plasticity and learning behavior. Stage
     * transitions are typically irreversible and represent significant changes
     * in the agent's capabilities and constraints.
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Gets current neuromodulation level affecting learning
     * 
     * Returns the overall neuromodulation level that modulates learning and attention
     * based on factors like reward prediction error, novelty, and curiosity.
     * 
     * @return Neuromodulation level (0.0 to 1.0, higher means more modulation)
     * @note This level influences how responsive the brain is to learning signals
     * and how selective attention is allocated to stimuli.
     * 
     * @see applyRewardModulation() Apply neuromodulatory signals
     * @see getCuriosityLevel() Access curiosity component
     * @see getNoveltyLevel() Access novelty component
     * @see getPredictionError() Access prediction error component
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Gets current curiosity level influencing exploration
     * 
     * Returns the curiosity level that drives exploration and discovery of novel
     * stimuli and behaviors. Curiosity promotes exploration when prediction errors
     * are high or when the environment provides opportunities for learning.
     * 
     * @return Curiosity level (0.0 to 1.0, higher means more exploration)
     * @note Curiosity is a neuromodulatory signal that encourages the agent to
     * explore novel stimuli and behaviors, facilitating discovery and learning.
     * 
     * @see applyRewardModulation() Neuromodulation application affects curiosity
     * @see getCuriosityEnabled() Check if curiosity is enabled
     * @see getNoveltyLevel() Access novelty component
     * 
     * @warning High curiosity levels may lead to excessive exploration and
     * inefficient behavior. Balance with other neuromodulatory signals is important.
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Gets current novelty detection level
     * 
     * Returns the novelty level that detects unexpected or unusual sensory input,
     * driving attention toward novel stimuli that may be important for learning
     * or survival.
     * 
     * @return Novelty level (0.0 to 1.0, higher means more novelty detection)
     * @note Novelty is triggered by sensory input that differs significantly from
     * past experience, directing attention to potentially important new information.
     * 
     * @see processVisionInput() Vision processing includes novelty computation
     * @see getNoveltyEnabled() Check if novelty detection is enabled
     * @see computeAndUpdateNovelty() Novelty computation method
     * 
     * @warning Novelty can be misleading if the environment contains noise or
     * if the agent's internal model is inaccurate. Should be balanced with other
     * attentional signals.
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Gets current prediction error signal for learning
     * 
     * Returns the prediction error that measures the difference between expected
     * and actual rewards, driving learning and plasticity in the brain.
     * 
     * @return Prediction error (positive for better-than-expected, negative for worse)
     * @note Prediction error is a fundamental learning signal in reinforcement
     * learning. It indicates when the agent's predictions are inaccurate and
     * learning is needed.
     * 
     * @see applyRewardModulation() Reward modulation uses prediction error
     * @see getNeuromodulationLevel() Overall neuromodulation level
     * @see getReward() Get actual reward (for comparison)
     * 
     * @warning Large prediction errors may indicate significant model mismatch
     * or unexpected environmental changes. The agent should investigate and
     * potentially adjust its internal model.
     */
    float getPredictionError() const;
    
    /**
     * @brief Resets agent for a new episode or learning session
     * 
     * Performs a complete reset of the agent's state, preparing it for a new
     * episode. This includes resetting developmental state, neuromodulation,
     * memory traces, and any accumulated learning effects.
     * 
     * @note This method should be called when starting a new episode or when
     * the agent needs to forget previous experience and begin fresh learning.
     * It preserves the agent's core brain structure while clearing learned effects.
     * 
     * @see reset() Alternative reset method
     * @see getDevelopmentalStage() Check if development was reset
     * @see getNeuromodulationLevel() Check if neuromodulation was reset
     * 
     * @warning Resetting an agent may interrupt ongoing learning and cause
     * temporary performance degradation. Use judiciously in learning scenarios.
     */
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { neuromodulationState_.setRewardModulationEnabled(enable); }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { neuromodulationState_.setCuriosityEnabled(enable); }
    
    bool isRewardModulationEnabled() const { return neuromodulationState_.isRewardModulationEnabled(); }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return neuromodulationState_.isCuriosityEnabled(); }
    
    // Configuration parameter setters
    void setMotorCommandThreshold(float threshold) { motorSelector_.setThreshold(threshold); }
    void setNoveltyThreshold(float threshold) { noveltyDetector_.setThreshold(threshold); }
    void setCuriosityLevel(float level) { neuromodulationState_.setCuriosityLevel(level); }
    
    // Development configuration
    void setDevelopmentalStageDuration(double initial, double critical, double maturation, double adult) {
        developmentManager_.setStageDurations(initial, critical, maturation, adult);
    }
    
    // Validation
    bool validateConfiguration() const { return ConfigValidator::validateAgentBrainConfig(*config_); }
    
private:
    // Helper methods for separated concerns
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    void computeAndUpdateNovelty(const std::vector<float>& vision);
    
    // Motor decoding components
    MotorCommand decodeFromMotorNeuronsOptimized();
    float calculateNeuronActivity(const std::vector<Neuron*>& neurons) const;
    void extractMotorActivities(std::vector<float>& activities) const;
    
    // Curiosity and exploration
    MotorCommand selectWithCuriosityOptimized(MotorCommand defaultCmd);
    MotorCommand getRandomExploratoryAction() const;
    
    // Neuromodulation components
    void applyRewardModulationOptimized(float reward, float predictedReward);
    float calculatePredictionError(float reward, float predicted) const;
    
    // Development components
    void updateDevelopmentOptimized(double timestep);
    void configureStructuralPlasticity(float plasticityModifier);
    
    // State management
    void updatePreviousVision(const std::vector<float>& vision) { previousVision_ = vision; }
    const std::vector<float>& getPreviousVision() const { return previousVision_; }

    // Core components
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<const Config> config_;
    
    // Helper systems
    MotorCommandSelector motorSelector_;
    NeuromodulationState neuromodulationState_;
    NoveltyDetector noveltyDetector_;
    DevelopmentManager developmentManager_;
    
    // Motor neuron groups (organized by action type)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (organized by modality)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Configuration flags
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Development state
    double developmentalAge_;
};

} // namespace nlm
