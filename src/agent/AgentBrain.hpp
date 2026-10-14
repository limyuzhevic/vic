#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief Connects NLM brain to the simulated world environment
 * 
 * The AgentBrain acts as an interface layer that translates between the neural
 * computation of the Brain class and the external world. It handles:
 * - Sensory transduction: converting world percepts into neural current injections
 * - Motor decoding: converting neural activity into actionable commands
 * - Neuromodulation: applying reward prediction error to plasticity
 * - Development: tracking agent maturation and changing plasticity
 * - Exploration: curiosity-driven behavior selection
 * 
 * This class bridges the gap between the abstract neural substrate and
 * embodied interaction with the environment.
 */
class AgentBrain {
public:
    /**
     * @brief Construct an AgentBrain with a neural brain instance
     * 
     * @param brain Shared pointer to the neural brain that will process
     *              sensory input and generate motor commands
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destroy the AgentBrain instance
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent brain with a world configuration
     * 
     * Sets up internal state and allocates memory for sensory buffers
     * based on the world's sensory capabilities (vision resolution, etc.)
     * 
     * @param world Reference to the world simulation that provides
     *              sensory inputs and executes motor commands
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected size of sensory input vector for the brain
     * 
     * Returns the total number of scalar values that the brain expects
     * as input, typically calculated from all sensory modalities combined
     * 
     * @return Total sensory input size (vision + touch + internal + proprioception)
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected size of motor output vector
     * 
     * Returns the number of motor action groups that the brain can activate
     * to produce behavior in the world
     * 
     * @return Number of available motor commands (typically 6)
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process new sensory percept and inject into the neural brain
     * 
     * Converts world percepts (vision, touch, internal signals, proprioception)
     * into neural current injections according to their respective strengths
     * and scaling factors. Updates novelty detection based on visual changes.
     * 
     * @param percept Sensory percept containing all current world inputs
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode current neural motor activity into a concrete command
     * 
     * Analyzes the activity patterns across motor neuron groups to determine
     * the most appropriate action, optionally modified by curiosity-driven
     * exploration when curiosity levels are high
     * 
     * @return The selected motor command for world interaction
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply reward prediction error neuromodulation to plasticity
     * 
     * Updates dopamine-like signals based on the difference between actual
     * and predicted rewards, which modulates synaptic plasticity throughout
     * the brain according to eligibility traces
     * 
     * @param reward The actual reward received from the world
     * @param predictedReward The brain's prediction of expected reward
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update developmental system based on agent age
     * 
     * Advances the agent's developmental stage, which modulates plasticity
     * rates and other developmental parameters according to biologically
     * inspired developmental trajectory
     * 
     * @param timestep Time elapsed since last development update
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get the current developmental stage of the agent
     * 
     * Returns the developmental stage (Initial, CriticalPeriod, Maturation, Adult)
     * which determines plasticity rates and behavioral capabilities
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level (dopamine-like signal)
     * 
     * Returns the current prediction error signal that modulates neural
     * excitability and synaptic plasticity
     * 
     * @return Neuromodulation level (-1.0 to 1.0)
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level driving exploration behavior
     * 
     * Curiosity combines novelty detection and prediction error to
     * determine the agent's motivation to explore novel sensory inputs
     * 
     * @return Curiosity level (0.0 to 1.0)
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level based on sensory changes
     * 
     * Novelty is computed as the difference between current and previous
     * visual input, providing a measure of environmental change that
     * can drive learning and exploration
     * 
     * @return Novelty level (0.0 to 1.0)
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get current prediction error signal
     * 
     * Prediction error represents the difference between received rewards
     * and expected rewards, serving as a teaching signal for learning
     * 
     * @return Prediction error value
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent state for a new episode
     * 
     * Clears all neuromodulatory signals, developmental age, plasticity
     * modifiers, and sensory history to prepare for fresh learning
     */
    void reset();
    
    /**
     * @brief Get pointer to the underlying neural brain
     * 
     * Provides access to the core neural computation engine for advanced
     * operations or direct manipulation
     * 
     * @return Pointer to the brain's implementation
     */
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration methods for tuning agent behavior
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
     * @brief Decode motor command from neural activity patterns
     * 
     * Analyzes average activity across all motor neuron groups to determine
     * which action to execute, selecting the command with highest activity
     * when it exceeds a threshold
     * 
     * @return The most active motor command
     */
    MotorCommand decodeFromMotorNeurons();
    
    /**
     * @brief Apply curiosity-driven exploration to motor command selection
     * 
     * When curiosity levels are high, randomly selects alternative motor
     * commands to promote exploration of novel behaviors and sensory inputs
     * 
     * @param defaultCmd The motor command selected by pure neural activity
     * @return Final motor command (may be modified by curiosity)
     */
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    /**
     * @brief Shared pointer to the neural brain implementing computation
     */
    std::shared_ptr<Brain> brain_;
    
    /**
     * @brief Motor neuron groups organized by action type
     * 
     * These vectors contain pointers to neurons that drive specific motor
     * commands: forward, backward, turn left, turn right, interact, wait
     */
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    /**
     * @brief Sensory neuron groups organized by modality
     * 
     * These vectors contain pointers to neurons that process different
     * types of sensory input: vision, touch, internal, and proprioception
     */
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    /**
     * @brief Neuromodulatory signal levels
     * 
     * These represent different brain chemicals or signals that modulate
     * neural function: dopamine for reward prediction error, curiosity
     * for exploration motivation, and other neuromodulators
     */
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    /**
     * @brief Developmental state tracking
     * 
     * These variables track the agent's developmental progress: age,
     * plasticity modifier (how changeable the brain is), and other
     * developmental parameters
     */
    double developmentalAge_;
    float plasticityModifier_;
    
    /**
     * @brief Configuration flags enabling/disabling behavioral systems
     * 
     * These flags control whether different cognitive systems are active:
     * reward modulation, structural plasticity, developmental updates,
     * and curiosity-driven exploration
     */
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    /**
     * @brief Previous visual input for novelty detection
     * 
     * Stores the last frame's vision data to compute changes over time,
     * which drives curiosity and exploration behavior
     */
    std::vector<float> previousVision_;
    
    /**
     * @brief Decay rate for novelty signals over time
     * 
     * Controls how quickly novelty signals fade, balancing immediate
     * response to new inputs with persistent motivation for exploration
     */
    float sensoryNoveltyDecay_;
};

} // namespace nlm
