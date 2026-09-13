#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class Neuron;

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
    
    // Debug and diagnostics
    size_t getSensoryVisionCount() const { return sensoryVision_.size(); }
    size_t getSensoryTouchCount() const { return sensoryTouch_.size(); }
    size_t getSensoryInternalCount() const { return sensoryInternal_.size(); }
    size_t getSensoryProprioceptionCount() const { return sensoryProprioception_.size(); }
    size_t getMotorForwardCount() const { return motorForward_.size(); }
    size_t getMotorBackwardCount() const { return motorBackward_.size(); }
    size_t getMotorTurnLeftCount() const { return motorTurnLeft_.size(); }
    size_t getMotorTurnRightCount() const { return motorTurnRight_.size(); }
    size_t getMotorInteractCount() const { return motorInteract_.size(); }
    size_t getMotorWaitCount() const { return motorWait_.size(); }
    
    // Configuration setters for advanced users
    void setConstants(const struct Constants& c) { constants_ = c; }
    
    // Status information
    std::string getDebugInfo() const;
    
private:
    // Constants for agent behavior
    struct Constants {
        // Sensory scaling factors
        float visionScale = 5.0f;
        float touchScale = 8.0f;
        float internalScale = 5.0f;
        float proprioceptionScale = 3.0f;
        
        // Activity and exploration thresholds
        float minActivityThreshold = 0.5f;
        float curiosityExplorationThreshold = 0.3f;
        float highCuriosityThreshold = 0.5f;
        float explorationChanceMultiplier = 0.3f;
        
        // Neuromodulation ranges
        float dopamineClampMin = -1.0f;
        float dopamineClampMax = 1.0f;
        float noveltyDecayFactor = 0.99f;
        
        // Development timing
        double initialStageTime = 60.0;
        double criticalPeriodTime = 300.0;
        double maturationTime = 900.0;
        float adultPlasticityModifier = 0.2f;
        
        // Memory and plasticity
        float eligibilityTraceThreshold = 0.001f;
        float eligibilityTraceDecay = 0.1f;
        float plasticityBaseFactor = 0.5f;
        float plasticityMinFactor = 0.1f;
        float plasticityMaxFactor = 2.0f;
        
        // STDP parameters
        float stdpLTPWeight = 0.01f;
        float stdpLTDWeight = 0.012f;
        float structuralSynaptogenesisRate = 0.0001f;
        float structuralPruningRate = 0.00001f;
    };

    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper function to calculate distribution index safely
    static size_t calculateDistributionIndex(const std::vector<Neuron*>& neurons, 
                                           size_t accumulatedSize);
    
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
    
    // Configuration constants
    Constants constants_;
};

} // namespace nlm
