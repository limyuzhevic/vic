#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>

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
    
    // Get expected reward
    float getExpectedReward() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    void enableMetaLearning(bool enable) { metaLearningEnabled_ = enable; }
    void enableMemoryBasedDecisionMaking(bool enable) { memoryBasedDecisionMakingEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    bool isMetaLearningEnabled() const { return metaLearningEnabled_; }
    bool isMemoryBasedDecisionMakingEnabled() const { return memoryBasedDecisionMakingEnabled_; }
    
    // Advanced behavior configuration
    void configureForExploration(bool enable);
    void configureForExploitation(bool enable);
    void configureForSocialLearning(bool enable);
    void configureForMetaLearning(bool enable);
    void configureForMemoryBasedDecisionMaking(bool enable);
    
    // Advanced behavior control
    void setExplorationStrategy(ExplorationStrategy strategy);
    void setSocialLearningStrategy(SocialLearningStrategy strategy);
    void setMetaLearningStrategy(MetaLearningStrategy strategy);
    void setMemoryBasedDecisionMakingStrategy(MemoryBasedDecisionMakingStrategy strategy);
    
    // Advanced behavior queries
    ExplorationStrategy getExplorationStrategy() const;
    SocialLearningStrategy getSocialLearningStrategy() const;
    MetaLearningStrategy getMetaLearningStrategy() const;
    MemoryBasedDecisionMakingStrategy getMemoryBasedDecisionMakingStrategy() const;
    
    // Reward shaping
    void addRewardShapingRule(const RewardShapingRule& rule);
    void removeRewardShapingRule(const std::string& id);
    void clearRewardShapingRules();
    
    // Credit assignment
    void setCreditAssignmentMethod(CreditAssignmentMethod method);
    CreditAssignmentMethod getCreditAssignmentMethod() const;
    
    // Advanced curiosity computation
    void setCuriosityComputationMethod(CuriosityComputationMethod method);
    CuriosityComputationMethod getCuriosityComputationMethod() const;
    float computeAdvancedCuriosity() const;
    
    // Memory-based decision making
    void storeExperience(const Experience& experience);
    std::vector<Experience> getRecentExperiences(size_t count) const;
    void setMemoryCapacity(size_t capacity);
    size_t getMemoryCapacity() const;
    
    // Social learning
    void addSocialPartner(std::shared_ptr<AgentBrain> partner);
    void removeSocialPartner(std::shared_ptr<AgentBrain> partner);
    void clearSocialPartners();
    std::vector<std::shared_ptr<AgentBrain>> getSocialPartners() const;
    void performSocialLearning();
    
    // Meta-learning
    void setMetaLearningRate(float rate);
    float getMetaLearningRate() const;
    void updateMetaLearningParameters();
    
    // Advanced neuromodulation
    void addNeuromodulator(const Neuromodulator& neuromodulator);
    void removeNeuromodulator(const std::string& id);
    void clearNeuromodulators();
    std::vector<Neuromodulator> getNeuromodulators() const;
    float computeAdvancedNeuromodulation() const;
    
    // Visualization and monitoring
    std::string getBehaviorSummary() const;
    std::string getSocialNetworkSummary() const;
    std::string getMemorySummary() const;
    
    // Learning statistics
    float getLearningProgress() const;
    float getSocialLearningProgress() const;
    float getMetaLearningProgress() const;
    float getMemoryBasedDecisionMakingProgress() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced exploration strategies
    MotorCommand exploreWithStrategy(ExplorationStrategy strategy);
    MotorCommand exploreWithSocialLearning();
    MotorCommand exploreWithMetaLearning();
    MotorCommand exploreWithMemoryBasedDecisionMaking();
    
    // Social learning methods
    void performImitationLearning(std::shared_ptr<AgentBrain> partner);
    void performCooperativeLearning(std::shared_ptr<AgentBrain> partner);
    void performCompensatoryLearning(std::shared_ptr<AgentBrain> partner);
    
    // Meta-learning methods
    void updateLearningRates();
    void adaptPlasticityParameters();
    void adjustExplorationExploitationBalance();
    
    // Memory-based decision making
    std::vector<Experience> retrieveSimilarExperiences(const Experience& query) const;
    float estimateValueFromMemory(const std::vector<Experience>& experiences) const;
    
    // Reward shaping methods
    float applyRewardShaping(float reward, const Experience& context) const;
    
    // Credit assignment methods
    float assignCredit(float reward, const Experience& experience, CreditAssignmentMethod method) const;
    
    // Advanced curiosity computation methods
    float computePredictionErrorBasedCuriosity(float predictionError) const;
    float computeNoveltyBasedCuriosity(float novelty) const;
    float computeUncertaintyBasedCuriosity(float uncertainty) const;
    
    // Neuromodulation methods
    float computeDopamineFromReward(float reward, float predictedReward) const;
    float computeCuriosityFromUncertainty(float uncertainty) const;
    float computeNoveltyFromSurprise(float surprise) const;
    
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
    bool socialLearningEnabled_;
    bool metaLearningEnabled_;
    bool memoryBasedDecisionMakingEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced behavior state
    ExplorationStrategy explorationStrategy_;
    SocialLearningStrategy socialLearningStrategy_;
    MetaLearningStrategy metaLearningStrategy_;
    MemoryBasedDecisionMakingStrategy memoryBasedDecisionMakingStrategy_;
    
    // Reward shaping
    std::map<std::string, RewardShapingRule> rewardShapingRules_;
    
    // Credit assignment
    CreditAssignmentMethod creditAssignmentMethod_;
    
    // Curiosity computation
    CuriosityComputationMethod curiosityComputationMethod_;
    
    // Memory systems for experience storage
    std::vector<Experience> experienceMemory_;
    size_t memoryCapacity_;
    
    // Social learning
    std::vector<std::shared_ptr<AgentBrain>> socialPartners_;
    
    // Meta-learning
    float metaLearningRate_;
    float learningProgress_;
    float socialLearningProgress_;
    float metaLearningProgress_;
    float memoryBasedDecisionMakingProgress_;
    
    // Neuromodulators
    std::vector<Neuromodulator> neuromodulators_;
    
    // Advanced state tracking
    struct BehavioralState {
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float rewardPrediction;
        std::vector<float> recentRewards;
        std::vector<float> recentNovelties;
        std::vector<float> recentUncertainties;
        size_t stepCount;
        
        BehavioralState() : curiosityLevel(0.0f), noveltyLevel(0.0f), predictionError(0.0f), 
                          rewardPrediction(0.0f), stepCount(0) {}
    };
    
    BehavioralState behavioralState_;
};

// Advanced behavior enumeration
enum class ExplorationStrategy {
    Random,
    CuriosityDriven,
    NoveltyDirected,
    UncertaintyBased,
    SocialLearning,
    MetaLearning,
    MemoryBased
};

enum class SocialLearningStrategy {
    Imitation,
    Cooperation,
    Compensation,
    Collective,
    Competitive,
    Observational
};

enum class MetaLearningStrategy {
    RateBased,
    CreditAssignment,
    ExplorationExploitation,
    PlasticityAdaptation,
    RewardShaping,
    CuriosityTuning
};

enum class MemoryBasedDecisionMakingStrategy {
    AssociativeLearning,
    PatternCompletion,
    ReinforcementLearning,
    CognitiveMapping,
    EpisodicMemory,
    WorkingMemory
};

// Advanced data structures
struct RewardShapingRule {
    std::string id;
    std::function<float(float, const Experience&)> rewardShapingFunction;
    float strength;
    std::string description;
    
    RewardShapingRule(const std::string& id, const std::function<float(float, const Experience&)>& func, 
                     float strength = 1.0f, const std::string& desc = "")
        : id(id), rewardShapingFunction(func), strength(strength), description(desc) {}
};

struct Experience {
    double timestamp;
    std::vector<float> sensoryInput;
    MotorCommand action;
    float reward;
    float novelty;
    float predictionError;
    std::vector<NeuronId> activeNeurons;
    std::vector<SynapseId> activeSynapses;
    
    Experience() : timestamp(0.0), reward(0.0f), novelty(0.0f), predictionError(0.0f) {}
    Experience(double ts, const std::vector<float>& sensory, const MotorCommand& act, 
                float rew, float nov, float predError)
        : timestamp(ts), sensoryInput(sensory), action(act), reward(rew), novelty(nov), predictionError(predError) {}
};

enum class CreditAssignmentMethod {
    None,
    Simple,
    EligibilityTrace,
    MonteCarlo,
    TemporalDifference,
    PolicyGradient
};

enum class CuriosityComputationMethod {
    NoveltyOnly,
    PredictionErrorOnly,
    Combined,
    UncertaintyBased,
    InformationGain,
    SurpriseBased
};

enum class NeuromodulatorType {
    Dopamine,
    Serotonin,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
    Custom
};

struct Neuromodulator {
    std::string id;
    NeuromodulatorType type;
    float level;
    float influence;
    std::function<float(const AgentBrain&)> computeLevel;
    
    Neuromodulator(const std::string& id, NeuromodulatorType type, 
                   const std::function<float(const AgentBrain&)>& compute)
        : id(id), type(type), level(0.0f), influence(1.0f), computeLevel(compute) {}
};

} // namespace nlm
