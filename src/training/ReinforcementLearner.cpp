#include "ReinforcementLearner.hpp"
#include "../core/Logger/Logger.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <random>

namespace nlm {

// ============= EXPERIENCE REPLAY IMPLEMENTATION =============

ExperienceReplay::ExperienceReplay(size_t capacity) : capacity_(capacity), head_(0) {
}

void ExperienceReplay::add(const TrainingExperience& experience) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (experiences_.size() < capacity_) {
        experiences_.push_back(experience);
    } else {
        experiences_[head_] = experience;
        head_ = (head_ + 1) % capacity_;
    }
}

TrainingExperience ExperienceReplay::sample() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (experiences_.empty()) {
        throw NLMRuntimeError("Cannot sample from empty experience replay buffer");
    }
    
    std::uniform_int_distribution<size_t> dist(0, experiences_.size() - 1);
    size_t index = dist(rng_);
    return experiences_[index];
}

TrainingExperience ExperienceReplay::sampleUniform() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (experiences_.empty()) {
        throw NLMRuntimeError("Cannot sample from empty experience replay buffer");
    }
    
    std::uniform_int_distribution<size_t> dist(0, experiences_.size() - 1);
    size_t index = dist(rng_);
    return experiences_[index];
}

bool ExperienceReplay::isReady(size_t minSize) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return experiences_.size() >= minSize;
}

void ExperienceReplay::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    experiences_.clear();
    head_ = 0;
}

// ============= Q-TABLE IMPLEMENTATION =============

QTable::QTable(int stateSpaceSize, int actionSpaceSize)
    : stateSpaceSize_(stateSpaceSize), actionSpaceSize_(actionSpaceSize), seed_(12345) {
    // Initialize with small random values
    std::mt19937 rng(seed_);
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    stateTable_.resize(stateSpaceSize, std::vector<float>(actionSpaceSize, 0.0f));
    actionTable_.resize(stateSpaceSize, std::vector<float>(actionSpaceSize, 0.0f));
    visitCountTable_.resize(stateSpaceSize, std::vector<int>(actionSpaceSize, 0));
    
    // Initialize with random weights
    for (int i = 0; i < stateSpaceSize; ++i) {
        for (int j = 0; j < actionSpaceSize; ++j) {
            actionTable_[i][j] = dist(rng);
        }
    }
}

float QTable::getValue(const std::vector<float>& state, int action) const {
    size_t stateHash = hashState(state);
    if (stateHash >= static_cast<size_t>(stateSpaceSize_)) {
        return 0.0f;
    }
    
    if (action < 0 || action >= actionSpaceSize_) {
        return 0.0f;
    }
    
    return actionTable_[stateHash][action];
}

void QTable::setValue(const std::vector<float>& state, int action, float value) {
    size_t stateHash = hashState(state);
    if (stateHash < static_cast<size_t>(stateSpaceSize_) &&
        action >= 0 && action < actionSpaceSize_) {
        actionTable_[stateHash][action] = value;
    }
}

int QTable::getBestAction(const std::vector<float>& state) const {
    size_t stateHash = hashState(state);
    if (stateHash >= static_cast<size_t>(stateSpaceSize_)) {
        return 0;
    }
    
    int bestAction = 0;
    float bestValue = std::numeric_limits<float>::lowest();
    
    for (int action = 0; action < actionSpaceSize_; ++action) {
        if (actionTable_[stateHash][action] > bestValue) {
            bestValue = actionTable_[stateHash][action];
            bestAction = action;
        }
    }
    
    return bestAction;
}

void QTable::update(const std::vector<float>& state, int action,
                   float reward, const std::vector<float>& nextState,
                   int nextAction, float alpha, float gamma) {
    size_t stateHash = hashState(state);
    size_t nextStateHash = hashState(nextState);
    
    if (stateHash >= static_cast<size_t>(stateSpaceSize_) ||
        nextStateHash >= static_cast<size_t>(stateSpaceSize_) ||
        action < 0 || action >= actionSpaceSize_ ||
        nextAction < 0 || nextAction >= actionSpaceSize_) {
        return;
    }
    
    float currentValue = actionTable_[stateHash][action];
    float nextValue = actionTable_[nextStateHash][nextAction];
    
    float tdTarget = reward + gamma * nextValue;
    float tdError = tdTarget - currentValue;
    
    actionTable_[stateHash][action] += alpha * tdError;
    
    // Increment visit count
    if (stateHash < static_cast<size_t>(stateSpaceSize_) &&
        action >= 0 && action < actionSpaceSize_) {
        visitCountTable_[stateHash][action]++;
    }
}

void QTable::incrementVisitCount(const std::vector<float>& state, int action) {
    size_t stateHash = hashState(state);
    if (stateHash < static_cast<size_t>(stateSpaceSize_) &&
        action >= 0 && action < actionSpaceSize_) {
        visitCountTable_[stateHash][action]++;
    }
}

float QTable::getVisitCount(const std::vector<float>& state, int action) const {
    size_t stateHash = hashState(state);
    if (stateHash >= static_cast<size_t>(stateSpaceSize_) ||
        action < 0 || action >= actionSpaceSize_) {
        return 0.0f;
    }
    
    return static_cast<float>(visitCountTable_[stateHash][action]);
}

size_t QTable::hashState(const std::vector<float>& state) const {
    // Simple hash function for state vector
    size_t hash = 0;
    for (size_t i = 0; i < std::min(state.size(), static_cast<size_t>(10)); ++i) { // Use first 10 values
        hash = (hash * 31) + static_cast<size_t>(std::round(state[i] * 100)); // Scale and discretize
    }
    return hash % stateSpaceSize_;
}

// ============= NEURAL NETWORK UTILS IMPLEMENTATION =============

std::vector<float> NeuralNetworkUtils::softmax(const std::vector<float>& logits) {
    std::vector<float> expValues;
    float maxLogit = *std::max_element(logits.begin(), logits.end());
    
    for (float logit : logits) {
        expValues.push_back(std::exp(logit - maxLogit));
    }
    
    float sum = std::accumulate(expValues.begin(), expValues.end(), 0.0f);
    
    for (float& value : expValues) {
        value /= sum;
    }
    
    return expValues;
}

float NeuralNetworkUtils::entropy(const std::vector<float>& probabilities) {
    float entropy = 0.0f;
    for (float p : probabilities) {
        if (p > 0.0f) {
            entropy -= p * std::log(p);
        }
    }
    return entropy;
}

float NeuralNetworkUtils::clippedGradient(float gradient, float clipNorm) {
    if (gradient > clipNorm) return clipNorm;
    if (gradient < -clipNorm) return -clipNorm;
    return gradient;
}

std::vector<float> NeuralNetworkUtils::oneHotEncode(int action, int actionSpaceSize) {
    std::vector<float> oneHot(actionSpaceSize, 0.0f);
    if (action >= 0 && action < actionSpaceSize) {
        oneHot[action] = 1.0f;
    }
    return oneHot;
}

std::vector<float> NeuralNetworkUtils::muSigmaToDistribution(float mu, float sigma) {
    std::vector<float> distribution(2);
    distribution[0] = mu; // mean
    distribution[1] = sigma; // standard deviation
    return distribution;
}

// ============= REINFORCEMENT LEARNER FACTORY IMPLEMENTATION =============

std::unique_ptr<ReinforcementLearner> ReinforcementLearnerFactory::createLearner(
    RLAlgorithm algorithm,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<Config> config) {
    
    switch (algorithm) {
        case RLAlgorithm::TABULAR_Q_LEARNING:
            return std::make_unique<TabularQLearning>(brain, agent, world, config);
        case RLAlgorithm::DEEP_Q_NETWORK:
            return std::make_unique<DeepQLearning>(brain, agent, world, config);
        case RLAlgorithm::POLICY_GRADIENT:
            return std::make_unique<PolicyGradientLearner>(brain, agent, world, config);
        default:
            // Default to tabular Q-learning
            return std::make_unique<TabularQLearning>(brain, agent, world, config);
    }
}

RLAlgorithm ReinforcementLearnerFactory::stringToAlgorithm(const std::string& algorithmName) {
    std::string lowerName = algorithmName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName == "q-learning" || lowerName == "tabular_q_learning") {
        return RLAlgorithm::TABULAR_Q_LEARNING;
    } else if (lowerName == "deep_q_network" || lowerName == "dqn") {
        return RLAlgorithm::DEEP_Q_NETWORK;
    } else if (lowerName == "policy_gradient" || lowerName == "pg") {
        return RLAlgorithm::POLICY_GRADIENT;
    } else if (lowerName == "ppo") {
        return RLAlgorithm::PPO;
    } else if (lowerName == "a2c") {
        return RLAlgorithm::A2C;
    } else {
        return RLAlgorithm::TABULAR_Q_LEARNING; // Default
    }
}

std::string ReinforcementLearnerFactory::algorithmToString(RLAlgorithm algorithm) {
    switch (algorithm) {
        case RLAlgorithm::TABULAR_Q_LEARNING: return "Tabular Q-Learning";
        case RLAlgorithm::DEEP_Q_NETWORK: return "Deep Q-Network";
        case RLAlgorithm::POLICY_GRADIENT: return "Policy Gradient";
        case RLAlgorithm::Q_LEARNING: return "Q-Learning";
        case RLAlgorithm::SARSA: return "SARSA";
        case RLAlgorithm::EXPECTATION_MAXIMIZATION: return "Expectation Maximization";
        case RLAlgorithm::PPO: return "Proximal Policy Optimization";
        case RLAlgorithm::A2C: return "Advantage Actor-Critic";
        case RLAlgorithm::DDPG: return "Deep Deterministic Policy Gradient";
        case RLAlgorithm::SAC: return "Soft Actor-Critic";
        default: return "Unknown";
    }
}

std::vector<std::string> ReinforcementLearnerFactory::getSupportedAlgorithms() {
    return {
        "Tabular Q-Learning",
        "Deep Q-Network", 
        "Policy Gradient",
        "Q-Learning",
        "SARSA",
        "Expectation Maximization",
        "Proximal Policy Optimization",
        "Advantage Actor-Critic",
        "Deep Deterministic Policy Gradient",
        "Soft Actor-Critic"
    };
}

// ============= TABULAR Q-LEARNING IMPLEMENTATION =============

TabularQLearning::TabularQLearning(std::shared_ptr<Brain> brain,
                                 std::shared_ptr<AgentBrain> agent,
                                 std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<Config> config)
    : ReinforcementLearner(brain, agent, world, config) {
    
    // Set algorithm type
    algorithm_ = RLAlgorithm::TABULAR_Q_LEARNING;
    explorationStrategy_ = ExplorationStrategy::EPSILON_GREEDY;
    
    // Initialize Q-table with state and action space sizes
    int stateSpaceSize = config->getOr<int>("state_space_size", 100);
    int actionSpaceSize = config->getOr<int>("action_space_size", 16);
    qTable_ = std::make_unique<QTable>(stateSpaceSize, actionSpaceSize);
    
    // Configure hyperparameters
    alpha_ = config->getOr<float>("learning_rate", 0.1f);
    gamma_ = config->getOr<float>("discount_factor", 0.99f);
    epsilon_ = config->getOr<float>("exploration_rate", 0.1f);
    temperature_ = config->getOr<float>("temperature", 1.0f);
    
    // Set state and action space sizes
    stateSpaceSize_ = stateSpaceSize;
    actionSpaceSize_ = actionSpaceSize;
    
    // Initialize experience replay
    size_t replayCapacity = config->getOr<size_t>("experience_replay_capacity", 10000);
    experienceReplay_ = std::make_unique<ExperienceReplay>(replayCapacity);
    
    NLM_LOG_INFO("Tabular Q-Learning initialized with alpha=" + std::to_string(alpha_) +
                ", gamma=" + std::to_string(gamma_) + ", epsilon=" + std::to_string(epsilon_));
}

bool TabularQLearning::train(size_t numEpisodes) {
    episodeCount_ = 0;
    totalReward_ = 0.0;
    
    NLM_LOG_INFO("Starting Tabular Q-Learning training for " + std::to_string(numEpisodes) + " episodes");
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        bool success = exploreAndLearnOneEpisode();
        if (!success) {
            NLM_LOG_WARN("Episode " + std::to_string(episode + 1) + " failed, continuing");
        }
        
        episodeCount_++;
    }
    
    NLM_LOG_INFO("Tabular Q-Learning training completed. Episodes: " + std::to_string(episodeCount_));
    return true;
}

bool TabularQLearning::predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) {
    return exploit(state, action);
}

bool TabularQLearning::learn() {
    if (!canLearn()) {
        return false;
    }
    
    // Sample a batch from experience replay
    std::vector<TrainingExperience> batch;
    size_t batchSize = config_->getOr<size_t>("batch_size", 32);
    
    if (experienceReplay_->size() >= batchSize) {
        for (size_t i = 0; i < batchSize; ++i) {
            batch.push_back(experienceReplay_->sample());
        }
    } else {
        return false; // Not enough experiences to learn
    }
    
    // Update Q-table with batch
    float alpha = alpha_;
    float gamma = gamma_;
    
    for (const auto& experience : batch) {
        int currentAction = experience.action;
        int bestNextAction = qTable_->getBestAction(experience.nextState);
        
        qTable_->update(experience.state, currentAction,
                       experience.reward, experience.nextState,
                       bestNextAction, alpha, gamma);
    }
    
    return true;
}

bool TabularQLearning::explore(const std::vector<float>& state, int& action) {
    // Epsilon-greedy exploration
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(rng_) < epsilon_) {
        // Explore: random action
        std::uniform_int_distribution<int> actionDist(0, actionSpaceSize_ - 1);
        action = actionDist(rng_);
        
        // Increment visit count for exploration
        qTable_->incrementVisitCount(state, action);
        return true; // Exploration
    }
    
    // Exploit: use Q-table to select best action
    return exploit(state, action);
}

bool TabularQLearning::exploit(const std::vector<float>& state, int& action) {
    // Use Q-table to select best action
    action = qTable_->getBestAction(state);
    
    // Increment visit count for exploitation
    qTable_->incrementVisitCount(state, action);
    return true; // Exploitation
}

bool TabularQLearning::saveModel(const std::string& filepath) {
    // Save Q-table parameters
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Save hyperparameters
        file.write(reinterpret_cast<const char*>(&alpha_), sizeof(alpha_));
        file.write(reinterpret_cast<const char*>(&gamma_), sizeof(gamma_));
        file.write(reinterpret_cast<const char*>(&epsilon_), sizeof(epsilon_));
        file.write(reinterpret_cast<const char*>(&temperature_), sizeof(temperature_));
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save Q-table model: " + std::string(e.what()));
        return false;
    }
}

bool TabularQLearning::loadModel(const std::string& filepath) {
    // Load Q-table parameters
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Load hyperparameters
        file.read(reinterpret_cast<char*>(&alpha_), sizeof(alpha_));
        file.read(reinterpret_cast<char*>(&gamma_), sizeof(gamma_));
        file.read(reinterpret_cast<char*>(&epsilon_), sizeof(epsilon_));
        file.read(reinterpret_cast<char*>(&temperature_), sizeof(temperature_));
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load Q-table model: " + std::string(e.what()));
        return false;
    }
}

bool TabularQLearning::storeExperience(const TrainingExperience& experience) {
    experienceReplay_->add(experience);
    experienceCount_++;
    return true;
}

bool TabularQLearning::canLearn() const {
    return experienceReplay_->isReady(config_->getOr<size_t>("min_experience_for_learning", 100));
}

void TabularQLearning::clearMemory() {
    experienceReplay_->clear();
    episodicMemory_.clear();
}

bool TabularQLearning::evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) {
    float totalReward = 0.0f;
    
    for (size_t episode = 0; episode < evalEpisodes; ++episode) {
        float episodeReward = simulateEpisode(brain, true); // Evaluation mode (no exploration)
        totalReward += episodeReward;
    }
    
    evaluationScore_ = totalReward / evalEpisodes;
    return true;
}

bool TabularQLearning::exploreAndLearnOneEpisode() {
    // Generate random initial state
    std::vector<float> state = generateRandomState();
    
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        // Select action
        int action;
        if (!explore(state, action)) {
            return false;
        }
        
        // Simulate environment transition (simplified)
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        // Check if episode is done
        done = (steps >= maxSteps - 1) || (reward < -10.0f); // Simple termination condition
        
        // Store experience
        TrainingExperience experience;
        experience.state = state;
        experience.action = action;
        experience.reward = reward;
        experience.nextState = nextState;
        experience.done = done;
        experience.timestamp = getCurrentTime();
        experience.step = totalSteps_++;
        
        if (!storeExperience(experience)) {
            return false;
        }
        
        // Learn from experience
        if (canLearn()) {
            learn();
        }
        
        // Update state
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    totalReward_ += totalReward;
    return true;
}

float TabularQLearning::simulateEpisode(std::shared_ptr<Brain> brain, bool evaluate) {
    // Simulate one episode for evaluation
    std::vector<float> state = generateRandomState();
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        int action;
        if (evaluate) {
            if (!exploit(state, action)) return -1.0f;
        } else {
            if (!explore(state, action)) return -1.0f;
        }
        
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        done = (steps >= maxSteps - 1) || (reward < -10.0f);
        
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    return totalReward;
}

std::vector<float> TabularQLearning::generateRandomState() {
    std::vector<float> state(stateSpaceSize_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < stateSpaceSize_; ++i) {
        state[i] = dist(rng_);
    }
    
    return state;
}

std::vector<float> TabularQLearning::generateNextState(const std::vector<float>& state, int action) {
    std::vector<float> nextState = state;
    
    // Simple state transition model
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < nextState.size(); ++i) {
        // Add some stochasticity
        nextState[i] += dist(rng_);
        
        // Clip to valid range
        nextState[i] = std::max(0.0f, std::min(1.0f, nextState[i]));
    }
    
    // Modify one dimension based on action
    if (!nextState.empty() && action >= 0 && action < actionSpaceSize_) {
        nextState[action % nextState.size()] = std::min(1.0f, nextState[action % nextState.size()] + 0.2f);
    }
    
    return nextState;
}

float TabularQLearning::simulateAction(const std::vector<float>& state, int action, float accumulatedReward) {
    // Simple reward function based on state and action
    float reward = 0.0f;
    
    // Reward based on action (higher action number gets more reward)
    reward += static_cast<float>(action) * 0.1f;
    
    // Reward based on state values
    float stateSum = std::accumulate(state.begin(), state.end(), 0.0f);
    reward += stateSum * 0.05f;
    
    // Add some stochasticity
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    reward += dist(rng_);
    
    return reward;
}

std::string TabularQLearning::getStatsSummary() const {
    std::stringstream ss;
    ss << "=== Tabular Q-Learning Statistics ===\n";
    ss << "Algorithm: " << ReinforcementLearnerFactory::algorithmToString(algorithm_) << "\n";
    ss << "Exploration Strategy: " << static_cast<int>(explorationStrategy_) << "\n";
    ss << "State Space Size: " << stateSpaceSize_ << "\n";
    ss << "Action Space Size: " << actionSpaceSize_ << "\n";
    ss << "Hyperparameters: alpha=" << alpha_ << ", gamma=" << gamma_ << ", epsilon=" << epsilon_ << "\n";
    ss << "Experience Count: " << experienceCount_ << "\n";
    ss << "Episode Count: " << episodeCount_ << "\n";
    ss << "Total Reward: " << totalReward_ << "\n";
    ss << "Average Reward: " << (episodeCount_ > 0 ? totalReward_ / episodeCount_ : 0.0) << "\n";
    ss << "Experience Replay Buffer Size: " << experienceReplay_->size() << "\n";
    ss << "Q-Table Size: " << stateSpaceSize_ * actionSpaceSize_ << " values\n";
    ss << "==================================\n";
    
    return ss.str();
}

// ============= DEEP Q-NETWORK IMPLEMENTATION =============

DeepQLearning::DeepQLearning(std::shared_ptr<Brain> brain,
                           std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<Config> config)
    : ReinforcementLearner(brain, agent, world, config) {
    
    // Set algorithm type
    algorithm_ = RLAlgorithm::DEEP_Q_NETWORK;
    explorationStrategy_ = ExplorationStrategy::EPSILON_GREEDY;
    
    // Configure hyperparameters
    alpha_ = config->getOr<float>("learning_rate", 0.001f);
    gamma_ = config->getOr<float>("discount_factor", 0.99f);
    epsilon_ = config->getOr<float>("exploration_rate", 0.1f);
    temperature_ = config->getOr<float>("temperature", 1.0f);
    
    // Initialize neural network parameters
    int inputSize = config->getOr<int>("state_space_size", 100);
    int hiddenSize = config->getOr<int>("hidden_layer_size", 128);
    int outputSize = config->getOr<int>("action_space_size", 16);
    
    // Initialize weights (simplified - in real implementation would use proper neural network)
    neuralNetworkWeights_.resize(2); // Two layers: input->hidden, hidden->output
    neuralNetworkBiases_.resize(2);
    
    neuralNetworkWeights_[0].resize(inputSize * hiddenSize);
    neuralNetworkWeights_[1].resize(hiddenSize * outputSize);
    
    neuralNetworkBiases_[0].resize(hiddenSize);
    neuralNetworkBiases_[1].resize(outputSize);
    
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (auto& layerWeights : neuralNetworkWeights_) {
        for (float& weight : layerWeights) {
            weight = dist(rng);
        }
    }
    
    for (auto& layerBias : neuralNetworkBiases_) {
        for (float& bias : layerBias) {
            bias = dist(rng);
        }
    }
    
    // Set state and action space sizes
    stateSpaceSize_ = inputSize;
    actionSpaceSize_ = outputSize;
    
    // Initialize experience replay
    size_t replayCapacity = config->getOr<size_t>("experience_replay_capacity", 10000);
    experienceReplay_ = std::make_unique<ExperienceReplay>(replayCapacity);
    
    NLM_LOG_INFO("Deep Q-Network initialized with alpha=" + std::to_string(alpha_) +
                ", gamma=" + std::to_string(gamma_) + ", epsilon=" + std::to_string(epsilon_));
}

bool DeepQLearning::train(size_t numEpisodes) {
    episodeCount_ = 0;
    totalReward_ = 0.0;
    
    NLM_LOG_INFO("Starting Deep Q-Network training for " + std::to_string(numEpisodes) + " episodes");
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        bool success = exploreAndLearnOneEpisode();
        if (!success) {
            NLM_LOG_WARN("Episode " + std::to_string(episode + 1) + " failed, continuing");
        }
        
        episodeCount_++;
    }
    
    NLM_LOG_INFO("Deep Q-Network training completed. Episodes: " + std::to_string(episodeCount_));
    return true;
}

bool DeepQLearning::predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) {
    return exploit(state, action);
}

bool DeepQLearning::learn() {
    if (!canLearn()) {
        return false;
    }
    
    // Sample a batch from experience replay
    std::vector<TrainingExperience> batch;
    size_t batchSize = config_->getOr<size_t>("batch_size", 32);
    
    if (experienceReplay_->size() >= batchSize) {
        for (size_t i = 0; i < batchSize; ++i) {
            batch.push_back(experienceReplay_->sample());
        }
    } else {
        return false; // Not enough experiences to learn
    }
    
    // Update neural network with batch
    updateNeuralNetwork(batch);
    
    return true;
}

bool DeepQLearning::explore(const std::vector<float>& state, int& action) {
    // Epsilon-greedy exploration
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    if (dist(rng_) < epsilon_) {
        // Explore: random action
        std::uniform_int_distribution<int> actionDist(0, actionSpaceSize_ - 1);
        action = actionDist(rng_);
        return true; // Exploration
    }
    
    // Exploit: use neural network to select best action
    return exploit(state, action);
}

bool DeepQLearning::exploit(const std::vector<float>& state, int& action) {
    // Use neural network to compute Q-values and select best action
    std::vector<float> qValues = computeQValues(state);
    
    int bestAction = 0;
    float bestValue = std::numeric_limits<float>::lowest();
    
    for (int i = 0; i < actionSpaceSize_; ++i) {
        if (qValues[i] > bestValue) {
            bestValue = qValues[i];
            bestAction = i;
        }
    }
    
    action = bestAction;
    return true; // Exploitation
}

bool DeepQLearning::saveModel(const std::string& filepath) {
    // Save neural network parameters
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Save hyperparameters
        file.write(reinterpret_cast<const char*>(&alpha_), sizeof(alpha_));
        file.write(reinterpret_cast<const char*>(&gamma_), sizeof(gamma_));
        file.write(reinterpret_cast<const char*>(&epsilon_), sizeof(epsilon_));
        file.write(reinterpret_cast<const char*>(&temperature_), sizeof(temperature_));
        
        // Save neural network weights (simplified)
        for (const auto& layerWeights : neuralNetworkWeights_) {
            file.write(reinterpret_cast<const char*>(layerWeights.data()),
                      layerWeights.size() * sizeof(float));
        }
        
        for (const auto& layerBias : neuralNetworkBiases_) {
            file.write(reinterpret_cast<const char*>(layerBias.data()),
                      layerBias.size() * sizeof(float));
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save Deep Q-Network model: " + std::string(e.what()));
        return false;
    }
}

bool DeepQLearning::loadModel(const std::string& filepath) {
    // Load neural network parameters
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Load hyperparameters
        file.read(reinterpret_cast<char*>(&alpha_), sizeof(alpha_));
        file.read(reinterpret_cast<char*>(&gamma_), sizeof(gamma_));
        file.read(reinterpret_cast<char*>(&epsilon_), sizeof(epsilon_));
        file.read(reinterpret_cast<char*>(&temperature_), sizeof(temperature_));
        
        // Load neural network weights (simplified)
        for (auto& layerWeights : neuralNetworkWeights_) {
            file.read(reinterpret_cast<char*>(layerWeights.data()),
                     layerWeights.size() * sizeof(float));
        }
        
        for (auto& layerBias : neuralNetworkBiases_) {
            file.read(reinterpret_cast<char*>(layerBias.data()),
                     layerBias.size() * sizeof(float));
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load Deep Q-Network model: " + std::string(e.what()));
        return false;
    }
}

bool DeepQLearning::storeExperience(const TrainingExperience& experience) {
    experienceReplay_->add(experience);
    experienceCount_++;
    return true;
}

bool DeepQLearning::canLearn() const {
    return experienceReplay_->isReady(config_->getOr<size_t>("min_experience_for_learning", 100));
}

void DeepQLearning::clearMemory() {
    experienceReplay_->clear();
}

bool DeepQLearning::evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) {
    float totalReward = 0.0f;
    
    for (size_t episode = 0; episode < evalEpisodes; ++episode) {
        float episodeReward = simulateEpisode(brain, true); // Evaluation mode (no exploration)
        totalReward += episodeReward;
    }
    
    evaluationScore_ = totalReward / evalEpisodes;
    return true;
}

std::vector<float> DeepQLearning::computeQValues(const std::vector<float>& state) const {
    // Simple feedforward neural network (2 layers)
    std::vector<float> hiddenLayer(stateSpaceSize_); // Size should match input
    
    // First layer: input -> hidden
    for (int j = 0; j < static_cast<int>(hiddenLayer.size()); ++j) {
        float sum = 0.0f;
        for (int i = 0; i < static_cast<int>(state.size()); ++i) {
            if (i * static_cast<int>(neuralNetworkWeights_[0].size()) / static_cast<int>(state.size()) + j < 
                static_cast<int>(neuralNetworkWeights_[0].size())) {
                int weightIndex = i * static_cast<int>(neuralNetworkWeights_[0].size()) / static_cast<int>(state.size()) + j;
                sum += state[i] * neuralNetworkWeights_[0][weightIndex];
            }
        }
        
        // ReLU activation
        hiddenLayer[j] = std::max(0.0f, sum + neuralNetworkBiases_[0][j]);
    }
    
    // Second layer: hidden -> output
    std::vector<float> outputLayer(actionSpaceSize_);
    for (int j = 0; j < actionSpaceSize_; ++j) {
        float sum = 0.0f;
        for (int i = 0; i < static_cast<int>(hiddenLayer.size()); ++i) {
            if (i * actionSpaceSize_ + j < static_cast<int>(neuralNetworkWeights_[1].size())) {
                int weightIndex = i * actionSpaceSize_ + j;
                sum += hiddenLayer[i] * neuralNetworkWeights_[1][weightIndex];
            }
        }
        
        // Linear activation (no softmax for Q-learning)
        outputLayer[j] = sum + neuralNetworkBiases_[1][j];
    }
    
    return outputLayer;
}

void DeepQLearning::updateNeuralNetwork(const std::vector<TrainingExperience>& batch) {
    // Simplified neural network training (placeholder for real implementation)
    float alpha = alpha_;
    float gamma = gamma_;
    
    for (const auto& experience : batch) {
        // Compute target Q-value
        float target = experience.reward;
        if (!experience.done) {
            std::vector<float> nextQValues = computeQValues(experience.nextState);
            float maxNextQ = *std::max_element(nextQValues.begin(), nextQValues.end());
            target += gamma * maxNextQ;
        }
        
        // Compute current Q-value
        std::vector<float> currentQValues = computeQValues(experience.state);
        float currentQ = currentQValues[experience.action];
        
        // Compute gradient and update weights (simplified)
        float error = target - currentQ;
        
        // In a real implementation, this would update neural network weights
        // For now, just log the error
        NLM_LOG_DEBUG("DQN update - error: " + std::to_string(error) + 
                     ", action: " + std::to_string(experience.action) +
                     ", reward: " + std::to_string(experience.reward));
    }
}

bool DeepQLearning::exploreAndLearnOneEpisode() {
    // Generate random initial state
    std::vector<float> state = generateRandomState();
    
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        // Select action
        int action;
        if (!explore(state, action)) {
            return false;
        }
        
        // Simulate environment transition (simplified)
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        // Check if episode is done
        done = (steps >= maxSteps - 1) || (reward < -10.0f); // Simple termination condition
        
        // Store experience
        TrainingExperience experience;
        experience.state = state;
        experience.action = action;
        experience.reward = reward;
        experience.nextState = nextState;
        experience.done = done;
        experience.timestamp = getCurrentTime();
        experience.step = totalSteps_++;
        
        if (!storeExperience(experience)) {
            return false;
        }
        
        // Learn from experience
        if (canLearn()) {
            learn();
        }
        
        // Update state
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    totalReward_ += totalReward;
    return true;
}

float DeepQLearning::simulateEpisode(std::shared_ptr<Brain> brain, bool evaluate) {
    // Simulate one episode for evaluation
    std::vector<float> state = generateRandomState();
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        int action;
        if (evaluate) {
            if (!exploit(state, action)) return -1.0f;
        } else {
            if (!explore(state, action)) return -1.0f;
        }
        
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        done = (steps >= maxSteps - 1) || (reward < -10.0f);
        
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    return totalReward;
}

std::vector<float> DeepQLearning::generateRandomState() {
    std::vector<float> state(stateSpaceSize_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < stateSpaceSize_; ++i) {
        state[i] = dist(rng_);
    }
    
    return state;
}

std::vector<float> DeepQLearning::generateNextState(const std::vector<float>& state, int action) {
    std::vector<float> nextState = state;
    
    // Simple state transition model
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < nextState.size(); ++i) {
        // Add some stochasticity
        nextState[i] += dist(rng_);
        
        // Clip to valid range
        nextState[i] = std::max(0.0f, std::min(1.0f, nextState[i]));
    }
    
    // Modify one dimension based on action
    if (!nextState.empty() && action >= 0 && action < actionSpaceSize_) {
        nextState[action % nextState.size()] = std::min(1.0f, nextState[action % nextState.size()] + 0.2f);
    }
    
    return nextState;
}

float DeepQLearning::simulateAction(const std::vector<float>& state, int action, float accumulatedReward) {
    // Simple reward function based on state and action
    float reward = 0.0f;
    
    // Reward based on action (higher action number gets more reward)
    reward += static_cast<float>(action) * 0.1f;
    
    // Reward based on state values
    float stateSum = std::accumulate(state.begin(), state.end(), 0.0f);
    reward += stateSum * 0.05f;
    
    // Add some stochasticity
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    reward += dist(rng_);
    
    return reward;
}

std::string DeepQLearning::getStatsSummary() const {
    std::stringstream ss;
    ss << "=== Deep Q-Network Statistics ===\n";
    ss << "Algorithm: " << ReinforcementLearnerFactory::algorithmToString(algorithm_) << "\n";
    ss << "Exploration Strategy: " << static_cast<int>(explorationStrategy_) << "\n";
    ss << "State Space Size: " << stateSpaceSize_ << "\n";
    ss << "Action Space Size: " << actionSpaceSize_ << "\n";
    ss << "Hyperparameters: alpha=" << alpha_ << ", gamma=" << gamma_ << ", epsilon=" << epsilon_ << "\n";
    ss << "Experience Count: " << experienceCount_ << "\n";
    ss << "Episode Count: " << episodeCount_ << "\n";
    ss << "Total Reward: " << totalReward_ << "\n";
    ss << "Average Reward: " << (episodeCount_ > 0 ? totalReward_ / episodeCount_ : 0.0) << "\n";
    ss << "Experience Replay Buffer Size: " << experienceReplay_->size() << "\n";
    ss << "Neural Network Parameters: " << neuralNetworkWeights_.size() << " layers\n";
    ss << "==================================\n";
    
    return ss.str();
}

// ============= POLICY GRADIENT IMPLEMENTATION =============

PolicyGradientLearner::PolicyGradientLearner(std::shared_ptr<Brain> brain,
                                             std::shared_ptr<AgentBrain> agent,
                                             std::shared_ptr<SimpleWorld> world,
                                             std::shared_ptr<Config> config)
    : ReinforcementLearner(brain, agent, world, config) {
    
    // Set algorithm type
    algorithm_ = RLAlgorithm::POLICY_GRADIENT;
    explorationStrategy_ = ExplorationStrategy::THERMAL;
    
    // Configure hyperparameters
    alpha_ = config->getOr<float>("learning_rate", 0.01f);
    gamma_ = config->getOr<float>("discount_factor", 0.99f);
    epsilon_ = config->getOr<float>("exploration_rate", 0.1f);
    temperature_ = config->getOr<float>("temperature", 1.0f);
    
    // Initialize policy and value network parameters
    int stateSpaceSize = config->getOr<int>("state_space_size", 100);
    int actionSpaceSize = config->getOr<int>("action_space_size", 16);
    
    policyWeights_.resize(stateSpaceSize * actionSpaceSize, 0.0f);
    valueWeights_.resize(stateSpaceSize, 0.0f);
    
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (float& weight : policyWeights_) {
        weight = dist(rng);
    }
    
    for (float& weight : valueWeights_) {
        weight = dist(rng);
    }
    
    // Set state and action space sizes
    stateSpaceSize_ = stateSpaceSize;
    actionSpaceSize_ = actionSpaceSize;
    
    // Initialize experience replay
    size_t replayCapacity = config->getOr<size_t>("experience_replay_capacity", 10000);
    experienceReplay_ = std::make_unique<ExperienceReplay>(replayCapacity);
    
    NLM_LOG_INFO("Policy Gradient Learner initialized with alpha=" + std::to_string(alpha_) +
                ", gamma=" + std::to_string(gamma_) + ", temperature=" + std::to_string(temperature_));
}

bool PolicyGradientLearner::train(size_t numEpisodes) {
    episodeCount_ = 0;
    totalReward_ = 0.0;
    
    NLM_LOG_INFO("Starting Policy Gradient training for " + std::to_string(numEpisodes) + " episodes");
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        bool success = exploreAndLearnOneEpisode();
        if (!success) {
            NLM_LOG_WARN("Episode " + std::to_string(episode + 1) + " failed, continuing");
        }
        
        episodeCount_++;
    }
    
    NLM_LOG_INFO("Policy Gradient training completed. Episodes: " + std::to_string(episodeCount_));
    return true;
}

bool PolicyGradientLearner::predict(std::shared_ptr<Brain> brain, const std::vector<float>& state, int& action) {
    return exploit(state, action);
}

bool PolicyGradientLearner::learn() {
    if (!canLearn()) {
        return false;
    }
    
    // Sample a batch from experience replay
    std::vector<TrainingExperience> batch;
    size_t batchSize = config_->getOr<size_t>("batch_size", 32);
    
    if (experienceReplay_->size() >= batchSize) {
        for (size_t i = 0; i < batchSize; ++i) {
            batch.push_back(experienceReplay_->sample());
        }
    } else {
        return false; // Not enough experiences to learn
    }
    
    // Update policy using policy gradient
    for (const auto& experience : batch) {
        std::vector<float> gradient = computePolicyGradient(experience);
        
        // Update policy weights (simplified)
        for (size_t i = 0; i < policyWeights_.size() && i < gradient.size(); ++i) {
            policyWeights_[i] += alpha_ * gradient[i];
        }
        
        // Update value weights (simplified)
        // In a real implementation, this would involve a separate value network
    }
    
    return true;
}

bool PolicyGradientLearner::explore(const std::vector<float>& state, int& action) {
    // Thermal exploration: sample from policy distribution
    std::vector<float> actionProbabilities = computeActionProbabilities(state);
    
    std::discrete_distribution<int> actionDist(actionProbabilities.begin(), actionProbabilities.end());
    action = actionDist(rng_);
    
    return true; // Always explore in policy gradient (sampled from policy)
}

bool PolicyGradientLearner::exploit(const std::vector<float>& state, int& action) {
    // Use policy to select action (greedy if temperature is low)
    std::vector<float> actionProbabilities = computeActionProbabilities(state);
    
    if (temperature_ < 0.1f) {
        // Near greedy: choose action with highest probability
        int bestAction = 0;
        float bestProb = actionProbabilities[0];
        
        for (int i = 1; i < actionSpaceSize_; ++i) {
            if (actionProbabilities[i] > bestProb) {
                bestProb = actionProbabilities[i];
                bestAction = i;
            }
        }
        
        action = bestAction;
    } else {
        // Sample from distribution
        std::discrete_distribution<int> actionDist(actionProbabilities.begin(), actionProbabilities.end());
        action = actionDist(rng_);
    }
    
    return true; // Exploitation
}

bool PolicyGradientLearner::saveModel(const std::string& filepath) {
    // Save policy and value network parameters
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Save hyperparameters
        file.write(reinterpret_cast<const char*>(&alpha_), sizeof(alpha_));
        file.write(reinterpret_cast<const char*>(&gamma_), sizeof(gamma_));
        file.write(reinterpret_cast<const char*>(&epsilon_), sizeof(epsilon_));
        file.write(reinterpret_cast<const char*>(&temperature_), sizeof(temperature_));
        
        // Save policy weights
        file.write(reinterpret_cast<const char*>(policyWeights_.data()),
                  policyWeights_.size() * sizeof(float));
        
        // Save value weights
        file.write(reinterpret_cast<const char*>(valueWeights_.data()),
                  valueWeights_.size() * sizeof(float));
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save Policy Gradient model: " + std::string(e.what()));
        return false;
    }
}

bool PolicyGradientLearner::loadModel(const std::string& filepath) {
    // Load policy and value network parameters
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Load hyperparameters
        file.read(reinterpret_cast<char*>(&alpha_), sizeof(alpha_));
        file.read(reinterpret_cast<char*>(&gamma_), sizeof(gamma_));
        file.read(reinterpret_cast<char*>(&epsilon_), sizeof(epsilon_));
        file.read(reinterpret_cast<char*>(&temperature_), sizeof(temperature_));
        
        // Load policy weights
        file.read(reinterpret_cast<char*>(policyWeights_.data()),
                 policyWeights_.size() * sizeof(float));
        
        // Load value weights
        file.read(reinterpret_cast<char*>(valueWeights_.data()),
                 valueWeights_.size() * sizeof(float));
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load Policy Gradient model: " + std::string(e.what()));
        return false;
    }
}

bool PolicyGradientLearner::storeExperience(const TrainingExperience& experience) {
    experienceReplay_->add(experience);
    experienceCount_++;
    return true;
}

bool PolicyGradientLearner::canLearn() const {
    return experienceReplay_->isReady(config_->getOr<size_t>("min_experience_for_learning", 100));
}

void PolicyGradientLearner::clearMemory() {
    experienceReplay_->clear();
}

bool PolicyGradientLearner::evaluate(std::shared_ptr<Brain> brain, size_t evalEpisodes) {
    float totalReward = 0.0f;
    
    for (size_t episode = 0; episode < evalEpisodes; ++episode) {
        float episodeReward = simulateEpisode(brain, true); // Evaluation mode (no exploration)
        totalReward += episodeReward;
    }
    
    evaluationScore_ = totalReward / evalEpisodes;
    return true;
}

std::vector<float> PolicyGradientLearner::computeActionProbabilities(const std::vector<float>& state) const {
    // Compute action probabilities using policy network (simplified)
    std::vector<float> actionLogits(actionSpaceSize_, 0.0f);
    
    // Simple policy computation (placeholder for real neural network)
    for (int action = 0; action < actionSpaceSize_; ++action) {
        float logit = 0.0f;
        
        // Compute action value based on state and policy weights
        for (int i = 0; i < stateSpaceSize_ && i < static_cast<int>(state.size()); ++i) {
            int weightIndex = i * actionSpaceSize_ + action;
            if (weightIndex < static_cast<int>(policyWeights_.size())) {
                logit += state[i] * policyWeights_[weightIndex];
            }
        }
        
        // Add bias term
        logit += valueWeights_[action % valueWeights_.size()];
        
        actionLogits[action] = logit / temperature_; // Temperature scaling
    }
    
    // Convert logits to probabilities using softmax
    return NeuralNetworkUtils::softmax(actionLogits);
}

std::vector<float> PolicyGradientLearner::computePolicyGradient(const TrainingExperience& experience) const {
    // Compute policy gradient (simplified)
    std::vector<float> actionProbabilities = computeActionProbabilities(experience.state);
    std::vector<float> gradient(policyWeights_.size(), 0.0f);
    
    // Monte Carlo policy gradient
    float discountedReturn = experience.reward;
    for (size_t i = 0; i < experience.reward; ++i) { // Simplified - just use immediate reward
        discountedReturn += std::pow(gamma_, i) * 0.0f; // No future rewards for simplicity
    }
    
    // Policy gradient update
    float policyProbability = actionProbabilities[experience.action];
    if (policyProbability > 0.0f) {
        float gradientFactor = discountedReturn / policyProbability;
        
        // Update gradient for policy weights
        for (int i = 0; i < stateSpaceSize_ && i < static_cast<int>(experience.state.size()); ++i) {
            int weightIndex = i * actionSpaceSize_ + experience.action;
            if (weightIndex < static_cast<int>(gradient.size())) {
                gradient[weightIndex] = gradientFactor * experience.state[i];
            }
        }
    }
    
    return gradient;
}

std::vector<float> PolicyGradientLearner::computeAdvantage(const std::vector<float>& rewards, float gamma) const {
    // Compute advantage function (simplified)
    std::vector<float> advantages(rewards.size(), 0.0f);
    
    if (rewards.empty()) return advantages;
    
    float discountedSum = 0.0f;
    for (int t = rewards.size() - 1; t >= 0; --t) {
        discountedSum = rewards[t] + gamma * discountedSum;
        advantages[t] = discountedSum;
    }
    
    return advantages;
}

bool PolicyGradientLearner::exploreAndLearnOneEpisode() {
    // Generate random initial state
    std::vector<float> state = generateRandomState();
    
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        // Select action
        int action;
        if (!explore(state, action)) {
            return false;
        }
        
        // Simulate environment transition (simplified)
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        // Check if episode is done
        done = (steps >= maxSteps - 1) || (reward < -10.0f); // Simple termination condition
        
        // Store experience
        TrainingExperience experience;
        experience.state = state;
        experience.action = action;
        experience.reward = reward;
        experience.nextState = nextState;
        experience.done = done;
        experience.timestamp = getCurrentTime();
        experience.step = totalSteps_++;
        
        if (!storeExperience(experience)) {
            return false;
        }
        
        // Learn from experience
        if (canLearn()) {
            learn();
        }
        
        // Update state
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    totalReward_ += totalReward;
    return true;
}

float PolicyGradientLearner::simulateEpisode(std::shared_ptr<Brain> brain, bool evaluate) {
    // Simulate one episode for evaluation
    std::vector<float> state = generateRandomState();
    float totalReward = 0.0f;
    bool done = false;
    size_t steps = 0;
    size_t maxSteps = config_->getOr<size_t>("max_steps_per_episode", 1000);
    
    while (!done && steps < maxSteps) {
        int action;
        if (evaluate) {
            if (!exploit(state, action)) return -1.0f;
        } else {
            if (!explore(state, action)) return -1.0f;
        }
        
        float reward = simulateAction(state, action, totalReward);
        std::vector<float> nextState = generateNextState(state, action);
        
        done = (steps >= maxSteps - 1) || (reward < -10.0f);
        
        state = nextState;
        totalReward += reward;
        steps++;
    }
    
    return totalReward;
}

std::vector<float> PolicyGradientLearner::generateRandomState() {
    std::vector<float> state(stateSpaceSize_);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < stateSpaceSize_; ++i) {
        state[i] = dist(rng_);
    }
    
    return state;
}

std::vector<float> PolicyGradientLearner::generateNextState(const std::vector<float>& state, int action) {
    std::vector<float> nextState = state;
    
    // Simple state transition model
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    
    for (size_t i = 0; i < nextState.size(); ++i) {
        // Add some stochasticity
        nextState[i] += dist(rng_);
        
        // Clip to valid range
        nextState[i] = std::max(0.0f, std::min(1.0f, nextState[i]));
    }
    
    // Modify one dimension based on action
    if (!nextState.empty() && action >= 0 && action < actionSpaceSize_) {
        nextState[action % nextState.size()] = std::min(1.0f, nextState[action % nextState.size()] + 0.2f);
    }
    
    return nextState;
}

float PolicyGradientLearner::simulateAction(const std::vector<float>& state, int action, float accumulatedReward) {
    // Simple reward function based on state and action
    float reward = 0.0f;
    
    // Reward based on action (higher action number gets more reward)
    reward += static_cast<float>(action) * 0.1f;
    
    // Reward based on state values
    float stateSum = std::accumulate(state.begin(), state.end(), 0.0f);
    reward += stateSum * 0.05f;
    
    // Add some stochasticity
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    reward += dist(rng_);
    
    return reward;
}

std::string PolicyGradientLearner::getStatsSummary() const {
    std::stringstream ss;
    ss << "=== Policy Gradient Statistics ===\n";
    ss << "Algorithm: " << ReinforcementLearnerFactory::algorithmToString(algorithm_) << "\n";
    ss << "Exploration Strategy: " << static_cast<int>(explorationStrategy_) << "\n";
    ss << "State Space Size: " << stateSpaceSize_ << "\n";
    ss << "Action Space Size: " << actionSpaceSize_ << "\n";
    ss << "Hyperparameters: alpha=" << alpha_ << ", gamma=" << gamma_ << ", epsilon=" << epsilon_ << "\n";
    ss << "Experience Count: " << experienceCount_ << "\n";
    ss << "Episode Count: " << episodeCount_ << "\n";
    ss << "Total Reward: " << totalReward_ << "\n";
    ss << "Average Reward: " << (episodeCount_ > 0 ? totalReward_ / episodeCount_ : 0.0) << "\n";
    ss << "Experience Replay Buffer Size: " << experienceReplay_->size() << "\n";
    ss << "Policy Parameters: " << policyWeights_.size() << " weights\n";
    ss << "Value Parameters: " << valueWeights_.size() << " weights\n";
    ss << "==================================\n";
    
    return ss.str();
}

// Implementation of base class methods

std::string ReinforcementLearner::getStatsSummary() const {
    std::stringstream ss;
    ss << "=== Reinforcement Learner Statistics ===\n";
    ss << "Algorithm: " << ReinforcementLearnerFactory::algorithmToString(algorithm_) << "\n";
    ss << "Exploration Strategy: " << static_cast<int>(explorationStrategy_) << "\n";
    ss << "State Space Size: " << stateSpaceSize_ << "\n";
    ss << "Action Space Size: " << actionSpaceSize_ << "\n";
    ss << "Hyperparameters: alpha=" << alpha_ << ", gamma=" << gamma_ << ", epsilon=" << epsilon_ << "\n";
    ss << "Experience Count: " << experienceCount_ << "\n";
    ss << "Episode Count: " << episodeCount_ << "\n";
    ss << "Total Reward: " << totalReward_ << "\n";
    ss << "Average Reward: " << (episodeCount_ > 0 ? totalReward_ / episodeCount_ : 0.0) << "\n";
    ss << "Evaluation Score: " << evaluationScore_ << "\n";
    ss << "==================================\n";
    
    return ss.str();
}

double ReinforcementLearner::getTotalReward() const {
    return totalReward_;
}

void ReinforcementLearner::setHyperparameters(float alpha, float gamma, float epsilon, float temperature) {
    alpha_ = alpha;
    gamma_ = gamma;
    epsilon_ = epsilon;
    temperature_ = temperature;
}

std::vector<float> ReinforcementLearner::discretizeState(const std::vector<float>& state) const {
    std::vector<float> discretizedState;
    
    for (size_t i = 0; i < state.size() && i < stateMinValues_.size(); ++i) {
        // Discretize based on bin configuration
        float normalized = (state[i] - stateMinValues_[i]) / 
                          (stateMaxValues_[i] - stateMinValues_[i]);
        
        int binIndex = static_cast<int>(normalized * stateBins_[i]);
        binIndex = std::max(0, std::min(static_cast<int>(stateBins_[i]) - 1, binIndex));
        
        discretizedState.push_back(static_cast<float>(binIndex));
    }
    
    return discretizedState;
}

float ReinforcementLearner::computeTDTarget(float reward, float nextValue, float done, float gamma) const {
    if (done) {
        return reward;
    }
    return reward + gamma * nextValue;
}

float ReinforcementLearner::computeLoss(float predicted, float target) const {
    return 0.5f * (predicted - target) * (predicted - target); // Mean squared error
}

std::string ReinforcementLearner::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
    
    std::stringstream ss;
    ss << seconds << "." << std::setfill('0') << std::setw(3) << milliseconds;
    return ss.str();
}

} // namespace nlm
