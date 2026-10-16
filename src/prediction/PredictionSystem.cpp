#include "PredictionSystem.hpp"
#include "PolicyNetwork.hpp"
#include "ValueNetwork.hpp"
#include "ForwardModel.hpp"
#include "InverseModel.hpp"
#include "CuriosityDrivenExploration.hpp"
#include "RewardPredictor.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    
    // State tracking
    std::vector<float> lastState;
    std::vector<float> lastAction;
    std::vector<float> predictedState;
    
    // Prediction statistics
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Reward tracking
    float lastReward;
    float predictedReward;
    
    // Novelty tracking
    float novelty;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), 
             lastReward(0.0f), predictedReward(0.0f), novelty(0.0f) {}
};

PredictionSystem::PredictionSystem()
    : pImpl(new Impl)
    , stateSize_(0)
    , actionSize_(0)
    , learningRate_(0.001f)
    , predictionHorizon_(1)
    , forwardModelEnabled_(true)
    , inverseModelEnabled_(true)
    , policyNetworkEnabled_(true)
    , valueNetworkEnabled_(true)
    , curiosityEnabled_(true)
    , rewardPredictionEnabled_(true)
    , brain_(nullptr)
{
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Create neural network components
    if (forwardModelEnabled_) {
        forwardModel_ = std::make_unique<ForwardModel>(stateSize_, actionSize_, learningRate_);
        forwardModel_->setBrain(brain);
    }
    
    if (inverseModelEnabled_) {
        inverseModel_ = std::make_unique<InverseModel>(stateSize_, actionSize_, learningRate_);
        inverseModel_->setBrain(brain);
    }
    
    if (policyNetworkEnabled_) {
        policyNetwork_ = std::make_unique<PolicyNetwork>(stateSize_, actionSize_, learningRate_);
        policyNetwork_->setBrain(brain);
    }
    
    if (valueNetworkEnabled_) {
        valueNetwork_ = std::make_unique<ValueNetwork>(stateSize_, learningRate_);
        valueNetwork_->setBrain(brain);
    }
    
    if (curiosityEnabled_) {
        curiosityExplorer_ = std::make_unique<CuriosityDrivenExploration>(0.5f, 0.5f, learningRate_);
    }
    
    if (rewardPredictionEnabled_) {
        rewardPredictor_ = std::make_unique<RewardPredictor>(stateSize_, actionSize_, learningRate_);
        rewardPredictor_->setBrain(brain);
    }
    
    NLM_LOG_INFO("PredictionSystem initialized with RL integration");
}

std::vector<float> PredictionSystem::predictNextState(const std::vector<float>& currentState,
                                                     ActionType action) {
    if (!forwardModelEnabled_ || !forwardModel_ || currentState.empty()) {
        return currentState; // No prediction, return current state
    }
    
    // Get prediction from forward model
    std::vector<float> predicted = forwardModel_->predict(currentState, action);
    
    if (predicted.empty()) {
        // Use inverse model to infer next state
        if (inverseModelEnabled_ && !lastState.empty()) {
            predicted = inverseModel_->predictAction(lastState, currentState);
            // In a real implementation, this would use the inverse model differently
            // For now, use current state as prediction
            predicted = currentState;
        } else {
            predicted = currentState;
        }
    }
    
    pImpl->predictedState = predicted;
    return predicted;
}

float PredictionSystem::updatePredictions(const std::vector<float>& predicted,
                                        const std::vector<float>& actual,
                                        float reward) {
    if (predicted.empty() || actual.empty()) {
        pImpl->predictionError = 0.0f;
        return 0.0f;
    }
    
    // Compute prediction error
    pImpl->predictionError = 0.0f;
    for (size_t i = 0; i < predicted.size() && i < actual.size(); ++i) {
        float diff = predicted[i] - actual[i];
        pImpl->predictionError += diff * diff;
    }
    pImpl->predictionError /= predicted.size();
    
    // Update confidence based on error
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    
    // Record error history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Train forward model with this experience
    train(pImpl->lastState, static_cast<ActionType>(pImpl->lastAction[0]),
          actual, reward);
    
    // Update novelty
    pImpl->novelty = computeNovelty(actual);
    
    // Update reward prediction if enabled
    if (rewardPredictionEnabled_ && rewardPredictor_) {
        rewardPredictor_->train(pImpl->lastState, 
                               static_cast<ActionType>(pImpl->lastAction[0]),
                               reward);
    }
    
    return pImpl->predictionError;
}

void PredictionSystem::train(const std::vector<float>& state,
                           ActionType action,
                           const std::vector<float>& nextState,
                           float reward) {
    // Store last state for reference
    pImpl->lastState = state;
    pImpl->lastAction.push_back(static_cast<float>(action));
    if (pImpl->lastAction.size() > 10) {
        pImpl->lastAction.erase(pImpl->lastAction.begin());
    }
    
    // Train forward model
    if (forwardModelEnabled_ && forwardModel_) {
        forwardModel_->train(state, action, nextState, reward);
    }
    
    // Train inverse model
    if (inverseModelEnabled_ && inverseModel_) {
        inverseModel_->train(state, action, nextState);
    }
    
    // Train policy network (RL)
    if (policyNetworkEnabled_ && policyNetwork_) {
        policyNetwork_->update(state, action, reward, nextState, false);
    }
    
    // Train value network (RL)
    if (valueNetworkEnabled_ && valueNetwork_) {
        valueNetwork_->update(state, reward);
    }
}

ActionType PredictionSystem::selectAction(const std::vector<float>& state,
                                        float explorationRate) {
    if (state.empty() || policyNetworkEnabled_) {
        return ActionType::Wait;
    }
    
    // Use policy network if enabled
    if (policyNetworkEnabled_ && policyNetwork_) {
        // Add curiosity bonus to guide exploration
        float curiosityBonus = 0.0f;
        if (curiosityEnabled_ && curiosityExplorer_) {
            curiosityBonus = curiosityExplorer_->computeExplorationBonus(
                pImpl->predictionError, pImpl->novelty, state, ActionType::Wait);
        }
        
        // Adjust exploration based on curiosity
        float adjustedExploration = explorationRate + curiosityBonus * 0.5f;
        adjustedExploration = std::clamp(adjustedExploration, 0.0f, 1.0f);
        
        return policyNetwork_->getActionWithExploration(state, adjustedExploration);
    }
    
    // Fallback: epsilon-greedy selection
    float randVal = getRandomFloat(0.0f, 1.0f);
    if (randVal < explorationRate) {
        // Random action
        return static_cast<ActionType>(getRandomInt(0, actionSize_ - 1));
    } else {
        // Greedy action (based on stored preferences)
        return getGreedyAction(state);
    }
}

float PredictionSystem::evaluateState(const std::vector<float>& state) {
    if (!valueNetworkEnabled_ || !valueNetwork_ || state.empty()) {
        return 0.0f;
    }
    
    return valueNetwork_->getValue(state);
}

float PredictionSystem::computeExplorationBonus(const std::vector<float>& state,
                                               ActionType action,
                                               float predictionError,
                                               float novelty) {
    if (!curiosityEnabled_ || !curiosityExplorer_) {
        return 0.0f;
    }
    
    return curiosityExplorer_->computeExplorationBonus(
        predictionError, novelty, state, action);
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->lastState.clear();
    pImpl->lastAction.clear();
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

float PredictionSystem::computeNovelty(const std::vector<float>& state) {
    if (!curiosityExplorer_) return 0.0f;
    
    curiosityExplorer_->updateStateNovelty(state);
    return curiosityExplorer_->getStateNoveltyBonus(state);
}

ActionType PredictionSystem::getGreedyAction(const std::vector<float>& state) {
    if (inverseModelEnabled_ && inverseModel_) {
        // Use inverse model to select best action
        return ActionType::Wait; // Simplified
    }
    
    return ActionType::Wait; // Default
}

float PredictionSystem::getRandomFloat(float min, float max) {
    if (!brain_) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    return brain_->getRandomGenerator()->uniformReal(min, max);
}

int PredictionSystem::getRandomInt(int min, int max) {
    if (!brain_) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
    return brain_->getRandomGenerator()->uniformInt(min, max);
}

} // namespace nlm
