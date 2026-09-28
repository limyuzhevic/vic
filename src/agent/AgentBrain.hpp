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
 * Handles sensory transduction and motor decoding between the neural brain
 * and the external environment. The AgentBrain implements the interface
 * between the brain's internal neural representation and the external world,
 * processing sensory inputs and converting brain activity into motor actions.
 * 
 * Key mechanisms:
 * - Sensory neuron distribution across vision, touch, internal, and proprioception channels
 * - Motor neuron group allocation for forward, backward, turn left/right, interact, and wait actions
 * - Novelty detection and curiosity-driven exploration
 * - Reward modulation and prediction error processing
 * - Developmental system integration and maturation tracking
 * 
 * The AgentBrain acts as the bridge between the internal neural world and the external
 * environment, enabling adaptive behavior through sensory input processing and
 * motor output generation.
 */
class AgentBrain {
public:
    /**
     * @brief Constructor
     * 
     * @param brain Shared pointer to the brain that this agent will interface with
     */
    AgentBrain(std::shared_ptr<Brain> brain);

    /**
     * @brief Destructor
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with a world reference
     * 
     * Sets up the sensory neuron distribution and initializes the agent with
     * the specified world. Must be called before any sensory processing or
     * motor decoding can occur.
     * 
     * @param world Reference to the simple world this agent interacts with
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size for the brain
     * 
     * Returns the total number of sensory values the brain expects based on
     * the agent's neural configuration. This includes vision, touch, internal,
     * and proprioception inputs.
     * 
     * @return size_t Number of sensory input values expected
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size
     * 
     * Returns the number of motor neurons/groups that the brain can activate
     * to produce different actions. This determines the granularity of motor
     * control available to the agent.
     * 
     * @return size_t Number of motor output neurons/groups
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * Converts sensory data from the world into neural activation patterns
     * that are injected into the appropriate sensory neuron populations in the brain.
     * This process includes:
     * - Vision input processing (256 values -> vision sensory neurons)
     * - Touch input processing (8 values -> touch sensory neurons)
     * - Internal signal processing (4 values -> internal sensory neurons)
     * - Proprioception processing (6 values -> proprioception neurons)
     * 
     * @param percept Sensory percept containing all sensory inputs from the world
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * Reads the current activity levels in motor neuron groups and converts
     * them into specific motor commands (MoveForward, MoveBackward, etc.)
     * based on the highest activity levels. This process includes:
     * - Calculating average activity in each motor group
     * - Selecting the action with maximum activity
     * - Applying curiosity-based exploration when curiosity is high
     * 
     * @return MotorCommand Selected motor command based on neural activity
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward prediction
     * 
     * Implements dopamine-based reward modulation using prediction error signals.
     * When a reward is received, this method:
     * - Computes prediction error (reward - expected reward)
     * - Updates expected reward (exponential moving average)
     * - Applies dopamine signal proportional to prediction error
     * - Modulates synaptic plasticity based on dopamine level
     * - Updates eligibility traces for synaptic modification
     * 
     * @param reward Actual reward received from the environment
     * @param predictedReward Expected reward predicted by the system
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system
     * 
     * Advances the agent's developmental stage based on accumulated time,
     * modulating plasticity rates and neural properties according to developmental
     * stage. This enables age-dependent learning capabilities.
     * 
     * @param timestep Time elapsed since last update (in simulation time units)
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the agent, which influences
     * learning rates, neural plasticity, and behavioral capabilities. The
     * developmental stage progresses through phases: Initial -> CriticalPeriod ->
     * Maturation -> Adult.
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level
     * 
     * Returns the current level of neuromodulation (primarily dopamine) which
     * influences neural excitability, plasticity, and learning rate. Higher
     * neuromodulation levels increase plasticity and exploration.
     * 
     * @return float Current neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * Returns the agent's curiosity level, which drives exploration behavior
     * and motivated learning. Curiosity is computed based on novelty detection
     * and prediction errors.
     * 
     * @return float Current curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * Returns the current level of novelty detected in the environment,
     * which is a key driver of curiosity and exploratory behavior. Novelty
     * represents the difference between current sensory input and previous
     * input patterns.
     * 
     * @return float Current novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get current prediction error
     * 
     * Returns the prediction error signal, which represents the difference
     * between received rewards and expected rewards. This is a key component
     * of dopamine-based learning systems.
     * 
     * @return float Current prediction error (-1.0 to 1.0)
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * Resets all neuromodulation state variables to their initial values,
     * clearing novelty, curiosity, prediction error, and developmental age.
     * This should be called between episodes to prepare the agent for new learning.
     */
    void reset();
    
    /**
     * @brief Get brain pointer
     * 
     * @return Brain* Pointer to the underlying brain object
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Configuration methods for agent behavior
     * 
     * These methods allow enabling or disabling specific agent subsystems
     * and functions. They provide fine-grained control over agent behavior.
     */
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable true to enable reward modulation, false to disable
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable true to enable structural plasticity, false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development system
     * 
     * @param enable true to enable development, false to disable
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable true to enable curiosity, false to disable
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
     * @return bool true if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Motor decoding: convert neural activity to motor command
     * 
     * This is a private method that implements the core motor decoding logic,
     * calculating average activity in each motor group and selecting the
     * action with maximum activity.
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Motor command selection with curiosity/exploration
     * 
     * This private method implements curiosity-driven exploration, occasionally
     * selecting random actions when curiosity levels are high to promote exploration
     * of novel behaviors.
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    /**
     * @brief Motor neuron groups for different action types
     * 
     * These vectors contain pointers to neurons that are responsible for
     * different motor functions:
     * - motorForward_: neurons that drive forward movement
     * - motorBackward_: neurons that drive backward movement
     * - motorTurnLeft_: neurons that drive left turns
     * - motorTurnRight_: neurons that drive right turns
     * - motorInteract_: neurons that drive interaction with objects
     * - motorWait_: neurons that maintain current state (do nothing)
     */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    /**
     * @brief Sensory neuron groups for different input modalities
     * 
     * These vectors contain pointers to neurons that process different types
     * of sensory input:
     * - sensoryVision_: neurons that process visual input
     * - sensoryTouch_: neurons that process tactile input
     * - sensoryInternal_: neurons that process internal state information
     * - sensoryProprioception_: neurons that process body position/orientation
     */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    /**
     * @brief Neuromodulation state variables
     * 
     * These variables track the current levels of different neuromodulators:
     * - dopamineLevel_: dopamine signal based on reward prediction error
     * - noveltyLevel_: novelty signal based on environmental changes
     * - curiosityLevel_: curiosity signal based on novelty and prediction error
     * - predictionError_: prediction error signal from reward mismatch
     * - expectedReward_: expected reward (exponential moving average)
     */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    /**
     * @brief Development state variables
     * 
     * These variables track the agent's developmental state:
     * - developmentalAge_: accumulated age of the agent (in simulation time)
     * - plasticityModifier_: multiplier for plasticity rates based on developmental stage
     */
    double developmentalAge_;
    float plasticityModifier_;
    
    /**
     * @brief Configuration flags for agent subsystems
     * 
     * These flags enable or disable specific agent subsystems:
     * - rewardModulationEnabled_: whether reward modulation is active
     * - structuralPlasticityEnabled_: whether structural plasticity is active
     * - developmentEnabled_: whether development system is active
     * - curiosityEnabled_: whether curiosity-driven exploration is active
     */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    /**
     * @brief Previous sensory state for novelty detection
     * 
     * Stores the previous vision input pattern to compute novelty as the
     * difference between current and previous sensory input. This enables
     * detection of environmental changes over time.
     */
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
