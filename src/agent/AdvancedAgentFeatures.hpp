// Advanced Agent Features Module
// Enhanced AgentBrain with sophisticated capabilities for Phase 6

#pragma once

#include "agent/AgentBrain.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "environment/Environment.hpp"
#include <unordered_map>
#include <memory>
#include <queue>

namespace nlm {

// Advanced agent capabilities for Phase 6
class AdvancedAgentFeatures {
public:
    AdvancedAgentFeatures(std::shared_ptr<AgentBrain> agentBrain);
    ~AdvancedAgentFeatures() = default;
    
    // Multi-modal novelty detection
    struct MultiModalNovelty {
        float sensoryNovelty;
        float cognitiveNovelty;
        float motorNovelty;
        float predictionError;
        float temporalNovelty;
        float socialNovelty;
        
        MultiModalNovelty() : sensoryNovelty(0.0f), cognitiveNovelty(0.0f),
                             motorNovelty(0.0f), predictionError(0.0f),
                             temporalNovelty(0.0f), socialNovelty(0.0f) {}
        
        float getCombinedNovelty() const {
            return (sensoryNovelty + cognitiveNovelty + motorNovelty + 
                   predictionError + temporalNovelty + socialNovelty) / 6.0f;
        }
    };
    
    // Skill system for motor learning
    struct Skill {
        std::string name;
        float proficiency;      // 0.0 to 1.0
        float lastUsedTime;     // Simulation time
        float successRate;      // 0.0 to 1.0
        float energyCost;       // Energy required to use skill
        int useCount;           // Number of times used
        
        Skill(const std::string& n, float energy = 1.0f) 
            : name(n), proficiency(0.0f), lastUsedTime(0.0f), 
              successRate(0.0f), energyCost(energy), useCount(0) {}
    };
    
    // Forward model for action prediction
    struct ForwardModel {
        std::vector<float> expectedOutcomes;
        std::vector<float> predictionConfidence;
        double lastPredictionTime;
        
        ForwardModel(size_t outputSize) {
            expectedOutcomes.resize(outputSize, 0.0f);
            predictionConfidence.resize(outputSize, 0.0f);
            lastPredictionTime = 0.0;
        }
        
        float predictActionOutcome(const MotorCommand& action) const {
            // Simple prediction based on action type
            switch (action) {
                case MotorCommand::MoveForward: return 0.8f;
                case MotorCommand::MoveBackward: return 0.6f;
                case MotorCommand::TurnLeft: return 0.7f;
                case MotorCommand::TurnRight: return 0.7f;
                case MotorCommand::LookLeft: return 0.5f;
                case MotorCommand::LookRight: return 0.5f;
                case MotorCommand::Interact: return 0.9f;
                case MotorCommand::Wait: return 0.3f;
                default: return 0.5f;
            }
        }
    };
    
    // Behavioral state machine for action selection
    enum class BehavioralState {
        IDLE,
        EXPLORATION,
        EXPLOITATION,
        LEARNING,
        REWARD_SEEKING,
        AVOIDANCE,
        SOCIAL_INTERACTION,
        SKILL_PRACTICE
    };
    
    // Social knowledge and relationships
    struct SocialKnowledge {
        std::unordered_map<std::string, float> relationshipStrength;  // -1.0 to 1.0
        std::unordered_map<std::string, float> mutualTrust;           // 0.0 to 1.0
        std::unordered_map<std::string, float> sharedHistory;         // Experience level
        std::vector<std::string> learnedBehaviors;                    // Behaviors observed
        
        void updateRelationship(const std::string& other, float delta) {
            auto it = relationshipStrength.find(other);
            if (it != relationshipStrength.end()) {
                it->second = std::max(-1.0f, std::min(1.0f, it->second + delta));
            } else {
                relationshipStrength[other] = delta;
            }
        }
        
        float getTrust(const std::string& other) const {
            auto it = mutualTrust.find(other);
            return (it != mutualTrust.end()) ? it->second : 0.0f;
        }
    };
    
    // Experience and learning records
    struct ExperienceRecord {
        double timestamp;
        std::vector<float> state;
        MotorCommand action;
        float reward;
        float predictionError;
        float noveltyLevel;
        std::string context;     // Current behavioral state
        
        ExperienceRecord(double t, const std::vector<float>& s, 
                         MotorCommand a, float r, float pe, float nl, 
                         const std::string& c)
            : timestamp(t), state(s), action(a), reward(r), 
              predictionError(pe), noveltyLevel(nl), context(c) {}
    };
    
    // Enhanced curiosity system with multiple dimensions
    class EnhancedCuriosity {
    public:
        EnhancedCuriosity() : explorationPressure_(0.5f), knowledgeValue_(0.5f),
                             intrinsicMotivation_(0.5f), extrinsicMotivation_(0.5f) {}
        
        void update(double timestep, const MultiModalNovelty& novelty,
                   const std::vector<float>& predictionErrors);
        
        float getExplorationPressure() const { return explorationPressure_; }
        float getKnowledgeValue() const { return knowledgeValue_; }
        float getIntrinsicMotivation() const { return intrinsicMotivation_; }
        float getExtrinsicMotivation() const { return extrinsicMotivation_; }
        
        MotorCommand selectActionWithCuriosity(const MotorCommand& defaultAction,
                                              const std::vector<MotorCommand>& availableActions);
        
    private:
        float explorationPressure_;
        float knowledgeValue_;
        float intrinsicMotivation_;
        float extrinsicMotivation_;
        std::queue<MotorCommand> explorationQueue_;
    };
    
    // Memory consolidation and replay system
    class MemorySystem {
    public:
        MemorySystem() : maxMemorySlots_(1000), consolidationInterval_(100),
                         replayStrength_(0.8f), experienceCapacity_(5000) {}
        
        void storeExperience(const ExperienceRecord& experience);
        void consolidateMemories(double currentTime);
        void replayImportantMemories();
        const std::vector<ExperienceRecord>& getRecentExperiences(size_t count) const;
        float getMemoryStrength(const ExperienceRecord& experience) const;
        
        void setMaxMemorySlots(size_t slots) { maxMemorySlots_ = slots; }
        void setConsolidationInterval(size_t interval) { consolidationInterval_ = interval; }
        void setReplayStrength(float strength) { replayStrength_ = strength; }
        
    private:
        std::vector<ExperienceRecord> experiences_;
        std::unordered_map<std::string, float> memoryAssociations_;
        size_t maxMemorySlots_;
        size_t consolidationInterval_;
        float replayStrength_;
        size_t experienceCapacity_;
        
        // Temporal memory indexing
        std::unordered_map<std::string, std::vector<size_t>> temporalIndex_;
        std::unordered_map<std::string, std::vector<size_t>> contentIndex_;
    };
    
    // Advanced motor control with skill-based behaviors
    class AdvancedMotorControl {
    public:
        AdvancedMotorControl(std::shared_ptr<AgentBrain> agentBrain);
        ~AdvancedMotorControl() = default;
        
        void learnSkill(const Skill& skill);
        bool executeSkill(const std::string& skillName, float energyBudget);
        float evaluateActionOutcome(const MotorCommand& action, const ActionResult& result);
        std::vector<MotorCommand> getAvailableActions() const;
        MotorCommand selectActionWithSkills(const MotorCommand& defaultAction);
        
        void setEnergyBudget(float budget) { energyBudget_ = budget; }
        float getEnergyBudget() const { return energyBudget_; }
        void setEnergyDecayRate(float rate) { energyDecayRate_ = rate; }
        
    private:
        std::shared_ptr<AgentBrain> agentBrain_;
        std::unordered_map<std::string, Skill> skills_;
        float energyBudget_;
        float energyDecayRate_;
        std::unordered_map<MotorCommand, float> actionValues_;
        std::queue<MotorCommand> actionQueue_;
    };
    
    // Social learning and interaction system
    class SocialLearningSystem {
    public:
        SocialLearningSystem() : observationProbability_(0.3f), imitationAccuracy_(0.7f),
                               socialLearningRate_(0.5f), trustUpdateRate_(0.1f) {}
        
        void observeBehavior(const std::string& agentId, const MotorCommand& action,
                            const std::vector<float>& outcome);
        MotorCommand imitateBehavior(const std::string& agentId) const;
        void updateSocialKnowledge(const std::string& otherAgent,
                                  const SocialKnowledge& knowledge);
        float getRelationshipStrength(const std::string& other) const;
        bool isTrustworthy(const std::string& otherAgent) const;
        
        void setObservationProbability(float prob) { observationProbability_ = prob; }
        void setImitationAccuracy(float accuracy) { imitationAccuracy_ = accuracy; }
        
    private:
        std::unordered_map<std::string, std::vector<MotorCommand>> behavioralObservations_;
        std::unordered_map<std::string, std::vector<float>> outcomeHistory_;
        SocialKnowledge socialKnowledge_;
        float observationProbability_;
        float imitationAccuracy_;
        float socialLearningRate_;
        float trustUpdateRate_;
    };
    
    // Predictive planning and decision making
    class PredictivePlanning {
    public:
        PredictivePlanning(std::shared_ptr<AgentBrain> agentBrain);
        ~PredictivePlanning() = default;
        
        void updateForwardModel(const std::vector<float>& state, 
                               const MotorCommand& action, float outcome);
        std::vector<float> predictFutureStates(const std::vector<float>& currentState,
                                               size_t steps) const;
        float evaluateActionPolicy(const MotorCommand& action,
                                  const std::vector<float>& currentState) const;
        MotorCommand selectActionWithPlanning(const std::vector<MotorCommand>& availableActions,
                                            const std::vector<float>& currentState);
        
        void setPlanningHorizon(size_t horizon) { planningHorizon_ = horizon; }
        void setExplorationRate(float rate) { explorationRate_ = rate; }
        
    private:
        std::shared_ptr<AgentBrain> agentBrain_;
        ForwardModel forwardModel_;
        size_t planningHorizon_;
        float explorationRate_;
        std::vector<float> valueFunction_;
    };
    
    // Public API for advanced agent features
    
    // Get enhanced novelty information
    MultiModalNovelty getEnhancedNovelty(const SensoryPercept& percept,
                                        const std::vector<float>& predictionErrors,
                                        double currentTime) const;
    
    // Skill management
    void addSkill(const Skill& skill) { skills_[skill.name] = skill; }
    bool useSkill(const std::string& skillName, float energyBudget);
    float getSkillProficiency(const std::string& skillName) const;
    std::vector<std::string> getAvailableSkills() const;
    
    // Experience management
    void recordExperience(const ExperienceRecord& experience);
    const std::vector<ExperienceRecord>& getExperienceHistory() const { return memorySystem_.getRecentExperiences(100); }
    
    // Social system access
    void addSocialContact(const std::string& agentId);
    void updateSocialKnowledge(const std::string& otherAgent,
                              const SocialKnowledge& knowledge);
    float getSocialTrust(const std::string& otherAgent) const;
    
    // Action selection with advanced features
    MotorCommand selectActionWithAdvancedFeatures(const MotorCommand& defaultAction,
                                                 const std::vector<MotorCommand>& availableActions,
                                                 const std::vector<float>& currentState);
    
    // State management
    void setBehavioralState(BehavioralState state) { behavioralState_ = state; }
    BehavioralState getBehavioralState() const { return behavioralState_; }
    
    // Configuration
    void setCuriosityWeight(float weight) { curiosityWeight_ = weight; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setExplorationBonus(float bonus) { explorationBonus_ = bonus; }
    
    // Statistics and monitoring
    struct AdvancedAgentStats {
        size_t totalSkills;
        size_t experiencesRecorded;
        float averageNovelty;
        float averageSuccessRate;
        float totalEnergyExpended;
        size_t socialInteractions;
        size_t planningStepsExecuted;
        
        AdvancedAgentStats() : totalSkills(0), experiencesRecorded(0), 
                              averageNovelty(0.0f), averageSuccessRate(0.0f),
                              totalEnergyExpended(0.0f), socialInteractions(0),
                              planningStepsExecuted(0) {}
    };
    
    AdvancedAgentStats getStatistics() const;
    
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    
    // Enhanced systems
    EnhancedCuriosity enhancedCuriosity_;
    MemorySystem memorySystem_;
    AdvancedMotorControl advancedMotorControl_;
    SocialLearningSystem socialLearningSystem_;
    PredictivePlanning predictivePlanning_;
    
    // Skills and knowledge
    std::unordered_map<std::string, Skill> skills_;
    
    // State and configuration
    BehavioralState behavioralState_;
    float curiosityWeight_;
    float learningRate_;
    float explorationBonus_;
    float lastUpdateTime_;
    
    // Performance tracking
    std::vector<float> noveltyHistory_;
    std::vector<float> successHistory_;
    std::vector<float> energyHistory_;
};

} // namespace nlm
