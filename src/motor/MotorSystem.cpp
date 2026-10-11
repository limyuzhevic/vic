// Include RandomGenerator header
#include "../core/Random/RandomGenerator.hpp"

// Implementation file for includes
// Rest of file remains the same

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;
    RandomGenerator rng;
    
    Impl() : rng(42) {  // Fixed seed for reproducibility
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
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Implement real action selection based on neural activity
    // Uses softmax policy gradient, reinforcement learning, or biologically plausible mechanisms
    
    // Get motor neuron activity from neural region
    auto allNeurons = motorRegion.getAllNeurons();
    std::vector<Neuron*> motorNeurons;
    
    for (Neuron* neuron : allNeurons) {
        if (neuron->getType() == NeuronType::Motor) {
            motorNeurons.push_back(neuron);
        }
    }
    
    if (motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Compute action selection probabilities using softmax over neural activity
    std::vector<float> actionProbabilities;
    float totalActivity = 0.0f;
    
    for (Neuron* neuron : motorNeurons) {
        float activity = neuron->getFiringRate();
        // Boost activity that has been reinforced recently
        activity *= (1.0f + neuron->getLastSpikeTime() > 0 ? 
                     std::pow(1.1f, neuron->getLastSpikeTime()) : 1.0f);
        totalActivity += activity;
        actionProbabilities.push_back(activity);
    }
    
    // Apply softmax to convert activities to probabilities
    std::vector<float> probabilities;
    float sumExp = 0.0f;
    
    for (float activity : actionProbabilities) {
        if (totalActivity > 0.0f) {
            float prob = std::exp(activity / totalActivity); // Temperature parameter
            probabilities.push_back(prob);
            sumExp += prob;
        } else {
            probabilities.push_back(1.0f); // Equal probability if no activity
            sumExp += 1.0f;
        }
    }
    
    // Normalize probabilities
    for (float& prob : probabilities) {
        prob /= sumExp;
    }
    
    // Apply action preferences (for exploration vs. exploitation)
    std::unordered_map<ActionType, float> preferenceMap;
    for (size_t i = 0; i < motorNeurons.size(); ++i) {
        // Map neuron to action type (simplified)
        ActionType actionType = static_cast<ActionType>(i % pImpl->availableActions.size());
        preferenceMap[actionType] += probabilities[i] * (1.0f + pImpl->actionPreferences[actionType]);
    }
    
    // Select action based on weighted preferences
    float selectionThreshold = pImpl->rng.uniformReal(0.0f, 1.0f);
    float cumulativeProb = 0.0f;
    
    for (ActionType action : pImpl->availableActions) {
        float prob = preferenceMap.count(action) ? preferenceMap[action] : 0.0f;
        if (prob > 0.0f) {
            cumulativeProb += prob;
            if (selectionThreshold <= cumulativeProb) {
                // Create action with parameters based on neural activity
                std::vector<float> params;
                for (size_t i = 0; i < motorNeurons.size(); ++i) {
                    if (pImpl->availableActions[i] == action) {
                        // Use motor neuron properties as action parameters
                        Neuron* neuron = motorNeurons[i];
                        params.push_back(neuron->getFiringRate() * 10.0f); // Scale firing rate
                        params.push_back(neuron->getMembranePotential() * 0.01f); // Scale membrane potential
                        break;
                    }
                }
                
                // Record action in history
                auto* actionPtr = new Action(action, params);
                pImpl->actionHistory.push_back(actionPtr);
                
                return std::unique_ptr<Action>(actionPtr);
            }
        }
    }
    
    // Fallback: select highest preference action
    ActionType bestAction = pImpl->availableActions[0];
    float bestPreference = getActionPreference(bestAction);
    
    for (ActionType action : pImpl->availableActions) {
        float pref = getActionPreference(action);
        if (pref > bestPreference) {
            bestAction = action;
            bestPreference = pref;
        }
    }
    
    return std::make_unique<Action>(bestAction);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Implement action preferences: modifies action selection probability
    // Positive preference increases chance of action selection (exploitation)
    // Negative preference decreases chance (exploration or avoidance)
    
    pImpl->actionPreferences[action] = preference;
    
    // Log preference change (in real system)
    // Logger::getInstance().logDebug("Action preference set for " + std::to_string(static_cast<int>(action)) +
    //     " to " + std::to_string(preference));
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
