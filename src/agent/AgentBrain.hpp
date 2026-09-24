#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Configuration structure for AgentBrain parameters
    struct Config {
        // Sensory processing configuration
        float visionScaleFactor = 5.0f;      // Scale factor for vision input
        float touchScaleFactor = 8.0f;      // Scale factor for touch input  
        float internalScaleFactor = 5.0f;   // Scale factor for internal signals
        float proprioceptionScaleFactor = 3.0f;  // Scale factor for proprioception
        
        // Novelty detection configuration
        float noveltyDecayRate = 0.99f;     // Rate of novelty decay per timestep
        float curiosityMultiplier = 2.0f;  // Multiplier for curiosity calculation
        float predictionErrorWeight = 0.5f; // Weight for prediction error in curiosity
        
        // Motor command configuration
        float actionThreshold = 0.5f;      // Minimum activity level to trigger action
        float curiosityThreshold = 0.3f;   // Threshold for curiosity-based exploration
        float explorationChance = 0.3f;    // Maximum exploration probability
        
        // Reward modulation configuration
        float rewardUpdateRate = 0.05f;     // Learning rate for reward updates
        float rewardClampingMin = -1.0f;   // Minimum dopamine level
        float rewardClampingMax = 1.0f;    // Maximum dopamine level
        
        // Development configuration
        double developmentalTimestep = 1.0; // Time per development update
        double criticalPeriodStart = 60.0;  // Start of critical period (seconds)
        double criticalPeriodEnd = 300.0;   // End of critical period (seconds)
        double maturationEnd = 900.0;       // End of maturation (seconds)
        
        // Structural plasticity configuration
        float synaptogenesisRate = 0.0001f; // Rate of new synapse formation
        float pruningRate = 0.00001f;      // Rate of synapse pruning
        
        // Callback type for custom curiosity calculation
        using CuriosityCalculator = std::function<float(float, float, float)>;
        
        // Callback type for custom action selection
        using ActionSelector = std::function<MotorCommand(MotorCommand, float, float)>;
        
        // Callback type for custom reward processing
        using RewardProcessor = std::function<void(float, float, float&)>;
        
        // Callbacks for extensibility
        CuriosityCalculator curiosityCalculator;
        ActionSelector actionSelector;
        RewardProcessor rewardProcessor;
        
        // Logging configuration
        bool enableLogging = true;
        std::string logPrefix = "AgentBrain";
        
        // Performance optimization
        bool enableActivityCaching = true;  // Cache neuron activity calculations
        size_t activityCacheSize = 100;     // Size of activity cache
        
        // Safety checks
        bool enableSafetyChecks = true;     // Enable null pointer and bounds checks
        float minActivityThreshold = 0.01f; // Minimum activity before considering action
    };

    // Default configuration
    static Config getDefaultConfig();

    // Constructor with optional configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const Config& config = getDefaultConfig());
    ~AgentBrain();
    
    // Initialize with world and configuration
    void initialize(const SimpleWorld& world, const Config& config);
    
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
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced configuration options
    void setVisionScaleFactor(float factor) { config_.visionScaleFactor = factor; }
    void setCuriosityThreshold(float threshold) { config_.curiosityThreshold = threshold; }
    void setExplorationChance(float chance) { config_.explorationChance = chance; }
    void setRewardClamping(float min, float max) { 
        config_.rewardClampingMin = min; 
        config_.rewardClampingMax = max; 
    }
    
    // Configuration getters
    float getVisionScaleFactor() const { return config_.visionScaleFactor; }
    float getCuriosityThreshold() const { return config_.curiosityThreshold; }
    float getExplorationChance() const { return config_.explorationChance; }
    float getRewardClampingMin() const { return config_.rewardClampingMin; }
    float getRewardClampingMax() const { return config_.rewardClampingMax; }
    
    // Custom callback setup
    void setCuriosityCalculator(const Config::CuriosityCalculator& calculator) {
        config_.curiosityCalculator = calculator;
    }
    
    void setActionSelector(const Config::ActionSelector& selector) {
        config_.actionSelector = selector;
    }
    
    void setRewardProcessor(const Config::RewardProcessor& processor) {
        config_.rewardProcessor = processor;
    }
    
    // Status and diagnostics
    std::string getStatusString() const;
    size_t getSensoryNeuronCount() const;
    size_t getMotorNeuronCount() const;
    
    // Performance monitoring
    double getLastStepTime() const { return lastStepTime_; }
    size_t getStepCount() const { return stepCount_; }
    
    // Safety and validation
    bool validateConfiguration() const;
    bool validateNeuralConnectivity() const;
    
protected:
    // Protected constructor for subclasses
    AgentBrain(std::shared_ptr<Brain> brain, Config&& config);
    
    // Processing methods (protected for extensibility)
    virtual void processVisionInput(const SensoryPercept& percept);
    virtual void processTouchInput(const SensoryPercept& percept);
    virtual void processInternalInput(const SensoryPercept& percept);
    virtual void processProprioceptionInput(const SensoryPercept& percept);
    virtual void updateNoveltyAndCuriosity(const SensoryPercept& percept);
    
    // Callback methods
    virtual float calculateCuriosityLevel(float novelty, float predictionError) const;
    virtual MotorCommand selectActionWithCuriosity(MotorCommand defaultCmd, float curiosityLevel) const;
    virtual void processRewardModulation(float reward, float predictedReward, float& dopamineLevel) const;
    
    // Helper methods
    virtual void logMessage(const std::string& message) const;
    virtual bool validatePercept(const SensoryPercept& percept) const;
    
private:
    // Core implementation methods
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    Config config_;
    
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
    
    // Performance monitoring
    mutable double lastStepTime_;
    mutable size_t stepCount_;
    
    // Cached values for performance
    mutable bool activityCacheDirty_;
    mutable float cachedAverageFiring_;
    
    // Private helper methods
    void initializeMotorNeurons();
    void initializeSensoryNeurons();
    static float calculateNeuronActivity(const std::vector<Neuron*>& neurons);
    static float calculateCuriosityLevel(float novelty, float predictionError, float multiplier, float weight);
    static float calculatePlasticityFactor(float dopamineLevel);
    
    // Validation and safety
    void validateInitialization() const;
    bool checkNeuralConnectivity() const;
    void safetyCheck(const std::string& context) const;
};

} // namespace nlm
