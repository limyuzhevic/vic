#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include <functional>

#include "AdvancedAgentLearning.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Social learning and collective intelligence
class SocialLearning {
public:
    SocialLearning(size_t bufferSize = 1000, float imitationWeight = 0.5f,
                  float peerInfluenceWeight = 0.3f, float collectiveWeight = 0.2f);
    ~SocialLearning();
    
    // Add expert demonstration for imitation learning
    void addExpertDemonstration(const std::string& expertId,
                               const std::vector<LearningState>& states,
                               const std::vector<Action>& actions,
                               const std::vector<float>& rewards);
    
    // Add peer experience for social reinforcement learning
    void addPeerExperience(const std::string& peerId,
                          const std::vector<LearningState>& states,
                          const std::vector<Action>& actions,
                          const std::vector<float>& rewards,
                          float influence = 1.0f);
    
    // Learn from expert demonstrations (imitation learning)
    void learnFromExperts(float learningRate = 0.1f, int epochs = 10);
    
    // Learn from peer experiences (social reinforcement learning)
    void learnFromPeers(float learningRate = 0.1f, float temperature = 0.1f);
    
    // Get collective intelligence (shared knowledge)
    std::vector<float> getCollectiveKnowledge(const std::vector<float>& state,
                                              const std::string& domain = "general");
    
    // Get action from expert
    Action getExpertAction(const std::vector<float>& state,
                           const std::string& expertId = "default");
    
    // Get action from peer (social policy)
    Action getPeerAction(const std::vector<float>& state,
                        const std::string& peerId);
    
    // Get aggregated social policy
    std::vector<float> getAggregatedSocialPolicy(const std::vector<float>& state,
                                                 const std::vector<std::string>& peerIds);
    
    // Enable/disable social learning components
    void enableImitationLearning(bool enabled) { imitationLearningEnabled_ = enabled; }
    void enableSocialReinforcementLearning(bool enabled) { socialRLLearningEnabled_ = enabled; }
    void enableCollectiveIntelligence(bool enabled) { collectiveIntelligenceEnabled_ = enabled; }
    
    // Get statistics
    struct Statistics {
        size_t expertDemonstrations;
        size_t peerExperiences;
        float imitationAccuracy;
        float socialCohesion;
        float collectiveKnowledgeQuality;
        
        Statistics() : expertDemonstrations(0), peerExperiences(0),
                      imitationAccuracy(0.0f), socialCohesion(0.0f),
                      collectiveKnowledgeQuality(0.0f) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
    // Reset social memory
    void reset();
    
private:
    size_t bufferSize_;
    float imitationWeight_;
    float peerInfluenceWeight_;
    float collectiveWeight_;
    
    // Expert demonstrations
    struct ExpertData {
        std::string expertId;
        std::vector<LearningState> states;
        std::vector<Action> actions;
        std::vector<float> rewards;
        float accuracy;
        size_t timestamp;
    };
    
    std::vector<ExpertData> expertData_;
    
    // Peer experiences
    struct PeerData {
        std::string peerId;
        std::vector<LearningState> states;
        std::vector<Action> actions;
        std::vector<float> rewards;
        float influence;
        float trustScore;
        size_t timestamp;
    };
    
    std::vector<PeerData> peerData_;
    
    // Collective intelligence
    std::vector<std::vector<float>> collectiveKnowledge_;
    std::unordered_map<std::string, std::vector<float>> domainKnowledge_;
    
    // Enable flags
    bool imitationLearningEnabled_;
    bool socialRLLearningEnabled_;
    bool collectiveIntelligenceEnabled_;
    
    // Statistics
    Statistics stats_;
    
    // Learning algorithms
    void daggerImitationLearning();
    void inverseReinforcementLearning();
    void policyDistillationFromPeers();
    void collectiveIntelligenceLearning();
    
    // Helper functions
    float computeExpertAccuracy(const ExpertData& expert);
    float computePeerSimilarity(const PeerData& peer1, const PeerData& peer2);
    float computeSocialCohesion(const std::vector<std::string>& peerIds);
    float computeImitationLoss(const std::vector<Action>& expertActions,
                              const std::vector<Action>& policyActions);
    
    // Random number generation
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
    std::normal_distribution<float> normalDist_;
};

// Meta-learning algorithms
class MetaLearning {
public:
    MetaLearning(size_t innerLR = 1, size_t outerLR = 0.1, size_t metaLR = 0.01,
                 size_t adaptationSteps = 5, size_t taskBatchSize = 20);
    ~MetaLearning();
    
    // MAML (Model-Agnostic Meta-Learning)
    void mamlUpdate(const std::vector<LearningTask>& tasks,
                   const std::vector<LearningState>& supportStates,
                   const std::vector<Action>& supportActions,
                   const std::vector<float>& supportRewards,
                   const std::vector<LearningState>& queryStates,
                   const std::vector<Action>& queryActions,
                   const std::vector<float>& queryRewards);
    
    // Reptile algorithm
    void reptileUpdate(const std::vector<LearningTask>& tasks,
                      const std::vector<LearningState>& supportStates,
                      const std::vector<Action>& supportActions,
                      const std::vector<float>& supportRewards,
                      const std::vector<LearningState>& queryStates,
                      const std::vector<Action>& queryActions,
                      const std::vector<float>& queryRewards);
    
    // AMAL (Adaptive MAML)
    void amelUpdate(const std::vector<LearningTask>& tasks,
                    const std::vector<LearningState>& fastStates,
                    const std::vector<Action>& fastActions,
                    const std::vector<float>& fastRewards,
                    const std::vector<LearningState>& metaStates,
                    const std::vector<Action>& metaActions,
                    const std::vector<float>& metaRewards);
    
    // Learn to learn (L2L) with recurrence
    void learnToLearnUpdate(const std::vector<LearningTask>& tasks,
                           const std::vector<LearningState>& sequenceStates,
                           const std::vector<Action>& sequenceActions,
                           const std::vector<float>& sequenceRewards);
    
    // Adapt parameters for new task
    std::vector<float> adaptToTask(const LearningTask& task,
                                   const std::vector<LearningState>& taskData);
    
    // Get meta-parameters
    const std::vector<float>& getMetaParameters() const { return metaParameters_; }
    
    // Set meta-parameters
    void setMetaParameters(const std::vector<float>& params) { metaParameters_ = params; }
    
    // Get statistics
    struct Statistics {
        float metaGradientNorm;
        float adaptationSuccessRate;
        float taskTransferEfficiency;
        size_t successfulAdaptations;
        
        Statistics() : metaGradientNorm(0.0f), adaptationSuccessRate(0.0f),
                      taskTransferEfficiency(0.0f), successfulAdaptations(0) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t innerLearningRate_;
    size_t outerLearningRate_;
    size_t metaLearningRate_;
    size_t adaptationSteps_;
    size_t taskBatchSize_;
    
    std::vector<float> metaParameters_;
    std::vector<float> fastWeights_;
    
    struct LearningTask {
        std::string taskId;
        std::string domain;
        size_t difficulty;
        float expectedPerformance;
    };
    
    // Loss and gradient computation
    float computeMetaLoss(const std::vector<float>& metaParams,
                        const std::vector<LearningState>& data,
                        const std::vector<float>& targets);
    
    std::vector<float> computeMetaGradient(const std::vector<float>& metaParams,
                                          const std::vector<LearningState>& data,
                                          const std::vector<float>& targets);
    
    // Task sampling
    LearningTask sampleTask(const std::vector<LearningTask>& availableTasks) const;
    
    // Adaptation evaluation
    float evaluateAdaptation(const std::vector<float>& adaptedParams,
                            const std::vector<LearningState>& testData,
                            const std::vector<float>& testTargets);
    
    // Statistics
    Statistics stats_;
};

// Neural attention and representation learning
class NeuralAttention {
public:
    NeuralAttention(size_t stateDim = 256, size_t numHeads = 8, size_t headDim = 32);
    ~NeuralAttention();
    
    // Initialize with networks
    void initialize(std::unique_ptr<LearningNetwork> embeddingNetwork,
                    std::unique_ptr<LearningNetwork> attentionNetwork,
                    std::unique_ptr<LearningNetwork> outputNetwork);
    
    // Compute attention weights
    std::vector<float> computeAttentionWeights(const std::vector<float>& query,
                                              const std::vector<float>& key,
                                              const std::vector<float>& value);
    
    // Multi-head attention
    std::vector<float> multiHeadAttention(const std::vector<float>& input);
    
    // Get attention representations
    std::vector<float> getAttentionRepresentation(const std::vector<float>& state);
    
    // Update attention parameters
    void update(const std::vector<float>& state, const std::vector<float>& action,
                const std::vector<float>& reward, float tdError);
    
    // Get statistics
    struct Statistics {
        float attentionEntropy;
        float attentionFocus;
        float crossAttentionScore;
        size_t updateCount;
        
        Statistics() : attentionEntropy(0.0f), attentionFocus(0.0f),
                      crossAttentionScore(0.0f), updateCount(0) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t stateDim_;
    size_t numHeads_;
    size_t headDim_;
    
    std::unique_ptr<LearningNetwork> embedding_;
    std::unique_ptr<LearningNetwork> attention_;
    std::unique_ptr<LearningNetwork> output_;
    
    // Attention statistics
    Statistics stats_;
    
    // Attention mechanisms
    std::vector<float> scaledDotProductAttention(const std::vector<float>& query,
                                                 const std::vector<float>& key,
                                                 const std::vector<float>& value);
    
    float computeAttentionEntropy(const std::vector<float>& attentionWeights);
    float computeAttentionFocus(const std::vector<float>& attentionWeights);
};

// Advanced reward shaping
class AdvancedRewardShaping {
public:
    AdvancedRewardShaping(float shapingWeight = 1.0f, float creditAssignmentGamma = 0.9f,
                         float spatialDiscountingFactor = 0.9f);
    ~AdvancedRewardShaping();
    
    // Apply potential-based reward shaping
    float applyPotentialShaping(float reward, const std::vector<float>& state,
                               const std::vector<float>& nextState,
                               const std::vector<float>& terminalStates);
    
    // Apply credit assignment across time and space
    std::vector<float> applyTemporalCreditAssignment(const std::vector<float>& rewards,
                                                     const std::vector<bool>& dones,
                                                     float gamma = 0.99f);
    
    std::vector<float> applySpatialCreditAssignment(const std::vector<float>& rewards,
                                                    const std::vector<std::vector<float>>& positions,
                                                    float discount = 0.9f);
    
    // Normalize and scale rewards
    float normalizeReward(float reward, const std::vector<float>& rewardHistory);
    
    // Handle sparse rewards with shaping functions
    float sparseRewardHandling(float reward, const std::vector<float>& state,
                              const std::vector<float>& goalState,
                              float goalDistanceThreshold = 1.0f);
    
    // Get reward shaping statistics
    struct Statistics {
        float shapingEffectiveness;
        float creditAssignmentAccuracy;
        float rewardNormalization;
        size_t shapingUpdates;
        
        Statistics() : shapingEffectiveness(0.0f), creditAssignmentAccuracy(0.0f),
                      rewardNormalization(0.0f), shapingUpdates(0) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    float shapingWeight_;
    float creditAssignmentGamma_;
    float spatialDiscountingFactor_;
    
    // Potential function for shaping
    float potentialFunction(const std::vector<float>& state);
    
    // Shaping statistics
    Statistics stats_;
};