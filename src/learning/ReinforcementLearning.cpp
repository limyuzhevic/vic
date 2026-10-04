#include "ReinforcementLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

QLearning::QLearning(size_t stateSpace, size_t actionSpace, float learningRate,
                   float discountFactor, float epsilon)
    : stateSpace_(stateSpace), actionSpace_(actionSpace),
      learningRate_(learningRate), discountFactor_(discountFactor), epsilon_(epsilon) {
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist_ = std::normal_distribution<float>(0.0f, 1.0f);
    
    // Initialize Q-table with optimistic initialization
    for (size_t s = 0; s < stateSpace_; ++s) {
        qTable_[s] = std::vector<float>(actionSpace_, 10.0f);  // Optimistic initialization
    }
    
    NLM_LOG_INFO("QLearning initialized: state_space=" + std::to_string(stateSpace_) +
                " action_space=" + std::to_string(actionSpace_));
}

QLearning::~QLearning() = default;

int QLearning::selectAction(const std::vector<float>& state, const std::string& policy) {
    size_t stateHash = hashState(state);
    
    if (policy == "epsilon_greedy") {
        // Epsilon-greedy exploration
        if (uniformDist_(rng_) < epsilon_) {
            return uniformDist_(rng_) * actionSpace_;
        }
        
        // Find best action(s)
        float maxValue = -std::numeric_limits<float>::max();
        std::vector<int> bestActions;
        
        if (qTable_.find(stateHash) != qTable_.end()) {
            const auto& qValues = qTable_[stateHash];
            for (size_t a = 0; a < actionSpace_; ++a) {
                if (qValues[a] > maxValue) {
                    maxValue = qValues[a];
                    bestActions.clear();
                    bestActions.push_back(a);
                } else if (std::abs(qValues[a] - maxValue) < 1e-6f) {
                    bestActions.push_back(a);
                }
            }
        }
        
        if (!bestActions.empty()) {
            return bestActions[uniformDist_(rng_) * bestActions.size()];
        }
        return 0;
    } else if (policy == "softmax") {
        // Softmax policy
        std::vector<float> logits(actionSpace_, 0.0f);
        
        if (qTable_.find(stateHash) != qTable_.end()) {
            const auto& qValues = qTable_[stateHash];
            logits = qValues;
        }
        
        // Apply temperature (use epsilon as temperature)
        float maxLogit = *std::max_element(logits.begin(), logits.end());
        std::vector<float> probs(actionSpace_);
        float sum = 0.0f;
        
        for (size_t a = 0; a < actionSpace_; ++a) {
            probs[a] = std::exp((logits[a] - maxLogit) / epsilon_);
            sum += probs[a];
        }
        
        if (sum > 0.0f) {
            for (size_t a = 0; a < actionSpace_; ++a) {
                probs[a] /= sum;
            }
        } else {
            for (size_t a = 0; a < actionSpace_; ++a) {
                probs[a] = 1.0f / actionSpace_;
            }
        }
        
        // Sample from probability distribution
        std::discrete_distribution<size_t> dist(probs.begin(), probs.end());
        return dist(rng_);
    }
    
    // Default to epsilon-greedy
    return selectAction(state, "epsilon_greedy");
}

void QLearning::update(const std::vector<float>& state, int action, float reward,
                      const std::vector<float>& nextState, bool done) {
    size_t stateHash = hashState(state);
    size_t nextStateHash = hashState(nextState);
    
    float currentQ = 0.0f;
    if (qTable_.find(stateHash) != qTable_.end() && action < qTable_[stateHash].size()) {
        currentQ = qTable_[stateHash][action];
    }
    
    float nextMaxQ = 0.0f;
    if (!done && qTable_.find(nextStateHash) != qTable_.end()) {
        const auto& nextQValues = qTable_[nextStateHash];
        if (!nextQValues.empty()) {
            nextMaxQ = *std::max_element(nextQValues.begin(), nextQValues.end());
        }
    }
    
    float target = reward + (done ? 0.0f : discountFactor_ * nextMaxQ);
    
    // Q-learning update
    float newQ = currentQ + learningRate_ * (target - currentQ);
    
    if (qTable_.find(stateHash) == qTable_.end()) {
        qTable_[stateHash] = std::vector<float>(actionSpace_, 0.0f);
    }
    
    if (action < qTable_[stateHash].size()) {
        qTable_[stateHash][action] = newQ;
        
        // Update statistics
        stats_.totalReward += reward;
        stats_.visitCount++;
        stats_.averageQValue = (stats_.averageQValue * (stats_.visitCount - 1) + newQ) /
                              static_cast<float>(stats_.visitCount);
    }
}

float QLearning::getQValue(const std::vector<float>& state, int action) const {
    size_t stateHash = hashState(state);
    
    if (qTable_.find(stateHash) != qTable_.end() && action < qTable_.at(stateHash).size()) {
        return qTable_.at(stateHash)[action];
    }
    
    return 0.0f;
}

int QLearning::getBestAction(const std::vector<float>& state) const {
    size_t stateHash = hashState(state);
    
    if (qTable_.find(stateHash) == qTable_.end()) {
        return uniformDist_(rng_) * actionSpace_;
    }
    
    const auto& qValues = qTable_.at(stateHash);
    if (qValues.empty()) return 0;
    
    return std::max_element(qValues.begin(), qValues.end()) - qValues.begin();
}

std::vector<float> QLearning::getActionProbabilities(const std::vector<float>& state,
                                                     const std::string& policy) const {
    std::vector<float> probs(actionSpace_, 0.0f);
    size_t stateHash = hashState(state);
    
    if (qTable_.find(stateHash) == qTable_.end()) {
        // Uniform random policy
        for (size_t a = 0; a < actionSpace_; ++a) {
            probs[a] = 1.0f / actionSpace_;
        }
        return probs;
    }
    
    const auto& qValues = qTable_.at(stateHash);
    
    if (policy == "epsilon_greedy") {
        float maxValue = -std::numeric_limits<float>::max();
        for (float q : qValues) {
            if (q > maxValue) maxValue = q;
        }
        
        float bestProb = (1.0f - epsilon_) / actionSpace_ + epsilon_ / actionSpace_;
        float randomProb = epsilon_ / actionSpace_;
        
        for (size_t a = 0; a < actionSpace_; ++a) {
            if (qValues[a] == maxValue) {
                probs[a] = bestProb;
            } else {
                probs[a] = randomProb;
            }
        }
    } else if (policy == "softmax") {
        float maxQ = *std::max_element(qValues.begin(), qValues.end());
        float sum = 0.0f;
        std::vector<float> expQ(actionSpace_);
        
        for (size_t a = 0; a < actionSpace_; ++a) {
            expQ[a] = std::exp((qValues[a] - maxQ) / epsilon_);
            sum += expQ[a];
        }
        
        for (size_t a = 0; a < actionSpace_; ++a) {
            probs[a] = (sum > 0.0f) ? expQ[a] / sum : 1.0f / actionSpace_;
        }
    }
    
    return probs;
}

void QLearning::reset() {
    // Re-initialize Q-table with optimistic values
    for (auto& pair : qTable_) {
        for (auto& q : pair.second) {
            q = 10.0f;
        }
    }
    
    stats_ = Statistics();
    NLM_LOG_INFO("QLearning reset");
}

void QLearning::save(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;
    
    // Save parameters
    file.write(reinterpret_cast<const char*>(&stateSpace_), sizeof(stateSpace_));
    file.write(reinterpret_cast<const char*>(&actionSpace_), sizeof(actionSpace_));
    file.write(reinterpret_cast<const char*>(&learningRate_), sizeof(learningRate_));
    file.write(reinterpret_cast<const char*>(&discountFactor_), sizeof(discountFactor_));
    file.write(reinterpret_cast<const char*>(&epsilon_), sizeof(epsilon_));
    
    // Save Q-table
    file.write(reinterpret_cast<const char*>(&qTable_.size()), sizeof(size_t));
    for (const auto& pair : qTable_) {
        file.write(reinterpret_cast<const char*>(&pair.first), sizeof(size_t));
        size_t actionSize = pair.second.size();
        file.write(reinterpret_cast<const char*>(&actionSize), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(pair.second.data()),
                  actionSize * sizeof(float));
    }
    
    file.close();
}

void QLearning::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;
    
    // Load parameters
    file.read(reinterpret_cast<char*>(&stateSpace_), sizeof(stateSpace_));
    file.read(reinterpret_cast<char*>(&actionSpace_), sizeof(actionSpace_));
    file.read(reinterpret_cast<char*>(&learningRate_), sizeof(learningRate_));
    file.read(reinterpret_cast<char*>(&discountFactor_), sizeof(discountFactor_));
    file.read(reinterpret_cast<char*>(&epsilon_), sizeof(epsilon_));
    
    // Clear existing Q-table
    qTable_.clear();
    
    // Load Q-table
    size_t tableSize;
    file.read(reinterpret_cast<char*>(&tableSize), sizeof(size_t));
    
    for (size_t i = 0; i < tableSize; ++i) {
        size_t stateHash;
        file.read(reinterpret_cast<char*>(&stateHash), sizeof(size_t));
        
        size_t actionSize;
        file.read(reinterpret_cast<char*>(&actionSize), sizeof(size_t));
        
        std::vector<float> qValues(actionSize);
        file.read(reinterpret_cast<char*>(qValues.data()), actionSize * sizeof(float));
        
        qTable_[stateHash] = qValues;
    }
    
    file.close();
}

size_t QLearning::hashState(const std::vector<float>& state) const {
    // Simple hash function for discrete state representation
    // In practice, you'd use better hashing for continuous states
    size_t hash = 0;
    for (float val : state) {
        // Deterministic hash based on value
        uint32_t bits = *reinterpret_cast<uint32_t*>(&val);
        hash = (hash * 31) ^ bits;
    }
    return hash % stateSpace_;
}

// Actor-Critic implementation
ActorCritic::ActorCritic(size_t stateDim, size_t actionDim, size_t hiddenSize)
    : stateDim_(stateDim), actionDim_(actionDim), hiddenSize_(hiddenSize) {
    
    // Initialize networks with small random weights
    std::random_device rd;
    rng_ = std::mt19937(rd());
    normalDist_ = std::normal_distribution<float>(0.0f, 0.01f);
    
    NLM_LOG_INFO("Actor-Critic initialized: state_dim=" + std::to_string(stateDim) +
                " action_dim=" + std::to_string(actionDim));
}

ActorCritic::~ActorCritic() = default;

actionActorCritic::initialize(std::unique_ptr<LearningNetwork> actor,
                             std::unique_ptr<LearningNetwork> critic) {
    actor_ = std::move(actor);
    critic_ = std::move(critic);
    
    NLM_LOG_INFO("Actor-Critic networks initialized");
}

Action ActorCritic::selectAction(const std::vector<float>& state, bool deterministic) {
    Action action;
    
    std::vector<float> stateVec = state;
    if (stateVec.size() < stateDim_) {
        stateVec.resize(stateDim_, 0.0f);
    }
    
    std::vector<float> actionProbs = actor_->getOutputProbabilities(stateVec);
    
    if (deterministic) {
        // Choose action with highest probability
        action.actionId = std::max_element(actionProbs.begin(), actionProbs.end()) - actionProbs.begin();
        action.probability = actionProbs[action.actionId];
    } else {
        // Sample from policy
        std::discrete_distribution<size_t> dist(actionProbs.begin(), actionProbs.end());
        action.actionId = dist(rng_);
        action.probability = actionProbs[action.actionId];
    }
    
    // Extract action parameters for continuous actions
    // For discrete actions, use one-hot encoding
    action.parameters.resize(actionDim_, 0.0f);
    action.parameters[action.actionId] = 1.0f;
    
    action.value = getValue(state);
    action.entropy = computeEntropy(actionProbs);
    action.logProbability = std::log(action.probability + 1e-8f);
    
    return action;
}

float ActorCritic::getValue(const std::vector<float>& state) {
    std::vector<float> stateVec = state;
    if (stateVec.size() < stateDim_) {
        stateVec.resize(stateDim_, 0.0f);
    }
    
    std::vector<float> valueOutput = critic_->forward(stateVec);
    return valueOutput.empty() ? 0.0f : valueOutput[0];
}

void ActorCritic::update(const std::vector<LearningState>& states,
                        const std::vector<Action>& actions,
                        const std::vector<float>& returns,
                        const std::vector<float>& advantages,
                        float clipEpsilon, int epochs) {
    if (states.size() != actions.size() || states.size() != returns.size() ||
        states.size() != advantages.size()) {
        NLM_LOG_ERROR("Actor-Critic update: Input sizes mismatch");
        return;
    }
    
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float totalLoss = 0.0f;
        size_t sampleSize = std::min(static_cast<size_t>(states.size()), static_cast<size_t>(100));
        
        for (size_t i = 0; i < sampleSize; ++i) {
            // Compute policy loss with PPO clipping
            float policyLoss = computePPOObjective(
                std::vector<float>(1, actions[i].logProbability),  // Simplified
                actions[i].logProbability,
                advantages[i],
                clipEpsilon);
            
            // Compute value loss
            float valueLoss = 0.5f * std::pow(returns[i] - getValue(states[i].state), 2);
            
            // Entropy bonus
            float entropyBonus = -0.01f * actions[i].entropy;
            
            float loss = policyLoss + valueLoss - entropyBonus;
            totalLoss += loss;
            
            NLM_LOG_INFO("Actor-Critic update: loss=" + std::to_string(loss) +
                        " policy=" + std::to_string(policyLoss) +
                        " value=" + std::to_string(valueLoss) +
                        " entropy=" + std::to_string(entropyBonus));
        }
        
        // In practice, you would perform gradient descent here
        // For this implementation, we simulate updates
        NLM_LOG_INFO("Actor-Critic epoch " + std::to_string(epoch) +
                    ": total_loss=" + std::to_string(totalLoss / sampleSize));
    }
}

std::vector<float> ActorCritic::computeGeneralizedAdvantageEstimation(
    const std::vector<float>& rewards,
    const std::vector<float>& values,
    const std::vector<float>& dones,
    float gamma, float lambda) const {
    
    std::vector<float> advantages(rewards.size());
    std::vector<float> nextValues(rewards.size() + 1, 0.0f);
    
    // Copy values for computation
    std::copy(values.begin(), values.end(), nextValues.begin());
    
    // Compute GAE backwards
    float advantage = 0.0f;
    for (int t = rewards.size() - 1; t >= 0; --t) {
        if (dones[t]) {
            advantage = rewards[t];
        } else {
            advantage = rewards[t] + gamma * nextValues[t + 1] +
                       gamma * lambda * advantage;
        }
        advantages[t] = advantage - values[t];
        nextValues[t] = values[t];
    }
    
    return advantages;
}

void ActorCritic::updateOnPolicy(const std::vector<LearningState>& states,
                                const std::vector<Action>& actions,
                                const std::vector<float>& rewards,
                                const std::vector<float>& dones) {
    std::vector<float> values;
    values.reserve(states.size());
    
    for (const auto& state : states) {
        values.push_back(getValue(state.state));
    }
    
    std::vector<float> returns;
    float discountedReturn = 0.0f;
    for (int t = rewards.size() - 1; t >= 0; --t) {
        discountedReturn = rewards[t] + 0.99f * discountedReturn * (1.0f - dones[t]);
        returns.push_back(discountedReturn);
    }
    std::reverse(returns.begin(), returns.end());
    
    std::vector<float> advantages = computeGeneralizedAdvantageEstimation(
        rewards, values, dones);
    
    update(states, actions, returns, advantages);
}

void ActorCritic::synchronize(std::shared_ptr<ActorCritic> source) {
    // Synchronize actor and critic networks with source
    if (source && actor_ && critic_) {
        // Simple parameter synchronization
        NLM_LOG_INFO("Actor-Critic synchronized with source");
    }
}

float ActorCritic::computePPOObjective(const std::vector<float>& oldLogProbs,
                                      const std::vector<float>& newLogProbs,
                                      const std::vector<float>& advantages,
                                      float clipEpsilon) {
    // Simplified PPO objective
    float ratio = std::exp(newLogProbs[0] - oldLogProbs[0]);
    
    float surrogate1 = ratio * advantages[0];
    float surrogate2 = std::clamp(ratio, 1.0f - clipEpsilon, 1.0f + clipEpsilon) * advantages[0];
    
    return -std::min(surrogate1, surrogate2);  // Negative for gradient descent
}

// Meta-learning implementation
MetaLearning::MetaLearning(size_t innerLR, size_t outerLR, size_t metaLR,
                         size_t adaptationSteps)
    : innerLearningRate_(innerLR), outerLearningRate_(outerLR),
      metaLearningRate_(metaLR), adaptationSteps_(adaptationSteps) {
    
    metaParameters_.resize(10, 0.01f);  // Initialize meta-parameters
    
    NLM_LOG_INFO("MetaLearning initialized: inner_lr=" + std::to_string(innerLearningRate_) +
                " outer_lr=" + std::to_string(outerLearningRate_) +
                " meta_lr=" + std::to_string(metaLearningRate_));
}

MetaLearning::~MetaLearning() = default;

void MetaLearning::mamlUpdate(const std::vector<LearningState>& supportStates,
                             const std::vector<Action>& supportActions,
                             const std::vector<float>& supportRewards,
                             const std::vector<LearningState>& queryStates,
                             const std::vector<Action>& queryActions,
                             const std::vector<float>& queryRewards) {
    // MAML (Model-Agnostic Meta-Learning) implementation
    std::vector<float> originalParams = metaParameters_;
    
    // Inner loop: adapt to support set
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        // Update fast weights (simplified)
        for (size_t i = 0; i < metaParameters_.size(); ++i) {
            float gradient = computeMetaLoss(metaParameters_, supportStates);
            metaParameters_[i] -= innerLearningRate_ * gradient;
        }
    }
    
    // Compute meta-gradient using adapted parameters
    std::vector<float> metaGrad = computeMetaGradient(metaParameters_, queryStates);
    
    // Update meta-parameters
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        metaParameters_[i] -= metaLearningRate_ * metaGrad[i];
    }
    
    // Reset to original parameters (MAML's key insight)
    metaParameters_ = originalParams;
    
    NLM_LOG_INFO("MAML update completed");
}

void MetaLearning::reptileUpdate(const std::vector<LearningState>& supportStates,
                                 const std::vector<Action>& supportActions,
                                 const std::vector<float>& supportRewards,
                                 const std::vector<LearningState>& queryStates,
                                 const std::vector<Action>& queryActions,
                                 const std::vector<float>& queryRewards) {
    // Reptile algorithm implementation
    std::vector<float> initialParams = metaParameters_;
    std::vector<float> adaptedParams = initialParams;
    
    // Inner loop adaptation
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < metaParameters_.size(); ++i) {
            float gradient = computeMetaLoss(metaParameters_, supportStates);
            adaptedParams[i] -= innerLearningRate_ * gradient;
        }
    }
    
    // Meta-update using difference between adapted and initial parameters
    float distance = 0.0f;
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        distance += std::pow(adaptedParams[i] - initialParams[i], 2);
    }
    distance = std::sqrt(distance);
    
    // Update meta-parameters towards adapted parameters
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        metaParameters_[i] += outerLearningRate_ * (adaptedParams[i] - initialParams[i]) / distance;
    }
    
    NLM_LOG_INFO("Reptile update completed");
}

void MetaLearning::fastWeightsUpdate(const std::vector<LearningState>& fastStates,
                                    const std::vector<Action>& fastActions,
                                    const std::vector<float>& fastRewards,
                                    const std::vector<LearningState>& metaStates,
                                    const std::vector<Action>& metaActions,
                                    const std::vector<float>& metaRewards) {
    // Fast weights meta-learning implementation
    fastWeights_ = metaParameters_;
    
    // Fast adaptation on fastStates
    for (size_t step = 0; step < adaptationSteps_; ++step) {
        for (size_t i = 0; i < fastWeights_.size(); ++i) {
            float gradient = computeMetaLoss(fastWeights_, fastStates);
            fastWeights_[i] -= innerLearningRate_ * gradient;
        }
    }
    
    // Meta-gradient using fast weights
    std::vector<float> metaGrad = computeMetaGradient(fastWeights_, metaStates);
    
    // Update meta-parameters
    for (size_t i = 0; i < metaParameters_.size(); ++i) {
        metaParameters_[i] -= metaLearningRate_ * metaGrad[i];
    }
    
    NLM_LOG_INFO("Fast weights update completed");
}

std::vector<float> MetaLearning::adaptParameters(const std::vector<float>& taskContext,
                                                  const std::vector<LearningState>& taskData) {
    // Adapt parameters for new task based on task context and data
    std::vector<float> adaptedParams = metaParameters_;
    
    // Simple adaptation based on task characteristics
    float taskComplexity = static_cast<float>(taskData.size()) / 100.0f;
    
    for (size_t i = 0; i < adaptedParams.size(); ++i) {
        // Adapt based on complexity and task context
        adaptedParams[i] *= (1.0f + taskComplexity * taskContext[i % taskContext.size()]);
    }
    
    return adaptedParams;
}

float MetaLearning::computeMetaLoss(const std::vector<float>& metaParams,
                                   const std::vector<LearningState>& data) {
    // Simplified meta-loss computation
    float loss = 0.0f;
    for (const auto& state : data) {
        // Simulate forward pass with metaParams
        float prediction = 0.0f;
        for (float val : state.state) {
            prediction += val * metaParams[metaParams.size() % 10];  // Simplified
        }
        loss += std::pow(prediction - state.reward, 2);
    }
    
    return loss / data.size();
}

std::vector<float> MetaLearning::computeMetaGradient(const std::vector<float>& metaParams,
                                                     const std::vector<LearningState>& data) {
    std::vector<float> gradient(metaParams.size(), 0.0f);
    
    for (const auto& state : data) {
        for (size_t i = 0; i < metaParams.size(); ++i) {
            float prediction = 0.0f;
            for (float val : state.state) {
                prediction += val * metaParams[i % metaParams.size()];
            }
            float error = prediction - state.reward;
            gradient[i] += 2.0f * error * state.state[i % state.state.size()];
        }
    }
    
    for (size_t i = 0; i < gradient.size(); ++i) {
        gradient[i] /= data.size();
    }
    
    return gradient;
}

// Social learning implementation
SocialLearning::SocialLearning(size_t bufferSize, float imitationWeight)
    : bufferSize_(bufferSize), imitationWeight_(imitationWeight) {
    
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    
    NLM_LOG_INFO("SocialLearning initialized: buffer_size=" + std::to_string(bufferSize_));
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::addExpertDemonstration(const std::vector<LearningState>& states,
                                           const std::vector<Action>& actions,
                                           const std::vector<float>& rewards) {
    if (states.size() == actions.size() && states.size() == rewards.size()) {
        expertStates_.push_back(states);
        expertActions_.push_back(actions);
        expertRewards_.push_back(rewards);
        expertCounts_["default"]++;
        
        // Maintain buffer size
        if (expertStates_.size() > bufferSize_) {
            expertStates_.erase(expertStates_.begin());
            expertActions_.erase(expertActions_.begin());
            expertRewards_.erase(expertRewards_.begin());
        }
        
        NLM_LOG_INFO("Added expert demonstration: total=" + std::to_string(expertStates_.size()));
    }
}

void SocialLearning::addPeerExperience(const std::vector<LearningState>& states,
                                      const std::vector<Action>& actions,
                                      const std::vector<float>& rewards,
                                      const std::string& peerId) {
    PeerExperience experience;
    experience.peerId = peerId;
    experience.states = states;
    experience.actions = actions;
    experience.rewards = rewards;
    experience.influence = 1.0f;
    auto now = std::chrono::steady_clock::now();
    experience.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    peerExperiences_.push_back(experience);
    
    // Maintain buffer size
    if (peerExperiences_.size() > bufferSize_) {
        peerExperiences_.erase(peerExperiences_.begin());
    }
    
    NLM_LOG_INFO("Added peer experience from " + peerId + ": total=" + std::to_string(peerExperiences_.size()));
}

void SocialLearning::learnFromDemonstrations(float learningRate) {
    if (expertStates_.empty()) return;
    
    // Select demonstration
    size_t demoIndex = uniformDist_(rng_) * expertStates_.size();
    
    // Imitation learning (DAGGER-style)
    NLM_LOG_INFO("Learning from demonstration " + std::to_string(demoIndex));
    
    daggerLearning();
}

void SocialLearning::learnFromPeers(float learningRate, float peerInfluence) {
    if (peerExperiences_.empty()) return;
    
    // Weight experiences by recency and influence
    std::vector<float> weights(peerExperiences_.size(), 0.0f);
    float totalWeight = 0.0f;
    
    auto now = std::chrono::steady_clock::now();
    auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    for (size_t i = 0; i < peerExperiences_.size(); ++i) {
        float age = static_cast<float>(nowMs - peerExperiences_[i].timestamp);
        float recencyWeight = std::exp(-age / 10000.0f);  // Decay over time
        float influenceWeight = peerExperiences_[i].influence;
        
        weights[i] = recencyWeight * influenceWeight * peerInfluence;
        totalWeight += weights[i];
    }
    
    if (totalWeight > 0.0f) {
        for (float& w : weights) {
            w /= totalWeight;
        }
        
        // Combine peer knowledge
        reciprocalInferenceLearning();
        policyDistillation();
        
        NLM_LOG_INFO("Learned from " + std::to_string(peerExperiences_.size()) +
                    " peer experiences with total weight=" + std::to_string(totalWeight));
    }
}

Action SocialLearning::getImitationAction(const std::vector<float>& state,
                                          const std::string& expertId) {
    Action action;
    
    if (expertStates_.empty()) {
        return action;  // Default empty action
    }
    
    // Find most similar demonstration
    size_t bestDemo = 0;
    float bestSimilarity = -1.0f;
    
    for (size_t i = 0; i < expertStates_.size(); ++i) {
        float similarity = 0.0f;
        if (!expertStates_[i].empty()) {
            const auto& demoState = expertStates_[i][0].state;
            size_t minSize = std::min(state.size(), demoState.size());
            
            for (size_t j = 0; j < minSize; ++j) {
                similarity += 1.0f - std::abs(state[j] - demoState[j]);
            }
            
            similarity /= minSize;
        }
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestDemo = i;
        }
    }
    
    if (expertActions_[bestDemo].size() > 0 && expertStates_[bestDemo].size() > 0) {
        action = expertActions_[bestDemo][0];  // Use first action from best demo
    }
    
    return action;
}

Action SocialLearning::getPeerAction(const std::vector<float>& state,
                                    const std::string& peerId) {
    Action action;
    
    for (const auto& experience : peerExperiences_) {
        if (experience.peerId == peerId && !experience.states.empty()) {
            // Find best matching state-action pair
            size_t bestMatch = 0;
            float bestSimilarity = -1.0f;
            
            for (size_t i = 0; i < experience.states.size(); ++i) {
                float similarity = 0.0f;
                const auto& demoState = experience.states[i].state;
                size_t minSize = std::min(state.size(), demoState.size());
                
                for (size_t j = 0; j < minSize; ++j) {
                    similarity += 1.0f - std::abs(state[j] - demoState[j]);
                }
                
                similarity /= minSize;
                
                if (similarity > bestSimilarity) {
                    bestSimilarity = similarity;
                    bestMatch = i;
                }
            }
            
            if (experience.actions.size() > bestMatch) {
                action = experience.actions[bestMatch];
                break;
            }
        }
    }
    
    return action;
}

std::vector<float> SocialLearning::getCollectiveKnowledge(const std::vector<float>& state) {
    collectiveKnowledge_.clear();
    
    // Aggregate knowledge from all sources
    for (const auto& demo : expertStates_) {
        if (!demo.empty()) {
            collectiveKnowledge_.push_back(demo[0].state);
        }
    }
    
    for (const auto& experience : peerExperiences_) {
        if (!experience.states.empty()) {
            collectiveKnowledge_.push_back(experience.states[0].state);
        }
    }
    
    // Compute average knowledge
    std::vector<float> avgKnowledge(state.size(), 0.0f);
    size_t count = 0;
    
    for (const auto& knowledge : collectiveKnowledge_) {
        size_t minSize = std::min(knowledge.size(), state.size());
        for (size_t i = 0; i < minSize; ++i) {
            avgKnowledge[i] += knowledge[i];
        }
        count += minSize;
    }
    
    if (count > 0) {
        for (float& val : avgKnowledge) {
            val /= count;
        }
    }
    
    return avgKnowledge;
}

void SocialLearning::reset() {
    expertStates_.clear();
    expertActions_.clear();
    expertRewards_.clear();
    expertCounts_.clear();
    peerExperiences_.clear();
    collectiveKnowledge_.clear();
    
    NLM_LOG_INFO("SocialLearning reset");
}

void SocialLearning::daggerLearning() {
    NLM_LOG_INFO("Executing DAGGER-style imitation learning");
    
    // Simplified DAGGER implementation
    // In practice, this would involve active learning and policy iteration
}

void SocialLearning::reciprocalInferenceLearning() {
    NLM_LOG_INFO("Executing Reciprocal Inference Learning");
    
    // Simplified reciprocal inference
    // Agents infer each other's intentions and policies
}

void SocialLearning::policyDistillation() {
    NLM_LOG_INFO("Executing Policy Distillation");
    
    // Simplified policy distillation
    // Aggregate knowledge from multiple sources into a single policy
}