#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    
    Impl() {
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
    // IMPROVED: Enhanced action selection based on neural activity patterns
    const auto& neurons = motorRegion.getAllNeurons();
    
    if (neurons.empty()) {
        NLM_LOG_WARNING("MotorSystem: No motor neurons available for action selection");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity levels in different neural populations
    std::map<ActionType, float> actionScores;
    
    // Analyze neural activity to determine action preferences
    float totalActivity = 0.0f;
    for (const auto* neuron : neurons) {
        const auto& state = neuron->getState();
        totalActivity += std::abs(state.membranePotential - state.restingPotential);
    }
    
    if (totalActivity < 0.1f) {
        // Low activity: default to wait or random exploration
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Determine action based on neural activity patterns
    // Group neurons by their resting potential to infer action type
    std::array<float, 9> actionPattern = {0.0f};  // For 9 action types
    
    for (const auto* neuron : neurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        // Assign activity to action categories based on neuron type and parameters
        // This is a simplified heuristic for action selection
        size_t actionIndex = 0;
        
        if (state.threshold > -45.0f) {
            // High threshold neurons might be for 'Look' or 'Wait'
            if (state.membranePotential > -30.0f) {
                actionIndex = 7;  // Look
            } else {
                actionIndex = 8;  // Wait
            }
        } else if (state.restingPotential < -65.0f) {
            // Low resting potential neurons might be for movement
            if (state.membranePotential > -40.0f) {
                actionIndex = 0;  // MoveForward
            } else if (state.membranePotential < -50.0f) {
                actionIndex = 1;  // MoveBackward
            } else {
                actionIndex = 2;  // MoveLeft
            }
        } else {
            // Medium resting potential neurons
            if (state.membranePotential > -35.0f) {
                actionIndex = 3;  // MoveRight
            } else if (state.membranePotential > -55.0f) {
                actionIndex = 4;  // TurnLeft
            } else {
                actionIndex = 5;  // TurnRight
            }
        }
        
        // Add activity to action score
        actionPattern[actionIndex] += activity;
    }
    
    // Convert neural patterns to action scores
    static const ActionType actionTypes[] = {
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
    
    // Normalize scores to probability distribution
    float maxScore = 0.0f;
    for (float score : actionPattern) {
        maxScore = std::max(maxScore, score);
    }
    
    if (maxScore < 0.1f) {
        // Low confidence: default to wait
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Convert to probabilities using softmax-like function
    std::vector<std::pair<float, ActionType>> actionProbabilities;
    for (size_t i = 0; i < actionPattern.size(); ++i) {
        float probability = std::exp(actionPattern[i]) / std::max(0.0001f, maxScore);
        actionProbabilities.emplace_back(probability, actionTypes[i]);
    }
    
    // Select action based on probability distribution
    float cumulativeProbability = 0.0f;
    float randValue = static_cast<float>(rand()) / RAND_MAX;
    
    for (const auto& pair : actionProbabilities) {
        cumulativeProbability += pair.first;
        if (randValue <= cumulativeProbability) {
            std::unique_ptr<Action> action = std::make_unique<Action>(pair.second);
            pImpl->actionHistory.push_back(action.get());
            return action;
        }
    }
    
    // Fallback: wait action
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
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
