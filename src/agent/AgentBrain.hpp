#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Exploration modes for agent behavior
enum class ExplorationMode {
    EPSILON_GREEDY,      // Simple epsilon-greedy exploration
    BOLTZMANN,            // Boltzmann temperature-based exploration
    THOMPSON_SAMPLING,    // Thompson sampling for bandit problems
    UPPER_CONFIDENCE_BOUND, // UCB exploration
    NOISE_INJECTION,     // Add Gaussian noise to action selection
    MULTI_ARM,          // Multi-armed bandit approach
    ADAPTIVE             // Adaptive exploration based on performance
};

// Curiosity modes
enum class CuriosityMode {
    PURE_NOVELTY,       // Based purely on sensory novelty
    PREDICTION_ERROR,   // Based on prediction errors
    COMBINED,          // Combination of novelty and prediction error
    ADAPTIVE           // Adapts based on developmental stage
};

// Exploration state for tracking
struct ExplorationState {
    float novelty;           // Current sensory novelty level
    float curiosity;         // Current curiosity level
    float dopamine;          // Current neuromodulation level
    float prediction_error;  // Current prediction error
    DevelopmentalStage developmental_stage; // Current developmental stage
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {

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
    
        // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Enhanced configuration options
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced exploration configuration
    void setExplorationMode(ExplorationMode mode);
    void setExplorationRate(float epsilon);
    void setCuriosityMode(CuriosityMode mode);
    void setLearningRate(float rate);
    void setParallelProcessing(bool enabled, int maxConcurrent);
    
    // Exploration mode getters
    ExplorationMode getExplorationMode() const;
    float getExplorationRate() const;
    CuriosityMode getCuriosityMode() const;
    bool isParallelProcessingEnabled() const;
    int getMaxConcurrentExplorations() const;
    
    // Statistics and analysis methods
    float getAverageCuriosity() const;
    float getAverageNovelty() const;
    float getAverageReward() const;
    double getDevelopmentalAge() const;
    float getPlasticityModifier() const;
    
    // History access
    const std::vector<ExplorationState>& getExplorationHistory() const;
    const std::vector<float>& getRewardHistory() const;
    const std::vector<float>& getCuriosityHistory() const;
    
public:
    // Parallel processing configuration
    bool parallelProcessingEnabled_;
    int maxConcurrentExplorations_;
    
    // Advanced exploration parameters
    ExplorationMode explorationMode_;
    float epsilon_;
    float temperature_;
    float explorationBonusWeight_;
    float learningRate_;
    float discountFactor_;
    float eligibilityTraceDecay_;
    float rewardSmoothing_;
    
    // Curiosity configuration
    bool intrinsicMotivationEnabled_;
    CuriosityMode curiosityMode_;
    
    // Adaptive exploration
    bool adaptiveExploration_;
    
    // History and statistics
    std::vector<ExplorationState> explorationHistory_;
    std::vector<float> rewardHistory_;
    std::vector<float> curiosityHistory_;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Enhanced motor decoding methods
    MotorCommand applyExploration(MotorCommand defaultCmd);
    MotorCommand applyEpsilonGreedy(MotorCommand defaultCmd);
    MotorCommand applyBoltzmannExploration(MotorCommand defaultCmd);
    MotorCommand applyThompsonSampling(MotorCommand defaultCmd);
    MotorCommand applyUCBExploration(MotorCommand defaultCmd);
    MotorCommand applyNoiseInjection(MotorCommand defaultCmd);
    MotorCommand applyMultiArmedBandit(MotorCommand defaultCmd);
    MotorCommand applyParallelProcessing(MotorCommand defaultCmd);
    
    // Helper methods
    void updateExplorationParameters();
    
    // Motor neuron groups (enhanced)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> motorLookLeft_;
    std::vector<Neuron*> motorLookRight_;
    std::vector<Neuron*> motorLookUp_;
    std::vector<Neuron*> motorLookDown_;
    std::vector<Neuron*> motorEat_;
    std::vector<Neuron*> motorDrink_;
    std::vector<Neuron*> motorRest_;
    
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
    bool intrinsicMotivationEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
