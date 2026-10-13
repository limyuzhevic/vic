#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Connects NLM brain to the world and handles sensory-motor transformation
 * 
 * The AgentBrain class serves as the interface between the NLM brain and the external world.
 * It handles:
 * - Sensory transduction: Converting world percepts into neural inputs
 * - Motor decoding: Converting neural activity into motor commands
 * - Neuromodulation: Applying reward and prediction error signals
 * - Development: Managing brain development over time
 * 
 * This class implements a closed-loop control system where:
 * 1. World provides sensory percepts
 * 2. AgentBrain processes them through the brain
 * 3. Brain produces motor commands
 * 4. Motor commands affect the world
 * 5. World provides feedback (rewards, novel events)
 * 6. This feedback modulates brain learning and development
 */
class AgentBrain {
public:
    /**
     * @brief Constructor - creates agent interface for a brain
     * 
     * @param brain Shared pointer to the brain to control
     * @throws std::invalid_argument if brain pointer is null
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor - cleans up agent resources
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize agent with world parameters
     * 
     * This method sets up the agent for interaction with the specified world,
     * including allocating sensory buffers and initializing internal state.
     * 
     * @param world Reference to the world the agent will interact with
     * @throws std::runtime_error if initialization fails
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size from the brain
     * 
     * @return Size of sensory input vector expected by the brain
     * @note Typically corresponds to vision + touch + internal + proprioception
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size from the brain
     * 
     * @return Size of motor output vector expected by the brain
     * @note Typically one motor neuron per available action type
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain
     * 
     * This method converts world percepts (vision, touch, internal, proprioception)
     * into neural currents injected into the brain's sensory neurons.
     * 
     * @param percept The sensory percept to process
     * @throws std::runtime_error if percept validation fails
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into motor command
     * 
     * This method analyzes the brain's motor neuron activity and selects
     * the most appropriate motor command based on activity levels and
     * curiosity-driven exploration.
     * 
     * @return Selected motor command for world interaction
     * @throws std::runtime_error if motor decoding fails
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward and prediction error
     * 
     * This method implements reward-modulated learning by applying
     * dopamine-like signals to synapses based on prediction error signals.
     * 
     * @param reward Actual reward received from world
     * @param predictedReward Expected reward based on prediction
     * @throws std::invalid_argument if reward values are invalid
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update development system with new timestep
     * 
     * This method advances brain development, adjusting plasticity rates
     * and setting developmental stage based on elapsed time.
     * 
     * @param timestep Time step since last update (seconds)
     * @throws std::invalid_argument if timestep is negative
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage of the brain
     * @throws std::runtime_error if brain is not initialized
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level (dopamine)
     * 
     * @return Current neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * @return Current curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * @return Current novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get current prediction error
     * 
     * @return Current prediction error (negative = reward larger than expected,
     *                               positive = reward smaller than expected)
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * This method resets all agent state to initial values, clearing
     * neuromodulation signals, development state, and sensory history.
     */
    void reset();
    
    /**
     * @brief Get brain pointer for direct access
     * 
     * @return Pointer to the brain (unsafe - use with caution)
     * @warning Caller must ensure brain_ is not null
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable true to enable, false to disable
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable true to enable, false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * 
     * @param enable true to enable, false to disable
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * 
     * @param enable true to enable, false to disable
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return true if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return true if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return true if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return true if curiosity is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    /**
     * @brief Convert neural activity to motor command (internal method)
     * 
     * @param defaultCmd Default motor command if no strong activity detected
     * @return Selected motor command based on neural activity
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Select motor command with curiosity-driven exploration
     * 
     * @param defaultCmd Default motor command to return if no exploration
     * @return Motor command with potential curiosity-driven random selection
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    /**
     * @brief Shared pointer to the brain being controlled
     * 
     * This uses shared_ptr to manage brain lifetime and ensure
     * proper cleanup when AgentBrain is destroyed.
     */
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups organized by action type
    std::vector<Neuron*> motorForward_;      ///< Neurons controlling forward movement
    std::vector<Neuron*> motorBackward_;     ///< Neurons controlling backward movement
    std::vector<Neuron*> motorTurnLeft_;     ///< Neurons controlling left turns
    std::vector<Neuron*> motorTurnRight_;    ///< Neurons controlling right turns
    std::vector<Neuron*> motorInteract_;     ///< Neurons controlling interaction
    std::vector<Neuron*> motorWait_;         ///< Neurons controlling waiting/idle
    
    // Sensory neuron groups organized by modality
    std::vector<Neuron*> sensoryVision_;         ///< Neurons for vision processing
    std::vector<Neuron*> sensoryTouch_;         ///< Neurons for touch/proximity
    std::vector<Neuron*> sensoryInternal_;      ///< Neurons for internal state
    std::vector<Neuron*> sensoryProprioception_; ///< Neurons for body position
    
    // Neuromodulation state variables
    float dopamineLevel_;     ///< Current dopamine level (-1.0 to 1.0)
    float noveltyLevel_;      ///< Current novelty detection level
    float curiosityLevel_;   ///< Current curiosity level (exploration drive)
    float predictionError_;  ///< Current prediction error (reward - expected)
    float expectedReward_;   ///< Expected reward (exponential moving average)
    
    // Development state
    double developmentalAge_;   ///< Current developmental age in simulation time
    float plasticityModifier_; ///< Current plasticity multiplier (1.0 = normal)
    
    // Configuration flags
    bool rewardModulationEnabled_;     ///< Whether reward modulation is active
    bool structuralPlasticityEnabled_; ///< Whether structural plasticity is active
    bool developmentEnabled_;          ///< Whether development system is active
    bool curiosityEnabled_;           ///< Whether curiosity-driven exploration is active
    
    // Previous sensory state for change detection
    std::vector<float> previousVision_;       ///< Previous vision frame for novelty detection
    float sensoryNoveltyDecay_;              ///< Decay rate for novelty signals

} // namespace nlm
