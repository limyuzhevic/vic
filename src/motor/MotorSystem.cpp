#include "MotorSystem.hpp"

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unordered_map<ActionType, float> actionPreferences;  // For exploration bias
    float preferenceDecayRate;
    
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
        preferenceDecayRate = 0.95f;
        
        // Initialize preferences
        for (auto action : availableActions) {
            actionPreferences[action] = 1.0f;
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

// Main action selection function
std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    if (!pImpl) {
        NLM_LOG_ERROR("MotorSystem::selectAction: pImpl is null");
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate scores for all available actions based on neural activity
    auto scores = calculateActionScores(motorRegion);
    
    if (scores.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Decay preferences for exploration
    decayPreferences();
    
    // Select best action based on scores
    ActionType selectedAction = selectBestAction(scores);
    
    // Create and store the action
    auto action = std::make_unique<Action>(selectedAction);
    pImpl->actionHistory.push_back(action.get());
    
    return action;
}

// Calculate action scores based on neural activity
std::vector<MotorSystem::ActionScore> MotorSystem::calculateActionScores(
    const NeuralRegion& motorRegion) const {
    
    if (!pImpl) {
        NLM_LOG_ERROR("MotorSystem::calculateActionScores: pImpl is null");
        return std::vector<MotorSystem::ActionScore>();
    }
    
    std::vector<ActionScore> scores;
    
    // Get all neurons in the motor region
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) {
        return scores;
    }
    
    // Get neural activity statistics
    auto stats = getNeuralActivityStats(motorRegion);
    
    // Group neurons by type for action mapping
    std::unordered_map<NeuronType, std::vector<const Neuron*>> neuronsByType;
    for (const auto* neuron : *neurons) {
        neuronsByType[neuron->getType()].push_back(neuron);
    }
    
    // Calculate scores for each available action
    for (const auto& action : pImpl->availableActions) {
        ActionScore score;
        score.type = action;
        score.preference = pImpl->actionPreferences[action];
        
        // Calculate action strength based on neural activity
        score.strength = calculateActionStrength(action, neurons);
        
        // Calculate confidence based on firing consistency
        score.confidence = calculateConfidence(neurons);
        
        // Calculate neural coherence
        score.coherence = calculateNeuralCoherence(neurons);
        
        // Combine factors to get final score (weighted combination)
        score.strength = score.strength * 0.4f;
        score.confidence = score.confidence * 0.3f;
        score.coherence = score.coherence * 0.2f;
        score.preference = score.preference * 0.1f;
        
        // Final combined score
        score.strength = score.strength + score.confidence + score.coherence + score.preference;
        
        scores.push_back(score);
    }
    
    return scores;
}

// Select best action from scored options
ActionType MotorSystem::selectBestAction(const std::vector<ActionScore>& scores) const {
    if (scores.empty()) {
        return ActionType::Wait;
    }
    
    // Find action with highest score
    float maxScore = -1.0f;
    ActionType bestAction = ActionType::Wait;
    
    for (const auto& score : scores) {
        if (score.strength > maxScore) {
            maxScore = score.strength;
            bestAction = score.type;
        }
    }
    
    return bestAction;
}

// Calculate neural coherence for a population of neurons
float MotorSystem::calculateNeuralCoherence(const std::vector<const Neuron*>& neurons) const {
    if (neurons.size() < 2) {
        return 1.0f;  // Full coherence for single neuron
    }
    
    // Calculate variance in firing rates
    float sumFiringRate = 0.0f;
    float sumSquaredFiringRate = 0.0f;
    
    for (const auto* neuron : neurons) {
        if (!neuron) {
            NLM_LOG_ERROR("MotorSystem::calculateNeuralCoherence: Found null neuron");
            continue;
        }
        float firingRate = neuron->getState().firingRate;
        sumFiringRate += firingRate;
        sumSquaredFiringRate += firingRate * firingRate;
    }
    
    if (neurons.empty()) {
        return 1.0f;  // Full coherence for empty list
    }
    
    float meanFiringRate = sumFiringRate / neurons.size();
    float variance = (sumSquaredFiringRate / neurons.size()) - (meanFiringRate * meanFiringRate);
    
    // Coherence is inverse of normalized variance (0 = no coherence, 1 = perfect coherence)
    float normalizedVariance = std::min(1.0f, variance * 10.0f);  // Scale variance
    return 1.0f - normalizedVariance;
}

// Calculate action strength based on neural firing patterns
float MotorSystem::calculateActionStrength(ActionType action, 
    const std::vector<const Neuron*>& neurons) const {
    
    float totalStrength = 0.0f;
    float count = 0.0f;
    
    // Action-specific neural activation patterns
    for (const auto* neuron : neurons) {
        if (!neuron) {
            NLM_LOG_ERROR("MotorSystem::calculateActionStrength: Found null neuron");
            continue;
        }
        NeuronType neuronType = neuron->getType();
        const NeuronState& state = neuron->getState();
        
        // Weight based on neuron type and firing state
        float weight = 0.0f;
        
        if (neuronType == NeuronType::Motor) {
            weight = 1.0f;
        } else if (neuronType == NeuronType::Internal) {
            weight = 0.5f;
        }
        
        if (state.firingState == FiringState::Active || 
            state.firingState == FiringState::Refractory) {
            weight *= state.firingRate;
        }
        
        totalStrength += weight;
        count++;
    }
    
    float averageStrength = count > 0 ? totalStrength / count : 0.0f;
    
    // Normalize strength to [0, 1] range
    return std::min(1.0f, averageStrength * 2.0f);
}

// Calculate confidence based on firing consistency and population size
float MotorSystem::calculateConfidence(const std::vector<const Neuron*>& neurons) const {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    // Calculate coefficient of variation of firing rates
    float sumFiringRate = 0.0f;
    float sumSquaredFiringRate = 0.0f;
    
    for (const auto* neuron : neurons) {
        if (!neuron) {
            NLM_LOG_ERROR("MotorSystem::calculateConfidence: Found null neuron");
            continue;
        }
        float firingRate = neuron->getState().firingRate;
        sumFiringRate += firingRate;
        sumSquaredFiringRate += firingRate * firingRate;
    }
    
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float meanFiringRate = sumFiringRate / neurons.size();
    float variance = (sumSquaredFiringRate / neurons.size()) - (meanFiringRate * meanFiringRate);
    
    // Coefficient of variation
    float cv = meanFiringRate > 0.0f ? std::sqrt(variance) / meanFiringRate : 0.0f;
    
    // Lower CV = higher confidence (consistent firing)
    float confidence = std::max(0.0f, 1.0f - cv);
    
    // Boost confidence with larger population
    confidence = confidence * std::min(1.0f, static_cast<float>(neurons.size()) / 10.0f);
    
    return confidence;
}

// Decay old action preferences over time
void MotorSystem::decayPreferences(float decayRate) {
    if (!pImpl) {
        NLM_LOG_ERROR("MotorSystem::decayPreferences: pImpl is null");
        return;
    }
    
    for (auto& pair : pImpl->actionPreferences) {
        pair.second *= decayRate;
    }
}

// Get neural activity summary statistics
MotorSystem::NeuralActivityStats MotorSystem::getNeuralActivityStats(
    const NeuralRegion& motorRegion) const {
    
    NeuralActivityStats stats;
    auto neurons = motorRegion.getAllNeurons();
    
    if (neurons.empty()) {
        return stats;
    }
    
    float sumFiringRate = 0.0f;
    float sumSquaredFiringRate = 0.0f;
    size_t activeCount = 0;
    
    for (const auto* neuron : neurons) {
        if (!neuron) {
            NLM_LOG_ERROR("MotorSystem::getNeuralActivityStats: Found null neuron");
            continue;
        }
        float firingRate = neuron->getState().firingRate;
        sumFiringRate += firingRate;
        sumSquaredFiringRate += firingRate * firingRate;
        
        if (neuron->getState().firingState != FiringState::Resting) {
            activeCount++;
        }
    }
    
    stats.meanFiringRate = sumFiringRate / neurons.size();
    stats.firingVariance = (sumSquaredFiringRate / neurons.size()) - 
                          (stats.meanFiringRate * stats.meanFiringRate);
    stats.activeFraction = static_cast<float>(activeCount) / neurons.size();
    stats.totalActiveNeurons = static_cast<float>(activeCount);
    stats.totalNeurons = static_cast<float>(neurons.size());
    
    // Clamp variance
    stats.firingVariance = std::max(0.0f, stats.firingVariance);
    
    return stats;
}

// Reset internal state
void MotorSystem::reset() {
    // Clear action history
    clearHistory();
    
    // Reset preferences to baseline
    if (!pImpl) {
        NLM_LOG_ERROR("MotorSystem::reset: pImpl is null");
        return;
    }
    
    for (auto& pair : pImpl->actionPreferences) {
        pair.second = 1.0f;
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    if (pImpl->actionPreferences.find(action) != pImpl->actionPreferences.end()) {
        pImpl->actionPreferences[action] = preference;
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    if (!pImpl) {
        NLM_LOG_ERROR("MotorSystem::clearHistory: pImpl is null");
        return;
    }
    
    for (auto* action : pImpl->actionHistory) {
        delete action;
    }
    pImpl->actionHistory.clear();
}

} // namespace nlm
