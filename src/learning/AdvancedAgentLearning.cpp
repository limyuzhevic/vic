#include "AdvancedAgentLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>

namespace nlm {

LearningNetwork::LearningNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize)
    : inputSize_(inputSize), hiddenSize_(hiddenSize), outputSize_(outputSize),
      normalDist_(0.0f, 1.0f), rng_(std::chrono::steady_clock::now().time_since_epoch().count()) {
    
    weights1_.resize(inputSize_, std::vector<float>(hiddenSize_, 0.0f));
    weights2_.resize(hiddenSize_, std::vector<float>(outputSize_, 0.0f));
    biases1_.resize(hiddenSize_, 0.0f);
    biases2_.resize(outputSize_, 0.0f);
    
    activations1_.resize(hiddenSize_, 0.0f);
    activations2_.resize(outputSize_, 0.0f);
    preActivations1_.resize(hiddenSize_, 0.0f);
    preActivations2_.resize(outputSize_, 0.0f);
    
    inputGradient1_.resize(inputSize_, 0.0f);
    outputGradient1_.resize(hiddenSize_, 0.0f);
    gradients1_.resize(hiddenSize_, 0.0f);
    gradients2_.resize(outputSize_, 0.0f);
    biasesGradients1_.resize(hiddenSize_, 0.0f);
    biasesGradients2_.resize(outputSize_, 0.0f);
    
    initializeWeights();
}

LearningNetwork::~LearningNetwork() = default;

void LearningNetwork::initializeWeights(float mean, float stddev) {
    normalDist_ = std::normal_distribution<float>(mean, stddev);
    
    for (auto& row : weights1_) {
        for (auto& weight : row) {
            weight = normalDist_(rng_);
        }
    }
    
    for (auto& row : weights2_) {
        for (auto& weight : row) {
            weight = normalDist_(rng_);
        }
    }
    
    for (auto& bias : biases1_) {
        bias = normalDist_(rng_);
    }
    
    for (auto& bias : biases2_) {
        bias = normalDist_(rng_);
    }
}

std::vector<float> LearningNetwork::forward(const std::vector<float>& input) {
    if (input.size() != inputSize_) {
        NLM_LOG_ERROR("Input size mismatch: expected " + std::to_string(inputSize_) +
                     " got " + std::to_string(input.size()));
        return std::vector<float>(outputSize_, 0.0f);
    }
    
    // Forward pass through first layer
    for (size_t j = 0; j < hiddenSize_; ++j) {
        float sum = biases1_[j];
        for (size_t i = 0; i < inputSize_; ++i) {
            sum += input[i] * weights1_[i][j];
        }
        preActivations1_[j] = sum;
        activations1_[j] = 1.0f / (1.0f + std::exp(-sum));  // ReLU
    }
    
    // Forward pass through second layer
    for (size_t j = 0; j < outputSize_; ++j) {
        float sum = biases2_[j];
        for (size_t i = 0; i < hiddenSize_; ++i) {
            sum += activations1_[i] * weights2_[i][j];
        }
        preActivations2_[j] = sum;
        activations2_[j] = std::exp(sum);  // Exponent for softmax
    }
    
    // Softmax normalization
    float maxAct = *std::max_element(activations2_.begin(), activations2_.end());
    float sumActs = std::accumulate(activations2_.begin(), activations2_.end(), 0.0f);
    
    for (size_t j = 0; j < outputSize_; ++j) {
        activations2_[j] = std::exp(activations2_[j] - maxAct) / sumActs;
    }
    
    return activations2_;
}

void LearningNetwork::backward(const std::vector<float>& target, float learningRate, float lambda) {
    // Compute output layer gradient (cross-entropy loss + softmax)
    std::vector<float> outputGradients(outputSize_);
    for (size_t j = 0; j < outputSize_; ++j) {
        outputGradients[j] = activations2_[j] - target[j];
    }
    
    // Backpropagate through second layer
    for (size_t i = 0; i < hiddenSize_; ++i) {
        float error = 0.0f;
        for (size_t j = 0; j < outputSize_; ++j) {
            error += outputGradients[j] * weights2_[i][j];
        }
        
        float reluGrad = (preActivations1_[i] > 0.0f) ? 1.0f : 0.0f;
        gradients1_[i] = error * reluGrad;
    }
    
    // Update second layer weights and biases
    for (size_t i = 0; i < hiddenSize_; ++i) {
        for (size_t j = 0; j < outputSize_; ++j) {
            float grad = activations1_[i] * outputGradients[j];
            weights2_[i][j] -= learningRate * (grad + lambda * weights2_[i][j]);
        }
        biases2_[j] -= learningRate * outputGradients[j];
    }
    
    // Update first layer weights and biases
    for (size_t i = 0; i < inputSize_; ++i) {
        for (size_t j = 0; j < hiddenSize_; ++j) {
            float grad = input[i] * gradients1_[j];
            weights1_[i][j] -= learningRate * (grad + lambda * weights1_[i][j]);
        }
    }
    
    for (size_t j = 0; j < hiddenSize_; ++j) {
        biases1_[j] -= learningRate * gradients1_[j];
    }
    
    // Store gradients for adaptive learning rate
    gradients_ = outputGradients;
}

std::vector<float> LearningNetwork::getOutputProbabilities(const std::vector<float>& input) {
    return forward(input);
}

void LearningNetwork::updateWeights(const std::vector<float>& gradients, float learningRate) {
    // Simple weight update based on external gradients
    for (size_t i = 0; i < inputSize_; ++i) {
        for (size_t j = 0; j < hiddenSize_; ++j) {
            weights1_[i][j] -= learningRate * gradients[i];
        }
    }
}

void LearningNetwork::save(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;
    
    // Save architecture
    file.write(reinterpret_cast<const char*>(&inputSize_), sizeof(inputSize_));
    file.write(reinterpret_cast<const char*>(&hiddenSize_), sizeof(hiddenSize_));
    file.write(reinterpret_cast<const char*>(&outputSize_), sizeof(outputSize_));
    
    // Save weights and biases
    for (const auto& row : weights1_) {
        for (float weight : row) {
            file.write(reinterpret_cast<const char*>(&weight), sizeof(weight));
        }
    }
    
    for (const auto& row : weights2_) {
        for (float weight : row) {
            file.write(reinterpret_cast<const char*>(&weight), sizeof(weight));
        }
    }
    
    for (float bias : biases1_) {
        file.write(reinterpret_cast<const char*>(&bias), sizeof(bias));
    }
    
    for (float bias : biases2_) {
        file.write(reinterpret_cast<const char*>(&bias), sizeof(bias));
    }
    
    file.close();
}

void LearningNetwork::load(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;
    
    // Load architecture
    file.read(reinterpret_cast<char*>(&inputSize_), sizeof(inputSize_));
    file.read(reinterpret_cast<char*>(&hiddenSize_), sizeof(hiddenSize_));
    file.read(reinterpret_cast<char*>(&outputSize_), sizeof(outputSize_));
    
    // Resize vectors
    weights1_.resize(inputSize_, std::vector<float>(hiddenSize_));
    weights2_.resize(hiddenSize_, std::vector<float>(outputSize_));
    biases1_.resize(hiddenSize_);
    biases2_.resize(outputSize_);
    
    // Load weights
    for (auto& row : weights1_) {
        for (auto& weight : row) {
            file.read(reinterpret_cast<char*>(&weight), sizeof(weight));
        }
    }
    
    for (auto& row : weights2_) {
        for (auto& weight : row) {
            file.read(reinterpret_cast<char*>(&weight), sizeof(weight));
        }
    }
    
    // Load biases
    for (auto& bias : biases1_) {
        file.read(reinterpret_cast<char*>(&bias), sizeof(bias));
    }
    
    for (auto& bias : biases2_) {
        file.read(reinterpret_cast<char*>(&bias), sizeof(bias));
    }
    
    file.close();
}

AdvancedAgentLearning::AdvancedAgentLearning(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain)
    : brain_(brain), agentBrain_(agentBrain), replayBufferSize_(100000),
      learningRate_(0.001f), discountFactor_(0.99f), explorationRate_(0.1f),
      entropyCoefficient_(0.01f), curiosityWeight_(1.0f),
      actionDimension_(6), stateDimension_(256 + 8 + 4 + 6),  // From AgentBrain::getSensoryInputSize()
      hiddenLayerSize_(256), episodeStep_(0), curiosityEnabled_(true),
      transferLearningEnabled_(false), metaLearningEnabled_(false),
      socialLearningEnabled_(false), experienceReplayEnabled_(true) {
    
    // Initialize random number generator
    std::random_device rd;
    rng_ = std::mt19937(rd());
    uniformDist_ = std::uniform_real_distribution<float>(0.0f, 1.0f);
    normalDist_ = std::normal_distribution<float>(0.0f, 1.0f);
    
    // Initialize algorithm enabled flags
    algorithmEnabled_["q_learning"] = true;
    algorithmEnabled_["actor_critic"] = true;
    algorithmEnabled_["reinforcement_learning"] = true;
    algorithmEnabled_["meta_learning"] = true;
    
    explorationStrategyEnabled_["epsilon_greedy"] = true;
    explorationStrategyEnabled_["thompson_sampling"] = true;
    explorationStrategyEnabled_["ucb"] = true;
    explorationStrategyEnabled_["entropy_regularized"] = true;
    
    // Initialize neuromodulation components
    curiosity_ = std::make_shared<Curiosity>();
    novelty_ = std::make_shared<Novelty>();
    neuromodulator_ = std::make_shared<Neuromodulator>();
    
    // Initialize networks
    valueNetwork_ = std::make_unique<LearningNetwork>(stateDimension_, hiddenLayerSize_, 1);
    policyNetwork_ = std::make_unique<LearningNetwork>(stateDimension_, hiddenLayerSize_, actionDimension_);
}

AdvancedAgentLearning::~AdvancedAgentLearning() = default;

void AdvancedAgentLearning::initialize(size_t stateDim, size_t actionDim, size_t hiddenSize) {
    stateDimension_ = stateDim;
    actionDimension_ = actionDim;
    hiddenLayerSize_ = hiddenSize;
    
    valueNetwork_ = std::make_unique<LearningNetwork>(stateDim, hiddenSize, 1);
    policyNetwork_ = std::make_unique<LearningNetwork>(stateDim, hiddenSize, actionDim);
    
    experiences_.clear();
    transferableKnowledge_.clear();
    socialKnowledge_.clear();
    
    NLM_LOG_INFO("AdvancedAgentLearning initialized with state_dim=" + std::to_string(stateDim) +
                " action_dim=" + std::to_string(actionDim) + " hidden_size=" + std::to_string(hiddenSize));
}

void AdvancedAgentLearning::processTransition(const LearningState& state, const Action& action,
                                            const LearningState& nextState, float reward, bool done) {
    // Store experience
    if (experienceReplayEnabled_) {
        Transition transition(state, action, nextState, reward, discountFactor_);
        
        // Compute priority based on TD error
        float tdError = std::abs(reward + discountFactor_ * 0.0f - state.reward);
        transition.priority = tdError;
        
        storeExperience(transition);
    }
    
    // Apply learning if not terminal
    if (!done) {
        learn();
    }
    
    // Update statistics
    stats_.totalReward += reward;
    stats_.stepCount++;
}

Action AdvancedAgentLearning::selectAction(const std::vector<float>& state, float epsilon, bool useExploration) {
    currentState_ = state;
    
    // Get action probabilities from policy network
    std::vector<float> actionProbs = policyNetwork_->getOutputProbabilities(state);
    
    Action action;
    
    if (useExploration && explorationRate_ > 0.0f) {
        // Use selected exploration strategy
        if (explorationStrategyEnabled_["epsilon_greedy"] && uniformDist_(rng_) < explorationRate_) {
            action = epsilonGreedyExploration(state, epsilon);
        } else if (explorationStrategyEnabled_["thompson_sampling"] && curiosityEnabled_) {
            action = thompsonSamplingExploration(state);
        } else if (explorationStrategyEnabled_["ucb"] && curiosityEnabled_) {
            action = ucbExploration(state);
        } else if (explorationStrategyEnabled_["entropy_regularized"] && curiosityEnabled_) {
            action = entropyRegularizedExploration(state, 1.0f / (explorationRate_ + 0.001f));
        } else {
            action = epsilonGreedyExploration(state, epsilon);
        }
    } else {
        action = epsilonGreedyExploration(state, 0.0f);
    }
    
    // Update curiosity-driven exploration
    if (curiosityEnabled_) {
        updateCuriosityDrivenExploration(currentState_);
    }
    
    // Update meta-learning if enabled
    if (metaLearningEnabled_) {
        metaLearningUpdate(currentState_, action, currentState_, 0.0f, false);
    }
    
    // Update social learning if enabled
    if (socialLearningEnabled_) {
        updateSocialKnowledge(currentState_, action);
    }
    
    return action;
}

void AdvancedAgentLearning::learn(float batchSize, float discountFactor) {
    if (experiences_.empty()) return;
    
    // Sample experiences
    std::vector<Transition> batch;
    size_t sampleSize = std::min(static_cast<size_t>(experiences_.size() * batchSize),
                               experiences_.size());
    
    if (sampleSize == 0) return;
    
    // Uniform sampling for now (can be enhanced with prioritized sampling)
    std::vector<size_t> indices(experiences_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng_);
    
    batch.reserve(sampleSize);
    for (size_t i = 0; i < sampleSize; ++i) {
        batch.push_back(experiences_[indices[i]]);
    }
    
    // Train on batch based on primary algorithm
    if (primaryAlgorithm_ == "actor_critic") {
        for (const auto& transition : batch) {
            actorCritic(transition.state, transition.action, transition.nextState,
                        transition.reward, transition.state.done);
        }
    } else if (primaryAlgorithm_ == "q_learning") {
        for (const auto& transition : batch) {
            qLearning(transition.state, transition.action, transition.nextState,
                     transition.reward, transition.state.done);
        }
    } else {
        // Default to reinforcement learning
        for (const auto& transition : batch) {
            reinforcementLearning(transition.state, transition.action, transition.nextState,
                                 transition.reward, transition.state.done);
        }
    }
}

void AdvancedAgentLearning::updateLearningParameters(float learningRate, float explorationRate,
                                                    float entropyCoefficient) {
    learningRate_ = clamp(learningRate, 0.0001f, 0.1f);
    explorationRate_ = clamp(explorationRate, 0.0f, 1.0f);
    entropyCoefficient_ = clamp(entropyCoefficient, 0.0f, 0.1f);
    
    NLM_LOG_INFO("Learning parameters updated: lr=" + std::to_string(learningRate_) +
                " explore=" + std::to_string(explorationRate_) +
                " entropy_coef=" + std::to_string(entropyCoefficient_));
}

void AdvancedAgentLearning::reset() {
    experiences_.clear();
    expertDemonstrations_.clear();
    socialKnowledge_.clear();
    transferableKnowledge_.clear();
    
    currentState_.clear();
    lastAction_ = Action();
    lastReward_ = 0.0f;
    episodeDone_ = false;
    episodeStep_ = 0;
    
    stats_ = Statistics();
    
    NLM_LOG_INFO("AdvancedAgentLearning reset");
}

float AdvancedAgentLearning::getValue(const std::vector<float>& state) {
    return computeValueEstimate(state);
}

float AdvancedAgentLearning::getPolicyEntropy(const std::vector<float>& state) {
    std::vector<float> probs = policyNetwork_->getOutputProbabilities(state);
    return computeEntropy(probs);
}

void AdvancedAgentLearning::enableAlgorithm(const std::string& algorithm, bool enable) {
    if (algorithmEnabled_.count(algorithm)) {
        algorithmEnabled_[algorithm] = enable;
        NLM_LOG_INFO("Algorithm " + algorithm + " " + (enable ? "enabled" : "disabled"));
    }
}

void AdvancedAgentLearning::enableExplorationStrategy(const std::string& strategy, bool enable) {
    if (explorationStrategyEnabled_.count(strategy)) {
        explorationStrategyEnabled_[strategy] = enable;
        NLM_LOG_INFO("Exploration strategy " + strategy + " " + (enable ? "enabled" : "disabled"));
    }
}

void AdvancedAgentLearning::setCuriositySource(const std::string& source, float weight) {
    NLM_LOG_INFO("Set curiosity source " + source + " weight=" + std::to_string(weight));
    // Implementation would track different curiosity sources
}

// Learning algorithm implementations
void AdvancedAgentLearning::qLearning(const LearningState& state, const Action& action,
                                    const LearningState& nextState, float reward, bool done) {
    // Simple Q-learning update
    float currentValue = valueNetwork_->getOutputProbabilities(state)[0];
    float nextValue = computeValueEstimate(nextState);
    
    float target = reward + (done ? 0.0f : discountFactor_ * nextValue);
    
    // Compute TD error
    float tdError = target - currentValue;
    
    // Update value network (simplified)
    std::vector<float> targetVec(1, target);
    valueNetwork_->backward(targetVec, learningRate_);
}

void AdvancedAgentLearning::actorCritic(const LearningState& state, const Action& action,
                                       const LearningState& nextState, float reward, bool done) {
    // Actor-Critic update with PPO-style clipped surrogate objective
    std::vector<float> stateVec = state.state;
    std::vector<float> actionVec = action.parameters;
    
    // Get current policy and value
    std::vector<float> policyProbs = policyNetwork_->getOutputProbabilities(stateVec);
    float currentValue = computeValueEstimate(state);
    float nextValue = computeValueEstimate(nextState);
    
    // Compute advantage
    float advantage = reward + (done ? 0.0f : discountFactor_ * nextValue) - currentValue;
    
    // Policy gradient with entropy regularization
    std::vector<float> policyGradient(actionDimension_, 0.0f);
    for (size_t i = 0; i < actionDimension_; ++i) {
        float prob = policyProbs[i];
        if (prob > 0.0f) {
            policyGradient[i] = (action.actionId == i ? 1.0f : 0.0f) * advantage +
                              entropyCoefficient_ * std::log(prob);
        }
    }
    
    // Update policy network (simplified)
    valueNetwork_->updateWeights(policyGradient, learningRate_);
    policyNetwork_->updateWeights(policyGradient, learningRate_);
}

void AdvancedAgentLearning::reinforcementLearning(const LearningState& state, const Action& action,
                                                 const LearningState& nextState, float reward,
                                                 bool done) {
    // Basic reinforcement learning update
    float currentValue = computeValueEstimate(state);
    float nextValue = computeValueEstimate(nextState);
    
    float target = reward + (done ? 0.0f : discountFactor_ * nextValue);
    float tdError = target - currentValue;
    
    // Update with adaptive learning rate
    float adaptiveLR = adaptiveLearningRate({tdError}, episodeStep_);
    
    std::vector<float> targetVec(1, target);
    valueNetwork_->backward(targetVec, adaptiveLR);
}

void AdvancedAgentLearning::metaLearningUpdate(const LearningState& state, const Action& action,
                                              const LearningState& nextState, float reward,
                                              bool done) {
    // Simple meta-learning update (MAML-style approximation)
    if (transferLearningEnabled_) {
        // Update transferable knowledge
        updateTransferKnowledge(state, action);
        
        // Apply transfer learning
        float transferReward = transferKnowledge("primary");
        
        // Use transferred knowledge in learning
        reinforcementLearning(state, action, nextState, reward + transferReward, done);
    } else {
        // Basic meta-learning
        updateLearningParameters(learningRate_ * 0.5f, explorationRate_ * 1.2f, entropyCoefficient_);
    }
}

// Exploration strategy implementations
Action AdvancedAgentLearning::epsilonGreedyExploration(const std::vector<float>& state, float epsilon) {
    Action action;
    
    if (uniformDist_(rng_) < epsilon) {
        // Random exploration
        action.actionId = uniformDist_(rng_) * actionDimension_;
        action.probability = 1.0f / actionDimension_;
    } else {
        // Greedy action
        std::vector<float> probs = policyNetwork_->getOutputProbabilities(state);
        action.actionId = std::max_element(probs.begin(), probs.end()) - probs.begin();
        action.probability = probs[action.actionId];
    }
    
    action.value = computeValueEstimate(state);
    action.entropy = computeEntropy(std::vector<float>(policyNetwork_->getOutputProbabilities(state)));
    action.logProbability = std::log(action.probability + 1e-8f);
    
    return action;
}

Action AdvancedAgentLearning::thompsonSamplingExploration(const std::vector<float>& state) {
    Action action;
    
    // Sample from Beta distribution for each action (simplified)
    std::vector<float> samples(actionDimension_);
    for (size_t i = 0; i < actionDimension_; ++i) {
        std::beta_distribution<float> betaDist(1.0f + i, 1.0f + actionDimension_ - i);
        samples[i] = betaDist(rng_);
    }
    
    action.actionId = std::max_element(samples.begin(), samples.end()) - samples.begin();
    action.probability = samples[action.actionId];
    action.value = computeValueEstimate(state);
    action.entropy = computeEntropy(samples);
    action.logProbability = std::log(action.probability + 1e-8f);
    
    return action;
}

Action AdvancedAgentLearning::ucbExploration(const std::vector<float>& state) {
    Action action;
    
    std::vector<float> probs = policyNetwork_->getOutputProbabilities(state);
    
    // UCB formula: probability + c * sqrt(log(N)/N_i)
    // For simplicity, using probability + exploration bonus
    std::vector<float> ucbScores(actionDimension_);
    for (size_t i = 0; i < actionDimension_; ++i) {
        float explorationBonus = 0.5f * std::sqrt(std::log(stats_.stepCount + 1) / (i + 1));
        ucbScores[i] = probs[i] + explorationBonus;
    }
    
    action.actionId = std::max_element(ucbScores.begin(), ucbScores.end()) - ucbScores.begin();
    action.probability = ucbScores[action.actionId];
    action.value = computeValueEstimate(state);
    action.entropy = computeEntropy(probs);
    action.logProbability = std::log(action.probability + 1e-8f);
    
    return action;
}

Action AdvancedAgentLearning::entropyRegularizedExploration(const std::vector<float>& state, float temperature) {
    Action action;
    
    std::vector<float> probs = policyNetwork_->getOutputProbabilities(state);
    
    // Apply temperature to probabilities
    std::vector<float> tempProbs(actionDimension_);
    float sum = 0.0f;
    for (size_t i = 0; i < actionDimension_; ++i) {
        tempProbs[i] = std::pow(probs[i], 1.0f / temperature);
        sum += tempProbs[i];
    }
    
    for (size_t i = 0; i < actionDimension_; ++i) {
        tempProbs[i] /= sum;
    }
    
    // Sample from temperature-adjusted distribution
    std::discrete_distribution<size_t> dist(tempProbs.begin(), tempProbs.end());
    action.actionId = dist(rng_);
    action.probability = tempProbs[action.actionId];
    action.value = computeValueEstimate(state);
    action.entropy = computeEntropy(tempProbs);
    action.logProbability = std::log(action.probability + 1e-8f);
    
    return action;
}

// Learning rate adaptation
float AdvancedAgentLearning::adaptiveLearningRate(const std::vector<float>& gradients, size_t step) {
    // RMSprop-style adaptive learning rate
    static std::vector<float> avgSqGrad(10, 0.0f);  // Simplified for demo
    static float decayRate = 0.9f;
    
    float learningRate = learningRate_;
    
    // Update running average of squared gradients
    for (size_t i = 0; i < gradients.size() && i < avgSqGrad.size(); ++i) {
        avgSqGrad[i] = decayRate * avgSqGrad[i] + (1 - decayRate) * gradients[i] * gradients[i];
        
        // Apply adaptive scaling
        float rms = std::sqrt(avgSqGrad[i] + 1e-8f);
        learningRate = learningRate / (rms + 1e-8f);
    }
    
    // Apply annealing schedule
    learningRate = learningRateAnnealing(step, learningRate, learningRate_ * 0.01f, 10000);
    
    return clamp(learningRate, 0.0001f, 0.1f);
}

float AdvancedAgentLearning::learningRateAnnealing(size_t step, float initialRate,
                                                  float finalRate, size_t totalSteps) {
    if (step >= totalSteps) {
        return finalRate;
    }
    
    float progress = static_cast<float>(step) / totalSteps;
    return finalRate + (initialRate - finalRate) * (1.0f - progress);
}

// Experience replay
void AdvancedAgentLearning::storeExperience(const Transition& transition) {
    experiences_.push_back(transition);
    
    // Limit buffer size
    if (experiences_.size() > replayBufferSize_) {
        // Remove oldest experiences (FIFO)
        experiences_.erase(experiences_.begin(),
                          experiences_.begin() + (experiences_.size() - replayBufferSize_ / 2));
    }
}

std::vector<Transition> AdvancedAgentLearning::sampleExperience(float batchSize, bool prioritize) {
    std::vector<Transition> batch;
    size_t sampleSize = std::min(static_cast<size_t>(experiences_.size() * batchSize),
                               experiences_.size());
    
    if (sampleSize == 0 || experiences_.empty()) return batch;
    
    std::vector<size_t> indices(experiences_.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng_);
    
    batch.reserve(sampleSize);
    for (size_t i = 0; i < sampleSize; ++i) {
        batch.push_back(experiences_[indices[i]]);
    }
    
    return batch;
}

// Curiosity mechanisms
void AdvancedAgentLearning::updateCuriosityDrivenExploration(LearningState& state) {
    // Update curiosity based on surprise and novelty
    float surprise = computeSurprise(state.state, state.state);  // Simplified
    state.curiosity = (surprise * curiosityWeight_) + state.curiosity * 0.9f;
    state.curiosity = clamp(state.curiosity, 0.0f, 1.0f);
}

float AdvancedAgentLearning::computeIntrinsicReward(const std::vector<float>& state,
                                                  const std::vector<float>& nextState) {
    float novelty = computeSurprise(state, nextState);
    float predictionError = 0.0f;  // Simplified
    
    return curiosityWeight_ * (novelty + predictionError);
}

// Transfer learning
void AdvancedAgentLearning::updateTransferKnowledge(const LearningState& state, const Action& action) {
    // Store state-action pairs for transfer
    std::vector<float> key = state.state;
    transferableKnowledge_["primary"] = key;
}

float AdvancedAgentLearning::transferKnowledge(const std::string& sourceDomain) {
    if (transferableKnowledge_.count(sourceDomain)) {
        return 1.0f;  // Simplified transfer reward
    }
    return 0.0f;
}

// Social learning
void AdvancedAgentLearning::updateSocialKnowledge(const LearningState& state, const Action& action) {
    socialKnowledge_[state.state] = action;
}

void AdvancedAgentLearning::imitationLearning(const Action& expertAction, const LearningState& state) {
    expertDemonstrations_.push_back(expertAction);
    
    // Learn from expert demonstrations
    // This would implement DAGGER or similar algorithms
}

// Neural attention and representation
void AdvancedAgentLearning::computeAttentionWeights(const std::vector<float>& state,
                                                  std::vector<float>& attentionWeights) {
    // Simplified attention mechanism
    size_t dim = std::min(static_cast<size_t>(12), state.size());
    attentionWeights.resize(dim, 1.0f / dim);
    
    // Compute attention based on state variance
    float variance = 0.0f;
    for (float val : state) variance += val * val;
    variance /= state.size();
    
    // Adjust attention based on variance
    for (size_t i = 0; i < dim; ++i) {
        attentionWeights[i] *= (1.0f + variance);
    }
}

void AdvancedAgentLearning::computeRepresentation(const std::vector<float>& state,
                                                 std::vector<float>& representation) {
    // Simple auto-encoder-like representation
    size_t repSize = std::min(static_cast<size_t>(64), state.size() / 2);
    representation.resize(repSize);
    
    // Compress state to representation
    for (size_t i = 0; i < repSize; ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < state.size(); j += 2) {
            if (i < state.size() / 2) {
                sum += state[j] * (i + 1) / repSize;
            }
        }
        representation[i] = sum / state.size();
    }
}

// Helper functions
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