#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * AgentBrain: Connects NLM brain to the world
 * 
 * Handles sensory transduction and motor decoding with advanced error handling
 * and comprehensive logging for debugging and monitoring.
 * 
 * Features:
 * - Robust error handling with detailed logging
 * - Null pointer checking throughout
 * - Developmental stage tracking
 * - Neuromodulation-based learning
 * - Curiosity-driven exploration
 * - Sensory processing with multiple modalities
 * - Motor command decoding with activity analysis
 * 
 * Usage:
 * @code
 * // Create brain and agent
 * auto brain = pynlm.createBrain(config);
 * brain.initialize();
 * auto agent = pynlm.createAgentBrain(brain);
 * 
 * // Initialize with world
 * agent.initialize(world);
 * 
 * // Enable learning subsystems
 * agent.enableRewardModulation(true);
 * agent.enableCuriosity(true);
 * agent.enableDevelopment(true);
 * 
 * // Main simulation loop
 * for (step in range(num_steps)) {
 *     world.update(0.1);
 *     agent.processSensoryInput(world.getSensoryPercept());
 *     brain.step(step);
 *     world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
 * }
 * @endcode
 */
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    /**
     * Delete copy operations - AgentBrain manages brain ownership
     */
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    /**
     * Move operations allowed - transfer brain ownership
     */
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * Initialize with world interface
     * 
     * @param world The world instance this agent interacts with
     * @throws std::invalid_argument if world has invalid dimensions
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * Process sensory percept and inject into brain
     * 
     * Handles vision, touch, internal signals, and proprioception with
     * comprehensive error handling and logging.
     * 
     * @param percept Sensory percept containing all input modalities
     * @throws std::runtime_error if processing fails
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * Decode brain motor activity into motor command
     * 
     * Analyzes motor neuron activity across different action groups and
     * returns the most appropriate command, with curiosity-based exploration.
     * 
     * @return Motor command for the agent to execute
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * Apply neuromodulation based on reward prediction error
     * 
     * Implements dopamine-based learning where prediction errors modify
     * synaptic weights and plasticity parameters.
     * 
     * @param reward Actual reward received
     * @param predictedReward Expected reward
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * Update developmental system based on simulation time
     * 
     * Tracks developmental age and adjusts plasticity parameters according
     * to biologically inspired developmental stages.
     * 
     * @param timestep Time since last development update
     */
    void updateDevelopment(double timestep);
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Get current neuromodulation level (dopamine)
     * 
     * @return Dopamine level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Get current curiosity level
     * 
     * @return Curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * Get current novelty level
     * 
     * @return Novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * Get prediction error
     * 
     * @return Prediction error (positive = better than expected, negative = worse)
     */
    float getPredictionError() const;
    
    /**
     * Reset agent state for new episode
     * 
     * Clears all neuromodulation state, resets developmental age,
     * and prepares for a new learning episode.
     */
    void reset();
    
    /**
     * Get brain pointer
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * Get brain pointer (const version)
     */
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration methods
    
    /**
     * Enable or disable reward modulation
     * 
     * When enabled, prediction errors modify synaptic weights (learning).
     * 
     * @param enable true to enable reward modulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * Enable or disable structural plasticity
     * 
     * When enabled, new synapses can be created and old ones pruned.
     * 
     * @param enable true to enable structural plasticity
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * Enable or disable development
     * 
     * When enabled, brain undergoes developmental stages with changing plasticity.
     * 
     * @param enable true to enable development
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * Enable or disable curiosity-driven exploration
     * 
     * When enabled, high curiosity leads to random actions for exploration.
     * 
     * @param enable true to enable curiosity
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * Check if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * Check if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * Check if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * Check if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Brain interface
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
    
    // Private helper methods
    void initializeNeuronGroups();
    void distributeNeurons(NeuronType type, const std::vector<Neuron*>& neurons);
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons);
    
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprioception);
    
    void computeNovelty(const std::vector<float>& vision);
    void updateCuriosity();
    void updateDevelopmentalStage();
    void updateStructuralPlasticity();
    void applyRewardToSynapses();
    void modulatePlasticity();
    
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
};

} // namespace nlm
