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
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    if (!pImpl->availableActions.empty()) {
        return std::make_unique<Action>(pImpl->availableActions[0]);
    }
    return std::make_unique<Action>(ActionType::Wait);
}

// Enhanced action selection using neural activity and working memory
std::unique_ptr<Action> MotorSystem::selectActionAdvanced(const NeuralRegion& motorRegion, 
                                                        const NeuralWorkingMemory* workingMemory,
                                                        const Neuromodulator* dopamine,
                                                        const Brain* brain) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // Integrate multiple systems for intelligent action selection
    
    // Analyze neural activity in motor region
    float maxActivity = 0.0f;
    size_t maxActivityGroup = 0;
    std::vector<float> groupActivities;
    
    const auto& populations = motorRegion.getPopulations();
    for (size_t i = 0; i < populations.size(); ++i) {
        const auto& neurons = populations[i]->getNeurons();
        float activity = 0.0f;
        for (const Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            float activityValue = std::abs(n->getState().membranePotential - n->getState().restingPotential);
            activity += activityValue;
        }
        activity /= neurons.size();
        groupActivities.push_back(activity);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            maxActivityGroup = i;
        }
    }
    
    // Apply neuromodulation (dopamine) to select actions
    float dopamineLevel = dopamine ? dopamine->getLevel() : 0.0f;
    
    // Use working memory to influence action selection
    std::vector<float> memoryWeights;
    if (workingMemory) {
        memoryWeights = workingMemory->getActiveTraceWeights();
        // If working memory has strong traces, bias towards actions associated with them
        for (size_t i = 0; i < std::min(memoryWeights.size(), groupActivities.size()); ++i) {
            groupActivities[i] *= (1.0f + memoryWeights[i]);
        }
    }
    
    // Apply dopamine scaling (exploration vs exploitation)
    if (dopamineLevel > 0.3f) {
        // High dopamine encourages exploration - more random action selection
        float exploreChance = (dopamineLevel - 0.3f) / 0.7f * 0.5f;  // 0 to 50% chance
        float r = brain ? brain->getRandomGenerator()->uniformReal(0.0f, 1.0f) : 0.5f;
        if (r < exploreChance) {
            // Random action from available actions
            size_t randomIdx = 0;
            if (!pImpl->availableActions.empty()) {
                randomIdx = brain ? brain->getRandomGenerator()->uniformInt(0, static_cast<int>(pImpl->availableActions.size() - 1)) : 0;
                return std::make_unique<Action>(pImpl->availableActions[randomIdx]);
            }
        }
    }
    
    // Select best action based on activity and preferences
    float bestActivity = groupActivities[maxActivityGroup];
    ActionType bestAction;
    
    switch (maxActivityGroup) {
        case 0: bestAction = ActionType::MoveForward; break;
        case 1: bestAction = ActionType::MoveBackward; break;
        case 2: bestAction = ActionType::TurnLeft; break;
        case 3: bestAction = ActionType::TurnRight; break;
        case 4: bestAction = ActionType::Look; break;
        case 5: bestAction = ActionType::Interact; break;
        case 6: bestAction = ActionType::Wait; break;
        default: bestAction = ActionType::Wait; break;
    }
    
    // Record action for history
    pImpl->actionHistory.push_back(new Action(bestAction));
    
    return std::make_unique<Action>(bestAction);
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
