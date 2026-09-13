#pragma once

/**
 * AgentBrain: Connects NLM brain to the world
 * Handles sensory transduction and motor decoding
 * 
 * This class serves as the interface between the internal neural simulation
 * (brain) and the external environment (world). It processes sensory input
 * from the world and decodes neural activity into motor commands.
 * 
 * Key functions:
 * - Sensory processing: Converts percepts into neural activation
 * - Motor decoding: Translates neural activity into actions
 * - Neuromodulation: Applies reward-based learning signals
 * - Development: Manages developmental stages and plasticity
 * 
 * The agent brain maintains neuron populations for different types:
 * - Motor neurons: Control movement and interaction
 * - Sensory neurons: Process environmental input
 * - Neuromodulatory neurons: Regulate learning and attention
 */

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * AgentBrain class connects NLM brain to the world environment
 * 
 * This class acts as the bridge between internal neural computation and
 * external world interaction. It processes sensory information from the
 * world and converts brain activity into motor commands to control the agent.
 * 
 * The agent brain implements several key subsystems:
 * 1. Sensory transduction - converts world percepts into neural signals
 * 2. Motor decoding - translates neural activity into actions
 * 3. Neuromodulation - applies learning signals (dopamine, curiosity)
 * 4. Development - manages maturation and plasticity changes
 * 5. Memory integration - connects brain memory systems to behavior
 * 
 * @note This class is essential for closed-loop interaction between
 *       artificial neural systems and their environment.
 */
class AgentBrain {
public:
    /**
     * Constructor - creates an agent brain interface
     * 
     * @param brain Shared pointer to the neural brain this agent will interface with
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * Destructor
     */
    ~AgentBrain();
    
    /**
     * Disable copying - AgentBrain holds unique resources
     */
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    /**
     * Move constructor - transfers ownership of brain pointer
     */
    AgentBrain(AgentBrain&&) noexcept;
    
    /**
     * Move assignment - transfers ownership of resources
     */
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * Initialize agent brain with world environment
     * 
     * This method must be called before using the agent brain. It sets up
     * the connection between the brain and world, preparing for sensory
     * processing and motor control.
     * 
     * @param world Reference to the world environment for interaction
     * 
     * @note Always call initialize() before using the agent brain
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * Get the expected size of sensory input
     * 
     * Returns the total number of sensory values the brain expects.
     * This is calculated from all sensory modalities:
     * - Vision (16x16 grid = 256 values)
     * - Touch (8 proximity sensors)
     * - Internal (4 homeostatic signals)
     * - Proprioception (6 body position signals)
     * 
     * @return Expected sensory input size (300 values)
     */
    size_t getSensoryInputSize() const;
    
    /**
     * Get the expected size of motor output
     * 
     * Returns the number of motor commands the brain can produce.
     * These correspond to different action types:
     * - Move forward, backward, turn left/right
     * - Look in different directions
     * - Interact with objects
     * - Eat, drink, rest
     * - Custom actions
     * 
     * @return Expected motor output size (7 values)
     */
    size_t getMotorOutputSize() const;
    
    /**
     * Process sensory input from the environment
     * 
     * This is the main sensory processing method. It takes a percept
     * (what the agent perceives about the world) and injects the
     * information into appropriate neurons in the brain.
     * 
     * The method handles each sensory modality separately:
     * 1. Vision - injects visual intensity into visual neurons
     * 2. Touch - injects collision signals into touch neurons
     * 3. Internal - injects homeostatic signals
     * 4. Proprioception - injects body state signals
     * 
     * @param percept The sensory percept from the world containing
     *                visual, touch, internal, and proprioceptive data
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * Decode motor command from brain activity
     * 
     * This method reads the current activity of motor neurons and
     * converts it into a specific action command for the world.
     * 
     * The decoding process:
     * 1. Calculates average activity in each motor neuron group
     * 2. Selects the action with highest activity
     * 3. Applies curiosity-based exploration when enabled
     * 4. Returns the chosen action
     * 
     * @return MotorCommand for the chosen action (MoveForward, TurnLeft, etc.)
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * Apply reward-based neuromodulation
     * 
     * This method implements reward-modulated learning, a key mechanism
     * for adaptive behavior. It updates synaptic weights based on
     * prediction errors between expected and actual rewards.
     * 
     * The neuromodulation process:
     * 1. Computes prediction error (reward - expected)
     * 2. Updates dopamine level based on error
     * 3. Applies weight changes with eligibility traces
     * 4. Modulates overall plasticity based on neuromodulation
     * 
     * @param reward The actual reward received
     * @param predictedReward The expected reward
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * Update development system based on simulation time
     * 
     * This method manages the developmental stages of the agent.
     * As time progresses, the agent goes through different developmental
     * phases: Initial → CriticalPeriod → Maturation → Adult → Aging.
     * 
     * Each stage has different characteristics:
     * - Initial: High plasticity, rapid learning
     * - CriticalPeriod: Sensitive to specific experiences
     * - Maturation: Fine-tuning of abilities
     * - Adult: Stable, efficient performance
     * - Aging: Reduced plasticity, maintenance mode
     * 
     * @param timestep Simulation time step for development update
     */
    void updateDevelopment(double timestep);
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage of the agent
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Get current neuromodulation level
     * 
     * Neuromodulation level represents the overall influence of
     * neuromodulators (dopamine, curiosity, etc.) on learning and
     * behavior. Higher levels indicate more exploratory or motivated
     * behavior.
     * 
     * @return Current neuromodulation level (0.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Get current curiosity level
     * 
     * Curiosity drives exploration and learning of novel stimuli.
     * Higher curiosity levels increase the likelihood of exploring
     * unfamiliar situations and trying new actions.
     * 
     * @return Current curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * Get current novelty level
     * 
     * Novelty indicates how new or different the current sensory input
     * is compared to previous experiences. High novelty suggests the
     * agent is encountering something new.
     * 
     * @return Current novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * Get prediction error
     * 
     * Prediction error is the difference between expected and actual
     * outcomes. It's a key signal for learning and adapting behavior.
     * Positive errors indicate better-than-expected outcomes,
     * negative errors indicate worse-than-expected outcomes.
     * 
     * @return Current prediction error
     */
    float getPredictionError() const;
    
    /**
     * Reset agent for new episode
     * 
     * This method resets all state variables to their initial values,
     * preparing the agent for a new simulation episode. It clears
     * neuromodulation state, developmental state, and memory traces.
     */
    void reset();
    
    /**
     * Get pointer to the underlying brain
     * 
     * @return Pointer to the brain this agent interfaces with
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * Enable or disable reward modulation
     * 
     * Reward modulation is the process of using rewards to guide
     * learning and behavior. When enabled, the agent can learn from
     * reward prediction errors.
     * 
     * @param enable True to enable reward modulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * Enable or disable structural plasticity
     * 
     * Structural plasticity allows the agent to create and eliminate
     * synapses, effectively rewiring its neural connections based on
     * experience. This is crucial for long-term adaptation.
     * 
     * @param enable True to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * Enable or disable developmental processes
     * 
     * Development allows the agent to mature and change its behavior
     * over time. When enabled, the agent goes through developmental
     * stages that affect learning and behavior.
     * 
     * @param enable True to enable development
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * Enable or disable curiosity-driven exploration
     * 
     * Curiosity drives the agent to explore novel situations and
     * learn new behaviors. When enabled, the agent is more likely
     * to try new actions and explore unfamiliar environments.
     * 
     * @param enable True to enable curiosity
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * Check if reward modulation is enabled
     * 
     * @return True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * Check if structural plasticity is enabled
     * 
     * @return True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * Check if development is enabled
     * 
     * @return True if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * Check if curiosity is enabled
     * 
     * @return True if curiosity-driven exploration is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    /**
     * Internal implementation details
     */
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    /**
     * Shared pointer to the brain
     */
    std::shared_ptr<Brain> brain_;
    
    /**
     * Motor neuron groups for different action types
     */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    /**
     * Sensory neuron groups for different input types
     */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    /**
     * Neuromodulation state variables
     */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    /**
     * Development state
     */
    double developmentalAge_;
    float plasticityModifier_;
    
    /**
     * Configuration flags
     */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    /**
     * Previous sensory state for novelty detection
     */
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;

    /**
     * Friend declaration for Brain class to access private members
     */
    friend class Brain;
};

} // namespace nlm
