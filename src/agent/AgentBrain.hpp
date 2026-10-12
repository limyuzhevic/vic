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
    
    // ========== CONVENIENCE METHODS ==========
    
    // Process complete sensory cycle: percept -> brain input -> motor output
    MotorCommand processSensoryToMotor(const SensoryPercept& percept);
    
    // Batch process multiple sensory inputs and get consolidated motor response
    MotorCommand processSensorSequence(const std::vector<SensoryPercept>& percepts);
    
    // Check if agent should explore based on curiosity and novelty
    bool shouldExplore(float curiosityThreshold = 0.5f, float noveltyThreshold = 0.3f) const;
    
    // Get comprehensive agent state summary
    std::string getStateSummary() const;
    
    // Enable/disable all learning systems
    void enableAllLearning(bool enable);
    void disableAllLearning();
    
    // Get effective neuromodulation (considering enabled status)
    float getEffectiveNeuromodulation() const;
    
    // Process sensory input and return if novel
    bool processInputAndCheckNovelty(const SensoryPercept& percept);
    
    // Get action with curiosity-driven exploration
    MotorCommand getActionWithExploration();
    
    // Batch update for multiple timesteps
    void batchUpdate(double timestep, int count = 1);
    
    // Reset and initialize for new episode with custom settings
    void resetForEpisode(const SimpleWorld& world, bool resetMemory = true);
    
    // Get agent readiness status
    bool isAgentReady() const;
    
    // Get current agent status as string
    std::string getStatus() const;
    
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
    
    // State tracking
    mutable bool isProcessing_;
    mutable SimulationStep lastProcessedStep_;
    
    // ========== PRIVATE IMPLEMENTATION HELPERS ==========
    
    // Check novelty using stored previous state
    bool checkNovelty(const std::vector<float>& currentVision) const;
    
    // Update previous state
    void updatePreviousState(const std::vector<float>& vision);
    
    // Reset internal processing state
    void resetProcessingState();

} // namespace nlm
