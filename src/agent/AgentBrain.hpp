/**
 * @file AgentBrain.hpp
 * @brief Agent interface for connecting NLM brain to the world
 * 
 * This file defines the AgentBrain class which serves as the bridge between the
 * neural brain and the external environment. It handles sensory transduction from
 * the world and motor decoding from neural activity, enabling interaction with
 * simulated environments.
 * 
 * The AgentBrain class is the primary interface for external users to:
 * - Process sensory input from the world and inject into the brain
 * - Decode neural motor activity into executable commands
 * - Apply neuromodulatory signals based on environmental feedback
 * - Monitor internal brain states and levels
 * - Reset the agent for new episodes
 * 
 * @note This class abstracts away the complexity of neural computation while
 *       maintaining direct access to the brain's internal state and parameters.
 *       It's designed to be the main user-facing interface for NLM applications.
 * 
 * @warning The AgentBrain depends on the Brain and world implementations being
 *         properly initialized. Using an uninitialized AgentBrain will result
 *         in undefined behavior. Always initialize with a valid world object
 *         before use.
 * 
 * @see Brain.hpp - Underlying neural computation engine
 * @see SimpleWorld.hpp - World simulation environment
 * @see AgentBody.hpp - Physical representation in world
 * @see SensoryPercept.hpp - Sensory input abstraction
 */

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
/**
 * @brief Connects NLM brain to the world and handles perception/action loop
 * 
 * The AgentBrain class is the primary interface for connecting neural computation
 * to the external world. It acts as a transducer that:
 * - Receives sensory information from the world and converts it into neural input
 * - Interprets neural motor activity and converts it into world commands
 * - Applies environmental feedback through neuromodulation
 * - Maintains the agent's internal state and developmental progress
 * 
 * This class is the main user-facing interface for NLM applications. It provides
 * a clean abstraction over the complex neural computation while maintaining full
 * access to brain functionality when needed.
 * 
 * @note External users should primarily interact with the AgentBrain class rather
 *       than directly accessing the Brain class. The AgentBrain handles the
 *       world-interaction aspects of the neural system.
 * 
 * @warning The AgentBrain must be properly initialized with both a brain and
 *         world object before use. Uninitialized use will result in undefined
 *         behavior or crashes. Always call initialize() before processing
 *         sensory input or decoding motor commands.
 * 
 * @see Brain.hpp - Underlying neural computation engine
 * @see SimpleWorld.hpp - World simulation environment
 * @see AgentBrain::initialize() - Initialization method
 */
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
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
