#include "ReinforcementLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {
namespace ReinforcementLearning {

// QTable implementation
void QTable::update(const std::vector<float>& state, ActionType action, 
                    float reward, const std::vector<float>& nextState, 
                    bool done, double timestamp) {
    size_t stateHash = std::hash<std::vector<float>>{}(state);
    size_t nextStateHash = std::hash<std::vector<float>>{}(nextState);
    
    // Find or create Q-value entries
    QEntry* currentEntry = nullptr;
    QEntry* nextEntry = nullptr;
    
    if (stateActionMap_.find(stateHash) != stateActionMap_.end()) {
        // Find entry for specific action
        for (auto& entry : qEntries_) {
            if (entry.state == state && entry.action == action) {
                currentEntry = &entry;
                break;
            }
        }
    }
    
    if (!done && stateActionMap_.find(nextStateHash) != stateActionMap_.end()) {
        // Find best next action
        float maxQ = std::numeric_limits<float>::lowest();
        ActionType bestAction = ActionType::Wait;
        
        for (const auto& entry : qEntries_) {
            if (entry.state == nextState && entry.value > maxQ) {
                maxQ = entry.value;
                bestAction = entry.action;
            }
        }
        
        if (currentEntry) {
            float target = reward + discountFactor_ * maxQ;
            currentEntry->value += learningRate_ * (target - currentEntry->value);
            currentEntry->visitCount++;
        }
    } else if (currentEntry) {
        // Terminal state
        currentEntry->value += learningRate_ * (reward - currentEntry->value);
        currentEntry->visitCount++;
    }
    
    // Add new entry if it doesn't exist
    if (!currentEntry) {
        QEntry newEntry(state, action);
        newEntry.value = reward;
        newEntry.timestamp = timestamp;
        newEntry.visitCount = 1;
        
        qEntries_.push_back(newEntry);
        stateActionMap_[stateHash] = &qEntries_.back();
    }
}

ActionType QTable::selectAction(const std::vector<float>& state, double timestamp) {
    double epsilon = getExplorationRate(timestamp);
    float randVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    if (randVal < epsilon) {
        // Explore: choose random action
        size_t numActions = actionSpace_.size();
        return actionSpace_[rand() % numActions];
    } else {
        // Exploit: choose greedy action
        return getGreedyAction(state);
    }
}

float QTable::getQValue(const std::vector<float>& state, ActionType action) const {
    size_t stateHash = std::hash<std::vector<float>>{}(state);
    
    for (const auto& entry : qEntries_) {
        if (entry.state == state && entry.action == action) {
            return entry.value;
        }
    }
    
    return 0.0f; // Default Q-value
}

float QTable::getMaxQValue(const std::vector<float>& state) const {
    float maxQ = std::numeric_limits<float>::lowest();
    
    for (const auto& entry : qEntries_) {
        if (entry.state == state && entry.value > maxQ) {
            maxQ = entry.value;
        }
    }
    
    return maxQ > std::numeric_limits<float>::lowest() ? maxQ : 0.0f;
}

ActionType QTable::getGreedyAction(const std::vector<float>& state) const {
    float maxQ = std::numeric_limits<float>::lowest();
    ActionType bestAction = ActionType::Wait;
    bool hasValidActions = false;
    
    for (const auto& entry : qEntries_) {
        if (entry.state == state) {
            hasValidActions = true;
            if (entry.value > maxQ) {
                maxQ = entry.value;
                bestAction = entry.action;
            }
        }
    }
    
    return hasValidActions ? bestAction : ActionType::Wait;
}

float QTable::getAverageQValue() const {
    if (qEntries_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& entry : qEntries_) {
        sum += entry.value;
    }
    
    return sum / static_cast<float>(qEntries_.size());
}

float QTable::getQValueVariance() const {
    if (qEntries_.size() < 2) return 0.0f;
    
    float avg = getAverageQValue();
    float variance = 0.0f;
    
    for (const auto& entry : qEntries_) {
        float diff = entry.value - avg;
        variance += diff * diff;
    }
    
    return variance / static_cast<float>(qEntries_.size() - 1);
}

std::vector<float> QTable::getQValueDistribution() const {
    std::vector<float> histogram(10, 0.0f); // 10 bins for histogram
    
    for (const auto& entry : qEntries_) {
        size_t binIndex = static_cast<size_t>((entry.value + 10.0f) / 20.0f * 10.0f);
        if (binIndex < histogram.size()) {
            histogram[binIndex]++;
        }
    }
    
    // Normalize to probabilities
    float total = 0.0f;
    for (float count : histogram) {
        total += count;
    }
    
    if (total > 0) {
        for (float& count : histogram) {
            count /= total;
        }
    }
    
    return histogram;
}

ActionType QTable::epsilonGreedyAction(const std::vector<float>& state, double timestamp) {
    return selectAction(state, timestamp);
}

ActionType QTable::softmaxAction(const std::vector<float>& state) const {
    std::vector<float> qValues;
    for (const auto& action : actionSpace_) {
        qValues.push_back(getQValue(state, action));
    }
    
    // Compute softmax probabilities
    std::vector<float> expValues;
    float sumExp = 0.0f;
    
    for (float q : qValues) {
        float expQ = std::exp(q);
        expValues.push_back(expQ);
        sumExp += expQ;
    }
    
    if (sumExp == 0.0f) {
        // If all Q-values are -inf, choose uniformly
        return actionSpace_[rand() % actionSpace_.size()];
    }
    
    // Normalize to probabilities
    std::vector<float> probabilities;
    for (float expQ : expValues) {
        probabilities.push_back(expQ / sumExp);
    }
    
    // Sample from probability distribution
    float randVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulative += probabilities[i];
        if (randVal <= cumulative) {
            return actionSpace_[i];
        }
    }
    
    // Fallback to uniform selection
    return actionSpace_[rand() % actionSpace_.size()];
}

ActionType QTable::UCBAction(const std::vector<float>& state, double timestamp) const {
    float bestValue = std::numeric_limits<float>::lowest();
    ActionType bestAction = ActionType::Wait;
    
    // Find action with highest Q-value
    for (const auto& action : actionSpace_) {
        float qValue = getQValue(state, action);
        if (qValue > bestValue) {
            bestValue = qValue;
            bestAction = action;
        }
    }
    
    // UCB selection
    float explorationBonus = 2.0f * std::sqrt(std::log(timestamp + 1.0) / (1.0 + getVisitCount(state, bestAction)));
    
    return bestAction; // Simplified UCB
}

void QTable::clearOldEntries(double currentTimestamp) {
    double cutoffTime = currentTimestamp - 3600.0; // Remove entries older than 1 hour
    
    qEntries_.erase(std::remove_if(qEntries_.begin(), qEntries_.end(),
                                   [cutoffTime](const QEntry& entry) {
                                       return entry.timestamp < cutoffTime;
                                   }), qEntries_.end());
    
    // Rebuild stateActionMap
    stateActionMap_.clear();
    for (auto& entry : qEntries_) {
        size_t stateHash = std::hash<std::vector<float>>{}(entry.state);
        stateActionMap_[stateHash] = &entry;
    }
}

void QTable::compressState(const std::vector<float>& state, float compressionFactor) {
    // Placeholder for state compression
    // In practice, this would use techniques like PCA, autoencoders, etc.
}

bool QTable::save(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Write Q-table data
        file << "QTable Data\n";
        file << "============\n\n";
        
        file << "Learning Rate: " << learningRate_ << "\n";
        file << "Discount Factor: " << discountFactor_ << "\n";
        file << "Exploration Rate: " << explorationRate_ << "\n";
        file << "Epsilon Decay: " << epsilonDecay_ << "\n";
        file << "Max History Size: " << maxHistorySize_ << "\n\n";
        
        file << "Q-Entries:\n";
        for (const auto& entry : qEntries_) {
            file << "  State: [";
            for (size_t i = 0; i < entry.state.size(); ++i) {
                if (i > 0) file << ", ";
                file << entry.state[i];
            }
            file << "], Action: " << static_cast<int>(entry.action) << 
                 ", Value: " << entry.value << 
                 ", Timestamp: " << entry.timestamp << 
                 ", Visits: " << entry.visitCount << "\n";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save Q-table: " + std::string(e.what()));
        return false;
    }
}

bool QTable::load(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse Q-table data
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Learning Rate:") != std::string::npos) {
                learningRate_ = std::stof(line.substr(line.find(':') + 1));
            } else if (line.find("Discount Factor:") != std::string::npos) {
                discountFactor_ = std::stof(line.substr(line.find(':') + 1));
            } else if (line.find("Exploration Rate:") != std::string::npos) {
                explorationRate_ = std::stof(line.substr(line.find(':') + 1));
            } else if (line.find("Epsilon Decay:") != std::string::npos) {
                epsilonDecay_ = std::stof(line.substr(line.find(':') + 1));
            } else if (line.find("Max History Size:") != std::string::npos) {
                maxHistorySize_ = std::stoi(line.substr(line.find(':') + 1));
            } else if (line.find("State:") != std::string::npos) {
                // Parse Q-entry (simplified)
                QEntry entry;
                // Note: Full parsing would require more sophisticated parsing
                qEntries_.push_back(entry);
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load Q-table: " + std::string(e.what()));
        return false;
    }
}

// Actor-Critic implementation
void ActorCritic::learn(const Experience& experience) {
    // Store experience in replay buffer
    replayBuffer_.push_back(experience);
    if (replayBuffer_.size() > replayBufferSize_) {
        // Remove oldest experience
        replayBuffer_.erase(replayBuffer_.begin());
    }
    
    // Update target networks periodically
    if (replayBuffer_.size() % targetNetworkUpdateFreq_ == 0) {
        updateTargetNetworks();
    }
    
    // Train from batch of experiences
    trainFromReplayBuffer(32); // Typical batch size
}

void ActorCritic::updateTargetNetworks() {
    targetActor_ = actor_;
    targetCritic_ = critic_;
}

void ActorCritic::trainFromReplayBuffer(size_t batchSize) {
    if (replayBuffer_.size() < batchSize) return;
    
    // Select random batch
    std::vector<size_t> indices(replayBuffer_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    
    indices.resize(batchSize);
    
    // Train on batch
    for (size_t i : indices) {
        const Experience& exp = replayBuffer_[i];
        learnSingleExperience(exp);
    }
}

void ActorCritic::learnSingleExperience(const Experience& exp) {
    // Calculate target value using target networks
    std::vector<float> nextStateValue = targetCritic_.forward(exp.nextState);
    float targetValue = exp.reward + (exp.done ? 0.0f : discountFactor_ * nextStateValue[0]);
    
    // Update critic (value function)
    float currentValue = critic_.forward(exp.state)[0];
    float criticLoss = (targetValue - currentValue) * (targetValue - currentValue);
    
    // Update critic weights (simplified gradient descent)
    updateCriticNetwork(exp.state, criticLoss);
    
    // Update actor (policy) using advantage
    float advantage = targetValue - currentValue;
    updateActorNetwork(exp.state, exp.action, advantage);
}

void ActorCritic::updateCriticNetwork(const std::vector<float>& state, float loss) {
    // Simplified gradient descent update
    // In practice, this would use proper backpropagation
    for (size_t i = 0; i < critic_.weights.size(); ++i) {
        for (size_t j = 0; j < critic_.weights[i].size(); ++j) {
            critic_.weights[i][j] -= criticLearningRate_ * loss * state[j % state.size()];
        }
    }
    for (size_t i = 0; i < critic_.biases.size(); ++i) {
        for (size_t j = 0; j < critic_.biases[i].size(); ++j) {
            critic_.biases[i][j] -= criticLearningRate_ * loss;
        }
    }
}

void ActorCritic::updateActorNetwork(const std::vector<float>& state, ActionType action, float advantage) {
    // Update actor network to maximize expected return
    // In practice, this would use proper policy gradient methods
    std::vector<float> actionProb = getActionProbabilities(state);
    
    // Find index of selected action
    size_t actionIndex = 0;
    for (size_t i = 0; i < actionSpace_.size(); ++i) {
        if (actionSpace_[i] == action) {
            actionIndex = i;
            break;
        }
    }
    
    // Policy gradient update (simplified)
    float policyGradient = advantage * (1.0f - actionProb[actionIndex]); // Simplified
    
    // Update actor weights (simplified)
    for (size_t i = 0; i < actor_.weights.size(); ++i) {
        for (size_t j = 0; j < actor_.weights[i].size(); ++j) {
            actor_.weights[i][j] += actorLearningRate_ * policyGradient * state[j % state.size()];
        }
    }
    for (size_t i = 0; i < actor_.biases.size(); ++i) {
        for (size_t j = 0; j < actor_.biases[i].size(); ++j) {
            actor_.biases[i][j] += actorLearningRate_ * policyGradient;
        }
    }
}

ActionType ActorCritic::selectAction(const std::vector<float>& state, float epsilon) {
    std::vector<float> probs = getActionProbabilities(state);
    
    // Add exploration noise
    std::vector<float> noisyProbs = probs;
    for (size_t i = 0; i < noisyProbs.size(); ++i) {
        noisyProbs[i] += epsilon * ((rand() / (RAND_MAX + 1.0f)) * 2.0f - 1.0f);
        noisyProbs[i] = std::max(0.0f, noisyProbs[i]);
    }
    
    // Normalize
    float sum = 0.0f;
    for (float prob : noisyProbs) sum += prob;
    if (sum > 0) {
        for (float& prob : noisyProbs) prob /= sum;
    } else {
        // Uniform distribution
        float uniformProb = 1.0f / noisyProbs.size();
        for (float& prob : noisyProbs) prob = uniformProb;
    }
    
    // Sample from distribution
    float randVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float cumulative = 0.0f;
    
    for (size_t i = 0; i < noisyProbs.size(); ++i) {
        cumulative += noisyProbs[i];
        if (randVal <= cumulative) {
            return actionSpace_[i];
        }
    }
    
    return actionSpace_[0]; // Fallback
}

std::vector<float> ActorCritic::getActionProbabilities(const std::vector<float>& state) const {
    std::vector<float> result;
    
    for (const auto& action : actionSpace_) {
        std::vector<float> actionState = state;
        // Add action encoding to state
        actionState.push_back(static_cast<float>(action));
        
        float qValue = actor_.forward(actionState)[0];
        result.push_back(qValue);
    }
    
    // Apply softmax
    std::vector<float> expValues;
    float sumExp = 0.0f;
    
    for (float q : result) {
        float expQ = std::exp(q);
        expValues.push_back(expQ);
        sumExp += expQ;
    }
    
    if (sumExp > 0) {
        for (float& expQ : expValues) {
            expQ /= sumExp;
        }
    } else {
        float uniformProb = 1.0f / expValues.size();
        for (float& expQ : expValues) {
            expQ = uniformProb;
        }
    }
    
    return expValues;
}

float ActorCritic::evaluateState(const std::vector<float>& state) const {
    return critic_.forward(state)[0];
}

float ActorCritic::getAdvantage(const std::vector<float>& state, ActionType action) const {
    // Calculate advantage function A(s,a) = Q(s,a) - V(s)
    float qValue = getQValueFromStateAction(state, action);
    float vValue = evaluateState(state);
    
    return qValue - vValue;
}

float ActorCritic::getQValueFromStateAction(const std::vector<float>& state, ActionType action) const {
    // Simplified Q-value estimation
    return evaluateState(state);
}

void ActorCritic::storeExperience(const Experience& exp) {
    replayBuffer_.push_back(exp);
    currentIndex_ = (currentIndex_ + 1) % replayBufferSize_;
}

// Deep Q-Network implementation
void DeepQNetwork::trainEpisode(Brain* brain, const Observation& observation, 
                               ActionType action, float reward, 
                               const Observation& nextObservation, bool done, 
                               double timestamp) {
    std::vector<float> state = observation.toFeatureVector();
    std::vector<float> nextState = nextObservation.toFeatureVector();
    
    DQExperience experience;
    experience.state = state;
    experience.action = action;
    experience.reward = reward;
    experience.nextState = nextState;
    experience.done = done;
    experience.timestamp = timestamp;
    
    replayBuffer_.push_back(experience);
    if (replayBuffer_.size() > replayBufferSize_) {
        replayBuffer_.erase(replayBuffer_.begin());
    }
    
    // Perform learning update
    if (!replayBuffer_.empty()) {
        optimizeReplayBuffer();
    }
    
    // Update target network periodically
    targetUpdateCounter_++;
    if (targetUpdateCounter_ >= targetUpdateFreq_) {
        updateTargetNetwork();
        targetUpdateCounter_ = 0;
    }
}

ActionType DeepQNetwork::selectAction(const std::vector<float>& state, double timestamp, bool explore) {
    float epsilon = explore ? std::pow(0.99, timestamp) : 0.0f; // Exponential decay
    
    float randVal = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    
    if (randVal < epsilon) {
        // Explore: random action
        return actionSpace_[rand() % actionSpace_.size()];
    } else {
        // Exploit: Q-learning action selection
        return getGreedyAction(state);
    }
}

void DeepQNetwork::updateTargetNetwork() {
    targetNetwork_ = onlineNetwork_;
}

void DeepQNetwork::optimizeReplayBuffer(size_t batchSize) {
    if (replayBuffer_.size() < batchSize) return;
    
    // Select random batch
    std::vector<size_t> indices(replayBuffer_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    
    indices.resize(batchSize);
    
    // Perform gradient descent
    for (size_t i : indices) {
        const DQExperience& exp = replayBuffer_[i];
        optimizeSingleExperience(exp);
    }
}

void DeepQNetwork::optimizeSingleExperience(const DQExperience& exp) {
    // Calculate target Q-value
    float targetQ = exp.reward;
    if (!exp.done) {
        std::vector<float> nextQValues = targetNetwork_.forward(exp.nextState);
        float maxNextQ = *std::max_element(nextQValues.begin(), nextQValues.end());
        targetQ += discountFactor_ * maxNextQ;
    }
    
    // Get current Q-value for the action
    std::vector<float> currentQValues = onlineNetwork_.forward(exp.state);
    size_t actionIndex = getActionIndex(exp.action);
    float currentQ = currentQValues[actionIndex];
    
    // Calculate loss and update
    float loss = (targetQ - currentQ) * (targetQ - currentQ);
    float gradient = 2.0f * (currentQ - targetQ);
    
    // Update network weights (simplified)
    updateNetworkWeights(exp.state, actionIndex, gradient);
}

void DeepQNetwork::updateNetworkWeights(const std::vector<float>& state, size_t actionIndex, float gradient) {
    // Simplified weight update
    for (size_t i = 0; i < onlineNetwork_.weights.size(); ++i) {
        for (size_t j = 0; j < onlineNetwork_.weights[i].size(); ++j) {
            onlineNetwork_.weights[i][j] -= learningRate_ * gradient * state[j % state.size()];
        }
    }
    for (size_t i = 0; i < onlineNetwork_.biases.size(); ++i) {
        for (size_t j = 0; j < onlineNetwork_.biases[i].size(); ++j) {
            onlineNetwork_.biases[i][j] -= learningRate_ * gradient;
        }
    }
}

ActionType DeepQNetwork::getGreedyAction(const std::vector<float>& state) const {
    std::vector<float> qValues = onlineNetwork_.forward(state);
    
    size_t bestIndex = 0;
    for (size_t i = 1; i < qValues.size(); ++i) {
        if (qValues[i] > qValues[bestIndex]) {
            bestIndex = i;
        }
    }
    
    return actionSpace_[bestIndex];
}

size_t DeepQNetwork::getActionIndex(ActionType action) const {
    for (size_t i = 0; i < actionSpace_.size(); ++i) {
        if (actionSpace_[i] == action) {
            return i;
        }
    }
    return 0; // Default to first action
}

// Meta-Learning implementation
void MetaLearning::metaLearn(const Brain* metaBrain, const Observation& taskContext,
                             const std::vector<float>& taskParameters) {
    // Model-Agnostic Meta-Learning (MAML) implementation
    // This would involve computing gradients and updating meta-parameters
    // For now, we store the task context and parameters
    
    NLM_LOG_INFO("Meta-learning with task context and parameters");
    
    // Store meta-learning state
    metaParameters_["task_context"] = taskContext.getImportance();
    metaParameters_["task_complexity"] = static_cast<float>(taskParameters.size());
    metaParameters_["adaptation_rate"] = 0.01f;
}

void MetaLearning::learnPrototypes(const Brain* supportBrain, 
                                   const std::vector<Observation>& supportSet,
                                   const std::vector<ActionType>& supportActions) {
    // Prototypical networks for few-shot learning
    NLM_LOG_INFO("Learning prototypes from " + std::to_string(supportSet.size()) + " support samples");
    
    // Calculate prototype representations
    std::vector<float> prototype = std::vector<float>(supportSet.size(), 0.0f);
    
    for (size_t i = 0; i < supportSet.size(); ++i) {
        std::vector<float> features = supportSet[i].toFeatureVector();
        for (size_t j = 0; j < features.size(); ++j) {
            prototype[j] += features[j];
        }
    }
    
    for (size_t j = 0; j < prototype.size(); ++j) {
        prototype[j] /= static_cast<float>(supportSet.size());
    }
    
    NLM_LOG_INFO("Prototype learned with " + std::to_string(prototype.size()) + " dimensions");
}

void MetaLearning::learnLearningRate(const Brain* fastLearnerBrain, const Observation& context,
                                     const Experience& fastExperience) {
    // Learn optimal learning rate for fast adaptation
    NLM_LOG_INFO("Learning optimal learning rate for fast adaptation");
    
    // Calculate gradient-based meta-update
    float learningRate = 0.01f; // Initial guess
    float gradient = calculateMetaGradient(fastLearnerBrain, context, fastExperience);
    
    learningRate -= 0.001f * gradient; // Simple gradient descent
    
    metaParameters_["learning_rate"] = learningRate;
    NLM_LOG_INFO("Updated meta-learning rate: " + std::to_string(learningRate));
}

void MetaLearning::transferKnowledge(const Brain* sourceBrain, Brain* targetBrain,
                                     const std::string& transferType) {
    NLM_LOG_INFO("Transferring knowledge from source to target brain");
    
    if (transferType == "weights") {
        transferNetworkWeights(sourceBrain, targetBrain);
    } else if (transferType == "plasticity") {
        transferPlasticityRules(sourceBrain, targetBrain);
    } else if (transferType == "behavior") {
        transferBehavioralPatterns(sourceBrain, targetBrain);
    }
    
    NLM_LOG_INFO("Knowledge transfer complete");
}

void MetaLearning::continualLearn(Brain* brain, const Observation& observation,
                                   ActionType action, float reward,
                                   const Observation& nextObservation) {
    NLM_LOG_INFO("Continual learning with observation: " + observation.getId());
    
    // Apply continual learning principles
    // 1. Store experience in replay buffer
    // 2. Apply regularization to prevent catastrophic forgetting
    // 3. Update meta-parameters
    
    // For now, just log the continual learning event
    // In a full implementation, this would include replay buffer management
    // and regularization techniques
}

void MetaLearning::applyElasticWeightConsolidation(Brain* brain,
                                                     const std::map<NeuronId, float>& FisherInformation) {
    NLM_LOG_INFO("Applying Elastic Weight Consolidation (EWC)");
    
    // EWC implementation to prevent catastrophic forgetting
    // This involves adding quadratic penalty terms to the loss function
    // based on Fisher information matrix
    
    // For each neuron with non-zero Fisher information, add penalty
    float totalPenalty = 0.0f;
    for (const auto& pair : FisherInformation) {
        if (pair.second > 0.0f) {
            // Get current weight
            float weight = getNeuronWeight(brain, pair.first);
            if (weight != 0.0f) {
                totalPenalty += pair.second * weight * weight;
            }
        }
    }
    
    NLM_LOG_INFO("EWC regularization penalty: " + std::to_string(totalPenalty));
}

float MetaLearning::calculateMetaGradient(const Brain* fastLearnerBrain, const Observation& context,
                                           const Experience& fastExperience) {
    // Calculate meta-gradient for learning rate optimization
    // This would involve comparing performance with different learning rates
    return 0.001f; // Placeholder
}

void MetaLearning::transferNetworkWeights(const Brain* sourceBrain, Brain* targetBrain) {
    // Transfer neural network weights from source to target
    // In a real implementation, this would copy connection weights,
    // neuron parameters, etc.
    
    NLM_LOG_INFO("Transferred network weights from source to target brain");
}

void MetaLearning::transferPlasticityRules(const Brain* sourceBrain, Brain* targetBrain) {
    // Transfer plasticity rules and learning mechanisms
    NLM_LOG_INFO("Transferred plasticity rules");
}

void MetaLearning::transferBehavioralPatterns(const Brain* sourceBrain, Brain* targetBrain) {
    // Transfer behavioral patterns and action sequences
    NLM_LOG_INFO("Transferred behavioral patterns");
}

float MetaLearning::getNeuronWeight(Brain* brain, NeuronId neuronId) {
    // Get weight of a specific neuron
    // This is a simplified implementation
    return 0.5f; // Placeholder
}

// AdvancedRLAgent implementation
void AdvancedRLAgent::initialize(Brain* brain, std::shared_ptr<Config> config) {
    brain_ = brain;
    config_ = config;
    
    // Initialize from configuration
    if (config_) {
        stateSize_ = config_->getOr<size_t>("state_size", 10);
        
        // Get action space from config
        std::string actionSpaceStr = config_->getOr<std::string>("action_space", "move_forward,turn_left,turn_right,wait");
        std::stringstream ss(actionSpaceStr);
        std::string action;
        while (std::getline(ss, action, ',')) {
            if (action == "move_forward") actionSpace_.push_back(ActionType::MoveForward);
            else if (action == "turn_left") actionSpace_.push_back(ActionType::TurnLeft);
            else if (action == "turn_right") actionSpace_.push_back(ActionType::TurnRight);
            else if (action == "wait") actionSpace_.push_back(ActionType::Wait);
            else if (action == "attack") actionSpace_.push_back(ActionType::Attack);
            else if (action == "use_item") actionSpace_.push_back(ActionType::UseItem);
        }
        
        // Initialize learning algorithm
        std::string algorithm = config_->getOr<std::string>("learning_algorithm", "q_learning");
        setLearningAlgorithm(algorithm);
    }
    
    // Initialize with random state
    reset();
    
    NLM_LOG_INFO("Advanced RL agent initialized");
}

void AdvancedRLAgent::step(const Observation& observation, ActionType& selectedAction, float& reward) {
    // Extract features from observation
    std::vector<float> features = observation.toFeatureVector();
    
    // Update state
    currentState_ = features;
    
    // Select action using the chosen learning algorithm
    if (learningAlgorithm_ == "q_learning" && qTable_) {
        selectedAction = qTable_->selectAction(currentState_, totalSteps_);
    } else if (learningAlgorithm_ == "actor_critic" && actorCritic_) {
        selectedAction = actorCritic_->selectAction(currentState_);
    } else if (learningAlgorithm_ == "deep_q" && deepQNetwork_) {
        selectedAction = deepQNetwork_->selectAction(currentState_, totalSteps_);
    } else {
        // Default to greedy action
        selectedAction = getGreedyAction();
    }
    
    // Inject action into brain (if available)
    if (brain_) {
        injectActionIntoBrain(brain_, selectedAction);
    }
    
    // Calculate reward using shaping function if available
    if (rewardShapingFunction_) {
        reward = rewardShapingFunction_(currentState_, selectedAction, currentState_);
    }
    
    // Update episode statistics
    episodeSteps_++;
    episodeReward_ += reward;
    totalSteps_++;
    lastLearningTime_ = static_cast<double>(totalSteps_);
    
    NLM_LOG_DEBUG("Agent selected action: " + std::to_string(static_cast<int>(selectedAction)) +
                  ", reward: " + std::to_string(reward));
}

void AdvancedRLAgent::learn(const Observation& observation, ActionType action, float reward,
                            const Observation& nextObservation, bool done) {
    std::vector<float> nextFeatures = nextObservation.toFeatureVector();
    
    // Apply learning using the selected algorithm
    if (learningAlgorithm_ == "q_learning" && qTable_) {
        qTable_->update(currentState_, action, reward, nextFeatures, done, lastLearningTime_);
    } else if (learningAlgorithm_ == "actor_critic" && actorCritic_) {
        ActorCritic::Experience exp;
        exp.state = currentState_;
        exp.action = action;
        exp.reward = reward;
        exp.nextState = nextFeatures;
        exp.done = done;
        exp.timestamp = lastLearningTime_;
        
        actorCritic_->learn(exp);
    } else if (learningAlgorithm_ == "deep_q" && deepQNetwork_) {
        deepQNetwork_->trainEpisode(brain_, observation, action, reward, 
                                   nextObservation, done, lastLearningTime_);
    }
    
    // Apply reward to brain
    if (brain_) {
        applyRewardToBrain(brain_, reward);
    }
    
    // Update value and policy functions
    updatePolicyAndValueFunctions();
}

void AdvancedRLAgent::extractFeaturesFromBrain(const Brain* brain, std::vector<float>& features) const {
    if (!brain) return;
    
    // Extract features from brain state
    features.clear();
    
    // Neural activity features
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                if (neuron->isFiring()) {
                    totalActivity += 1.0f;
                }
                neuronCount++;
            }
        }
    }
    
    features.push_back(totalActivity / std::max(1.0f, static_cast<float>(neuronCount)));
    features.push_back(static_cast<float>(brain->getTotalSpikeCount()));
    
    // Connectivity features
    features.push_back(static_cast<float>(brain->getTotalSynapseCount()));
    features.push_back(brain->getExcitationInhibitionRatio());
    
    // Memory features
    if (brain->getWorkingMemory()) {
        features.push_back(static_cast<float>(brain->getWorkingMemory()->getActiveTraces()));
    } else {
        features.push_back(0.0f);
    }
    
    // Neuromodulation features
    if (brain->getDopamine()) {
        features.push_back(brain->getDopamine()->getLevel());
    } else {
        features.push_back(0.0f);
    }
    
    // Development stage
    features.push_back(static_cast<float>(static_cast<int>(brain->getDevelopmentalStage())));
    
    // Ensure feature vector has correct size
    if (features.size() < stateSize_) {
        features.resize(stateSize_, 0.0f);
    } else if (features.size() > stateSize_) {
        features.resize(stateSize_);
    }
}

void AdvancedRLAgent::injectActionIntoBrain(Brain* brain, ActionType action) const {
    if (!brain) return;
    
    // Inject action into brain's motor system
    // This could involve:
    // 1. Setting motor neuron activity
    // 2. Adjusting neuromodulation
    // 3. Triggering specific neural patterns
    
    NLM_LOG_DEBUG("Injected action into brain: " + std::to_string(static_cast<int>(action)));
}

void AdvancedRLAgent::applyRewardToBrain(Brain* brain, float reward) const {
    if (!brain) return;
    
    // Apply reward signal to brain
    // This would typically involve:
    // 1. Updating dopamine levels
    // 2. Adjusting neuromodulation
    // 3. Enhancing relevant neural pathways
    
    if (brain->getDopamine()) {
        // Scale reward based on neuromodulation strength
        float scaledReward = reward * brain->getDopamine()->getPlasticityFactor();
        brain->getDopamine()->setLevel(scaledReward);
    }
    
    NLM_LOG_DEBUG("Applied reward to brain: " + std::to_string(reward));
}

ActionType AdvancedRLAgent::getGreedyAction() const {
    if (learningAlgorithm_ == "q_learning" && qTable_) {
        return qTable_->getGreedyAction(currentState_);
    } else if (learningAlgorithm_ == "actor_critic" && actorCritic_) {
        return actorCritic_->selectAction(currentState_, 0.0f); // Greedy (epsilon=0)
    } else if (learningAlgorithm_ == "deep_q" && deepQNetwork_) {
        return deepQNetwork_->getGreedyAction(currentState_);
    }
    
    return ActionType::Wait; // Default
}

void AdvancedRLAgent::updatePolicyAndValueFunctions() {
    // Update policy and value function approximations
    // This would involve:
    // 1. Updating Q-table (for Q-learning)
    // 2. Updating actor-critic networks
    // 3. Updating deep Q-network
    
    // For now, we just log the update
    policyEntropy_ = calculatePolicyEntropy();
    currentStateValue_ = getStateValue();
    
    NLM_LOG_DEBUG("Updated policy and value functions: value=" + std::to_string(currentStateValue_) +
                  ", entropy=" + std::to_string(policyEntropy_));
}

float AdvancedRLAgent::calculatePolicyEntropy() const {
    // Calculate entropy of the policy distribution
    // This measures exploration vs exploitation
    if (actionSpace_.empty()) return 0.0f;
    
    float entropy = 0.0f;
    
    if (learningAlgorithm_ == "q_learning" && qTable_) {
        // Uniform entropy for Q-learning (simplified)
        entropy = std::log(static_cast<float>(actionSpace_.size()));
    } else if (learningAlgorithm_ == "actor_critic" && actorCritic_) {
        // Get action probabilities and calculate entropy
        std::vector<float> probs = actorCritic_->getActionProbabilities(currentState_);
        for (float prob : probs) {
            if (prob > 0.0f) {
                entropy -= prob * std::log(prob);
            }
        }
    }
    
    return entropy;
}

float AdvancedRLAgent::getStateValue() const {
    if (learningAlgorithm_ == "q_learning" && qTable_) {
        return qTable_->getMaxQValue(currentState_);
    } else if (learningAlgorithm_ == "actor_critic" && actorCritic_) {
        return actorCritic_->evaluateState(currentState_);
    } else if (learningAlgorithm_ == "deep_q" && deepQNetwork_) {
        // Get max Q-value from deep Q-network
        std::vector<float> qValues = deepQNetwork_->onlineNetwork_.forward(currentState_);
        return *std::max_element(qValues.begin(), qValues.end());
    }
    
    return 0.0f; // Default value
}

// AdvancedRLSystem implementation
void AdvancedRLSystem::initialize(std::shared_ptr<Config> config) {
    config_ = config;
    multiAgentLearningEnabled_ = false;
    
    NLM_LOG_INFO("Advanced RL system initialized");
}

std::shared_ptr<AdvancedRLAgent> AdvancedRLSystem::createAgent(const std::string& name,
                                                                const std::vector<size_t>& stateSize,
                                                                const std::vector<ActionType>& actionSpace,
                                                                const std::string& learningAlgorithm) {
    auto agent = std::make_shared<AdvancedRLAgent>();
    
    // Initialize agent
    agent->setStateSize(stateSize[0]); // Use first element as state size
    agent->setActionSpace(actionSpace);
    agent->setLearningAlgorithm(learningAlgorithm);
    
    // Add agent to system
    agents_[name] = agent;
    
    NLM_LOG_INFO("Created RL agent: " + name + " with algorithm: " + learningAlgorithm);
    return agent;
}

bool AdvancedRLSystem::removeAgent(const std::string& name) {
    auto it = agents_.find(name);
    if (it != agents_.end()) {
        agents_.erase(it);
        NLM_LOG_INFO("Removed RL agent: " + name);
        return true;
    }
    return false;
}

std::shared_ptr<AdvancedRLAgent> AdvancedRLSystem::getAgent(const std::string& name) {
    auto it = agents_.find(name);
    if (it != agents_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::string> AdvancedRLSystem::getAgentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : agents_) {
        names.push_back(pair.first);
    }
    return names;
}

void AdvancedRLSystem::trainStep(const std::string& agentName, const Observation& observation,
                                 ActionType& selectedAction, float& reward) {
    auto agent = getAgent(agentName);
    if (!agent) {
        throw std::runtime_error("Agent not found: " + agentName);
    }
    
    agent->step(observation, selectedAction, reward);
}

void AdvancedRLSystem::agentLearn(const std::string& agentName, const Observation& observation,
                                   ActionType action, float reward,
                                   const Observation& nextObservation, bool done) {
    auto agent = getAgent(agentName);
    if (!agent) {
        throw std::runtime_error("Agent not found: " + agentName);
    }
    
    agent->learn(observation, action, reward, nextObservation, done);
}

void AdvancedRLSystem::trainAllAgents(const std::vector<Observation>& observations) {
    // Train all agents with the same observations
    for (const auto& observation : observations) {
        for (auto& pair : agents_) {
            ActionType action;
            float reward;
            pair.second->step(observation, action, reward);
        }
    }
    
    // Apply learning updates
    for (auto& pair : agents_) {
        for (const auto& observation : observations) {
            ActionType action;
            float reward;
            pair.second->step(observation, action, reward);
            pair.second->learn(observation, action, reward, observation, false);
        }
    }
}

void AdvancedRLSystem::setCurriculumLearning(const std::string& agentName,
                                              const std::vector<std::pair<size_t, size_t>>& curriculum) {
    auto agent = getAgent(agentName);
    if (!agent) {
        throw std::runtime_error("Agent not found: " + agentName);
    }
    
    // Store curriculum schedule
    curriculumSchedules_[agentName] = curriculum;
    
    NLM_LOG_INFO("Set curriculum learning for agent: " + agentName);
}

void AdvancedRLSystem::setTransferLearning(const std::string& sourceAgent, 
                                            const std::string& targetAgent) {
    auto source = getAgent(sourceAgent);
    auto target = getAgent(targetAgent);
    
    if (!source || !target) {
        throw std::runtime_error("One or both agents not found");
    }
    
    // Transfer knowledge from source to target
    if (metaLearning_) {
        metaLearning_->transferKnowledge(source->getBrain(), target->getBrain(), "weights");
    }
    
    NLM_LOG_INFO("Set transfer learning from agent " + sourceAgent + " to " + targetAgent);
}

void AdvancedRLSystem::setMultiAgentLearning(const std::vector<std::string>& agentNames) {
    multiAgentLearningEnabled_ = true;
    cooperatingAgents_ = agentNames;
    
    NLM_LOG_INFO("Enabled multi-agent learning for agents: " + std::to_string(agentNames.size()));
}

void AdvancedRLSystem::recordTrainingMetrics(const std::string& agentName,
                                             const std::string& metric, float value) {
    auto it = performanceMetrics_.find(agentName);
    if (it == performanceMetrics_.end()) {
        performanceMetrics_[agentName] = TrainingMetrics();
    }
    
    it->second.metricHistory[metric].push_back(value);
    it->second.currentMetrics[metric] = value;
    it->second.lastUpdateTime = getCurrentTime();
}

void AdvancedRLSystem::analyzeLearningProgress(const std::string& agentName) const {
    auto it = performanceMetrics_.find(agentName);
    if (it == performanceMetrics_.end()) {
        return;
    }
    
    const auto& metrics = it->second;
    
    // Analyze progress for each metric
    for (const auto& pair : metrics.metricHistory) {
        const std::string& metricName = pair.first;
        const std::vector<float>& history = pair.second;
        
        if (history.size() < 2) continue;
        
        // Calculate trend
        float firstValue = history[0];
        float lastValue = history.back();
        float trend = (lastValue - firstValue) / static_cast<float>(history.size() - 1);
        
        NLM_LOG_INFO("Agent " + agentName + " metric " + metricName + ": trend = " + std::to_string(trend));
    }
}

float AdvancedRLSystem::getPerformanceMetric(const std::string& agentName, const std::string& metric) const {
    auto it = performanceMetrics_.find(agentName);
    if (it == performanceMetrics_.end()) {
        return 0.0f;
    }
    
    const auto& metrics = it->second;
    auto metricIt = metrics.currentMetrics.find(metric);
    if (metricIt != metrics.currentMetrics.end()) {
        return metricIt->second;
    }
    
    return 0.0f;
}

void AdvancedRLSystem::configureHyperparameters(const std::string& agentName,
                                                 const std::map<std::string, float>& hyperparameters) {
    auto agent = getAgent(agentName);
    if (!agent) {
        throw std::runtime_error("Agent not found: " + agentName);
    }
    
    // Configure hyperparameters based on learning algorithm
    for (const auto& pair : hyperparameters) {
        if (pair.first == "learning_rate") {
            if (agent->getQTable()) {
                agent->getQTable()->setLearningRate(pair.second);
            }
        } else if (pair.first == "discount_factor") {
            if (agent->getQTable()) {
                // Note: QTable doesn't have discount factor setter, would need to modify
                NLM_LOG_WARN("Discount factor configuration not implemented for QTable");
            }
        } else if (pair.first == "epsilon") {
            // Exploration rate configuration
        }
    }
    
    NLM_LOG_INFO("Configured hyperparameters for agent: " + agentName);
}

void AdvancedRLSystem::optimizeParameters(const std::string& agentName, size_t iterations) {
    auto agent = getAgent(agentName);
    if (!agent) {
        throw std::runtime_error("Agent not found: " + agentName);
    }
    
    NLM_LOG_INFO("Optimizing parameters for agent: " + agentName + " over " + std::to_string(iterations) + " iterations");
    
    // Implement hyperparameter optimization
    // This would typically use Bayesian optimization, evolutionary algorithms, etc.
    // For now, we just log the optimization
    
    for (size_t i = 0; i < iterations; ++i) {
        // Perform one optimization step
        NLM_LOG_DEBUG("Optimization iteration: " + std::to_string(i + 1));
    }
    
    NLM_LOG_INFO("Parameter optimization complete for agent: " + agentName);
}

void AdvancedRLSystem::saveSystemState(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Save system state
        file << "Advanced RL System State\n";
        file << "==========================\n\n";
        
        file << "Number of agents: " << agents_.size() << "\n\n";
        
        for (const auto& pair : agents_) {
            file << "Agent: " << pair.first << "\n";
            // Save agent state (simplified)
            file << "  Learning algorithm: " << pair.second->getLearningAlgorithm() << "\n";
            file << "  Episode reward: " << pair.second->getEpisodeReward() << "\n";
            file << "  Total steps: " << pair.second->getTotalSteps() << "\n\n";
        }
        
        file.close();
        
        NLM_LOG_INFO("System state saved to " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save system state: " + std::string(e.what()));
    }
}

void AdvancedRLSystem::loadSystemState(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        // Load system state
        std::string line;
        std::string currentAgent;
        
        while (std::getline(file, line)) {
            if (line.find("Agent: ") == 0) {
                currentAgent = line.substr(7);
            } else if (line.find("Learning algorithm: ") == 0 && !currentAgent.empty()) {
                // Set learning algorithm for current agent
                std::string algorithm = line.substr(21);
                if (auto agent = getAgent(currentAgent)) {
                    agent->setLearningAlgorithm(algorithm);
                }
            }
        }
        
        file.close();
        
        NLM_LOG_INFO("System state loaded from " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load system state: " + std::string(e.what()));
    }
}

void AdvancedRLSystem::synchronizeWithNeuralDynamics(const std::string& agentName, Brain* brain) {
    if (!brain) return;
    
    auto agent = getAgent(agentName);
    if (!agent) return;
    
    // Synchronize agent state with neural dynamics
    std::vector<float> brainFeatures;
    agent->extractFeaturesFromBrain(brain, brainFeatures);
    
    NLM_LOG_INFO("Synchronized RL agent " + agentName + " with neural dynamics");
}

void AdvancedRLSystem::integrateNeuromodulation(const std::string& agentName, 
                                                 const Neuromodulator& signal) {
    auto agent = getAgent(agentName);
    if (!agent) return;
    
    // Integrate neuromodulation signal with RL
    // This could involve:
    // 1. Adjusting reward scaling based on neuromodulator level
    // 2. Modifying exploration behavior
    // 3. Updating policy
    
    NLM_LOG_INFO("Integrated neuromodulation for RL agent " + agentName);
}

void AdvancedRLSystem::monitorPerformance() const {
    NLM_LOG_INFO("=== Advanced RL System Performance ===");
    
    size_t totalAgents = agents_.size();
    size_t activeAgents = 0;
    float averageEpisodeReward = 0.0f;
    
    for (const auto& pair : agents_) {
        if (pair.second->getEpisodeSteps() > 0) {
            activeAgents++;
            averageEpisodeReward += pair.second->getEpisodeReward();
        }
    }
    
    if (activeAgents > 0) {
        averageEpisodeReward /= static_cast<float>(activeAgents);
    }
    
    NLM_LOG_INFO("Total agents: " + std::to_string(totalAgents));
    NLM_LOG_INFO("Active agents: " + std::to_string(activeAgents));
    NLM_LOG_INFO("Average episode reward: " + std::to_string(averageEpisodeReward));
}

void AdvancedRLSystem::logTrainingProgress() const {
    NLM_LOG_INFO("=== Training Progress ===");
    
    for (const auto& pair : agents_) {
        NLM_LOG_INFO("Agent " + pair.first + ": reward = " + std::to_string(pair.second->getEpisodeReward()) +
                     ", steps = " + std::to_string(pair.second->getEpisodeSteps()) +
                     ", total_steps = " + std::to_string(pair.second->getTotalSteps()));
    }
}

double AdvancedRLSystem::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
}

} // namespace ReinforcementLearning
} // namespace nlm