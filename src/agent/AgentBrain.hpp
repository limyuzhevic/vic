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
// Provides interface between neural computation and environmental interaction
class AgentBrain {
public:
    /**
     * @brief Construct an AgentBrain with a given brain
     * 
     * @param brain Shared pointer to the NLM brain to connect
     * @note The AgentBrain does not take ownership of the brain pointer
     */
    AgentBrain(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Destroy the AgentBrain instance
     * 
     * @note Properly cleans up internal neuron pointer vectors
     */
    ~AgentBrain();
    
    /**
     * @brief Initialize the agent with world information
     * 
     * @param world Reference to the SimpleWorld for initialization
     * @note Sets up internal state vectors based on world dimensions
     */
    void initialize(const SimpleWorld& world);
    
    /**
     * @brief Get the expected sensory input size for the brain
     * 
     * @return size_t Number of sensory input values (256 vision + 8 touch + 4 internal + 6 proprioception)
     * @note Used by brain to allocate appropriate sensory processing buffers
     */
    size_t getSensoryInputSize() const;
    
    /**
     * @brief Get the expected motor output size from the brain
     * 
     * @return size_t Number of motor outputs (6: forward, backward, turn left, turn right, interact, wait)
     * @note Corresponds to the 6 action groups defined in the constructor
     */
    size_t getMotorOutputSize() const;
    
    /**
     * @brief Process sensory percept and inject into brain neural activity
     * 
     * @param percept The sensory perception containing vision, touch, internal, and proprioception data
     * @note Scales sensory inputs and injects currents into appropriate sensory neurons
     * @warning Neurons in refractory state are not updated
     */
    void processSensoryInput(const SensoryPercept& percept);
    
    /**
     * @brief Decode brain motor activity into a motor command
     * 
     * @return MotorCommand The decoded motor command containing action intensities
     * @note Reads activity from motor neuron groups and applies curiosity-based selection
     */
    MotorCommand decodeMotorCommand();
    
    /**
     * @brief Apply neuromodulation based on reward prediction error
     * 
     * @param reward Actual received reward
     * @param predictedReward Expected reward predicted by brain
     * @note Adjusts dopamine level and plasticity based on prediction error
     */
    void applyRewardModulation(float reward, float predictedReward);
    
    /**
     * @brief Update developmental system
     * 
     * @param timestep Current simulation timestep
     * @note Advances developmental age and adjusts plasticity
     */
    void updateDevelopment(double timestep);
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     * @note Determines plasticity rates and learning capabilities
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Get current neuromodulation level (dopamine)
     * 
     * @return float Current dopamine level (0.0 to 1.0)
     * @note Influences plasticity and learning rate
     */
    float getNeuromodulationLevel() const;
    
    /**
     * @brief Get current curiosity level
     * 
     * @return float Current curiosity level (0.0 to 1.0)
     * @note Drives exploratory behavior
     */
    float getCuriosityLevel() const;
    
    /**
     * @brief Get current novelty level
     * 
     * @return float Current novelty level
     * @note Measures difference from previous sensory input
     */
    float getNoveltyLevel() const;
    
    /**
     * @brief Get prediction error
     * 
     * @return float Current prediction error
     * @note Difference between expected and actual reward
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset agent for new episode
     * 
     * @note Resets developmental state and neuromodulation levels
     */
    void reset();
    
    /**
     * @brief Get the underlying brain pointer
     * 
     * @return Brain* Pointer to the connected brain
     * @warning Returned pointer is managed by the AgentBrain
     */
    Brain* getBrain() { return brain_.get(); }
    
    /**
     * @brief Enable or disable reward modulation
     * 
     * @param enable True to enable reward-based neuromodulation
     */
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    
    /**
     * @brief Enable or disable structural plasticity
     * 
     * @param enable True to enable structural changes (synaptogenesis/pruning)
     */
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    
    /**
     * @brief Enable or disable development system
     * 
     * @param enable True to enable developmental processes
     */
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    
    /**
     * @brief Enable or disable curiosity-driven behavior
     * 
     * @param enable True to enable curiosity-based exploration
     */
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    /**
     * @brief Check if reward modulation is enabled
     * 
     * @return bool True if reward modulation is enabled
     */
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    
    /**
     * @brief Check if structural plasticity is enabled
     * 
     * @return bool True if structural plasticity is enabled
     */
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    /**
     * @brief Check if development is enabled
     * 
     * @return bool True if development is enabled
     */
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    /**
     * @brief Check if curiosity is enabled
     * 
     * @return bool True if curiosity is enabled
     */
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
