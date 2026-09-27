#pragma once

#include "../agent/AgentBody.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Agent configuration parameters for easy customization
struct AgentConfig {
    // Motor scaling factors
    float visionSensoryScale = 5.0f;        // Scale factor for vision input
    float touchSensoryScale = 8.0f;         // Scale factor for touch input
    float internalSensoryScale = 5.0f;      // Scale factor for internal signals
    float proprioceptionSensoryScale = 3.0f; // Scale factor for proprioception
    
    // Neuromodulation parameters
    float noveltyDecayRate = 0.99f;         // How quickly novelty decays
    float curiosityUpdateWeight = 2.0f;     // Weight of novelty in curiosity calculation
    float predictionErrorWeight = 0.5f;     // Weight of prediction error in curiosity
    
    // Exploration parameters
    float curiosityThreshold = 0.3f;       // Minimum curiosity to trigger exploration
    float explorationThreshold = 0.5f;      // High curiosity threshold for random exploration
    float explorationChance = 0.3f;         // Maximum exploration probability
    
    // Motor processing
    float motorActivityThreshold = 0.5f;    // Minimum activity to take action
    
    // Development parameters
    double developmentalStage1End = 60.0;   // ~1 minute
    double developmentalStage2End = 300.0;  // ~5 minutes
    double developmentalStage3End = 900.0;  // ~15 minutes
    float adultPlasticity = 0.2f;           // Plasticity rate in adult stage
    
    // Reward modulation
    float rewardDecayRate = 0.95f;          // Exponential moving average decay
    float eligibilityTraceDecay = 0.1f;     // How fast eligibility traces decay
    float eligibilityThreshold = 0.001f;    // Minimum eligibility for weight updates
    float dopamineClampMin = -1.0f;         // Minimum dopamine level
    float dopamineClampMax = 1.0f;          // Maximum dopamine level
    
    // Synaptogenesis and pruning rates
    float earlySynaptogenesisRate = 0.0001f;  // Rate in initial stage
    float earlyPruningRate = 0.00001f;         // Rate in initial stage
    float adultSynaptogenesisRate = 0.000001f; // Rate in adult stage
    float adultPruningRate = 0.00002f;         // Rate in adult stage
    
    // Developmental plasticity modifiers
    float initialPlasticity = 1.0f;        // High plasticity in initial stage
    float criticalPeriodPlasticity = 0.8f; // Moderately high in critical period
    float maturationPlasticity = 0.5f;     // Reduced in maturation
    
    // Logging and debugging
    bool enableDebugLogging = true;        // Enable debug logging
    bool enablePerformanceMetrics = false; // Track performance metrics
};

// Motor decoder for converting neural activity to motor commands
class MotorDecoder {
public:
    MotorDecoder(const AgentConfig& config);
    ~MotorDecoder() = default;
    
    // Calculate average activity in a group of neurons
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    
    // Decode motor command from motor neuron activity
    MotorCommand decodeFromMotorNeurons(
        const std::vector<Neuron*>& forward,
        const std::vector<Neuron*>& backward,
        const std::vector<Neuron*>& left,
        const std::vector<Neuron*>& right,
        const std::vector<Neuron*>& interact,
        const std::vector<Neuron*>& wait
    ) const;
    
    // Select action with curiosity-based exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd, float curiosityLevel, 
                                    const class RandomGenerator* rng) const;
    
    // Set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
private:
    AgentConfig config_;
};

// Sensory processor for handling sensory input and novelty detection
class SensoryProcessor {
public:
    SensoryProcessor(const AgentConfig& config);
    ~SensoryProcessor() = default;
    
    // Process sensory input and inject currents into neurons
    void processSensoryInput(
        const SensoryPercept& percept,
        const std::vector<Neuron*>& visionNeurons,
        const std::vector<Neuron*>& touchNeurons,
        const std::vector<Neuron*>& internalNeurons,
        const std::vector<Neuron*>& proprioceptionNeurons
    );
    
    // Compute novelty based on vision change
    float computeNovelty(const std::vector<float>& currentVision) const;
    
    // Get novelty decay rate
    float getNoveltyDecayRate() const { return config_.noveltyDecayRate; }
    
    // Initialize with world dimensions
    void initialize(int visionWidth, int visionHeight);
    
    // Reset state
    void reset();
    
    // Set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
private:
    AgentConfig config_;
    std::vector<float> previousVision_;
};

// Neuromodulation manager for reward, dopamine, and prediction error
class NeuromodulationManager {
public:
    NeuromodulationManager(const AgentConfig& config);
    ~NeuromodulationManager() = default;
    
    // Update neuromodulation based on reward prediction
    void update(float reward, float predictedReward,
                float& dopamineLevel,
                float& predictionError,
                float& expectedReward) const;
    
    // Apply reward-modulated weight changes to synapses
    void applyPlasticityChanges(
        const std::vector<Neuron*>& neurons,
        float dopamineLevel,
        float plasticityModifier,
        const class STDP* stdp
    ) const;
    
    // Get configuration
    float getRewardDecayRate() const { return config_.rewardDecayRate; }
    float getEligibilityDecayRate() const { return config_.eligibilityTraceDecay; }
    float getEligibilityThreshold() const { return config_.eligibilityThreshold; }
    float getDopamineMin() const { return config_.dopamineClampMin; }
    float getDopamineMax() const { return config_.dopamineClampMax; }
    
    // Set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
private:
    AgentConfig config_;
};

// Development manager for developmental stages and plasticity
class DevelopmentManager {
public:
    DevelopmentManager(const AgentConfig& config);
    ~DevelopmentManager() = default;
    
    // Update developmental stage based on age
    void updateDevelopment(double timestep, double& developmentalAge,
                          float& plasticityModifier,
                          DevelopmentalStage& currentStage) const;
    
    // Update structural plasticity rates
    void updateStructuralPlasticity(
        float plasticityModifier,
        float& synaptogenesisRate,
        float& pruningRate
    ) const;
    
    // Get developmental stage based on age
    DevelopmentalStage getDevelopmentalStage(double age) const;
    
    // Get plasticity modifier for a given age
    float getPlasticityModifier(double age) const;
    
    // Set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
private:
    AgentConfig config_;
};

// Curiosity manager for exploration motivation
class CuriosityManager {
public:
    CuriosityManager(const AgentConfig& config);
    ~CuriosityManager() = default;
    
    // Update curiosity level based on novelty and prediction error
    float updateCuriosity(float novelty, float predictionError) const;
    
    // Determine if exploration should be triggered
    bool shouldExplore(float curiosityLevel) const;
    
    // Calculate exploration probability
    float getExplorationProbability(float curiosityLevel) const;
    
    // Set configuration
    void setConfig(const AgentConfig& config) { config_ = config; }
    const AgentConfig& getConfig() const { return config_; }
    
private:
    AgentConfig config_;
};

} // namespace nlm
