#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
// Manages neuromodulation, development, and behavioral output
class AgentBrain {
public:
    // Constructor and destructor
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    /**
     * Initialize agent brain with world for sensory processing and action execution
     * 
     * @param world Reference to the world instance containing agent body and objects
     * @throws std::invalid_argument if world configuration is invalid
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * Process sensory perception from the world
     * Injects sensory signals into the brain's sensory neurons for processing
     * 
     * @param percept Sensory perception containing vision, touch, and internal signals
     * @note Updates internal sensory state for novelty detection
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * Decode brain motor activity into actionable motor command
     * Converts neural activity patterns from motor neurons into behavioral commands
     * 
     * @return MotorCommand The highest-activity motor command from the brain
     * @note Includes curiosity/exploration-based command selection
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * Apply reward-based neuromodulation to brain plasticity
     * Updates neuromodulatory signals (dopamine) based on reward prediction error
     * 
     * @param reward Actual reward received from world
     * @param predictedReward Expected reward predicted by brain
     * @note Enhances learning for reward-prediction mismatches
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * Update developmental system state
     * Manages critical periods, maturation, and age-dependent plasticity
     * 
     * @param timestep Time step duration for development updates
     * @note Affects plasticity modifiers and developmental stage
     */
    void updateDevelopment(double timestep);
    
    /**
     * Get the size of sensory input vector expected by brain
     * Determines number of sensory channels the brain requires
     * 
     * @return size_t Number of sensory input elements expected
     * @note Used to validate sensory percept dimensions
     */
    size_t getSensoryInputSize() const;
    
    /**
     * Get the size of motor output vector expected by brain
     * Determines number of motor neurons or output channels
     * 
     * @return size_t Number of motor output elements expected
     * @note Used for motor command generation
     */
    size_t getMotorOutputSize() const;
    
    /**
     * Get current prediction error signal
     * Measures discrepancy between predicted and actual sensory input
     * 
     * @return float Current prediction error magnitude (higher = larger error)
     * @note Drives neuromodulation and learning rate adjustments
     */
    float getPredictionError() const;
    
    /**
     * Get current neuromodulation level
     * Returns overall neuromodulatory signal strength
     * 
     * @return float Neuromodulation level (typically 0.0-1.0)
     * @note Combines dopamine, curiosity, and novelty signals
     */
    float getNeuromodulationLevel() const;
    
    /**
     * Get current curiosity level
     * Measures agent's tendency toward exploration
     * 
     * @return float Curiosity level (0.0 = exploitative, 1.0 = exploratory)
     * @note Influences motor command selection
     */
    float getCuriosityLevel() const;
    
    /**
     * Get current novelty level
     * Measures difference from previous sensory input
     * 
     * @return float Novelty level (0.0 = familiar, higher = novel)
     * @note Drives curiosity and information-seeking behavior
     */
    float getNoveltyLevel() const;
    
    /**
     * Get current developmental stage
     * Indicates phase of agent development and plasticity
     * 
     * @return DevelopmentalStage Current developmental stage
     * @note Affects learning rates and plasticity
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Enable or disable reward-based neuromodulation
     * 
     * @param enable Whether to enable reward modulation
     * @note Affects learning from reward signals
     */
    void enableRewardModulation(bool enable);
    
    /**
     * Enable or disable structural synaptic plasticity
     * 
     * @param enable Whether to enable structural changes
     * @note Enables formation/destruction of synapses
     */
    void enableStructuralPlasticity(bool enable);
    
    /**
     * Enable or disable developmental processes
     * 
     * @param enable Whether to enable development
     * @note Affects critical periods and maturation
     */
    void enableDevelopment(bool enable);
    
    /**
     * Enable or disable curiosity-driven exploration
     * 
     * @param enable Whether to enable curiosity
     * @note Increases exploratory behavior
     */
    void enableCuriosity(bool enable);
    
    /**
     * Check if reward-based neuromodulation is enabled
     * 
     * @return bool true if reward modulation is active
     * @note Affects learning from reward signals
     */
    bool isRewardModulationEnabled() const;
    
    /**
     * Check if structural plasticity is enabled
     * 
     * @return bool true if structural changes are allowed
     * @note Enables formation/destruction of synapses
     */
    bool isStructuralPlasticityEnabled() const;
    
    /**
     * Check if development system is enabled
     * 
     * @return bool true if developmental processes are active
     * @note Affects critical periods and maturation
     */
    bool isDevelopmentEnabled() const;
    
    /**
     * Check if curiosity-driven exploration is enabled
     * 
     * @return bool true if curiosity system is active
     * @note Increases exploratory behavior
     */
    bool isCuriosityEnabled() const;
    
    /**
     * Reset agent brain for new episode
     * Clears all internal state and prepares for fresh initialization
     * 
     * @note Resets neural activity, developmental state, and configuration
     */
    void reset();
    
    /**
     * Get non-const pointer to underlying brain implementation
     * 
     * @return Brain* Pointer to the brain instance
     * @note For internal use only, allows modification of brain state
     */
    Brain* getBrain();
    
    /**
     * Get const pointer to underlying brain implementation
     * 
     * @return const Brain* Const pointer to the brain instance
     * @note Allows read-only access to brain state
     */
    const Brain* getBrain() const;

private:
    // Core brain interface
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron populations
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron populations
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
    
    // Sensory state history for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Private implementation methods
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
};

} // namespace nlm
