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
    // PHASE 2: Implement neural action selection based on motor region activity
    std::vector<float> actionPreferences(pImpl->availableActions.size(), 0.0f);
    
    // Sample neural activity from motor region
    size_t actionCount = motorRegion.getPopulationCount() * 0.75; // Use 75% of populations
    size_t populationIndex = 0;
    for (size_t i = 0; i < actionCount && populationIndex < pImpl->availableActions.size(); ++i) {
        const NeuralPopulation* population = motorRegion.getPopulation(populationIndex);
        if (population) {
            // Get average firing rate of this population
            float avgFiringRate = population->getAverageFiringRate();
            actionPreferences[populationIndex] = avgFiringRate;
        }
        ++populationIndex;
    }
    
    // Normalize preferences to sum to 1
    float totalPref = 0.0f;
    for (float pref : actionPreferences) {
        totalPref += pref;
    }
    
    if (totalPref > 0.0f) {
        for (float& pref : actionPreferences) {
            pref /= totalPref;
        }
    } else {
        // Uniform distribution if no activity
        float uniform = 1.0f / actionPreferences.size();
        for (float& pref : actionPreferences) {
            pref = uniform;
        }
    }
    
    // Select action based on preferences (roulette wheel selection)
    float selectionValue = 0.0f;
    float randomValue = static_cast<float>(rand()) / RAND_MAX;
    
    for (size_t i = 0; i < actionPreferences.size(); ++i) {
        selectionValue += actionPreferences[i];
        if (randomValue <= selectionValue) {
            // Found action to execute
            pImpl->actionHistory.push_back(new Action(pImpl->availableActions[i]));
            return std::make_unique<Action>(pImpl->availableActions[i]);
        }
    }
    
    // Fallback to last action if selection fails
    ActionType selectedAction = pImpl->availableActions.empty() ? ActionType::Wait : pImpl->availableActions.back();
    pImpl->actionHistory.push_back(new Action(selectedAction));
    return std::make_unique<Action>(selectedAction);
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
