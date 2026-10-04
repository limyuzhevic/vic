#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

SocialLearning::SocialLearning(size_t bufferSize, float imitationWeight,
                              float peerInfluenceWeight, float collectiveWeight)
    : bufferSize_(bufferSize), imitationWeight_(imitationWeight),
      peerInfluenceWeight_(peerInfluenceWeight), collectiveWeight_(collectiveWeight),
      imitationLearningEnabled_(true), socialRLLearningEnabled_(true),
      collectiveIntelligenceEnabled_(true) {
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist_ = std::normal_distribution<float>(0.0f, 1.0f);
    
    NLM_LOG_INFO("SocialLearning initialized: buffer_size=" + std::to_string(bufferSize_) +
                " imitation_weight=" + std::to_string(imitationWeight_) +
                " peer_influence_weight=" + std::to_string(peerInfluenceWeight_));
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::addExpertDemonstration(const std::string& expertId,
                                           const std::vector<LearningState>& states,
                                           const std::vector<Action>& actions,
                                           const std::vector<float>& rewards) {
    if (states.size() != actions.size() || states.size() != rewards.size()) {
        NLM_LOG_ERROR("SocialLearning::addExpertDemonstration: Input sizes mismatch");
        return;
    }
    
    ExpertData expert;
    expert.expertId = expertId;
    expert.states = states;
    expert.actions = actions;
    expert.rewards = rewards;
    expert.accuracy = 0.0f; // Will be computed during learning
    auto now = std::chrono::steady_clock::now();
    expert.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    expertData_.push_back(expert);
    stats_.expertDemonstrations++;
    
    // Maintain buffer size
    if (expertData_.size() > bufferSize_) {
        expertData_.erase(expertData_.begin());
    }
    
    NLM_LOG_INFO("Added expert demonstration from " + expertId + ": total=" + std::to_string(expertData_.size()));
}

void SocialLearning::addPeerExperience(const std::string& peerId,
                                      const std::vector<LearningState>& states,
                                      const std::vector<Action>& actions,
                                      const std::vector<float>& rewards,
                                      float influence) {
    if (states.size() != actions.size() || states.size() != rewards.size()) {
        NLM_LOG_ERROR("SocialLearning::addPeerExperience: Input sizes mismatch");
        return;
    }
    
    PeerData peer;
    peer.peerId = peerId;
    peer.states = states;
    peer.actions = actions;
    peer.rewards = rewards;
    peer.influence = influence;
    peer.trustScore = 1.0f; // Would be updated based on performance
    auto now = std::chrono::steady_clock::now();
    peer.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    peerData_.push_back(peer);
    stats_.peerExperiences++;
    
    // Maintain buffer size
    if (peerData_.size() > bufferSize_) {
        peerData_.erase(peerData_.begin());
    }
    
    NLM_LOG_INFO("Added peer experience from " + peerId + ": total=" + std::to_string(peerData_.size()));
}

void SocialLearning::learnFromExperts(float learningRate, int epochs) {
    if (!imitationLearningEnabled_ || expertData_.empty()) {
        return;
    }
    
    NLM_LOG_INFO("Learning from " + std::to_string(expertData_.size()) + " expert demonstrations");
    
    // Aggregate expert knowledge
    std::vector<Action> aggregatedActions;
    std::vector<float> aggregatedConfidences;
    
    for (const auto& expert : expertData_) {
        // Compute expert accuracy (simplified)
        float accuracy = computeExpertAccuracy(expert);
        
        // Weight actions by confidence
        for (size_t i = 0; i < expert.actions.size() && i < expert.states.size(); ++i) {
            if (i >= aggregatedActions.size()) {
                aggregatedActions.push_back(expert.actions[i]);
                aggregatedConfidences.push_back(accuracy);
            } else {
                aggregatedConfidences[i] = (aggregatedConfidences[i] + accuracy) / 2.0f;
            }
        }
    }
    
    // DAgGER-style imitation learning
    daggerImitationLearning();
    
    // Update statistics
    float totalAccuracy = 0.0f;
    for (const auto& expert : expertData_) {
        totalAccuracy += computeExpertAccuracy(expert);
    }
    stats_.imitationAccuracy = expertData_.empty() ? 0.0f : totalAccuracy / expertData_.size();
}

void SocialLearning::learnFromPeers(float learningRate, float temperature) {
    if (!socialRLLearningEnabled_ || peerData_.empty()) {
        return;
    }
    
    NLM_LOG_INFO("Learning from " + std::to_string(peerData_.size()) + " peer experiences");
    
    // Policy distillation from peers
    policyDistillationFromPeers();
    
    // Social reinforcement learning
    inverseReinforcementLearning();
    
    // Update social cohesion
    std::vector<std::string> peerIds;
    for (const auto& peer : peerData_) {
        peerIds.push_back(peer.peerId);
    }
    
    stats_.socialCohesion = computeSocialCohesion(peerIds);
}

std::vector<float> SocialLearning::getCollectiveKnowledge(const std::vector<float>& state,
                                                          const std::string& domain) {
    std::vector<float> collectiveKnowledge;
    
    if (!collectiveIntelligenceEnabled_) {
        return collectiveKnowledge;
    }
    
    // Aggregate expert demonstrations
    for (const auto& expert : expertData_) {
        if (!expert.states.empty()) {
            const auto& demoState = expert.states[0].state;
            size_t minSize = std::min(demoState.size(), state.size());
            
            for (size_t i = 0; i < minSize; ++i) {
                collectiveKnowledge.push_back(demoState[i]);
            }
        }
    }
    
    // Aggregate peer experiences
    for (const auto& peer : peerData_) {
        if (!peer.states.empty()) {
            const auto& demoState = peer.states[0].state;
            size_t minSize = std::min(demoState.size(), state.size());
            
            for (size_t i = 0; i < minSize; ++i) {
                collectiveKnowledge.push_back(demoState[i]);
            }
        }
    }
    
    // Normalize collective knowledge
    if (!collectiveKnowledge.empty()) {
        float sum = std::accumulate(collectiveKnowledge.begin(), collectiveKnowledge.end(), 0.0f);
        for (float& val : collectiveKnowledge) {
            val /= sum;
        }
    }
    
    // Store domain-specific knowledge
    domainKnowledge_[domain] = collectiveKnowledge;
    
    // Compute knowledge quality
    stats_.collectiveKnowledgeQuality = collectiveKnowledge.empty() ? 0.0f : 1.0f - computeNovelty(collectiveKnowledge);
    
    return collectiveKnowledge;
}

Action SocialLearning::getExpertAction(const std::vector<float>& state,
                                       const std::string& expertId) {
    Action action;
    
    for (const auto& expert : expertData_) {
        if (expert.expertId == expertId && !expert.states.empty()) {
            // Find most similar state
            size_t bestMatch = 0;
            float bestSimilarity = -1.0f;
            
            for (size_t i = 0; i < expert.states.size(); ++i) {
                float similarity = 0.0f;
                const auto& demoState = expert.states[i].state;
                size_t minSize = std::min(demoState.size(), state.size());
                
                for (size_t j = 0; j < minSize; ++j) {
                    similarity += 1.0f - std::abs(state[j] - demoState[j]);
                }
                
                similarity /= minSize;
                
                if (similarity > bestSimilarity) {
                    bestSimilarity = similarity;
                    bestMatch = i;
                }
            }
            
            if (expert.actions.size() > bestMatch) {
                action = expert.actions[bestMatch];
                break;
            }
        }
    }
    
    return action;
}

Action SocialLearning::getPeerAction(const std::vector<float>& state,
                                     const std::string& peerId) {
    Action action;
    
    for (const auto& peer : peerData_) {
        if (peer.peerId == peerId && !peer.states.empty()) {
            // Find best matching state-action pair
            size_t bestMatch = 0;
            float bestSimilarity = -1.0f;
            
            for (size_t i = 0; i < peer.states.size(); ++i) {
                float similarity = 0.0f;
                const auto& demoState = peer.states[i].state;
                size_t minSize = std::min(demoState.size(), state.size());
                
                for (size_t j = 0; j < minSize; ++j) {
                    similarity += 1.0f - std::abs(state[j] - demoState[j]);
                }
                
                similarity /= minSize;
                
                if (similarity > bestSimilarity) {
                    bestSimilarity = similarity;
                    bestMatch = i;
                }
            }
            
            if (peer.actions.size() > bestMatch) {
                action = peer.actions[bestMatch];
                break;
            }
        }
    }
    
    return action;
}

std::vector<float> SocialLearning::getAggregatedSocialPolicy(const std::vector<float>& state,
                                                             const std::vector<std::string>& peerIds) {
    std::vector<float> aggregatedPolicy(peerIds.size(), 0.0f);
    
    for (size_t i = 0; i < peerIds.size(); ++i) {
        Action peerAction = getPeerAction(state, peerIds[i]);
        aggregatedPolicy[i] = peerAction.probability;
    }
    
    // Average the policies
    float sum = std::accumulate(aggregatedPolicy.begin(), aggregatedPolicy.end(), 0.0f);
    if (sum > 0.0f) {
        for (float& val : aggregatedPolicy) {
            val /= sum;
        }
    }
    
    return aggregatedPolicy;
}

void SocialLearning::reset() {
    expertData_.clear();
    peerData_.clear();
    collectiveKnowledge_.clear();
    domainKnowledge_.clear();
    
    stats_ = Statistics();
    
    NLM_LOG_INFO("SocialLearning reset");
}

void SocialLearning::daggerImitationLearning() {
    NLM_LOG_INFO("Executing DAgGER imitation learning");
    
    // Simplified DAgGER implementation
    // In practice, this would involve:
    // 1. Running expert policy to collect demonstrations
    // 2. Training imitation policy on demonstrations
    // 3. Running imitation policy to collect more data
    // 4. Repeating until convergence
    
    // For now, simulate learning
    for (auto& expert : expertData_) {
        expert.accuracy = std::max(0.5f, expert.accuracy + 0.1f);
    }
}

void SocialLearning::inverseReinforcementLearning() {
    NLM_LOG_INFO("Executing Inverse Reinforcement Learning");
    
    // Simplified IRL implementation
    // Infer reward function from observed behavior
    
    // For now, simulate IRL
    float totalTrust = 0.0f;
    for (auto& peer : peerData_) {
        peer.trustScore = std::min(1.0f, peer.trustScore + 0.05f);
        totalTrust += peer.trustScore;
    }
}

void SocialLearning::policyDistillationFromPeers() {
    NLM_LOG_INFO("Executing Policy Distillation from peers");
    
    // Aggregate peer knowledge into a single policy
    if (peerData_.empty()) {
        return;
    }
    
    // Create distilled policy
    std::vector<Action> distilledActions;
    std::vector<float> distilledConfidences;
    
    // Compute average policy across peers
    std::vector<std::vector<float>> peerPolicies(peerData_.size());
    for (size_t i = 0; i < peerData_.size(); ++i) {
        peerPolicies[i].resize(peerData_[i].actions.size(), 0.0f);
    }
    
    // In practice, this would compute actual policy probabilities
    // For simplicity, just set to uniform
    for (size_t i = 0; i < peerPolicies.size(); ++i) {
        float confidence = peerData_[i].trustScore * peerData_[i].influence;
        for (size_t j = 0; j < peerPolicies[i].size(); ++j) {
            peerPolicies[i][j] = confidence / peerPolicies[i].size();
        }
    }
}

void SocialLearning::collectiveIntelligenceLearning() {
    NLM_LOG_INFO("Executing Collective Intelligence Learning");
    
    // Learn from the collective intelligence of all agents
    std::vector<std::vector<float>> allKnowledge;
    
    // Collect knowledge from experts and peers
    for (const auto& expert : expertData_) {
        if (!expert.states.empty()) {
            allKnowledge.push_back(expert.states[0].state);
        }
    }
    
    for (const auto& peer : peerData_) {
        if (!peer.states.empty()) {
            allKnowledge.push_back(peer.states[0].state);
        }
    }
    
    if (!allKnowledge.empty()) {
        collectiveKnowledge_ = allKnowledge;
    }
}

float SocialLearning::computeExpertAccuracy(const ExpertData& expert) {
    if (expert.states.empty() || expert.actions.empty()) {
        return 0.0f;
    }
    
    // Simplified accuracy computation
    float totalAccuracy = 0.0f;
    for (size_t i = 0; i < expert.states.size(); ++i) {
        // In practice, would compare predicted vs actual actions
        totalAccuracy += 0.8f;  // Simulated accuracy
    }
    
    return totalAccuracy / expert.states.size();
}

float SocialLearning::computePeerSimilarity(const PeerData& peer1, const PeerData& peer2) {
    float similarity = 0.0f;
    
    size_t minStates = std::min(peer1.states.size(), peer2.states.size());
    if (minStates == 0) {
        return 0.0f;
    }
    
    for (size_t i = 0; i < minStates; ++i) {
        float stateSimilarity = 0.0f;
        const auto& state1 = peer1.states[i].state;
        const auto& state2 = peer2.states[i].state;
        size_t minSize = std::min(state1.size(), state2.size());
        
        for (size_t j = 0; j < minSize; ++j) {
            stateSimilarity += 1.0f - std::abs(state1[j] - state2[j]);
        }
        
        stateSimilarity /= minSize;
        similarity += stateSimilarity;
    }
    
    return similarity / minStates;
}

float SocialLearning::computeSocialCohesion(const std::vector<std::string>& peerIds) {
    if (peerIds.size() <= 1) {
        return 1.0f;
    }
    
    // Compute pairwise similarities
    float totalSimilarity = 0.0f;
    size_t pairCount = 0;
    
    for (size_t i = 0; i < peerIds.size(); ++i) {
        for (size_t j = i + 1; j < peerIds.size(); ++j) {
            PeerData* peer1 = nullptr;
            PeerData* peer2 = nullptr;
            
            for (auto& peer : peerData_) {
                if (peer.peerId == peerIds[i]) peer1 = &peer;
                if (peer.peerId == peerIds[j]) peer2 = &peer;
            }
            
            if (peer1 && peer2) {
                totalSimilarity += computePeerSimilarity(*peer1, *peer2);
                pairCount++;
            }
        }
    }
    
    if (pairCount == 0) return 0.0f;
    
    return totalSimilarity / pairCount;
}

float SocialLearning::computeImitationLoss(const std::vector<Action>& expertActions,
                                          const std::vector<Action>& policyActions) {
    if (expertActions.size() != policyActions.size() || expertActions.empty()) {
        return 1.0f; // Maximum loss
    }
    
    float totalLoss = 0.0f;
    for (size_t i = 0; i < expertActions.size(); ++i) {
        // Compute loss based on action difference
        float actionDiff = std::abs(expertActions[i].actionId - policyActions[i].actionId);
        totalLoss += actionDiff / (float)std::max(1, (int)expertActions.size());
    }
    
    return totalLoss / expertActions.size();
}

float SocialLearning::computeNovelty(const std::vector<float>& state) {
    // Simplified novelty computation based on state variance
    if (state.empty()) return 0.0f;
    
    float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
    float variance = 0.0f;
    
    for (float val : state) {
        variance += std::pow(val - mean, 2);
    }
    variance /= state.size();
    
    return std::sqrt(variance);
}

// Meta-learning implementation
MetaLearning::MetaLearning(size_t innerLR, size_t outerLR, size_t metaLR,
                         size_t adaptationSteps, size_t taskBatchSize)
    : innerLearningRate_(innerLR), outerLearningRate_(outerLR),
      metaLearningRate_(metaLR), adaptationSteps_(adaptationSteps),
      taskBatchSize_(taskBatchSize) {
    
    metaParameters_.resize(10, 0.01f);
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist_ = std::normal_distribution<float>(0.0f, 1.0f);
    
    NLM_LOG_INFO("MetaLearning initialized: inner_lr=" + std::to_string(innerLearningRate_) +
                " outer_lr=" + std::to_string(outerLearningRate_));
}

MetaLearning::~MetaLearning() = default;

void MetaLearning::mamlUpdate(const std::vector<LearningTask>& tasks,
                             const std::vector<LearningState>& supportStates,
                             const std::vector<Action>& supportActions,
                             const std::vector<float>& supportRewards,
                             const std::vector<LearningState>& queryStates,
                             const std::vector<Action>& queryActions,
                             const std::vector<float>& queryRewards) {
    NLM_LOG_INFO("Executing MAML update");
    
    std::vector<float> originalParams = metaParameters_;
    std::vector<float> adaptedParams = originalParams;
    
    // Inner loop: adapt to support set
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < adaptedParams.size(); ++i) {
            float gradient = computeMetaLoss(adaptedParams, supportStates, supportRewards);
            adaptedParams[i] -= innerLearningRate_ * gradient;
        }
    }
    
    // Meta-gradient computation
    std::vector<float> metaGrad = computeMetaGradient(adaptedParams, queryStates, queryRewards);
    
    // Update meta-parameters
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        metaParameters_[i] -= metaLearningRate_ * metaGrad[i];
    }
    
    // Reset to original parameters (MAML's key insight)
    metaParameters_ = originalParams;
    
    stats_.successfulAdaptations++;
}

void MetaLearning::reptileUpdate(const std::vector<LearningTask>& tasks,
                                const std::vector<LearningState>& supportStates,
                                const std::vector<Action>& supportActions,
                                const std::vector<float>& supportRewards,
                                const std::vector<LearningState>& queryStates,
                                const std::vector<Action>& queryActions,
                                const std::vector<float>& queryRewards) {
    NLM_LOG_INFO("Executing Reptile update");
    
    std::vector<float> initialParams = metaParameters_;
    std::vector<float> adaptedParams = initialParams;
    
    // Inner loop adaptation
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < adaptedParams.size(); ++i) {
            float gradient = computeMetaLoss(adaptedParams, supportStates, supportRewards);
            adaptedParams[i] -= innerLearningRate_ * gradient;
        }
    }
    
    // Meta-update using parameter difference
    float totalDiff = 0.0f;
    for (size_t i = 0; i < adaptedParams.size(); ++i) {
        totalDiff += std::pow(adaptedParams[i] - initialParams[i], 2);
    }
    totalDiff = std::sqrt(totalDiff);
    
    if (totalDiff > 0.0f) {
        for (size_t i = 0; i < metaParameters_.size(); ++i) {
            metaParameters_[i] += outerLearningRate_ * (adaptedParams[i] - initialParams[i]) / totalDiff;
        }
    }
}

void MetaLearning::amelUpdate(const std::vector<LearningTask>& tasks,
                             const std::vector<LearningState>& fastStates,
                             const std::vector<Action>& fastActions,
                             const std::vector<float>& fastRewards,
                             const std::vector<LearningState>& metaStates,
                             const std::vector<Action>& metaActions,
                             const std::vector<float>& metaRewards) {
    NLM_LOG_INFO("Executing AMAL update");
    
    // Adaptive MAML with dynamic learning rates
    fastWeights_ = metaParameters_;
    
    // Fast adaptation with adaptive learning rates
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < fastWeights_.size(); ++i) {
            float gradient = computeMetaLoss(fastWeights_, fastStates, fastRewards);
            
            // Adaptive learning rate based on gradient magnitude
            float adaptiveLR = innerLearningRate_ / (std::abs(gradient) + 1e-8f);
            fastWeights_[i] -= adaptiveLR * gradient;
        }
    }
    
    // Meta-gradient with adaptive scaling
    std::vector<float> metaGrad = computeMetaGradient(fastWeights_, metaStates, metaRewards);
    
    // Scale gradients based on task complexity
    float gradientNorm = 0.0f;
    for (float grad : metaGrad) {
        gradientNorm += grad * grad;
    }
    gradientNorm = std::sqrt(gradientNorm);
    
    float scalingFactor = std::min(1.0f, 1.0f / (gradientNorm + 1e-8f));
    
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        metaParameters_[i] -= metaLearningRate_ * scalingFactor * metaGrad[i];
    }
}

void MetaLearning::learnToLearnUpdate(const std::vector<LearningTask>& tasks,
                                     const std::vector<LearningState>& sequenceStates,
                                     const std::vector<Action>& sequenceActions,
                                     const std::vector<float>& sequenceRewards) {
    NLM_LOG_INFO("Executing Learn-to-Learn update with recurrence");
    
    // L2L with recurrence through time
    std::vector<float> losses;
    for (size_t i = 0; i < sequenceStates.size(); ++i) {
        float loss = computeMetaLoss(metaParameters_, sequenceStates, sequenceRewards);
        losses.push_back(loss);
    }
    
    // Backpropagate through time (simplified BPTT)
    for (int t = sequenceStates.size() - 1; t > 0; --t) {
        for (size_t i = 0; i < metaParameters_.size(); ++i) {
            // Gradient contribution from future time steps
            float futureGrad = 0.0f;
            for (size_t j = t; j < sequenceStates.size(); ++j) {
                futureGrad += losses[j];
            }
            metaParameters_[i] -= metaLearningRate_ * futureGrad * 0.01f;
        }
    }
}

std::vector<float> MetaLearning::adaptToTask(const LearningTask& task,
                                            const std::vector<LearningState>& taskData) {
    std::vector<float> adaptedParams = metaParameters_;
    
    // Adapt based on task characteristics
    float taskComplexity = 1.0f + task.difficulty * 0.1f;
    float taskTransferability = 1.0f - task.expectedPerformance * 0.5f;
    
    for (size_t i = 0; i < adaptedParams.size(); ++i) {
        // Scale adaptation based on task properties
        adaptedParams[i] *= taskComplexity * taskTransferability;
    }
    
    // Perform inner loop adaptation
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < adaptedParams.size(); ++i) {
            float gradient = computeMetaLoss(adaptedParams, taskData, std::vector<float>(taskData.size(), 1.0f));
            adaptedParams[i] -= innerLearningRate_ * gradient;
        }
    }
    
    return adaptedParams;
}

float MetaLearning::computeMetaLoss(const std::vector<float>& metaParams,
                                   const std::vector<LearningState>& data,
                                   const std::vector<float>& targets) {
    if (data.empty()) return 0.0f;
    
    float totalLoss = 0.0f;
    for (size_t i = 0; i < data.size(); ++i) {
        // Simulate forward pass with metaParams
        float prediction = 0.0f;
        for (float val : data[i].state) {
            prediction += val * metaParams[i % metaParams.size()];
        }
        
        float target = (i < targets.size()) ? targets[i] : 0.0f;
        totalLoss += std::pow(prediction - target, 2);
    }
    
    return totalLoss / data.size();
}

std::vector<float> MetaLearning::computeMetaGradient(const std::vector<float>& metaParams,
                                                     const std::vector<LearningState>& data,
                                                     const std::vector<float>& targets) {
    std::vector<float> gradient(metaParams.size(), 0.0f);
    
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < metaParams.size(); ++j) {
            float prediction = 0.0f;
            for (float val : data[i].state) {
                prediction += val * metaParams[j % metaParams.size()];
            }
            
            float target = (i < targets.size()) ? targets[i] : 0.0f;
            float error = prediction - target;
            gradient[j] += 2.0f * error * data[i].state[j % data[i].state.size()];
        }
    }
    
    for (size_t j = 0; j < gradient.size(); ++j) {
        gradient[j] /= data.size();
    }
    
    return gradient;
}

MetaLearning::LearningTask MetaLearning::sampleTask(const std::vector<LearningTask>& availableTasks) const {
    if (availableTasks.empty()) {
        return LearningTask{ "default", "general", 1, 0.5f };
    }
    
    // Sample task based on difficulty and expected performance
    std::vector<float> weights;
    for (const auto& task : availableTasks) {
        float weight = task.expectedPerformance * (1.0f / (task.difficulty + 1.0f));
        weights.push_back(weight);
    }
    
    float totalWeight = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (totalWeight == 0.0f) {
        return availableTasks[uniformDist_(rng_) * availableTasks.size()];
    }
    
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    size_t index = dist(rng_);
    
    return availableTasks[index];
}

float MetaLearning::evaluateAdaptation(const std::vector<float>& adaptedParams,
                                       const std::vector<LearningState>& testData,
                                       const std::vector<float>& testTargets) {
    float totalError = 0.0f;
    
    for (size_t i = 0; i < testData.size(); ++i) {
        float prediction = 0.0f;
        for (float val : testData[i].state) {
            prediction += val * adaptedParams[i % adaptedParams.size()];
        }
        
        float target = (i < testTargets.size()) ? testTargets[i] : 0.0f;
        totalError += std::pow(prediction - target, 2);
    }
    
    return std::sqrt(totalError / testData.size());
}

// NeuralAttention implementation
NeuralAttention::NeuralAttention(size_t stateDim, size_t numHeads, size_t headDim)
    : stateDim_(stateDim), numHeads_(numHeads), headDim_(headDim) {
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist_ = std::normal_distribution<float>(0.0f, 1.0f);
    
    NLM_LOG_INFO("NeuralAttention initialized: state_dim=" + std::to_string(stateDim_) +
                " num_heads=" + std::to_string(numHeads_));
}

NeuralAttention::~NeuralAttention() = default;

void NeuralAttention::initialize(std::unique_ptr<LearningNetwork> embeddingNetwork,
                                std::unique_ptr<LearningNetwork> attentionNetwork,
                                std::unique_ptr<LearningNetwork> outputNetwork) {
    embedding_ = std::move(embeddingNetwork);
    attention_ = std::move(attentionNetwork);
    output_ = std::move(outputNetwork);
    
    NLM_LOG_INFO("NeuralAttention networks initialized");
}

std::vector<float> NeuralAttention::computeAttentionWeights(const std::vector<float>& query,
                                                          const std::vector<float>& key,
                                                          const std::vector<float>& value) {
    // Simplified attention computation
    std::vector<float> attentionWeights;
    
    if (query.empty() || key.empty() || value.empty()) {
        return attentionWeights;
    }
    
    // Scaled dot-product attention
    float scale = std::sqrt(static_cast<float>(std::min(query.size(), key.size())));
    float scores = 0.0f;
    
    size_t minSize = std::min(query.size(), key.size());
    for (size_t i = 0; i < minSize; ++i) {
        scores += query[i] * key[i] / scale;
    }
    
    float attention = std::exp(scores) / (1.0f + std::exp(scores));
    attentionWeights.push_back(attention);
    
    return attentionWeights;
}

std::vector<float> NeuralAttention::multiHeadAttention(const std::vector<float>& input) {
    std::vector<float> output;
    
    if (!embedding_ || !attention_ || !output_) {
        return output;
    }
    
    // Simplified multi-head attention
    std::vector<float> embedding = embedding_->forward(input);
    std::vector<float> attention = attention_->forward(embedding);
    output = output_->forward(attention);
    
    // Update statistics
    stats_.attentionEntropy = computeAttentionEntropy(attention);
    stats_.attentionFocus = computeAttentionFocus(attention);
    
    return output;
}

std::vector<float> NeuralAttention::getAttentionRepresentation(const std::vector<float>& state) {
    std::vector<float> representation;
    
    if (embedding_ && attention_) {
        std::vector<float> embedding = embedding_->forward(state);
        representation = attention_->forward(embedding);
    }
    
    return representation;
}

void NeuralAttention::update(const std::vector<float>& state, const std::vector<float>& action,
                            const std::vector<float>& reward, float tdError) {
    // Update attention parameters based on experience
    stats_.updateCount++;
    
    // Update attention statistics
    stats_.crossAttentionScore = std::min(1.0f, stats_.crossAttentionScore + 0.1f * tdError);
}

float NeuralAttention::computeAttentionEntropy(const std::vector<float>& attentionWeights) {
    if (attentionWeights.empty()) return 0.0f;
    
    float entropy = 0.0f;
    for (float weight : attentionWeights) {
        if (weight > 0.0f) {
            entropy -= weight * std::log(weight + 1e-8f);
        }
    }
    
    return entropy;
}

float NeuralAttention::computeAttentionFocus(const std::vector<float>& attentionWeights) {
    if (attentionWeights.empty()) return 0.0f;
    
    float maxWeight = *std::max_element(attentionWeights.begin(), attentionWeights.end());
    float sum = std::accumulate(attentionWeights.begin(), attentionWeights.end(), 0.0f);
    
    if (sum == 0.0f) return 0.0f;
    
    return maxWeight / sum;  // Focus = max weight / sum
}

// Advanced reward shaping implementation
AdvancedRewardShaping::AdvancedRewardShaping(float shapingWeight, float creditAssignmentGamma,
                                           float spatialDiscountingFactor)
    : shapingWeight_(shapingWeight), creditAssignmentGamma_(creditAssignmentGamma),
      spatialDiscountingFactor_(spatialDiscountingFactor) {
    
    NLM_LOG_INFO("AdvancedRewardShaping initialized: shaping_weight=" + std::to_string(shapingWeight_));
}

AdvancedRewardShaping::~AdvancedRewardShaping() = default;

float AdvancedRewardShaping::applyPotentialShaping(float reward, const std::vector<float>& state,
                                                  const std::vector<float>& nextState,
                                                  const std::vector<float>& terminalStates) {
    float shapedReward = reward;
    
    if (shapingWeight_ > 0.0f) {
        float currentPotential = potentialFunction(state);
        float nextPotential = potentialFunction(nextState);
        
        // Potential-based reward shaping
        shapedReward += shapingWeight_ * (nextPotential - currentPotential);
        
        // Terminal state bonus
        bool isTerminal = false;
        for (float terminal : terminalStates) {
            if (terminal > 0.5f) {  // Simplified terminal check
                isTerminal = true;
                break;
            }
        }
        
        if (isTerminal) {
            shapedReward += 10.0f * shapingWeight_;  // Terminal bonus
        }
    }
    
    stats_.shapingUpdates++;
    return shapedReward;
}

std::vector<float> AdvancedRewardShaping::applyTemporalCreditAssignment(const std::vector<float>& rewards,
                                                                        const std::vector<bool>& dones,
                                                                        float gamma) {
    std::vector<float> credits(rewards.size());
    
    if (rewards.empty()) return credits;
    
    float cumulativeCredit = 0.0f;
    
    // Backwards credit assignment
    for (int t = rewards.size() - 1; t >= 0; --t) {
        if (dones[t]) {
            cumulativeCredit = rewards[t];
        } else {
            cumulativeCredit = rewards[t] + gamma * cumulativeCredit;
        }
        
        credits[t] = cumulativeCredit;
    }
    
    return credits;
}

std::vector<float> AdvancedRewardShaping::applySpatialCreditAssignment(const std::vector<float>& rewards,
                                                                       const std::vector<std::vector<float>>& positions,
                                                                       float discount) {
    std::vector<float> spatialCredits(rewards.size(), 0.0f);
    
    if (rewards.empty()) return spatialCredits;
    
    // Spatial discounting based on distance from goal
    float totalDiscount = 0.0f;
    for (size_t i = 0; i < positions.size(); ++i) {
        float distance = 0.0f;
        if (!positions[i].empty()) {
            distance = positions[i][0];  // Simplified distance calculation
        }
        totalDiscount += std::pow(discount, distance);
    }
    
    float averageDiscount = totalDiscount / positions.size();
    
    for (size_t i = 0; i < rewards.size(); ++i) {
        spatialCredits[i] = rewards[i] * averageDiscount;
    }
    
    return spatialCredits;
}

float AdvancedRewardShaping::normalizeReward(float reward, const std::vector<float>& rewardHistory) {
    if (rewardHistory.empty()) return reward;
    
    float mean = std::accumulate(rewardHistory.begin(), rewardHistory.end(), 0.0f) / rewardHistory.size();
    float variance = 0.0f;
    
    for (float r : rewardHistory) {
        variance += std::pow(r - mean, 2);
    }
    variance /= rewardHistory.size();
    
    float stddev = std::sqrt(variance);
    
    if (stddev == 0.0f) return reward;
    
    return (reward - mean) / stddev;
}

float AdvancedRewardShaping::sparseRewardHandling(float reward, const std::vector<float>& state,
                                                  const std::vector<float>& goalState,
                                                  float goalDistanceThreshold) {
    if (reward != 0.0f) {
        return reward;  // Non-sparse reward, return as-is
    }
    
    // Sparse reward shaping based on goal proximity
    float goalDistance = 0.0f;
    size_t minSize = std::min(state.size(), goalState.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        goalDistance += std::abs(state[i] - goalState[i]);
    }
    
    goalDistance /= minSize;
    
    if (goalDistance < goalDistanceThreshold) {
        return 1.0f;  // Close to goal
    } else if (goalDistance < goalDistanceThreshold * 2.0f) {
        return 0.1f;  // Getting closer
    }
    
    return 0.0f;  // Too far
}

float AdvancedRewardShaping::potentialFunction(const std::vector<float>& state) {
    // Simplified potential function based on state values
    if (state.empty()) return 0.0f;
    
    float potential = 0.0f;
    for (float val : state) {
        potential += val * 0.1f;  // Weight state values
    }
    
    // Apply non-linear transformation
    return std::tanh(potential);
}

// Helper functions for AdvancedAgentLearning
float AdvancedAgentLearning::computeEntropy(const std::vector<float>& probabilities) {
    float entropy = 0.0f;
    for (float p : probabilities) {
        if (p > 0.0f) {
            entropy -= p * std::log(p);
        }
    }
    return entropy;
}

float AdvancedAgentLearning::computeValueEstimate(const std::vector<float>& state) {
    std::vector<float> input = state;
    if (input.size() < stateDimension_) {
        input.resize(stateDimension_, 0.0f);
    }
    
    std::vector<float> valueOutput = valueNetwork_->forward(input);
    return valueOutput.empty() ? 0.0f : valueOutput[0];
}

std::vector<float> AdvancedAgentLearning::computeFeatureRepresentation(const std::vector<float>& state) {
    // Simple feature extraction
    std::vector<float> features;
    features.reserve(state.size() / 4);
    
    // Downsample and compute statistics
    for (size_t i = 0; i < state.size(); i += 4) {
        float val = 0.0f;
        size_t count = 0;
        for (size_t j = i; j < std::min(i + 4, state.size()); ++j) {
            val += state[j];
            count++;
        }
        features.push_back(val / count);
    }
    
    return features;
}

float AdvancedAgentLearning::computeSurprise(const std::vector<float>& state,
                                            const std::vector<float>& nextState) {
    // Simple surprise based on state difference
    float surprise = 0.0f;
    size_t minSize = std::min(state.size(), nextState.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        surprise += std::abs(state[i] - nextState[i]);
    }
    
    return surprise / minSize;
}

float AdvancedAgentLearning::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float AdvancedAgentLearning::softmax(const std::vector<float>& inputs, std::vector<float>& output) {
    if (inputs.empty()) return 0.0f;
    
    float maxInput = *std::max_element(inputs.begin(), inputs.end());
    float sum = 0.0f;
    
    output.resize(inputs.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
        output[i] = std::exp(inputs[i] - maxInput);
        sum += output[i];
    }
    
    if (sum == 0.0f) {
        for (size_t i = 0; i < output.size(); ++i) {
            output[i] = 1.0f / output.size();
        }
        return 1.0f / output.size();
    }
    
    for (float& val : output) {
        val /= sum;
    }
    
    return sum / output.size();
}