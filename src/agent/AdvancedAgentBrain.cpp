#include "AdvancedAgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// PolicyNetwork implementation
PolicyNetwork::PolicyNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize)
    : inputSize_(inputSize), hiddenSize_(hiddenSize), outputSize_(outputSize),
      strategy_(ActionSelectionStrategy::SOFTMAX), advancedLearningEnabled_(true),
      explorationRate_(0.1f), entropyFactor_(0.01f), maxBufferSize_(10000),
      replayProbability_(0.8f), learningSteps_(0), totalReward_(0.0f),
      lastUpdateTime_(0.0) {
    
    // Initialize neural network weights with better initialization
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> weightDist(0.0f, 0.1f);
    std::uniform_real_distribution<float> biasDist(-0.1f, 0.1f);
    
    policyWeights1_.resize(inputSize_ * hiddenSize_);
    policyWeights2_.resize(hiddenSize_ * outputSize_);
    policyBiases1_.resize(hiddenSize_, 0.0f);
    policyBiases2_.resize(outputSize_, 0.0f);
    
    for (float& w : policyWeights1_) w = weightDist(gen);
    for (float& w : policyWeights2_) w = weightDist(gen);
    for (float& b : policyBiases1_) b = biasDist(gen);
    for (float& b : policyBiases2_) b = biasDist(gen);
}

std::vector<float> PolicyNetwork::forward(const std::vector<float>& input) {
    if (input.size() != inputSize_) {
        NLM_LOG_WARNING("PolicyNetwork input size mismatch: expected " + 
                       std::to_string(inputSize_) + ", got " + std::to_string(input.size()));
        return std::vector<float>(outputSize_, 0.0f);
    }
    
    // Forward pass through hidden layer
    std::vector<float> hidden(hiddenSize_, 0.0f);
    for (size_t i = 0; i < hiddenSize_; ++i) {
        float sum = policyBiases1_[i];
        for (size_t j = 0; j < inputSize_; ++j) {
            sum += input[j] * policyWeights1_[i * inputSize_ + j];
        }
        hidden[i] = sigmoid(sum);
    }
    
    // Forward pass through output layer
    std::vector<float> output(outputSize_, 0.0f);
    for (size_t i = 0; i < outputSize_; ++i) {
        float sum = policyBiases2_[i];
        for (size_t j = 0; j < hiddenSize_; ++j) {
            sum += hidden[j] * policyWeights2_[i * hiddenSize_ + j];
        }
        output[i] = sigmoid(sum);
    }
    
    return output;
}

void PolicyNetwork::update(const std::vector<float>& input, const std::vector<float>& target, float learningRate) {
    if (input.size() != inputSize_ || target.size() != outputSize_) {
        NLM_LOG_ERROR("PolicyNetwork update size mismatch");
        return;
    }
    
    // Forward pass to get current output
    std::vector<float> output = forward(input);
    
    // Calculate error gradient
    std::vector<float> error(outputSize_);
    for (size_t i = 0; i < outputSize_; ++i) {
        error[i] = (target[i] - output[i]) * output[i] * (1.0f - output[i]);
    }
    
    // Backpropagation through second layer
    std::vector<float> hiddenError(hiddenSize_, 0.0f);
    for (size_t j = 0; j < hiddenSize_; ++j) {
        for (size_t i = 0; i < outputSize_; ++i) {
            hiddenError[j] += error[i] * policyWeights2_[i * hiddenSize_ + j];
        }
        hiddenError[j] *= hidden[j] * (1.0f - hidden[j]);
    }
    
    // Update weights (simple SGD)
    for (size_t i = 0; i < outputSize_; ++i) {
        for (size_t j = 0; j < hiddenSize_; ++j) {
            policyWeights2_[i * hiddenSize_ + j] += learningRate * error[i] * hidden[j];
        }
        policyBiases2_[i] += learningRate * error[i];
    }
    
    for (size_t i = 0; i < hiddenSize_; ++i) {
        for (size_t j = 0; j < inputSize_; ++j) {
            policyWeights1_[i * inputSize_ + j] += learningRate * hiddenError[i] * input[j];
        }
        policyBiases1_[i] += learningRate * hiddenError[i];
    }
    
    learningSteps_++;
}

void PolicyNetwork::addExperience(const std::vector<float>& state, MotorCommand action,
                                 float reward, const std::vector<float>& nextState, bool done) {
    Experience exp(state, action, reward, nextState, done);
    exp.timestamp = lastUpdateTime_++;
    
    experienceBuffer_.push_back(exp);
    
    if (experienceBuffer_.size() > maxBufferSize_) {
        experienceBuffer_.erase(experienceBuffer_.begin());
    }
}

void PolicyNetwork::trainExperienceReplay(size_t batchSize) {
    if (experienceBuffer_.empty()) return;
    
    size_t batchSizeActual = std::min(batchSize, experienceBuffer_.size());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, experienceBuffer_.size() - 1);
    
    float totalLoss = 0.0f;
    size_t samplesProcessed = 0;
    
    for (size_t i = 0; i < batchSizeActual; ++i) {
        size_t idx = dist(gen);
        const Experience& exp = experienceBuffer_[idx];
        
        // Get current policy prediction
        std::vector<float> currentPolicy = forward(exp.state);
        std::vector<float> target = currentPolicy;
        
        // Calculate TD target
        float nextValue = 0.0f;
        if (!exp.done && !exp.nextState.empty()) {
            nextValue = forward(exp.nextState)[0]; // Assume first output is value estimate
        }
        
        float tdTarget = exp.reward + discountFactor_ * nextValue;
        target[0] = tdTarget; // Update value estimate
        
        // Update policy network
        update(exp.state, target, learningRate_);
        
        // Calculate loss
        float loss = 0.0f;
        for (size_t j = 0; j < outputSize_; ++j) {
            loss += (target[j] - currentPolicy[j]) * (target[j] - currentPolicy[j]);
        }
        totalLoss += loss;
        samplesProcessed++;
    }
    
    if (samplesProcessed > 0) {
        totalReward_ += experienceBuffer_[dist(gen)].reward; // Add some reward for successful learning
    }
}

void PolicyNetwork::reset() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> weightDist(0.0f, 0.1f);
    std::uniform_real_distribution<float> biasDist(-0.1f, 0.1f);
    
    for (float& w : policyWeights1_) w = weightDist(gen);
    for (float& w : policyWeights2_) w = weightDist(gen);
    for (float& b : policyBiases1_) b = biasDist(gen);
    for (float& b : policyBiases2_) b = biasDist(gen);
    
    experienceBuffer_.clear();
    learningSteps_ = 0;
    totalReward_ = 0.0f;
}

float PolicyNetwork::softmax(const std::vector<float>& logits) const {
    float maxLogit = *std::max_element(logits.begin(), logits.end());
    float sum = 0.0f;
    for (float logit : logits) {
        sum += std::exp(logit - maxLogit);
    }
    return sum;
}

float PolicyNetwork::entropy(const std::vector<float>& probabilities) const {
    float entropy = 0.0f;
    for (float p : probabilities) {
        if (p > 0.0f) {
            entropy -= p * std::log(p);
        }
    }
    return entropy;
}

// AdvancedAgentBrain implementation
AdvancedAgentBrain::AdvancedAgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain), currentState_(AgentState::IDLE), lastTimestamp_(0.0),
      learningRate_(0.001f), discountFactor_(0.99f), polyakAvgTau_(0.005f),
      metaLearningEnabled_(false), dopamineLevel_(0.0f), serotoninLevel_(0.0f),
      noradrenalineLevel_(0.0f), acetylcholineLevel_(0.0f),
      intrinsicMotivation_(0.0f), extrinsicCuriosity_(0.0f),
      adaptiveExplorationRate_(0.1f), developmentalAge_(0.0), plasticityModifier_(1.0f),
      learningCapacity_(1.0f), rewardModulationEnabled_(true),
      structuralPlasticityEnabled_(true), developmentEnabled_(true),
      curiosityEnabled_(true), adaptiveExplorationEnabled_(false) {
}

AdvancedAgentBrain::~AdvancedAgentBrain() = default;

void AdvancedAgentBrain::initialize(const SimpleWorld& world, std::shared_ptr<Config> config) {
    config_ = config;
    
    // Configure based on brain size and world dimensions
    size_t inputSize = world.getVisionWidth() * world.getVisionHeight() + 8 + 4 + 6; // vision + touch + internal + proprioception
    size_t hiddenSize = std::max(size_t(64), brain_->getTotalNeuronCount() / 100);
    size_t outputSize = 6; // Number of motor actions
    
    policyNetwork_.configurePolicyNetwork(inputSize, hiddenSize, outputSize);
    
    // Configure advanced learning based on config
    if (config_) {
        if (auto lr = config_->get<double>("advanced.learning_rate")) {
            learningRate_ = static_cast<float>(*lr);
        }
        if (auto gamma = config_->get<double>("advanced.discount_factor")) {
            discountFactor_ = static_cast<float>(*gamma);
        }
        if (auto tau = config_->get<double>("advanced.polyak_tau")) {
            polyakAvgTau_ = static_cast<float>(*tau);
        }
        if (auto enabled = config_->get<bool>("advanced.meta_learning_enabled")) {
            metaLearningEnabled_ = *enabled;
        }
        if (auto size = config_->get<size_t>("advanced.experience_replay_size")) {
            policyNetwork_.setExperienceReplaySize(*size);
        }
        if (auto strategy = config_->get<std::string>("advanced.action_strategy")) {
            setActionSelectionStrategyFromString(*strategy);
        }
    }
    
    currentState_ = AgentState::EXPLORATION;
    episodeStartTime_ = 0.0;
    metrics_.episodesCompleted = 0;
    performanceHistory_.clear();
    
    NLM_LOG_INFO("Advanced AgentBrain initialized with policy network: " +
                 std::to_string(inputSize) + " inputs, " + std::to_string(hiddenSize) + 
                 " hidden, " + std::to_string(outputSize) + " outputs");
}

void AdvancedAgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Extract state features for policy network
    currentStateVector_ = extractStateFeatures(percept);
    
    // Update novelty and intrinsic motivation
    updateIntrinsicMotivation(currentState_);
    
    // Apply neuromodulation
    updateNeuromodulationSignals(0.0f, 0.0f); // Will be updated when reward is received
    
    // Update development
    updateAdvancedDevelopment(0.001); // Default timestep
}

MotorCommand AdvancedAgentBrain::selectAction(const std::vector<float>& state) {
    if (state.empty() || policyNetwork_.getStrategy() == ActionSelectionStrategy::EPSILON_GREEDY) {
        // Simple epsilon-greedy for basic compatibility
        float r = std::rand() / (RAND_MAX + 1.0f);
        if (r < policyNetwork_.getExplorationRate()) {
            // Random action
            int choice = std::rand() % 7; // Match the original random selection range
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    // Use policy network for action selection
    std::vector<float> policy = policyNetwork_.forward(state);
    
    switch (policyNetwork_.getStrategy()) {
        case ActionSelectionStrategy::SOFTMAX:
        case ActionSelectionStrategy::POLICY_GRADIENT:
        case ActionSelectionStrategy::ACTOR_CRITIC:
        case ActionSelectionStrategy::PPO:
            // Probabilistic action selection based on policy
            float sum = policyNetwork_.softmax(policy);
            float r = std::rand() / (RAND_MAX + 1.0f) * sum;
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < policy.size(); ++i) {
                cumulative += std::exp(policy[i] - *std::max_element(policy.begin(), policy.end()));
                if (r <= cumulative) {
                    // Map policy index to motor command
                    switch (i) {
                        case 0: return MotorCommand::MoveForward;
                        case 1: return MotorCommand::MoveBackward;
                        case 2: return MotorCommand::TurnLeft;
                        case 3: return MotorCommand::TurnRight;
                        case 4: return MotorCommand::LookLeft;
                        case 5: return MotorCommand::LookRight;
                        case 6: return MotorCommand::Interact;
                        default: return MotorCommand::Wait;
                    }
                }
            }
            break;
            
        case ActionSelectionStrategy::UCB1:
            // Upper Confidence Bound for action selection
            // Simplified UCB1 implementation
            float explorationBonus = std::sqrt(std::log(metrics_.episodesCompleted + 1) / 1.0f);
            for (size_t i = 0; i < policy.size(); ++i) {
                policy[i] += explorationBonus;
            }
            break;
            
        case ActionSelectionStrategy::BAYESIAN_OPTIMIZATION:
            // Bayesian action selection (placeholder)
            break;
            
        case ActionSelectionStrategy::TRPO:
            // Trust Region Policy Optimization (placeholder)
            break;
            
        default:
            // Default to softmax
            float sum = policyNetwork_.softmax(policy);
            float r = std::rand() / (RAND_MAX + 1.0f) * sum;
            float cumulative = 0.0f;
            
            for (size_t i = 0; i < policy.size(); ++i) {
                cumulative += std::exp(policy[i] - *std::max_element(policy.begin(), policy.end()));
                if (r <= cumulative) {
                    switch (i) {
                        case 0: return MotorCommand::MoveForward;
                        case 1: return MotorCommand::MoveBackward;
                        case 2: return MotorCommand::TurnLeft;
                        case 3: return MotorCommand::TurnRight;
                        case 4: return MotorCommand::LookLeft;
                        case 5: return MotorCommand::LookRight;
                        case 6: return MotorCommand::Interact;
                        default: return MotorCommand::Wait;
                    }
                }
            }
    }
    
    // Fallback to epsilon-greedy if policy selection fails
    float r = std::rand() / (RAND_MAX + 1.0f);
    if (r < policyNetwork_.getExplorationRate()) {
        int choice = std::rand() % 7;
        switch (choice) {
            case 0: return MotorCommand::MoveForward;
            case 1: return MotorCommand::MoveBackward;
            case 2: return MotorCommand::TurnLeft;
            case 3: return MotorCommand::TurnRight;
            case 4: return MotorCommand::LookLeft;
            case 5: return MotorCommand::LookRight;
            case 6: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
    
    // Default action
    return MotorCommand::Wait;
}

void AdvancedAgentBrain::learnFromExperience(const std::vector<float>& state, MotorCommand action,
                                           float reward, const std::vector<float>& nextState, bool done) {
    // Store experience in episode buffer
    Experience exp(state, action, reward, nextState, done);
    exp.timestamp = lastTimestamp_++;
    episodeBuffer_.push_back(exp);
    
    // Apply advanced reward modulation
    applyAdvancedRewardModulation(reward, 0.0f); // Will be updated with prediction error later
    
    // Update policy network if learning is enabled
    if (policyNetwork_.isAdvancedLearningEnabled()) {
        policyNetwork_.addExperience(state, action, reward, nextState, done);
        
        // Perform experience replay periodically
        if (episodeBuffer_.size() > 100) {
            performExperienceReplay();
        }
    }
    
    // Update development
    updateAdvancedDevelopment(0.001);
    
    // Update performance metrics
    updatePerformanceMetrics(reward, lastTimestamp_);
    
    // Store last action/reward for metrics
    lastAction_ = action;
    lastReward_ = reward;
}

void AdvancedAgentBrain::applyAdvancedRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_) return;
    
    // Calculate prediction error
    float predictionError = reward - predictedReward;
    
    // Apply multiple neuromodulators based on reward prediction error
    dopamineLevel_ = std::clamp(predictionError * 0.5f, -1.0f, 1.0f);  // Reward prediction error
    serotoninLevel_ = std::clamp(1.0f - std::abs(predictionError) * 0.5f, 0.0f, 1.0f);  // Punishment sensitivity
    noradrenalineLevel_ = std::clamp(intrinsicMotivation_ * 0.3f, 0.0f, 1.0f);  // Arousal/attention
    acetylcholineLevel_ = std::clamp(plasticityModifier_ * 0.2f, 0.0f, 1.0f);  // Learning rate modulation
    
    // Apply neuromodulation to policy network
    if (metaLearningEnabled_) {
        // Update exploration rate based on neuromodulators
        adaptiveExplorationRate_ = 0.1f + (0.5f * noradrenalineLevel_) - (0.2f * std::abs(predictionError));
        adaptiveExplorationRate_ = std::clamp(adaptiveExplorationRate_, 0.01f, 0.5f);
        policyNetwork_.setExplorationRate(adaptiveExplorationRate_);
    }
    
    // Apply to brain systems
    if (brain_) {
        // Update plasticity based on neuromodulators
        float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Update STDP parameters
        auto* stdp = brain_->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(0.01f * plasticityFactor);
            stdp->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

void AdvancedAgentBrain::updateAdvancedDevelopment(double timestep) {
    if (!developmentEnabled_) return;
    
    developmentalAge_ += timestep;
    
    // Advanced developmental stages with more granularity
    if (developmentalAge_ < 30.0) {  // ~0.5 minute
        currentState_ = AgentState::EXPLORATION;
        plasticityModifier_ = 1.2f;  // Slightly higher plasticity for exploration
        learningCapacity_ = 1.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 150.0) {  // ~2.5 minutes
        currentState_ = AgentState::LEARNING;
        plasticityModifier_ = 1.0f;
        learningCapacity_ = 1.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 450.0) {  // ~7.5 minutes
        currentState_ = AgentState::EXPLOITATION;
        plasticityModifier_ = 0.7f;
        learningCapacity_ = 0.9f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        currentState_ = AgentState::CONSOLIDATING;
        plasticityModifier_ = 0.3f;  // Adult - stable but capable of learning
        learningCapacity_ = 0.7f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // More sophisticated synaptogenesis and pruning
            float synRate = 0.0001f * plasticityModifier_ * learningCapacity_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_) * (2.0f - learningCapacity_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

void AdvancedAgentBrain::performExperienceReplay() {
    if (episodeBuffer_.empty()) return;
    
    // Use policy network for experience replay
    policyNetwork_.trainExperienceReplay(std::min<size_t>(32, episodeBuffer_.size()));
    
    // Clear episode buffer after replay
    episodeBuffer_.clear();
    metrics_.episodesCompleted++;
}

void AdvancedAgentBrain::adaptPolicyBasedOnPerformance(float performanceMetric) {
    if (!metaLearningEnabled_) return;
    
    // Update performance history
    performanceHistory_.push_back(performanceMetric);
    if (performanceHistory_.size() > 100) {
        performanceHistory_.erase(performanceHistory_.begin());
    }
    
    // Calculate average performance
    float avgPerformance = std::accumulate(performanceHistory_.begin(), performanceHistory_.end(), 0.0f) / performanceHistory_.size();
    
    // Adapt exploration strategy based on performance
    if (avgPerformance > 0.8f) {
        // Good performance, reduce exploration
        policyNetwork_.setExplorationRate(std::max(0.01f, policyNetwork_.getExplorationRate() * 0.9f));
    } else if (avgPerformance < 0.3f) {
        // Poor performance, increase exploration
        policyNetwork_.setExplorationRate(std::min(0.5f, policyNetwork_.getExplorationRate() * 1.1f));
    }
}

void AdvancedAgentBrain::updateExplorationStrategy() {
    if (!adaptiveExplorationEnabled_) return;
    
    // Update exploration based on novelty and curiosity
    float noveltyBonus = intrinsicMotivation_ * 0.3f;
    float curiosityBonus = extrinsicCuriosity_ * 0.2f;
    float combinedBonus = noveltyBonus + curiosityBonus;
    
    // Adaptive exploration rate
    float targetExploration = 0.1f + combinedBonus * 0.3f;
    targetExploration = std::clamp(targetExploration, 0.05f, 0.4f);
    
    // Smooth transition
    float explorationChange = targetExploration - policyNetwork_.getExplorationRate();
    float newExplorationRate = policyNetwork_.getExplorationRate() + explorationChange * 0.1f;
    policyNetwork_.setExplorationRate(newExplorationRate);
}

float AdvancedAgentBrain::getExpectedValue(const std::vector<float>& state) const {
    std::vector<float> policy = policyNetwork_.forward(state);
    if (policy.empty()) return 0.0f;
    return policy[0]; // Assume first output is value estimate
}

float AdvancedAgentBrain::getPolicyEntropy() const {
    // Calculate entropy of the policy distribution
    float entropy = 0.0f;
    for (float p : currentStateVector_) {
        if (p > 0.0f && p < 1.0f) {
            entropy -= p * std::log(p);
        }
    }
    return entropy;
}

float AdvancedAgentBrain::getLearningProgress() const {
    return std::min(1.0f, static_cast<float>(policyNetwork_.getTotalLearningSteps()) / 1000.0f);
}

float AdvancedAgentBrain::getCuriosityScore() const {
    return intrinsicMotivation_ * 0.6f + extrinsicCuriosity_ * 0.4f;
}

float AdvancedAgentBrain::getNoveltyScore() const {
    return noveltyLevel_ * 0.5f + curiosityLevel_ * 0.5f;
}

void AdvancedAgentBrain::setActionSelectionStrategy(ActionSelectionStrategy strategy) {
    policyNetwork_.setStrategy(strategy);
    
    // Configure strategy-specific parameters
    switch (strategy) {
        case ActionSelectionStrategy::UCB1:
            policyNetwork_.setExplorationRate(0.2f);
            break;
        case ActionSelectionStrategy::SOFTMAX:
            policyNetwork_.setExplorationRate(0.1f);
            break;
        case ActionSelectionStrategy::POLICY_GRADIENT:
            policyNetwork_.setExplorationRate(0.15f);
            break;
        default:
            policyNetwork_.setExplorationRate(0.1f);
    }
}

void AdvancedAgentBrain::setActionSelectionStrategyFromString(const std::string& strategyName) {
    std::string lowerStrategy = strategyName;
    std::transform(lowerStrategy.begin(), lowerStrategy.end(), lowerStrategy.begin(), ::tolower);
    
    if (lowerStrategy == "epsilon_greedy") {
        setActionSelectionStrategy(ActionSelectionStrategy::EPSILON_GREEDY);
    } else if (lowerStrategy == "softmax") {
        setActionSelectionStrategy(ActionSelectionStrategy::SOFTMAX);
    } else if (lowerStrategy == "ucb1") {
        setActionSelectionStrategy(ActionSelectionStrategy::UCB1);
    } else if (lowerStrategy == "bayesian") {
        setActionSelectionStrategy(ActionSelectionStrategy::BAYESIAN_OPTIMIZATION);
    } else if (lowerStrategy == "policy_gradient") {
        setActionSelectionStrategy(ActionSelectionStrategy::POLICY_GRADIENT);
    } else if (lowerStrategy == "actor_critic") {
        setActionSelectionStrategy(ActionSelectionStrategy::ACTOR_CRITIC);
    } else if (lowerStrategy == "trpo") {
        setActionSelectionStrategy(ActionSelectionStrategy::TRPO);
    } else if (lowerStrategy == "ppo") {
        setActionSelectionStrategy(ActionSelectionStrategy::PPO);
    } else {
        NLM_LOG_WARNING("Unknown action selection strategy: " + strategyName + ", using SOFTMAX");
        setActionSelectionStrategy(ActionSelectionStrategy::SOFTMAX);
    }
}

void AdvancedAgentBrain::enableAdaptiveExploration(bool enable) {
    adaptiveExplorationEnabled_ = enable;
    if (enable) {
        policyNetwork_.setExplorationRate(0.2f);
    }
}

void AdvancedAgentBrain::setExplorationSchedule(const std::vector<float>& schedule) {
    // Implement exploration schedule (e.g., epsilon decay over time)
    explorationSchedule_ = schedule;
}

void AdvancedAgentBrain::updatePerformanceMetrics(float reward, double timestamp) {
    metrics_.recentReturn = reward;
    metrics_.episodeLength = static_cast<float>(timestamp - episodeStartTime_);
    metrics_.explorationEfficiency = (reward > 0.0f) ? 1.0f : 0.0f;
    metrics_.learningRate = learningRate_;
    metrics_.policyEntropy = getPolicyEntropy();
    metrics_.lastUpdateTime = timestamp;
    
    // Calculate exploration efficiency
    float explorationCost = 1.0f - (policyNetwork_.getExplorationRate() * 10.0f);
    explorationCost = std::clamp(explorationCost, 0.0f, 1.0f);
    metrics_.explorationEfficiency = (reward - explorationCost) / (reward + 0.1f);
}

void AdvancedAgentBrain::savePolicy(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save policy to " + filepath);
        return;
    }
    
    // Save policy network parameters
    file.write(reinterpret_cast<const char*>(&inputSize_), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(&hiddenSize_), sizeof(size_t));
    file.write(reinterpret_cast<const char*>(&outputSize_), sizeof(size_t));
    
    // Save weights
    file.write(reinterpret_cast<const char*>(policyNetwork_.getPolicyWeights1().data()),
               policyNetwork_.getPolicyWeights1().size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(policyNetwork_.getPolicyWeights2().data()),
               policyNetwork_.getPolicyWeights2().size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(policyNetwork_.getPolicyBiases1().data()),
               policyNetwork_.getPolicyBiases1().size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(policyNetwork_.getPolicyBiases2().data()),
               policyNetwork_.getPolicyBiases2().size() * sizeof(float));
    
    file.close();
}

bool AdvancedAgentBrain::loadPolicy(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load policy from " + filepath);
        return false;
    }
    
    // Load policy network parameters
    file.read(reinterpret_cast<char*>(&inputSize_), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&hiddenSize_), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&outputSize_), sizeof(size_t));
    
    policyNetwork_.configurePolicyNetwork(inputSize_, hiddenSize_, outputSize_);
    
    // Load weights
    std::vector<float> weights1(inputSize_ * hiddenSize_);
    std::vector<float> weights2(hiddenSize_ * outputSize_);
    std::vector<float> biases1(hiddenSize_);
    std::vector<float> biases2(outputSize_);
    
    file.read(reinterpret_cast<char*>(weights1.data()), weights1.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(weights2.data()), weights2.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(biases1.data()), biases1.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(biases2.data()), biases2.size() * sizeof(float));
    
    policyNetwork_.setPolicyWeights1(weights1);
    policyNetwork_.setPolicyWeights2(weights2);
    policyNetwork_.setPolicyBiases1(biases1);
    policyNetwork_.setPolicyBiases2(biases2);
    
    file.close();
    return true;
}

std::vector<float> AdvancedAgentBrain::extractStateFeatures(const SensoryPercept& percept) const {
    std::vector<float> state;
    
    // Vision features
    const auto& vision = percept.getVision();
    state.insert(state.end(), vision.begin(), vision.end());
    
    // Touch features
    const auto& touch = percept.getTouch();
    state.insert(state.end(), touch.begin(), touch.end());
    
    // Internal features
    const auto& internal = percept.getInternal();
    state.insert(state.end(), internal.begin(), internal.end());
    
    // Proprioception features
    const auto& proprioception = percept.getProprioception();
    state.insert(state.end(), proprioception.begin(), proprioception.end());
    
    // Audio features (if available)
    const auto& audio = percept.getAudio();
    state.insert(state.end(), audio.begin(), audio.end());
    
    return state;
}

float AdvancedAgentBrain::calculateTDTarget(float reward, float nextValue, bool done) const {
    if (done) {
        return reward;
    }
    return reward + discountFactor_ * nextValue;
}

void AdvancedAgentBrain::softUpdatePolicy(float tau) {
    // Polyak averaging for target network (simplified)
    // In practice, this would update target network parameters
    // For now, just log the update
    NLM_LOG_INFO("Policy soft update with tau = " + std::to_string(tau));
}

} // namespace nlm