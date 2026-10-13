#include "MotorSystem.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::mt19937 rng;
    
    // Action selection weights based on neural activity
    std::vector<float> actionWeights;
    float explorationRate;
    
    Impl() : rng(std::random_device()()), explorationRate(0.1f) {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::Interact,
            ActionType::Wait
        };
        actionWeights.resize(availableActions.size(), 1.0f);
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const class NeuralRegion& motorRegion) {
    // Intelligent action selection based on neural activity patterns
    // Uses weighted combination of motor neuron activity and exploration
    
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get motor neuron activity from the region
    auto neurons = motorRegion.getAllNeurons();
    std::vector<float> neuronActivations;
    
    for (auto* neuron : neurons) {
        if (neuron) {
            // Convert membrane potential to activation level (0-1)
            float activation = std::tanh(neutron->getMembranePotential() / 50.0f);
            neuronActivations.push_back(activation);
        }
    }
    
    if (neuronActivations.empty()) {
        // No neural activity, use exploration
        return selectActionRandom();
    }
    
    // Map neural activity to action selection
    std::vector<float> actionScores(pImpl->availableActions.size(), 0.0f);
    
    // Distribute neural activity across actions based on connectivity
    float totalActivation = 0.0f;
    for (size_t i = 0; i < neurons.size() && i < pImpl->availableActions.size(); ++i) {
        float activation = neuronActivations[i];
        actionScores[i] = activation;
        totalActivation += activation;
    }
    
    // Apply action weights (learned preferences)
    for (size_t i = 0; i < actionScores.size(); ++i) {
        actionScores[i] *= pImpl->actionWeights[i];
    }
    
    // Add exploration component
    if (std::rand() / (RAND_MAX + 1.0) < pImpl->explorationRate) {
        // Random action selection for exploration
        size_t randomAction = std::uniform_int_distribution<size_t>(0, pImpl->availableActions.size() - 1)(pImpl->rng);
        float randomScore = std::rand() / (RAND_MAX + 1.0) * 0.5f;
        actionScores[randomAction] += randomScore;
    }
    
    // Select action with highest score
    size_t bestAction = 0;
    float bestScore = actionScores[0];
    for (size_t i = 1; i < actionScores.size(); ++i) {
        if (actionScores[i] > bestScore) {
            bestScore = actionScores[i];
            bestAction = i;
        }
    }
    
    // Create and return the selected action
    auto action = std::make_unique<Action>(pImpl->availableActions[bestAction]);
    
    // Record in action history
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

std::unique_ptr<Action> MotorSystem::selectActionRandom() {
    // Random action selection for exploration or when no neural activity
    if (!pImpl->availableActions.empty()) {
        size_t randomAction = std::uniform_int_distribution<size_t>(0, pImpl->availableActions.size() - 1)(pImpl->rng);
        return std::make_unique<Action>(pImpl->availableActions[randomAction]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set preference for a specific action based on experience
    auto it = std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action);
    if (it != pImpl->availableActions.end()) {
        size_t index = std::distance(pImpl->availableActions.begin(), it);
        pImpl->actionWeights[index] = std::clamp(preference, 0.0f, 2.0f);
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    // Clean up action history
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

// Initialize with brain reference for integration
void MotorSystem::initialize(class Brain* brain) {
    // Store brain reference for future integration
    // This would be used for more sophisticated action selection
    // based on cognitive goals and predictions
}

} // namespace nlm