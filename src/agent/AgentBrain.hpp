#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// AgentBrainState: Comprehensive system state structure for advanced users
struct AgentBrainState {
    // Neuromodulation state
    float dopamine;
    float novelty;
    float curiosity;
    float predictionError;
    float expectedReward;
    
    // Development state
    double developmentalAge;
    float plasticityModifier;
    
    // Configuration state
    bool rewardModulationEnabled;
    bool structuralPlasticityEnabled;
    bool developmentEnabled;
    bool curiosityEnabled;
    
    // Neuron group counts (for performance monitoring)
    size_t sensoryVisionCount;
    size_t sensoryTouchCount;
    size_t sensoryInternalCount;
    size_t sensoryProprioceptionCount;
    size_t motorForwardCount;
    size_t motorBackwardCount;
    size_t motorTurnLeftCount;
    size_t motorTurnRightCount;
    size_t motorInteractCount;
    size_t motorWaitCount;
    
    // Default constructor
    AgentBrainState()
        : dopamine(0.0f), novelty(0.0f), curiosity(0.0f), predictionError(0.0f), expectedReward(0.0f)
        , developmentalAge(0.0), plasticityModifier(1.0f)
        , rewardModulationEnabled(true), structuralPlasticityEnabled(true), developmentEnabled(true), curiosityEnabled(true)
        , sensoryVisionCount(0), sensoryTouchCount(0), sensoryInternalCount(0), sensoryProprioceptionCount(0)
        , motorForwardCount(0), motorBackwardCount(0), motorTurnLeftCount(0), motorTurnRightCount(0)
        , motorInteractCount(0), motorWaitCount(0) {}
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
    
    // Initialize motor and sensory neuron groups
    void initializeMotorSensoryGroups();
    
    // Process individual sensory input channels
    void injectVisionInput(const std::vector<float>& vision);
    void injectTouchInput(const std::vector<float>& touch);
    void injectInternalInput(const std::vector<float>& internal);
    void injectProprioceptiveInput(const std::vector<float>& proprioception);
    
    // Compute novelty from vision changes
    void computeNoveltyLevel(const std::vector<float>& vision);
    
    // Update curiosity based on novelty and prediction error
    void updateCuriosity();
    
    // Apply reward to all synapses
    void applyRewardToSynapses();
    
    // Modulate STDP plasticity based on dopamine
    void modulateSTDPPlasticity();
    
    // Update developmental stage
    void updateDevelopmentalStage();
    
    // Update structural plasticity with development
    void updateStructuralPlasticityWithDevelopment();
    
public:
    // Advanced API: Configure neuromodulation levels
    void setTargetNeuromodulation(float targetDopamine, float targetCuriosity);
    
    // Advanced API: Get comprehensive system state
    AgentBrainState getBrainState() const;
    
    // Advanced API: Performance monitoring
    float getComputationalLoad() const;
    
    // Advanced API: Debug information
    struct DebugInfo {
        std::vector<float> visionData;
        std::vector<float> touchData;
        std::vector<float> internalData;
        std::vector<float> proprioceptionData;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float developmentalAge;
        float plasticityModifier;
        std::vector<float> motorActivity;
        double currentTime;
        SimulationStep currentStep;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float computationalLoad;
    };
    
    DebugInfo getDebugInfo() const;
    
    // Advanced API: Batch processing
    void processBatchSensoryInput(const std::vector<SensoryPercept>& percepts);
};

} // namespace nlm