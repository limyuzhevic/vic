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
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
        // Get expected reward
    float getExpectedReward() const { return expectedReward_; }
    
    // Get developmental age
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    // Get current plasticity modifier
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced configuration
    void enableActionSequencing(bool enable) { actionSequencingEnabled_ = enable; }
    void enableMemoryBasedNavigation(bool enable) { memoryBasedNavigationEnabled_ = enable; }
    void enableGoalDirectedBehavior(bool enable) { goalDirectedBehaviorEnabled_ = enable; }
    void setExplorationRadius(float radius) { explorationRadius_ = std::max(0.0f, radius); }
    void setMemoryDecayRate(float decay) { memoryDecayRate_ = std::clamp(decay, 0.0f, 1.0f); }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isActionSequencingEnabled() const { return actionSequencingEnabled_; }
    bool isMemoryBasedNavigationEnabled() const { return memoryBasedNavigationEnabled_; }
    bool isGoalDirectedBehaviorEnabled() const { return goalDirectedBehaviorEnabled_; }
    
    // Visit tracking for memory-based navigation
    void recordVisitedLocation(float x, float y);
    bool isLocationVisited(float x, float y) const;
    std::vector<std::pair<float, float>> getVisitedLocations() const { return visitedLocations_; }
    void clearVisitedLocations() { visitedLocations_.clear(); }
    
    // Action sequence management for advanced behavior
    void startActionSequence(const std::vector<MotorCommand>& sequence);
    bool isInActionSequence() const { return !actionSequence_.empty(); }
    MotorCommand getNextActionFromSequence();
    void cancelActionSequence();
    
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
    
    // Enhanced exploration parameters
    float explorationRadius_;
    float memoryDecayRate_;
    float goalTolerance_;
    
    // Advanced behavior flags
    bool actionSequencingEnabled_;
    bool memoryBasedNavigationEnabled_;
    bool goalDirectedBehaviorEnabled_;
    
    // Advanced behavior state
    std::vector<std::pair<float, float>> visitedLocations_;  // For memory-based navigation
    std::vector<MotorCommand> actionSequence_;              // For action sequencing
    float targetX_, targetY_;                               // For goal-directed behavior
    
    // Reward prediction for advanced learning
    float rewardPredictionError_;
    float rewardPredictionDecay_;
    std::vector<float> rewardHistory_;                       // Track recent rewards
};

} // namespace nlm
