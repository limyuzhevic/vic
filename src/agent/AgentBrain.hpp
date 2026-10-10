#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <random>
#include <array>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration struct for AgentBrain parameters
    struct AgentBrainConfig {
        // Neuron distribution parameters
        bool balancedNeuronDistribution = true;
        float minNeuronsPerGroup = 1.0f;
        
        // Motor decoding parameters
        float motorThreshold = 0.3f;  // Dynamic threshold base
        float motorThresholdAdaptRate = 0.01f;  // How fast threshold adapts
        float motorThresholdMin = 0.1f;  // Minimum threshold
        float motorThresholdMax = 0.8f;  // Maximum threshold
        size_t maxMotorGroups = 8;  // Allow more groups
        
        // Novelty detection parameters
        float noveltySmoothingFactor = 0.9f;  // Exponential moving average factor
        float noveltyMinDifference = 0.01f;  // Minimum difference to consider novel
        
        // Curiosity system parameters
        float curiosityBaseExplorationRate = 0.2f;  // Base exploration rate
        float curiosityAdaptationRate = 0.05f;  // How fast curiosity adapts
        float curiosityDecayRate = 0.995f;  // Decay factor per timestep
        float predictionErrorInfluence = 0.3f;  // How much prediction error affects curiosity
        float noveltyInfluence = 0.7f;  // How much novelty affects curiosity
        
        // Memory efficiency parameters
        size_t maxVisionBufferSize = 256;  // Maximum size of vision buffer
        bool useDeltaEncoding = true;  // Use difference-based novelty detection
        
        // Safety and robustness
        bool strictNullChecks = true;  // Perform strict null pointer checks
        float safetyClampMin = -100.0f;  // Safety clamp minimum
        float safetyClampMax = 100.0f;  // Safety clamp maximum
        
        // Performance optimization
        bool precomputeGroupStatistics = true;  // Precompute group stats for efficiency
        float groupActivitySmoothing = 0.95f;  // Smoothing factor for group activity
    };

    enum class BalancedDistributionStrategy {
        Balanced,
        Original
    };

    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
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
    
    // Configuration management
    void setNeuronDistributionStrategy(BalancedDistributionStrategy strategy);
    BalancedDistributionStrategy getNeuronDistributionStrategy() const;
    
    void setMotorThreshold(float threshold);
    void setCuriosityParameters(float baseRate, float adaptationRate, float decayRate);
    
    void enableMemoryEfficiency(bool enable);
    void setSafetyMode(bool strict);
    
    AgentBrainConfig getConfig() const;
    void updateConfig(const AgentBrainConfig& newConfig);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods
    void distributeBalancedNeurons(const std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>>& neuronsByType);
    void distributeMotorNeuronsBalanced(const std::vector<std::vector<Neuron*>>& neuronPopulations);
    void distributeSensoryNeuronsBalanced(const std::vector<std::vector<Neuron*>>& neuronPopulations);
    void distributeOriginalNeurons(const std::unordered_map<NeuronType, std::vector<std::vector<Neuron*>>>& neuronsByType);
    void clearNeuronGroups();
    void processVisionInput(const SensoryPercept& percept);
    void processTouchInput(const SensoryPercept& percept);
    void processInternalInput(const SensoryPercept& percept);
    void processProprioceptionInput(const SensoryPercept& percept);
    void updateNoveltyAndCuriosity(const SensoryPercept& percept);
    float computeDeltaNovelty(const std::vector<float>& currentVision);
    void updateCuriositySystem();
    void reinitializeGroups();
    
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    std::mt19937 rng_;
    BalancedDistributionStrategy distributionStrategy_;
    
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
    float previousNoveltyLevel_;  // For novelty smoothing
    
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
