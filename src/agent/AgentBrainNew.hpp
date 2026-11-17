// AgentBrain Component Headers
#pragma once

#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// === Component Forward Declarations ===
class SensoryProcessor;
class MotorDecoder;
class NeuromodulationController;
class CognitiveIntegration;

// === Main AgentBrain Class (Orchestrator) ===
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
    
    // Get cognitive states
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get diagnostic counters
    std::string getThoughtTrace() const;
    float getLearnedPreference() const;
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }
    
    // Phase 6: text processing
    static std::vector<size_t> encodeText(const std::string& text);
    std::vector<float> getInternalFiringRates() const;
    
    // Phase 6 counters
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }
    
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    size_t getSocialObservationCount() const;
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
private:
    // Component instances
    std::unique_ptr<SensoryProcessor> sensoryProcessor_;
    std::unique_ptr<MotorDecoder> motorDecoder_;
    std::unique_ptr<NeuromodulationController> neuromodulationController_;
    std::unique_ptr<CognitiveIntegration> cognitiveIntegration_;
    
    // Original state variables (kept for backward compatibility)
    std::shared_ptr<Brain> brain_;
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    std::vector<Neuron*> internalNeurons_;
    
    // State variables
    std::vector<float> predictedVision_;
    float maxTouch_ = 0.0f;
    float forwardTouch_ = 0.0f;
    float forwardObjectVision_ = 1.0f;
    float thoughtReward_ = 0.0f;
    
    // Winner-take-all state
    int lastWinnerGroup_ = -1;
    float lastWinnerMargin_ = 0.0f;
    int consecutiveWinnerCount_ = 0;
    
    // Diversity counters
    int stepsSinceTurn_ = 0;
    int repeatCount_ = 0;
    
    // Diagnostic counters
    int reflexBonusCount_ = 0;
    int reflexSuppressCount_ = 0;
    int wtaPulseCount_ = 0;
    
    // Action history
    MotorCommand lastDecoded_ = MotorCommand::Wait;
    bool hasLastDecoded_ = false;
    
    // Thought trace
    std::vector<float> lastVision_;
    std::string thoughtPercept_ = "none";
    MotorCommand thoughtPlanned_ = MotorCommand::Wait;
    bool hasThoughtPlanned_ = false;
    bool hasThoughtOutcome_ = false;
    
    // Phase 6 counters
    int plannerOverrideCount_ = 0;
    int plannerAgreementCount_ = 0;
    int plannerDecisionCount_ = 0;
    int conceptBiasCount_ = 0;
    int selfBiasCount_ = 0;
    int socialBiasCount_ = 0;
    
    // Learning
    std::vector<float> lastTouch_;
    std::string lastText_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    float sensoryNoveltyDecay_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
};

} // namespace nlm
