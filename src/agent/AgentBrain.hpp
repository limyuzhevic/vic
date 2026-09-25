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
    
    // ========== ADVANCED COGNITIVE SYSTEM INTEGRATION ==========
    
    // Neural planning for action sequences
    void enableNeuralPlanner(bool enable) { neuralPlannerEnabled_ = enable; }
    bool isNeuralPlannerEnabled() const { return neuralPlannerEnabled_; }
    
    // Concept formation from experiences
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    
    // Self-model for body schema and agency
    void enableSelfModel(bool enable) { selfModelEnabled_ = enable; }
    bool isSelfModelEnabled() const { return selfModelEnabled_; }
    
    // Social learning from observations
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Get current plan confidence
    float getPlanConfidence() const;
    
    // Get self-model awareness
    float getSelfModelAwareness() const;
    
    // Get social learning influence
    float getSocialLearningInfluence() const;
    
    // Update attention with cognitive context
    void updateAttentionWithCognitiveContext(float novelty, float predictionError);
    
    // Process prediction error for learning
    void processPredictionErrorForLearning(float predictionError);
    
    // Store experience for concept formation
    void storeExperienceForConceptFormation(const SensoryPercept& percept, MotorCommand action, float reward);
    
    // Update self-model with new sensory input
    void updateSelfModelWithSensoryInput(const SensoryPercept& percept);
    
    // Observe other agents for social learning
    void observeOtherAgent(const AgentBrain& otherAgent);
    
    // Get cognitive metrics for debugging
    struct CognitiveMetrics {
        float planConfidence;
        float conceptStability;
        float selfModelAwareness;
        float socialLearningInfluence;
        float attentionFocus;
        float predictionAccuracy;
    };
    
    CognitiveMetrics getCognitiveMetrics() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced cognitive processing methods
    void updateCognitiveSystems();
    void updateNeuralPlanner(MotorCommand currentAction);
    void updateConceptFormation();
    void updateSelfModel();
    void updateSocialLearning();
    
    // Helper methods for cognitive integration
    void updatePlanBasedOnContext();
    void updateConceptsFromExperience();
    void updateSelfModelRepresentation();
    void updateSocialKnowledge();
    
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
    
    // ========== ADVANCED COGNITIVE SYSTEM STATE ==========
    
    // Neural planning state
    bool neuralPlannerEnabled_;
    float planConfidence_;
    std::vector<MotorCommand> currentPlan_;
    
    // Concept formation state
    bool conceptFormationEnabled_;
    std::vector<std::vector<float>> learnedConcepts_;
    std::vector<float> conceptStability_;
    
    // Self-model state
    bool selfModelEnabled_;
    float selfModelAwareness_;
    std::vector<float> bodySchema_;
    std::vector<float> actionConsequences_;
    
    // Social learning state
    bool socialLearningEnabled_;
    float socialLearningInfluence_;
    std::vector<float> socialRules_;
    std::vector<float> observedBehaviors_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Experience storage for concept formation
    struct Experience {
        std::vector<float> percept;
        MotorCommand action;
        float reward;
        double timestamp;
    };
    std::vector<Experience> experienceBuffer_;
    
    // Social observation storage
    struct SocialObservation {
        std::vector<float> observerPercept;
        MotorCommand observedAction;
        std::vector<float> socialContext;
        double observationTime;
    };
    std::vector<SocialObservation> socialObservations_;
    
    // Attention state
    float attentionFocus_;
    float predictionAccuracy_;
};

} // namespace nlm
