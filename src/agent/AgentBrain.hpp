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
    
    // Advanced agent behaviors
    // Social learning and meta-cognition
    void enableSocialLearning(bool enable);
    void enableMetaCognition(bool enable);
    void enableSelfReflection(bool enable);
    void enableTransferLearning(bool enable);
    void enableStrategicPlanning(bool enable);
    
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    bool isMetaCognitionEnabled() const { return metaCognitionEnabled_; }
    bool isSelfReflectionEnabled() const { return selfReflectionEnabled_; }
    bool isTransferLearningEnabled() const { return transferLearningEnabled_; }
    bool isStrategicPlanningEnabled() const { return strategicPlanningEnabled_; }
    
    // Social learning state
    struct SocialState {
        float socialConfidence;
        float empathyLevel;
        float cooperationTendency;
        float competitionLevel;
        std::vector<float> socialMemories;
        double lastSocialInteraction;
        
        SocialState() : socialConfidence(0.5f), empathyLevel(0.5f), cooperationTendency(0.5f), 
                       competitionLevel(0.5f), lastSocialInteraction(0.0) {}
    };
    
    // Meta-cognitive state
    struct MetaCognitiveState {
        float selfAwareness;
        float goalImportance;
        float strategyImportance;
        float predictionConfidence;
        std::vector<float> learnedStrategies;
        double lastMetaCognitiveUpdate;
        
        MetaCognitiveState() : selfAwareness(0.5f), goalImportance(0.5f), strategyImportance(0.5f), 
                              predictionConfidence(0.5f), lastMetaCognitiveUpdate(0.0) {}
    };
    
    // Transfer learning state
    struct TransferState {
        float skillTransferability;
        float domainAdaptability;
        std::vector<float> learnedSkills;
        std::vector<std::string> learnedBehaviors;
        double lastTransferLearning;
        
        TransferState() : skillTransferability(0.5f), domainAdaptability(0.5f), lastTransferLearning(0.0) {}
    };
    
    // Strategic planning state
    struct StrategicState {
        float longTermPlanning;
        float shortTermPrioritization;
        float riskAssessment;
        std::vector<MotorCommand> plannedSequence;
        double lastStrategicUpdate;
        
        StrategicState() : longTermPlanning(0.5f), shortTermPrioritization(0.5f), riskAssessment(0.5f), 
                          lastStrategicUpdate(0.0) {}
    };
    
    // Social learning methods
    void updateSocialState(const std::vector<float>& socialSignals, double timestep);
    void applySocialLearning(const std::vector<float>& peerBehavior, float socialReward);
    float getSocialConfidence() const { return socialState_.socialConfidence; }
    float getEmpathyLevel() const { return socialState_.empathyLevel; }
    float getCooperationTendency() const { return socialState_.cooperationTendency; }
    
    // Meta-cognition methods
    void updateMetaCognitiveState(double timestep);
    void reflectOnPerformance(float performanceMetric, double timestep);
    void adjustStrategies(float strategyImportance, float goalImportance);
    float getSelfAwareness() const { return metaCognitiveState_.selfAwareness; }
    float getPredictionConfidence() const { return metaCognitiveState_.predictionConfidence; }
    
    // Transfer learning methods
    void learnFromExperience(const std::vector<float>& experience);
    void adaptToNewDomain(const std::vector<float>& newDomainData);
    float getSkillTransferability() const { return transferState_.skillTransferability; }
    float getDomainAdaptability() const { return transferState_.domainAdaptability; }
    
    // Strategic planning methods
    void planActionSequence(const std::vector<MotorCommand>& availableActions);
    void adjustStrategy(float newImportance, float riskLevel);
    float getLongTermPlanning() const { return strategicState_.longTermPlanning; }
    float getRiskAssessment() const { return strategicState_.riskAssessment; }
    
    // Integrated advanced behavior
    MotorCommand selectWithAdvancedBehaviors(MotorCommand defaultCmd, const std::vector<float>& environmentalContext);
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced behavior selection
    MotorCommand selectWithAdvancedBehaviors(MotorCommand defaultCmd, const std::vector<float>& environmentalContext);
    
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
    
    // Advanced behavior flags
    bool socialLearningEnabled_;
    bool metaCognitionEnabled_;
    bool selfReflectionEnabled_;
    bool transferLearningEnabled_;
    bool strategicPlanningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Initialization state
    bool initialized_;
    
    // Advanced state components
    SocialState socialState_;
    MetaCognitiveState metaCognitiveState_;
    TransferState transferState_;
    StrategicState strategicState_;
    
    // Statistics for monitoring
    struct Statistics {
        size_t sensoryProcessingCount_;
        size_t motorCommandCount_;
        size_t rewardModulationCount_;
        size_t developmentUpdates_;
        size_t socialLearningEvents_;
        size_t metaCognitiveUpdates_;
        size_t transferLearningEvents_;
        size_t strategicPlans_;
        double totalProcessingTime_;
        
        Statistics() : sensoryProcessingCount_(0), motorCommandCount_(0)
            , rewardModulationCount_(0), developmentUpdates_(0)
            , socialLearningEvents_(0), metaCognitiveUpdates_(0)
            , transferLearningEvents_(0), strategicPlans_(0)
            , totalProcessingTime_(0.0) {}
        
        void incrementSensoryProcessing() { sensoryProcessingCount_++; }
        void incrementMotorCommand() { motorCommandCount_++; }
        void incrementRewardModulation() { rewardModulationCount_++; }
        void incrementDevelopmentUpdate() { developmentUpdates_++; }
        void incrementSocialLearningEvent() { socialLearningEvents_++; }
        void incrementMetaCognitiveUpdate() { metaCognitiveUpdates_++; }
        void incrementTransferLearningEvent() { transferLearningEvents_++; }
        void incrementStrategicPlan() { strategicPlans_++; }
        void addProcessingTime(double time) { totalProcessingTime_ += time; }
    };
    
    Statistics stats_;
};
    
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
    
    // Initialization state
    bool initialized_;
    
    // Statistics for monitoring
    struct Statistics {
        size_t sensoryProcessingCount_;
        size_t motorCommandCount_;
        size_t rewardModulationCount_;
        size_t developmentUpdates_;
        double totalProcessingTime_;
        
        Statistics() : sensoryProcessingCount_(0), motorCommandCount_(0)
            , rewardModulationCount_(0), developmentUpdates_(0)
            , totalProcessingTime_(0.0) {}
        
        void incrementSensoryProcessing() { sensoryProcessingCount_++; }
        void incrementMotorCommand() { motorCommandCount_++; }
        void incrementRewardModulation() { rewardModulationCount_++; }
        void incrementDevelopmentUpdate() { developmentUpdates_++; }
        void addProcessingTime(double time) { totalProcessingTime_ += time; }
    };
    
    Statistics stats_;
};

} // namespace nlm
