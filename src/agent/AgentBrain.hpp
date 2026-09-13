#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace nlm {

// Enhanced AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with advanced features
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
    
    // Advanced features
    void enableAdvancedFeatures();
    void enableAdvancedLearning();
    void enablePersonalizedDevelopment();
    void enableEnvironmentalAdaptation();
    void enableSocialLearning();
    void enableEmotionalIntelligence();
    void enableMoralDevelopment();
    void enableConsciousnessDevelopment();
    void enableAutonomyDevelopment();
    
    // Configuration setters
    void setExplorationRate(float rate);
    void setRewardPredictionHorizon(size_t horizon);
    void setDevelopmentStyle(DevelopmentStyle style);
    void setLearningStyle(LearningStyle style);
    
    // State management
    void recordReward(float reward);
    void recordAction(MotorCommand action);
    void recordLearningOutcome(bool success, float reward);
    void recordSocialInteraction(const std::string& interactionType, bool success, float outcomeValue);
    void recordEnvironmentalCondition(float temperature, float humidity, float lightLevel);
    void recordMoralDecision(const std::string& decision, const std::string& reasoning, bool ethical);
    
    // Goal management
    void setGoals(const std::vector<std::string>& goals);
    void updateGoalProgress(const std::string& goal, float progress);
    
    // State queries
    float predictNextReward() const;
    float calculateLearningEfficiency() const;
    float calculateActionNovelty(MotorCommand currentAction) const;
    float calculateEmotionalStability() const;
    float calculateEthicalConsistency() const;
    float calculateSocialSuccessRate() const;
    float calculateSensoryAwareness() const;
    float calculateMemoryAwareness() const;
    float calculateSocialAwareness() const;
    float calculateSuccessRate() const;
    float calculateRewardConsistency() const;
    float calculatePlasticityVariance() const;
    float calculateAdaptation() const;
    float calculateNoveltyLevel();
    float calculateSynapticActivityLevel();
    float calculateSynaptogenesisRate();
    float calculatePruningRate();
    float calculateActionVariety() const;
    float calculateEnvironmentalStress() const;
    float calculateAutonomyLevel() const;
    
    // State updates
    void adaptNeuromodulationLevels();
    void adaptToEnvironment();
    void applyAdvancedNeuromodulation();
    void applySocialLearning();
    void applyMoralDevelopment();
    void applyAutonomyDevelopment();
    void applyEmotionalStateToBehavior();
    void adjustBehaviorForEnvironment(float temperature, float humidity, float lightLevel);
    
    // Consciousness updates
    void updateAwarenessLevel();
    void updateSelfAwareness();
    void updateMetacognitiveAbility();
    void updateConsciousnessMetrics();
    
    // Emotional state management
    void setEmotionalState(float fear, float pleasure, float surprise, float anger);
    
    // Environmental adaptation
    void enableEnvironmentalAdaptation();
    void recordEnvironmentalCondition(float temperature, float humidity, float lightLevel);
    void adaptToEnvironment();
    
    // Social learning
    void enableSocialLearning();
    void recordSocialInteraction(const std::string& interactionType, bool success, float outcomeValue);
    void applySocialLearning();
    
    // Emotional intelligence
    void enableEmotionalIntelligence();
    void setEmotionalState(float fear, float pleasure, float surprise, float anger);
    void applyEmotionalStateToBehavior();
    
    // Moral development
    void enableMoralDevelopment();
    void recordMoralDecision(const std::string& decision, const std::string& reasoning, bool ethical);
    void applyMoralDevelopment();
    
    // Consciousness development
    void enableConsciousnessDevelopment();
    void updateAwarenessLevel();
    void updateSelfAwareness();
    void updateMetacognitiveAbility();
    void updateConsciousnessMetrics();
    
    // Autonomy development
    void enableAutonomyDevelopment();
    void setGoals(const std::vector<std::string>& goals);
    void updateGoalProgress(const std::string& goal, float progress);
    void applyAutonomyDevelopment();
    
    // Nested types
    enum class DevelopmentStyle {
        Initial,
        CriticalPeriod,
        Maturation,
        Adult,
        Aging
    };
    
    enum class LearningStyle {
        Exploratory,
        TrialAndError,
        Balanced,
        Reflective
    };
    
    // Internal state structures
    struct EnvironmentalCondition {
        float temperature;
        float humidity;
        float lightLevel;
        double timestamp;
    };
    
    struct SocialInteraction {
        std::string type;
        bool success;
        float outcomeValue;
        double timestamp;
    };
    
    struct EmotionalState {
        float fear;
        float pleasure;
        float surprise;
        float anger;
        double timestamp;
        
        void reset() {
            fear = 0.0f;
            pleasure = 0.0f;
            surprise = 0.0f;
            anger = 0.0f;
            timestamp = 0.0;
        }
    };
    
    struct MoralDecision {
        std::string decision;
        std::string reasoning;
        bool ethical;
        double timestamp;
    };
    
    struct ActionHistory {
        MotorCommand action;
        float timestamp;
        float outcomeValue;
    };
    
    struct RewardHistory {
        float reward;
        double timestamp;
    };
    
    struct LearningMetric {
        float success;
        float reward;
        double timestamp;
    };
    
    struct CuriosityState {
        float level;
        float novelty;
        double timestamp;
    };
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods
    void initializeNeuronGroups();
    void distributeMotorNeurons(const std::vector<Neuron*>& neurons);
    void distributeSensoryNeurons(const std::vector<Neuron*>& neurons);
    void clearNeuronGroups();
    void logInitializationStats() const;
    void checkActionHabituation(MotorCommand action);
    float calculateActionVariety() const;
    float calculateEnvironmentalStress() const;
    
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
    std::vector<float> vision_;
    float sensoryNoveltyDecay_;
    
    // Advanced state
    double lastActionTime_;
    size_t actionSequenceLength_;
    float explorationRate_;
    size_t rewardPredictionHorizon_;
    float adaptationFactor_;
    float habituationThreshold_;
    
    // History tracking
    std::vector<float> rewardPrediction_;
    std::vector<ActionHistory> curiosityHistory_;
    std::vector<RewardHistory> adaptationHistory_;
    std::vector<LearningMetric> learningMetrics_;
    std::vector<RewardHistory> successHistory_;
    std::vector<RewardHistory> failureHistory_;
    std::vector<SocialInteraction> socialInteractions_;
    std::vector<float> socialSuccessRate_;
    std::vector<EmotionalState> emotionHistory_;
    std::vector<float> moralValues_;
    std::vector<MoralDecision> ethicalDecisions_;
    std::vector<EnvironmentalCondition> environmentalHistory_;
    std::vector<float> developmentalMetrics_;
    
    // Advanced features state
    bool advancedFeaturesEnabled_;
    bool personalizedDevelopment_;
    bool environmentalAdaptation_;
    bool socialLearning_;
    bool emotionalIntelligence_;
    bool moralDevelopment_;
    bool consciousnessDevelopment_;
    bool autonomyDevelopment_;
    
    // Style and preference tracking
    DevelopmentStyle developmentStyle_;
    LearningStyle learningStyle_;
    float awarenessLevel_;
    float selfAwareness_;
    float metacognitiveAbility_;
    
    // Autonomy state
    std::vector<std::string> goals_;
    std::map<std::string, float> goalProgress_;
    float goalSettingAbility_;
    float decisionMakingAbility_;
    float selfRegulationAbility_;
    
    // Environmental adaptation
    float temperatureSensitivity_;
    float lightSensitivity_;
    float humiditySensitivity_;
    
    // Consciousness and awareness
    bool selfReflectionEnabled_;
    float introspectionLevel_;
    float metaAwarenessLevel_;
    
    // Memory and learning metrics
    size_t workingMemoryCapacity_;
    size_t episodicMemoryCapacity_;
    float memoryConsolidationRate_;
    float learningRate_;
    float retentionRate_;
    
    // Social and emotional intelligence
    float empathyLevel_;
    float theoryOfMindLevel_;
    float emotionalIntelligenceLevel_;
    float moralReasoningLevel_;
    
    // Personalization and adaptation
    std::string personalityProfile_;
    float adaptabilityLevel_;
    float innovationRate_;
    float conformityLevel_;
};

} // namespace nlm
