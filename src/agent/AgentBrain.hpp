#pragma once

#include "AgentBody.hpp"
#include "../sensory/SensoryInput.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class SelfModel;
class SocialLearning;

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
    
    // Get diagnostic information about motor group activities
    std::vector<MotorGroupActivity> getMotorGroupActivities() const;
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation levels
    float getNeuromodulationLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Get learned preference trait (survives checkpoint reload)
    float getLearnedPreference() const;
    
    // Get thought trace (Phase 5): human-readable record of recent loop
    std::string getThoughtTrace() const;
    
    // Phase 5 diagnostic counters
    int getReflexBonusCount() const { return reflexBonusCount_; }
    int getReflexSuppressCount() const { return reflexSuppressCount_; }
    int getWtaPulseCount() const { return wtaPulseCount_; }
    
    // Phase 6: text-as-a-sense functionality
    static std::vector<size_t> encodeText(const std::string& text);
    std::vector<float> getInternalFiringRates() const;
    
    // Phase 6 thought/planning counters
    int getPlannerOverrideCount() const { return plannerOverrideCount_; }
    int getPlannerAgreementCount() const { return plannerAgreementCount_; }
    int getPlannerDecisionCount() const { return plannerDecisionCount_; }
    int getConceptBiasCount() const { return conceptBiasCount_; }
    int getSelfBiasCount() const { return selfBiasCount_; }
    int getSocialBiasCount() const { return socialBiasCount_; }
    
    // Phase 6 wired systems
    size_t getConceptCount() const;
    float getSelfModelConfidence() const;
    size_t getSocialObservationCount() const;
    void observeOtherAgent(MotorCommand observed, const std::vector<float>& beforeVision,
                          const std::vector<float>& afterVision);
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration setters/getters
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
private:
    // Core functionality
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Motor group classifications
    struct MotorGroupActivity {
        const char* name;
        float activity;
        size_t refractoryCount;  // neurons in Refractory state (recently spiked)
        size_t size = 0;
        float firingRate = 0.0f;
    };
    
    // Agent components
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
    
    // Phase 5: Vision-guided reflex features
    std::vector<float> predictedVision_;
    float sensoryNoveltyDecay_;
    float maxTouch_;
    float forwardTouch_;
    float forwardObjectVision_;
    
    // Phase 5: Winner-take-all (WTA) mechanism
    int lastWinnerGroup_;
    float lastWinnerMargin_;
    int consecutiveWinnerCount_;
    
    // Phase 5: Action diversity
    int stepsSinceTurn_;
    int repeatCount_;
    
    // Phase 5: Diagnostic counters
    int reflexBonusCount_;
    int reflexSuppressCount_;
    int wtaPulseCount_;
    
    // Phase 6: Wired cognition
    SelfModel selfModel_;
    SocialLearning socialLearning_;
    bool selfModelReady_;
    bool socialReady_;
    std::vector<float> lastTouch_;
    std::string lastText_;
    
    // Phase 6: Planning/concept/social counters
    int plannerOverrideCount_;
    int plannerAgreementCount_;
    int plannerDecisionCount_;
    int conceptBiasCount_;
    int selfBiasCount_;
    int socialBiasCount_;
    
    // Phase 6: State tracking
    std::vector<float> lastVision_;
    std::string thoughtPercept_;
    MotorCommand thoughtPlanned_;
    bool hasThoughtPlanned_;
    float thoughtReward_;
    bool hasThoughtOutcome_;
    
    // Helper methods
    std::vector<float> conceptPattern() const;
    std::string formatThoughtTrace() const;
    
    friend class AgentBrainImpl;  // For implementation details if needed
};

} // namespace nlm