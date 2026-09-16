#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief Connects NLM brain to the world
 * 
 * Handles sensory transduction and motor decoding, bridging the neural processing
 * of the Brain class with the external world. The AgentBrain manages conversion
 * between low-level neural activity and high-level motor commands.
 * 
 * This class serves as the interface layer, allowing a spiking neural network to
 * interact with an environment through sensory input and motor output.
 */
class AgentBrain {
public:
    /**
     * @brief Constructs an AgentBrain with a reference to the neural brain
     * 
     * @param brain Shared pointer to the Brain instance to control
     */
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destructor
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with a world reference
     * 
     * @param world Constant reference to the SimpleWorld instance
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the size of sensory input expected by the brain
     * 
     * @return Number of sensory input values required by the brain
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the size of motor output produced by the brain
     * 
     * @return Number of motor output values expected
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject it into the brain
     * 
     * Converts world sensor readings (vision, touch, proprioception) into
     * neural current injections in the brain's sensory neurons.
     * 
     * @param percept The sensory data packet to process
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into a motor command
     * 
     * Reads activity from motor neuron groups in the brain and converts it
     * into a concrete MotorCommand that can be executed in the world.
     * 
     * @return The selected motor command for the current brain state
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward prediction error
     * 
     * Implements dopamine-based reward modulation, allowing the brain to learn
     * from the difference between expected and actual rewards.
     * 
     * @param reward The actual reward received
     * @param predictedReward The expected reward predicted by the brain
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update the developmental system
     * 
     * Advances the agent's developmental stage, adjusting plasticity and
     * learning rates based on age/development stage.
     * 
     * @param timestep Time step for the development update
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return The current developmental stage of the agent
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get the current neuromodulation level (dopamine)
     * 
     * @return The current dopamine level, which modulates plasticity
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get the current curiosity level
     * 
     * @return The current curiosity level driving exploration behavior
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get the current novelty level
     * 
     * @return The current novelty level, driving stimulus seeking
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get the current prediction error
     * 
     * @return The difference between predicted and actual rewards
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset the agent for a new episode
     * 
     * Clears all neuromodulation state, novelty detection, and developmental
     * counters to prepare for a fresh simulation.
     */
    void reset();
    
    /**
     * @brief Get a pointer to the underlying brain
     * 
     * @return Raw pointer to the Brain instance (caller responsible for lifetime)
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @name Configuration Methods
     * Control which neural subsystems are enabled
     */
    ///@{ 
    /**
     * @brief Enable or disable reward modulation
     * @param enable True to enable, false to disable
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * @param enable True to enable, false to disable
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development
     * @param enable True to enable, false to disable
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven exploration
     * @param enable True to enable, false to disable
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    ///@}
    
    /**
     * @name Configuration Status Methods
     */
    ///@{ 
    /**
     * @brief Check if reward modulation is enabled
     * @return True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * @return True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * @return True if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * @return True if curiosity-driven exploration is enabled
     */
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    ///@}
    
private:
    /**
     * @brief Convert motor neuron activity to a motor command
     * 
     * @return The decoded motor command based on neural activity patterns
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Select motor command with curiosity-driven exploration
     * 
     * @param defaultCmd The default motor command from neural decoding
     * @return Motor command, potentially modified by curiosity
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Core neural system
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups organized by action type
    std::vector<Neuron*> motorForward_;     ///< Neurons controlling forward movement
    std::vector<Neuron*> motorBackward_;    ///< Neurons controlling backward movement
    std::vector<Neuron*> motorTurnLeft_;    ///< Neurons controlling left turn
    std::vector<Neuron*> motorTurnRight_;   ///< Neurons controlling right turn
    std::vector<Neuron*> motorInteract_;    ///< Neurons controlling interaction
    std::vector<Neuron*> motorWait_;        ///< Neurons controlling waiting/idle
    
    // Sensory neuron groups organized by modality
    std::vector<Neuron*> sensoryVision_;         ///< Neurons processing visual input
    std::vector<Neuron*> sensoryTouch_;          ///< Neurons processing touch/proximity
    std::vector<Neuron*> sensoryInternal_;       ///< Neurons processing internal state
    std::vector<Neuron*> sensoryProprioception_;  ///< Neurons processing body position
    
    // Neuromodulation state representing chemical signals
    float dopamineLevel_;     ///< Dopamine level (reward prediction error)
    float noveltyLevel_;      ///< Novelty level (stimulus unfamiliarity)
    float curiosityLevel_;    ///< Curiosity level (exploration drive)
    float predictionError_;   ///< Prediction error (reward discrepancy)
    float expectedReward_;    ///< Running average of expected rewards
    
    // Development state affecting plasticity
    double developmentalAge_;    ///< Age in simulation steps
    float plasticityModifier_;   ///< Modifier affecting plasticity strength
    
    // Configuration flags for neural subsystems
    bool rewardModulationEnabled_;       ///< Whether reward-based learning is active
    bool structuralPlasticityEnabled_;   ///< Whether structural changes are allowed
    bool developmentEnabled_;            ///< Whether developmental changes occur
    bool curiosityEnabled_;              ///< Whether exploration is encouraged
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;      ///< Previous vision for novelty calculation
    float sensoryNoveltyDecay_;               ///< Decay rate for novelty signals
};

} // namespace nlm
