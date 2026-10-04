#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include <numeric>
#include <functional>

#include "AdvancedAgentLearning.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Q-learning table and function approximation
class QLearning {
public:
    QLearning(size_t stateSpace, size_t actionSpace, float learningRate = 0.1f,
              float discountFactor = 0.99f, float epsilon = 0.1f);
    ~QLearning();
    
    // Action selection using epsilon-greedy or softmax policy
    int selectAction(const std::vector<float>& state, const std::string& policy = "epsilon_greedy");
    
    // Update Q-values
    void update(const std::vector<float>& state, int action, float reward,
                const std::vector<float>& nextState, bool done);
    
    // Get Q-value for state-action pair
    float getQValue(const std::vector<float>& state, int action) const;
    
    // Get best action for state
    int getBestAction(const std::vector<float>& state) const;
    
    // Get action probabilities (for exploration)
    std::vector<float> getActionProbabilities(const std::vector<float>& state,
                                             const std::string& policy = "softmax") const;
    
    // Reset learning
    void reset();
    
    // Save and load Q-table
    void save(const std::string& filename) const;
    void load(const std::string& filename);
    
    // Get statistics
    struct Statistics {
        float totalReward;
        size_t visitCount;
        float averageQValue;
        
        Statistics() : totalReward(0.0f), visitCount(0), averageQValue(0.0f) {}
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    size_t stateSpace_;
    size_t actionSpace_;
    float learningRate_;
    float discountFactor_;
    float epsilon_;
    
    // Q-table (state, action) -> value
    std::unordered_map<size_t, std::vector<float>> qTable_;
    
    // Statistics tracking
    Statistics stats_;
    
    // State hashing for discrete states
    size_t hashState(const std::vector<float>& state) const;
    
    // Random number generation
    std::mt19937 rng_;
};

// Actor-Critic methods (A2C, A3C, PPO)
class ActorCritic {
public:
    ActorCritic(size_t stateDim, size_t actionDim, size_t hiddenSize = 256);
    ~ActorCritic();
    
    // Initialize with neural networks
    void initialize(std::unique_ptr<LearningNetwork> actor, std::unique_ptr<LearningNetwork> critic);
    
    // Get action from actor network
    Action selectAction(const std::vector<float>& state, bool deterministic = false);
    
    // Get state value from critic network
    float getValue(const std::vector<float>& state);
    
    // Update actor and critic networks
    void update(const std::vector<LearningState>& states,
                const std::vector<Action>& actions,
                const std::vector<float>& returns,
                const std::vector<float>& advantages,
                float clipEpsilon = 0.2f, int epochs = 4);
    
    // Compute advantages using GAE
    std::vector<float> computeGeneralizedAdvantageEstimation(
        const std::vector<float>& rewards,
        const std::vector<float>& values,
        const std::vector<float>& dones,
        float gamma = 0.99f, float lambda = 0.95f) const;
    
    // Update with single trajectory (on-policy learning)
    void updateOnPolicy(const std::vector<LearningState>& states,
                       const std::vector<Action>& actions,
                       const std::vector<float>& rewards,
                       const std::vector<float>& dones);
    
    // Synchronize networks (for A3C)
    void synchronize(std::shared_ptr<ActorCritic> source);
    
    // Get network parameters
    const std::vector<std::vector<float>>& getActorWeights() const { return actor_->getWeights(); }
    const std::vector<std::vector<float>>& getCriticWeights() const { return critic_->getWeights(); }
    
private:
    std::unique_ptr<LearningNetwork> actor_;
    std::unique_ptr<LearningNetwork> critic_;
    
    // Policy gradient computation
    std::vector<float> computePolicyGradient(const std::vector<float>& logProbs,
                                            const std::vector<float>& advantages);
    
    // Value function gradient
    std::vector<float> computeValueGradient(const std::vector<float>& targets,
                                           const std::vector<float>& values);
    
    // Clipped surrogate objective (PPO)
    float computePPOObjective(const std::vector<float>& oldLogProbs,
                            const std::vector<float>& newLogProbs,
                            const std::vector<float>& advantages,
                            float clipEpsilon);
};

// Meta-learning (Learning to learn)
class MetaLearning {
public:
    MetaLearning(size_t innerLR, size_t outerLR, size_t metaLR,
                 size_t adaptationSteps = 5);
    ~MetaLearning();
    
    // MAML (Model-Agnostic Meta-Learning)
    void mamlUpdate(const std::vector<LearningState>& supportStates,
                   const std::vector<Action>& supportActions,
                   const std::vector<float>& supportRewards,
                   const std::vector<LearningState>& queryStates,
                   const std::vector<Action>& queryActions,
                   const std::vector<float>& queryRewards);
    
    // Reptile algorithm
    void reptileUpdate(const std::vector<LearningState>& supportStates,
                      const std::vector<Action>& supportActions,
                      const std::vector<float>& supportRewards,
                      const std::vector<LearningState>& queryStates,
                      const std::vector<Action>& queryActions,
                      const std::vector<float>& queryRewards);
    
    // Fast weights meta-learning
    void fastWeightsUpdate(const std::vector<LearningState>& fastStates,
                          const std::vector<Action>& fastActions,
                          const std::vector<float>& fastRewards,
                          const std::vector<LearningState>& metaStates,
                          const std::vector<Action>& metaActions,
                          const std::vector<float>& metaRewards);
    
    // Adapt parameters for new task
    std::vector<float> adaptParameters(const std::vector<float>& taskContext,
                                       const std::vector<LearningState>& taskData);
    
    // Get meta-parameters
    const std::vector<float>& getMetaParameters() const { return metaParameters_; }
    
    // Set meta-parameters
    void setMetaParameters(const std::vector<float>& params) { metaParameters_ = params; }
    
private:
    size_t innerLearningRate_;
    size_t outerLearningRate_;
    size_t metaLearningRate_;
    size_t adaptationSteps_;
    
    std::vector<float> metaParameters_;
    
    // Fast weights for inner loop
    std::vector<float> fastWeights_;
    
    // Loss computation for meta-gradients
    float computeMetaLoss(const std::vector<float>& metaParams,
                        const std::vector<LearningState>& data);
    
    // Gradient computation
    std::vector<float> computeMetaGradient(const std::vector<float>& metaParams,
                                          const std::vector<LearningState>& data);
};

// Social learning and imitation
class SocialLearning {
public:
    SocialLearning(size_t bufferSize = 1000, float imitationWeight = 0.5f);
    ~SocialLearning();
    
    // Add expert demonstration
    void addExpertDemonstration(const std::vector<LearningState>& states,
                               const std::vector<Action>& actions,
                               const std::vector<float>& rewards);
    
    // Add peer experience
    void addPeerExperience(const std::vector<LearningState>& states,
                          const std::vector<Action>& actions,
                          const std::vector<float>& rewards,
                          const std::string& peerId);
    
    // Learn from expert demonstrations
    void learnFromDemonstrations(float learningRate = 0.1f);
    
    // Learn from peer experiences (social reinforcement learning)
    void learnFromPeers(float learningRate = 0.1f, float peerInfluence = 0.3f);
    
    // Get imitation policy
    Action getImitationAction(const std::vector<float>& state,
                              const std::string& expertId = "default");
    
    // Get peer policy
    Action getPeerAction(const std::vector<float>& state,
                        const std::string& peerId);
    
    // Get collective intelligence (shared knowledge)
    std::vector<float> getCollectiveKnowledge(const std::vector<float>& state);
    
    // Reset social memory
    void reset();
    
private:
    size_t bufferSize_;
    float imitationWeight_;
    
    // Expert demonstrations
    std::vector<std::vector<LearningState>> expertStates_;
    std::vector<std::vector<Action>> expertActions_;
    std::vector<std::vector<float>> expertRewards_;
    std::unordered_map<std::string, size_t> expertCounts_;
    
    // Peer experiences
    struct PeerExperience {
        std::string peerId;
        std::vector<LearningState> states;
        std::vector<Action> actions;
        std::vector<float> rewards;
        float influence;
        size_t timestamp;
    };
    
    std::vector<PeerExperience> peerExperiences_;
    
    // Collective knowledge
    std::vector<std::vector<float>> collectiveKnowledge_;
    
    // Learning algorithms
    void daggerLearning();
    void reciprocalInferenceLearning();
    void policyDistillation();
};