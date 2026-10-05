#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

// Exploration strategy enumeration
    enum class ExplorationStrategy {
        None,              // No exploration, purely exploitative
        EpsilonGreedy,     // Simple epsilon-greedy exploration
        Softmax,           // Temperature-based softmax action selection
        ThompsonSampling,  // Thompson sampling for Bayesian learning
        CuriosityDriven,   // Curiosity-driven exploration based on novelty/prediction error
        Adaptive,          // Adaptive exploration that learns from experience
        IntrinsicMotivation // Exploration based on intrinsic motivation
    };
    
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Enhanced exploration configuration
    void setExplorationStrategy(ExplorationStrategy strategy) { explorationStrategy_ = strategy; }
    void setTemperature(float temp) { temperature_ = temp; }
    void setAdaptiveEpsilon(float epsilon, float decayRate) { 
        adaptiveEpsilon_ = epsilon; 
        epsilonDecayRate_ = decayRate;
    }
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    ExplorationStrategy getExplorationStrategy() const { return explorationStrategy_; }
    float getTemperature() const { return temperature_; }
    float getExplorationRate() const { return explorationRate_; }
    float getAdaptiveEpsilon() const { return adaptiveEpsilon_; }

private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    MotorCommand selectWithExploration(MotorCommand defaultCmd);
    MotorCommand softmaxActionSelection(const std::vector<float>& actionValues);
    MotorCommand epsilonGreedyActionSelection(const std::vector<float>& actionValues, float epsilon);
    MotorCommand thompsonSamplingActionSelection(const std::vector<float>& actionValues, const std::vector<float>& uncertainties);
    
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
    
    // Enhanced exploration state
    ExplorationStrategy explorationStrategy_;
    float temperature_;
    float explorationRate_;
    float adaptiveEpsilon_;
    float epsilonDecayRate_;
    
    // Action value estimation
    std::vector<float> actionValues_;
    std::vector<float> actionUncertainties_;
    std::vector<float> actionRewards_;
    std::vector<int> actionCounts_;
    
    // Exploration history
    std::vector<float> explorationHistory_;
    std::vector<float> learningProgress_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
