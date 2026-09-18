#include "MotorSystem.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;  // ActionType -> preference score
    
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

MotorSystem::~MotorSystem() {
    clearHistory();
}

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // TODO PHASE 2: Implement real action selection based on neural activity
    // PLACEHOLDER: Returns random action
    
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_WARNING("No neurons found in motor region for action selection");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate neural activity for each neuron
    float totalActivity = 0.0f;
    std::vector<float> neuronActivities;
    neuronActivities.reserve(neurons.size());
    
    for (Neuron* neuron : neurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        neuronActivities.push_back(activity);
        totalActivity += activity;
    }
    
    if (totalActivity < 0.001f) {
        // No significant neural activity - random selection based on preferences
        return selectRandomAction();
    }
    
    // Find the neuron with maximum activity
    float maxActivity = 0.0f;
    Neuron* mostActiveNeuron = nullptr;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (neuronActivities[i] > maxActivity) {
            maxActivity = neuronActivities[i];
            mostActiveNeuron = neurons[i];
        }
    }
    
    // Map neuron position/population to action type
    // This is a simplified mapping - could be more sophisticated
    ActionType selectedAction = mapNeuronToAction(mostActiveNeuron, neuronActivities);
    
    NLM_LOG_INFO("Motor selection: action " + std::to_string(static_cast<int>(selectedAction)) + 
                 " based on neural activity " + std::to_string(maxActivity));
    
    return std::make_unique<Action>(selectedAction);
}

ActionType MotorSystem::mapNeuronToAction(Neuron* neuron, const std::vector<float>& activities) {
    // Simple mapping from motor neuron to action type
    // In a real implementation, this would be based on:
    // - Which motor neuron group is most active (forward, backward, turn, etc.)
    // - Consideration of action preferences
    // - Top-down modulation (goals, intentions)
    
    // For now, use random selection weighted by preferences
    return selectRandomAction();
}

std::unique_ptr<Action> MotorSystem::selectRandomAction() {
    if (pImpl->availableActions.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate weighted selection based on preferences
    std::vector<std::pair<ActionType, float>> actionWeights;
    float totalWeight = 0.0f;
    
    for (ActionType action : pImpl->availableActions) {
        float weight = 1.0f; // Default weight
        
        // Apply preference if defined
        auto prefIt = pImpl->actionPreferences.find(action);
        if (prefIt != pImpl->actionPreferences.end()) {
            weight += prefIt->second;
        }
        
        // Bias away from wait action
        if (action == ActionType::Wait) {
            weight *= 0.1f;
        }
        
        actionWeights.emplace_back(action, weight);
        totalWeight += weight;
    }
    
    if (totalWeight < 0.001f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Select random action weighted by preferences
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, totalWeight);
    
    float selectedWeight = dist(gen);
    float cumulativeWeight = 0.0f;
    
    for (const auto& pair : actionWeights) {
        cumulativeWeight += pair.second;
        if (selectedWeight <= cumulativeWeight) {
            return std::make_unique<Action>(pair.first);
        }
    }
    
    // Fallback
    return std::make_unique<Action>(ActionType::Wait);
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // TODO PHASE 2: Implement action preferences
    // PLACEHOLDER: Just store the preference
    
    if (std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action) != pImpl->availableActions.end()) {
        pImpl->actionPreferences[action] = preference;
        NLM_LOG_INFO("Set preference for action " + std::to_string(static_cast<int>(action)) + 
                     " to " + std::to_string(preference));
    }
}

void MotorSystem::setActionActivations(const std::vector<std::pair<ActionType, float>>& activations) {
    // TODO PHASE 2: Implement action preference learning
    // PLACEHOLDER: Update preferences based on activation
    
    for (const auto& activation : activations) {
        ActionType action = activation.first;
        float strength = activation.second;
        
        // Learn from action execution
        auto prefIt = pImpl->actionPreferences.find(action);
        if (prefIt != pImpl->actionPreferences.end()) {
            // Increase preference for successful actions
            prefIt->second += strength * 0.1f;
            
            // Decay preferences over time
            prefIt->second *= 0.95f;
        }
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::recordAction(const Action* action) {
    if (action && std::find(pImpl->availableActions.begin(), pImpl->availableActions.end(), action->getType()) != pImpl->availableActions.end()) {
        // Store history
        pImpl->actionHistory.push_back(action);
        
        // Keep history limited
        if (pImpl->actionHistory.size() > 100) {
            delete pImpl->actionHistory.front();
            pImpl->actionHistory.erase(pImpl->actionHistory.begin());
        }
        
        NLM_LOG_INFO("Recorded action " + std::to_string(static_cast<int>(action->getType())));
    }
}

void MotorSystem::clearHistory() {
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
    pImpl->actionPreferences.clear();
}

} // namespace nlm

