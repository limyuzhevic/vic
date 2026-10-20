#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @class AgentBrain
 * @brief Connects NLM brain to the world, handling sensory transduction and motor decoding
 * @details The AgentBrain serves as the interface layer between the internal neural
 * computation system (Brain) and the external environment (world). It translates
 * sensory percepts into neural inputs and decodes neural activity into motor commands,
 * effectively bridging the gap between the artificial brain and actionable behavior.
 * 
 * Core Functions:
 * - Sensory transduction: Converts external/world sensory inputs into brain-compatible formats
 * - Motor decoding: Translates neural activity patterns into executable motor commands
 * - Neuromodulation: Integrates reward signals and developmental signals
 * - State management: Maintains coherence between brain and world states
 * 
 * Architecture:
 * - Input layer: Maps sensory percepts to brain neuron groups
 * - Processing layer: Applies neuromodulation and developmental signals
 * - Output layer: Decodes motor neuron activity to action commands
 * - State layer: Maintains neuromodulation levels and developmental stage
 * 
 * Thread Safety: Not thread-safe for concurrent simulation. Should be accessed
 * through a single agent simulation thread or with external synchronization.
 * 
 * Performance: O(1) for basic operations, O(n) for large-scale neural group access.
 * Memory usage is constant per agent (~few KB for state tracking).
 * 
 * Agent Lifecycle:
 * @code
 * // Create and initialize agent
 * auto brain = std::make_shared<Brain>(config);
 * AgentBrain agentBrain(brain);
 * 
 * // Initialize with world
 * agentBrain.initialize(world);
 * 
 * // Main simulation loop
 * while (!world.isEpisodeDone()) {
 *     // Process sensory input
 *     SensoryPercept percept = world.getSensoryInput();
 *     agentBrain.processSensoryInput(percept);
 *     
 *     // Step brain
 *     brain->step(currentStep);
 *     
 *     // Decode motor command
 *     MotorCommand cmd = agentBrain.decodeMotorCommand();
 *     
 *     // Apply to world
 *     world.applyAction(cmd);
 * }
 * 
 * // Reset for next episode
 * agentBrain.reset();
 * @endcode
 * 
 * Biological Plausibility:
 * - Mirrors biological sensory-motor integration
 * - Implements basal ganglia pathways for action selection
 * - Uses neuromodulation for reinforcement learning
 * - Maintains internal models of the world
 * 
 * @see AgentBody
 * @see SensoryPercept
 * @see MotorCommand
 * @see Brain
 * @see SimpleWorld
 * @see DevelopmentalStage
 */
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
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
