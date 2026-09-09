#include "MotorSystem.hpp"
#include "../../brain/NeuralRegion.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>

namespace nlm {

struct MotorSystem::Impl {
    // Action selection parameters
    float explorationRate;              // ε for exploration vs exploitation
    float temperature;                   // Temperature parameter for softmax
    float actionSelectionBias;           // Bias toward preferred actions
    float timeConstant;                   // For temporal integration
    
    // Action preference system
    std::vector<float> actionPreferences; // Preference values for each action type
    std::vector<float> actionHistory;     // Recent action outcomes
    
    // Neuromodulation integration
    float motivationLevel;               // From dopamine system
    float explorationLevel;              // From novelty/curiosity
    float arousalLevel;                  // From norepinephrine
    
    // Decision-making parameters
    float decisionNoise;                 // Random component of selection
    float selectionUrgency;              // Pressure to act quickly
    float rewardPrediction;              // Expected reward from each action
    
    // State tracking
    std::vector<float> actionValues;      // Estimated value for each action
    std::vector<size_t> actionCounts;     // Frequency of action selection
    
    // Implementation details
    std::mt19937 rng;                     // Random number generator
    uint64_t seed;                        // For reproducibility
    
    Impl() : explorationRate(0.1f), temperature(1.0f), actionSelectionBias(0.0f),
             timeConstant(0.5f), motivationLevel(1.0f), explorationLevel(0.5f),
             arousalLevel(0.5f), decisionNoise(0.1f), selectionUrgency(1.0f),
             rewardPrediction(0.0f), seed(42) {
        
        // Initialize random generator
        rng.seed(seed);
        
        // Initialize for standard action types
        const size_t numActions = static_cast<size_t>(ActionType::Count);
        actionPreferences.resize(numActions, 1.0f);
        actionValues.resize(numActions, 0.0f);
        actionCounts.resize(numActions, 0);
        
        // Set up exploration distribution
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        for (float& pref : actionPreferences) {
            pref = dist(rng);
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Real action selection based on neural activity
    // Implements multiple decision-making mechanisms
    
    if (pImpl->actionPreferences.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Get neural activity from motor region
    std::vector<float> neuralActivity;
    const auto& neurons = motorRegion.getAllNeurons();
    neuralActivity.reserve(neurons.size());
    
    for (const auto& neuron : neurons) {
        neuralActivity.push_back(neuron->getFiringRate());
    }
    
    if (neuralActivity.empty()) {
        // Fallback to default action if no neural activity
        return selectDefaultAction();
    }
    
    // Compute action values from neural activity
    computeActionValues(neuralActivity);
    
    // Apply exploration-exploitation tradeoff
    std::vector<float> selectionProbabilities;
    computeSelectionProbabilities(selectionProbabilities);
    
    // Sample action based on probabilities
    size_t selectedIndex = sampleAction(selectionProbabilities);
    
    // Convert to ActionType (simplified mapping)
    ActionType selectedAction = mapToActionType(selectedIndex);
    
    // Record selection
    pImpl->actionCounts[selectedIndex]++;
    pImpl->actionHistory.push_back(selectedAction);
    if (pImpl->actionHistory.size() > 1000) {
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
    }
    
    // Create and return action
    auto action = std::make_unique<Action>(selectedAction);
    
    // Apply neural modulation to action parameters
    modulateActionParameters(*action, neuralActivity);
    
    return action;
}

void MotorSystem::computeActionValues(const std::vector<float>& neuralActivity) {
    // Compute action values from neural activity
    size_t numActions = pImpl->actionPreferences.size();
    
    // Reset action values
    std::fill(pImpl->actionValues.begin(), pImpl->actionValues.end(), 0.0f);
    
    // Calculate average neural activity
    float avgActivity = 0.0f;
    for (float activity : neuralActivity) {
        avgActivity += activity;
    }
    avgActivity /= neuralActivity.size();
    
    // Compute value for each action based on neural patterns
    for (size_t i = 0; i < numActions; ++i) {
        // Base value from preference
        float value = pImpl->actionPreferences[i];
        
        // Neural modulation component
        float neuralMod = computeNeuralModulation(i, neuralActivity);
        value += neuralMod * 0.5f;
        
        // Neuromodulation influence
        value *= getNeuromodulationInfluence(i);
        
        // Add exploration bonus based on curiosity
        float explorationBonus = pImpl->explorationLevel * avgActivity * 0.1f;
        value += explorationBonus;
        
        // Apply reward prediction
        value += pImpl->rewardPrediction * 0.3f;
        
        // Clamp to reasonable bounds
        pImpl->actionValues[i] = std::clamp(value, -1.0f, 2.0f);
    }
}

float MotorSystem::computeNeuralModulation(size_t actionIndex, 
                                          const std::vector<float>& neuralActivity) const {
    // Compute neural modulation for specific action
    float modulation = 0.0f;
    
    if (neuralActivity.empty()) return modulation;
    
    // Get neuron indices corresponding to this action type
    std::vector<size_t> relevantNeurons = getRelevantNeurons(actionIndex);
    
    if (relevantNeurons.empty()) return modulation;
    
    // Compute average activity of relevant neurons
    float relevantActivity = 0.0f;
    for (size_t neuronIdx : relevantNeurons) {
        if (neuronIdx < neuralActivity.size()) {
            relevantActivity += neuralActivity[neuronIdx];
        }
    }
    relevantActivity /= relevantNeurons.size();
    
    // Apply nonlinear transformation
    modulation = 1.0f / (1.0f + std::exp(-2.0f * (relevantActivity - 0.5f)));
    
    // Scale by action preference
    modulation *= pImpl->actionPreferences[actionIndex];
    
    return modulation;
}

std::vector<size_t> MotorSystem::getRelevantNeurons(size_t actionIndex) const {
    // Get neuron indices that are relevant for a specific action
    // This is a simplified mapping - real systems would use learned associations
    
    std::vector<size_t> relevantNeurons;
    
    // Create deterministic mapping based on action index
    // In real implementation, this would come from learned weights
    size_t patternBase = (actionIndex * 7) % 100;  // Pseudo-random pattern
    
    for (size_t i = 0; i < 20; ++i) {  // Check 20 neurons per action
        size_t neuronIdx = (patternBase + i * 13) % 500;  // Spread across 500 neurons
        relevantNeurons.push_back(neuronIdx);
    }
    
    return relevantNeurons;
}

float MotorSystem::getNeuromodulationInfluence(size_t actionIndex) const {
    // Get influence of neuromodulators on action selection
    float totalInfluence = 1.0f;
    
    // Dopamine influence (reward-based learning)
    float dopamineInfluence = 0.5f + 0.5f * pImpl->motivationLevel;
    totalInfluence *= dopamineInfluence;
    
    // Novelty/curiosity influence (exploration)
    float noveltyInfluence = 1.0f + pImpl->explorationLevel * 0.5f;
    totalInfluence *= noveltyInfluence;
    
    // Arousal influence (vigilance)
    float arousalInfluence = 0.8f + 0.4f * pImpl->arousalLevel;
    totalInfluence *= arousalInfluence;
    
    // Action-specific modulation
    if (actionIndex == static_cast<size_t>(ActionType::MoveForward)) {
        // Forward movement benefits from motivation
        totalInfluence *= 1.2f;
    } else if (actionIndex == static_cast<size_t>(ActionType::Look)) {
        // Looking benefits from arousal/vigilance
        totalInfluence *= 1.1f;
    } else if (actionIndex == static_cast<size_t>(ActionType::Interact)) {
        // Interaction benefits from reward prediction
        totalInfluence *= (1.0f + pImpl->rewardPrediction * 0.5f);
    }
    
    return totalInfluence;
}

void MotorSystem::computeSelectionProbabilities(std::vector<float>& probabilities) {
    // Compute action selection probabilities using softmax with temperature
    size_t numActions = pImpl->actionValues.size();
    probabilities.clear();
    probabilities.resize(numActions, 0.0f);
    
    if (numActions == 0) return;
    
    // Find maximum action value for numerical stability
    float maxValue = pImpl->actionValues[0];
    for (float value : pImpl->actionValues) {
        if (value > maxValue) maxValue = value;
    }
    
    // Compute softmax probabilities
    float sumExp = 0.0f;
    for (float value : pImpl->actionValues) {
        // Apply temperature and bias
        float adjustedValue = (value - maxValue) / pImpl->temperature + 
                             pImpl->actionSelectionBias;
        float expValue = std::exp(adjustedValue);
        probabilities.push_back(expValue);
        sumExp += expValue;
    }
    
    // Normalize
    if (sumExp > 0.0f) {
        for (float& prob : probabilities) {
            prob /= sumExp;
        }
    } else {
        // Uniform distribution if all values are equal
        float uniformProb = 1.0f / numActions;
        for (float& prob : probabilities) {
            prob = uniformProb;
        }
    }
    
    // Apply exploration rate
    float exploreProb = pImpl->explorationRate;
    for (float& prob : probabilities) {
        prob = prob * (1.0f - exploreProb) + exploreProb / numActions;
    }
    
    // Apply urgency bias
    if (pImpl->selectionUrgency > 1.0f) {
        // Bias toward higher probability actions
        float totalProb = 0.0f;
        for (float prob : probabilities) {
            totalProb += prob;
        }
        for (float& prob : probabilities) {
            prob = prob * std::pow(prob / totalProb, pImpl->selectionUrgency);
        }
        // Renormalize
        totalProb = 0.0f;
        for (float prob : probabilities) {
            totalProb += prob;
        }
        for (float& prob : probabilities) {
            prob /= totalProb;
        }
    }
}

size_t MotorSystem::sampleAction(const std::vector<float>& probabilities) {
    // Sample action index based on probabilities using roulette wheel selection
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    float randomValue = dist(pImpl->rng);
    float cumulativeProb = 0.0f;
    
    for (size_t i = 0; i < probabilities.size(); ++i) {
        cumulativeProb += probabilities[i];n            if (randomValue <= cumulativeProb) {
                return i;
            }
        }
    }
    
    // Fallback to first action
    return 0;
}

ActionType MotorSystem::mapToActionType(size_t index) {
    // Map action index to ActionType (simplified implementation)
    const size_t numActions = static_cast<size_t>(ActionType::Count);
    if (index >= numActions) index = index % numActions;
    
    return static_cast<ActionType>(index);
}

void MotorSystem::modulateActionParameters(Action& action, 
                                          const std::vector<float>& neuralActivity) {
    // Modulate action parameters based on neural activity
    float avgActivity = 0.0f;
    for (float activity : neuralActivity) {
        avgActivity += activity;
    }
    if (!neuralActivity.empty()) {
        avgActivity /= neuralActivity.size();
    }
    
    // Modify action strength/intensity based on neural activity
    float intensity = std::min(1.0f, avgActivity * 2.0f);
    action.setIntensity(intensity);
    
    // Add noise for biological realism
    std::uniform_real_distribution<float> noiseDist(-0.1f, 0.1f);
    float noise = noiseDist(pImpl->rng);
    action.setIntensity(action.getIntensity() + noise);
    
    // Clamp to valid range
    action.setIntensity(std::clamp(action.getIntensity(), 0.1f, 1.0f));
}

std::unique_ptr<Action> MotorSystem::selectDefaultAction() {
    // Select a default action based on current preferences
    if (pImpl->actionPreferences.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find action with highest preference
    size_t bestAction = 0;
    float bestPreference = pImpl->actionPreferences[0];
    
    for (size_t i = 1; i < pImpl->actionPreferences.size(); ++i) {
        if (pImpl->actionPreferences[i] > bestPreference) {
            bestPreference = pImpl->actionPreferences[i];
            bestAction = i;
        }
    }
    
    return std::make_unique<Action>(mapToActionType(bestAction));
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    // Set preference for a specific action type
    size_t index = static_cast<size_t>(action);
    if (index < pImpl->actionPreferences.size()) {
        pImpl->actionPreferences[index] = std::clamp(preference, 0.0f, 10.0f);
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
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

void MotorSystem::setExplorationRate(float rate) {
    pImpl->explorationRate = std::clamp(rate, 0.0f, 1.0f);
}

void MotorSystem::setTemperature(float temp) {
    pImpl->temperature = std::max(0.1f, temp);
}

void MotorSystem::setSelectionBias(float bias) {
    pImpl->actionSelectionBias = bias;
}

void MotorSystem::setMotivationLevel(float level) {
    pImpl->motivationLevel = std::clamp(level, 0.0f, 2.0f);
}

void MotorSystem::setExplorationLevel(float level) {
    pImpl->explorationLevel = std::clamp(level, 0.0f, 1.0f);
}

void MotorSystem::setArousalLevel(float level) {
    pImpl->arousalLevel = std::clamp(level, 0.0f, 1.0f);
}

void MotorSystem::setSelectionUrgency(float urgency) {
    pImpl->selectionUrgency = std::max(1.0f, urgency);
}

void MotorSystem::setRewardPrediction(float prediction) {
    pImpl->rewardPrediction = prediction;
}

const std::vector<float>& MotorSystem::getActionPreferences() const {
    return pImpl->actionPreferences;
}

const std::vector<float>& MotorSystem::getActionValues() const {
    return pImpl->actionValues;
}

const std::vector<size_t>& MotorSystem::getActionCounts() const {
    return pImpl->actionCounts;
}

} // namespace nlm
