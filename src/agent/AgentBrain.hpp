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
    
    // Planner influence on action selection
    MotorCommand applyPlannerInfluence(MotorCommand baseCmd);
    
    // Cognitive system integration
    void initializeCognitiveSystems();
    void passSensoryToCognition(const std::vector<float>& vision);
    void updateCognitiveWithNovelty(const std::vector<float>& percept);
    void applyCognitiveInfluenceToPlasticity();
    void updateDevelopmentWithCognitiveState();
    
    // Cognitive complexity assessment
    float getCognitiveComplexityFactor() const;
    float calculateDevelopmentRate(float cognitiveFactor) const;
    
    // Self-model management
    void initializeSelfModel();
    SelfModel createBodyModel();
    GoalSet createGoalSet();
    MotorCapabilities analyzeMotorCapabilities();
    SensoryCapabilities analyzeSensoryCapabilities();
    
    // Feature extraction
    std::vector<float> extractSensoryFeatures(const std::vector<float>& vision);
    
    // Entity classes for self-model
    struct SelfModel {
        std::string type;
        std::string position;
        std::vector<std::string> capabilities;
        std::vector<std::string> constraints;
        float energy;
        float maxEnergy;
        SelfMemory memory;
        MotorCapabilities motorCapabilities;
        SensoryCapabilities sensoryCapabilities;
        
        SelfModel() : energy(1.0f), maxEnergy(1.0f) {}
    };
    
    struct GoalSet {
        std::string primaryGoal;
        std::vector<std::string> subgoals;
        std::vector<float> priority;
    };
    
    struct MotorCapabilities {
        std::vector<std::string> movementTypes;
        std::vector<float> speedRanges;
        float accuracy;
    };
    
    struct SensoryCapabilities {
        std::vector<std::string> modality;
        std::vector<float> range;
        std::vector<size_t> resolution;
    };
    
    struct SelfMemory {
        std::string name;
        std::string identity;
        std::vector<std::string> experiences;
        float confidence;
    };
    
    // Override methods for integration
    void enableRewardModulation(bool enable) override { 
        rewardModulationEnabled_ = enable; 
        NLM_LOG_INFO("[COGNITIVE] Reward modulation " + 
                     std::string(enable ? "enabled" : "disabled")); 
    }
    
    void enableStructuralPlasticity(bool enable) override { 
        structuralPlasticityEnabled_ = enable; 
        NLM_LOG_INFO("[COGNITIVE] Structural plasticity " + 
                     std::string(enable ? "enabled" : "disabled")); 
    }
    
    void enableDevelopment(bool enable) override { 
        developmentEnabled_ = enable; 
        NLM_LOG_INFO("[COGNITIVE] Development " + 
                     std::string(enable ? "enabled" : "disabled")); 
    }
    
    void enableCuriosity(bool enable) override { 
        curiosityEnabled_ = enable; 
        NLM_LOG_INFO("[COGNITIVE] Curiosity " + 
                     std::string(enable ? "enabled" : "disabled")); 
    }
    
    bool isRewardModulationEnabled() const override { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const override { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const override { return developmentEnabled_; }
    bool isCuriosityEnabled() const override { return curiosityEnabled_; }
    
    // Additional cognitive getters
    float getCognitiveComplexity() const { return getCognitiveComplexityFactor(); }
    std::vector<float> getCurrentSensoryState() const { return getSensoryFeatures(); }
    
private:
    // Cognitive systems integration
    void registerCognitiveSystems();
    void registerSelfModel();
    
    // Motor and sensory setup
    void setupSensoryNeurons();
    void setupMotorNeurons();
    
    // Processing methods
    void storePreviousSensoryState(const SensoryPercept& percept);
    void injectSensoryInput(const SensoryPercept& percept);
    void updatePredictionFromSensory(const SensoryPercept& percept);
    void planWithCurrentCognition(const std::vector<float>& state);
    
    // Feature extraction and planning
    std::vector<float> getSensoryFeatures() const;
    void planWithCurrentState(const std::vector<float>& state, float reward);
    void updateConceptWithExperience(const std::vector<float>& pattern, 
                                     const std::vector<float>& features,
                                     float reward);
    void updateAttentionWithNovelty(const std::vector<float>& percept);
    
    // State and feature management
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Cognitive integration
    std::shared_ptr<Brain> brain_;
    SelfModel selfModel_;
    
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
};

} // namespace nlm
