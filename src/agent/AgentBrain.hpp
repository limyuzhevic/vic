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
// This class acts as the interface between the NLM brain and the external
// world simulation. It converts between the brain's internal representation
// of neural activity and the world's motor commands and sensory percepts.
// 
// AgentBrain performs two main functions:
// 1. Sensory processing: Converts world sensory percepts into neural inputs
//    for the brain through current injection into sensory neurons
// 2. Motor output: Converts neural activity from motor neurons into world
//    motor commands for action execution
// 
// It also implements key cognitive and neuromodulation systems:
// - Reward-based neuromodulation (dopamine)
// - Developmental updates (plasticity modulation)
// - Curiosity-driven exploration
// - Prediction error computation
// - Novelty detection
// 
// The AgentBrain is essential for embodied intelligence, allowing the
// neural network to interact with and learn from the environment.
// 
// Typical workflow:
//   auto config = std::make_shared<Config>();
//   auto brain = std::make_shared<Brain>(config);
//   brain->initialize();
//   
//   auto agentBrain = std::make_unique<AgentBrain>(brain);
//   agentBrain->initialize(world);
//   
//   for each timestep:
//       // Process sensory input from world
//       SensoryPercept percept = world->getSensoryPercept();
//       agentBrain->processSensoryInput(percept);
//       brain->step(step);
//       
//       // Get motor output
//       MotorCommand cmd = agentBrain->decodeMotorCommand();
//       world->applyMotorCommand(cmd, step);
//       
//       // Update development
//       agentBrain->updateDevelopment(step);
// 
// Example Python usage:
//   import nlm
//   config = nlm.createDefaultConfig()
//   brain = nlm.createBrain(config)
//   agent_brain = nlm.createAgentBrain(brain)
//   
//   world = nlm.createSimpleWorld()
//   agent_brain.initialize(world)
//   
//   for step in range(100):
//       percept = world.getSensoryPercept()
//       agent_brain.processSensoryInput(percept)
//       brain.step(step)
//       cmd = agent_brain.decodeMotorCommand()
//       world.applyMotorCommand(cmd, step)

class AgentBrain {
public:
    //! @brief Constructor
    //! 
    //! Creates an agent brain interface connected to a neural brain.
    //! The agent brain will use this brain for sensory processing and motor
    //! decoding. A brain must be provided and initialized before use.
    //! 
    //! Parameters:
    //!   brain: Shared pointer to the neural brain to interface with
    //!   
    //! Example:
    //!   auto config = std::make_shared<Config>();
    //!   auto brain = std::make_shared<Brain>(config);
    //!   auto agentBrain = std::make_unique<AgentBrain>(brain);

    AgentBrain(std::shared_ptr<Brain> brain);
    
    //! @brief Destructor
    //! 
    //! Cleans up agent brain resources.

    ~AgentBrain();
    
    //! @brief Initialize with world
    //! 
    //! Prepares the agent brain for operation with a specific world.
    //! This sets up the neural connections and configures the agent for
    //! interaction with the given world simulation.
    //! 
    //! Parameters:
    //!   world: Reference to the world to interface with
    //!   
    //! This method should be called after creating the AgentBrain but before
    //! using it for simulation.
    //! 
    //! Example:
    //!   auto config = std::make_shared<Config>();
    //!   auto brain = std::make_shared<Brain>(config);
    //!   brain->initialize();
    //!   
    //!   auto agentBrain = std::make_unique<AgentBrain>(brain);
    //!   agentBrain->initialize(world);

    void initialize(const SimpleWorld& world);

    //! @brief Get sensory input size expected by brain
    //! 
    //! Returns the expected size of sensory input vectors for the connected brain.
    //! This is used to validate sensory percepts before processing.
    //! 
    //! Returns:
    //!   size_t Expected sensory input size in elements
    //!   
    //! Example:
    //!   size_t expectedSize = agentBrain->getSensoryInputSize();
    //!   std::cout << "Expected " << expectedSize << " sensory elements" << std::endl;

    size_t getSensoryInputSize() const;

    //! @brief Get motor output size expected
    //! 
    //! Returns the expected size of motor output from the brain.
    //! This indicates how many motor neurons or outputs the brain produces.
    //! 
    //! Returns:
    //!   size_t Expected motor output size
    //!   
    //! Example:
    //!   size_t motorSize = agentBrain->getMotorOutputSize();
    //!   std::cout << "Motor output size: " << motorSize << std::endl;

    size_t getMotorOutputSize() const;

    //! @brief Process sensory percept and inject into brain
    //! 
    //! Converts a sensory percept from the world into neural current
    //! injection for the connected brain. This implements the sensory
    //! transduction process where external/internal stimuli are converted
    //! into neural activity.
    //! 
    //! Parameters:
    //!   percept: Sensory percept from the world containing all sensory data
    //!   
    //! The percept contains vision, touch, internal, proprioception, and
    //! audio signals (if enabled). These are injected into corresponding
    //! sensory neurons in the brain based on their type and intensity.
    //! 
    //! Example:
    //!   SensoryPercept percept = world->getSensoryPercept();
    //!   agentBrain->processSensoryInput(percept);
    //!   // Brain now has sensory input for this timestep

    void processSensoryInput(const SensoryPercept& percept);

    //! @brief Decode brain motor activity into motor command
    //! 
    //! Analyzes neural activity from motor neurons and produces a low-level
    //! motor command for world interaction. This implements the motor output
    //! process where neural decisions become physical actions.
    //! 
    //! Returns:
    //!   MotorCommand The decoded motor command (MoveForward, TurnLeft, etc.)
    //!   
    //! The motor command is selected based on which motor neuron groups
    //! have the highest activity, with additional modulation from curiosity
    //! and neuromodulation systems.
    //! 
    //! Example:
    //!   MotorCommand cmd = agentBrain->decodeMotorCommand();
    //!   if (cmd == MotorCommand::MoveForward) {
    //!       // Move forward in world
    //!   }

    MotorCommand decodeMotorCommand();

    //! @brief Apply reward-based neuromodulation
    //! 
    //! Applies reward prediction error signals to neuromodulation systems,
    //! particularly the dopamine system. This implements reinforcement
    //! learning where unexpected rewards strengthen or weaken neural pathways.
    //! 
    //! Parameters:
    //!   reward: Actual reward received from world
    //!   predictedReward: Expected reward predicted by brain
    //!   
    //! The difference (prediction error) drives learning through dopamine
    //! modulation of synaptic plasticity.
    //! 
    //! Example:
    //!   agentBrain->applyRewardModulation(actualReward, predictedReward);
    //!   // Dopamine system updates based on prediction error

    void applyRewardModulation(float reward, float predictedReward);

    //! @brief Update development system
    //! 
    //! Advances the developmental stage of the brain, affecting neural
    //! properties, plasticity rates, and cognitive capabilities. Development
    //! occurs at specific stages (Initial, CriticalPeriod, Maturation, etc.).
    //! 
    //! Parameters:
    //!   timestep: Current simulation timestep
    //!   
    //! Development affects multiple systems:
    //! - Reduces plasticity over time (stabilization)
    //! - Enhances cognitive capabilities
    //! - Modifies behavioral patterns
    //! 
    //! Example:
    //!   agentBrain->updateDevelopment(currentStep);
    //!   // Brain develops (becomes less plastic, more capable)

    void updateDevelopment(double timestep);

    //! @brief Get current developmental stage
    //! 
    //! Returns the current developmental stage of the brain.
    //! Developmental stage affects neural properties, learning rates, and behavior.
    //! 
    //! Returns:
    //!   DevelopmentalStage Current developmental stage
    //!   
    //! Stages:
    //! - Initial: High plasticity, basic capabilities
    //! - CriticalPeriod: Rapid learning and development
    //! - Maturation: Refining connections
    //! - Adult: Stable, optimized performance
    //! - Aging: Decline in plasticity
    //! 
    //! Example:
    //!   DevelopmentalStage stage = agentBrain->getDevelopmentalStage();
    //!   if (stage == DevelopmentalStage::CriticalPeriod) {
    //!       // Optimize for rapid learning
    //!   }

    DevelopmentalStage getDevelopmentalStage() const;

    //! @brief Get neuromodulation level
    //! 
    //! Returns the current level of neuromodulation affecting neural excitability.
    //! Neuromodulation combines effects from dopamine, curiosity, and other systems.
    //! 
    //! Returns:
    //!   float Neuromodulation level (typically 0.0-1.0)
    //!   
    //! Higher levels increase neural excitability and plasticity.
    //! 
    //! Example:
    //!   float neuromodLevel = agentBrain->getNeuromodulationLevel();
    //!   std::cout << "Neuromodulation: " << neuromodLevel << std::endl;

    float getNeuromodulationLevel() const;

    //! @brief Get curiosity level
    //! 
    //! Returns the current curiosity level driving exploration behavior.
    //! Curiosity motivates the agent to seek novel and interesting experiences.
    //! 
    //! Returns:
    //!   float Curiosity level (typically 0.0-1.0)
    //!   
    //! Higher curiosity leads to more exploration and less predictable behavior.
    //! 
    //! Example:
    //!   float curiosity = agentBrain->getCuriosityLevel();
    //!   if (curiosity > 0.5) {
    //!       // Encourage exploration
    //!   }

    float getCuriosityLevel() const;

    //! @brief Get novelty level
    //! 
    //! Returns the current novelty level detecting unexpected sensory input.
    //! Novelty signals drive learning and attention to new stimuli.
    //! 
    //! Returns:
    //!   float Novelty level (typically 0.0-1.0)
    //!   
    //! Example:
    //!   float novelty = agentBrain->getNoveltyLevel();
    //!   if (novelty > 0.7) {
    //!       // Highly novel experience
    //!   }

    float getNoveltyLevel() const;

    //! @brief Get prediction error
    //! 
    //! Returns the current prediction error signal from the prediction system.
    //! Prediction error indicates how well the agent predicted sensory input.
    //! 
    //! Returns:
    //!   float Prediction error (positive = over-predicted, negative = under-predicted)
    //!   
    //! Example:
    //!   float error = agentBrain->getPredictionError();
    //!   std::cout << "Prediction error: " << error << std::endl;

    float getPredictionError() const;

    //! @brief Reset agent for new episode
    //! 
    //! Resets the agent brain to its initial state, preparing for a new
    //! simulation episode. This resets:
    //! - Neural state to initial conditions
    //! - Memory systems
    //! - Developmental age
    //! - Neuromodulation state
    //! - Sensory history
    //! 
    //! After reset, the agent is ready for a fresh start with the same
    //! world or a new one.
    //! 
    //! Example:
    //!   agentBrain->reset();
    //!   // Agent brain is now at initial state for new episode

    void reset();

    //! @brief Get brain pointer
    //! 
    //! Returns a pointer to the connected neural brain.
    //! 
    //! Returns:
    //!   Brain* Pointer to the neural brain, or nullptr if not connected
    //!   
    //! Example:
    //!   Brain* brain = agentBrain->getBrain();
    //!   if (brain) {
    //!       brain->step(step);
    //!   }

    Brain* getBrain() { return brain_.get(); }
    
    // Configuration

    //! @brief Enable reward modulation
    //! 
    //! Enables or disables reward-based neuromodulation (dopamine system).
    //! When enabled, reward prediction errors affect neural plasticity.
    //! 
    //! Parameters:
    //!   enable: true to enable, false to disable
    //!   
    //! Example:
    //!   agentBrain->enableRewardModulation(true);

    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }

    //! @brief Enable structural plasticity
    //! 
    //! Enables or disables structural plasticity (synaptogenesis, pruning).
    //! When enabled, the brain can create or eliminate synapses over time.
    //! 
    //! Parameters:
    //!   enable: true to enable, false to disable
    //!   
    //! Example:
    //!   agentBrain->enableStructuralPlasticity(true);

    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }

    //! @brief Enable development
    //! 
    //! Enables or disables developmental processes (age-dependent changes).
    //! When enabled, the brain ages and changes over time.
    //! 
    //! Parameters:
    //!   enable: true to enable, false to disable
    //!   
    //! Example:
    //!   agentBrain->enableDevelopment(true);

    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }

    //! @brief Enable curiosity
    //! 
    //! Enables or disables curiosity-driven exploration.
    //! When enabled, the agent seeks novel and interesting experiences.
    //! 
    //! Parameters:
    //!   enable: true to enable, false to disable
    //!   
    //! Example:
    //!   agentBrain->enableCuriosity(true);

    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }

    //! @brief Check if reward modulation is enabled
    //! 
    //! Returns whether reward modulation is currently enabled.
    //! 
    //! Returns:
    //!   bool true if reward modulation enabled
    //!   
    //! Example:
    //!   if (agentBrain->isRewardModulationEnabled()) {
    //!       // Reward modulation active
    //!   }

    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }

    //! @brief Check if structural plasticity is enabled
    //! 
    //! Returns whether structural plasticity is currently enabled.
    //! 
    //! Returns:
    //!   bool true if structural plasticity enabled
    //!   
    //! Example:
    //!   if (agentBrain->isStructuralPlasticityEnabled()) {
    //!       // Structural plasticity active
    //!   }

    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }

    //! @brief Check if development is enabled
    //! 
    //! Returns whether development processes are currently enabled.
    //! 
    //! Returns:
    //!   bool true if development enabled
    //!   
    //! Example:
    //!   if (agentBrain->isDevelopmentEnabled()) {
    //!       // Development active
    //!   }

    bool isDevelopmentEnabled() const { return developmentEnabled_; }

    //! @brief Check if curiosity is enabled
    //! 
    //! Returns whether curiosity-driven exploration is currently enabled.
    //! 
    //! Returns:
    //!   bool true if curiosity enabled
    //!   
    //! Example:
    //!   if (agentBrain->isCuriosityEnabled()) {
    //!       // Curiosity active - agent will explore
    //!   }

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
