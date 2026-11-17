#include "ActionConsequencePredictor.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {

struct ActionConsequencePredictor::Impl {
    float learningRate;           // How quickly associations are learned
    float decayRate;              // How quickly associations weaken
    float confidenceThreshold;    // Minimum confidence for action prediction
    float maxActionQuality;       // Maximum action quality value
    
    // Statistics
    size_t totalExperiencesRecorded;
    size_t totalPredictionsMade;
    size_t totalUpdatesApplied;
    
    Impl() : learningRate(0.01f), decayRate(0.05f), confidenceThreshold(0.7f),
             maxActionQuality(1.0f),
             totalExperiencesRecorded(0), totalPredictionsMade(0), totalUpdatesApplied(0) {}
};

ActionConsequencePredictor::ActionConsequencePredictor() : pImpl(new Impl), brain_(nullptr) {}

ActionConsequencePredictor::~ActionConsequencePredictor() = default;

void ActionConsequencePredictor::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("ActionConsequencePredictor initialized with brain reference");
}

void ActionConsequencePredictor::recordExperience(ActionType action,
                                                  const std::vector<float>& beforeState,
                                                  const std::vector<float>& afterState,
                                                  float reward) {
    if (!brain_ || beforeState.empty() || afterState.empty()) {
        return;
    }
    
    // Find or create neurons for patterns
    NeuronId beforeNeuron = findOrCreatePatternNeuron(beforeState);
    NeuronId afterNeuron = findOrCreatePatternNeuron(afterState);
    
    if (beforeNeuron == INVALID_NEURON_ID || afterNeuron == INVALID_NEURON_ID) {
        return;
    }
    
    // Create or update action-consequence association
    for (auto& assoc : actionConsequences_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float& strength = std::get<2>(assoc);
        
        if ((a == beforeNeuron && b == afterNeuron) || (a == afterNeuron && b == beforeNeuron)) {
            // Strengthen association based on reward
            strength += pImpl->learningRate * reward;
            strength = std::clamp(strength, �חٍ::clamp(0.0f, pImpl->maxActionQuality);
            return;
        }
    }
    
    // Create new association
    float strength = pImpl->learningRate * reward;
    actionConsequences_.emplace_back(beforeNeuron, afterNeuron, strength);
    ++pImpl->totalExperiencesRecorded;
    
    NLM_LOG_INFO("Recorded action-consequence: action=" + std::to_string(static_cast<int>(action)) +
                 " reward=" + std::to_string(reward) +
                 " association_strength=" + std::to_string(strength));
}

std::vector<float> ActionConsequencePredictor::predictConsequence(
    ActionType action,
    const std::vector<float>& currentState) {
    std::vector<float> predictedState;
    
    if (!brain_ || currentState.empty() || action == ActionType::Wait) {
        return predictedState;
    }
    
    // Find neuron for current state
    NeuronId currentNeuron = INVALID_NEURON_ID;
    for (const auto& patternNeuron : patternNeurons_) {
        if (computePatternSimilarity(currentState, patternNeuron.second) >= 0.8f) {
            currentNeuron = patternNeuron.first;
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return predictedState;
    }
    
    // Find associated patterns
    std::vector<NeuronId> associatedNeurons;
    for (const auto& assoc : actionConsequences_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if (a == currentNeuron) {
            associatedNeurons.push_back(b);
        } else if (b == currentNeuron) {
            associatedNeurons.push_back(a);
        }
    }
    
    // Get the most strongly associated pattern
    float maxStrength = 0.0f;
    NeuronId targetNeuron = INVALID_NEURON_ID;
    
    for (const auto& assoc : actionConsequences_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        if ((a == currentNeuron && b != INVALID_NEURON_ID && strength > maxStrength) ||
            (b == currentNeuron && a != INVALID_NEURON_ID && strength > maxStrength)) {
            maxStrength = strength;
            targetNeuron = (a == currentNeuron) ? b : a;
        }
    }
    
    if (targetNeuron != INVALID_NEURON_ID) {
        // Find pattern for target neuron
        for (const auto& patternNeuron : patternNeurons_) {
            if (patternNeuron.first == targetNeuron) {
                predictedState = patternNeuron.second;
                break;
            }
        }
    }
    
    ++pImpl->totalPredictionsMade;
    
    NLM_LOG_TRACE("Predicted consequence for action " + std::to_string(static_cast<int>(action)) +
                  " strength=" + std::to_string(maxStrength));
    
    return predictedState;
}

float ActionConsequencePredictor::getConsequenceConfidence(
    ActionType action,
    const std::vector<float>& currentState) const {
    if (!brain_ || currentState.empty() || action == ActionType::Wait) {
        return 0.0f;
    }
    
    // Find neuron for current state
    NeuronId currentNeuron = INVALID_NEURON_ID;
    for (const auto& patternNeuron : patternNeurons_) {
        if (computePatternSimilarity(currentState, patternNeuron.second) >= 0.8f) {
            currentNeuron = patternNeuron.first;
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    // Calculate average confidence based on associated action qualities
    float totalConfidence = 0.0f;
    size_t count = 0;
    
    for (const auto& assoc : actionConsequences_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        if ((a == currentNeuron || b == currentNeuron) && strength > 0.1f) {
            totalConfidence += strength / pImpl->maxActionQuality;
            ++count;
        }
    }
    
    float confidence = (count > 0) ? (totalConfidence / static_cast<float>(count)) : 0.0f;
    
    NLM_LOG_TRACE("Action consequence confidence for action " + std::to_string(static_cast<int>(action)) +
                  " = " + std::to_string(confidence));
    
    return confidence;
}

void ActionConsequencePredictor::updatePrediction(ActionType action,
                                                  const std::vector<float>& predicted,
                                                  const std::vector<float>& actual,
                                                  float reward) {
    if (!brain_ || predicted.empty() || actual.empty()) {
        return;
    }
    
    // Find or create neurons for predicted and actual states
    NeuronId predictedNeuron = findOrCreatePatternNeuron(predicted);
    NeuronId actualNeuron = findOrCreatePatternNeuron(actual);
    
    if (predictedNeuron == INVALID_NEURON_ID || actualNeuron == INVALID_NEURON_ID) {
        return;
    }
    
    // Update action quality based on prediction error
    bool foundAction = false;
    for (auto& actionQual : actionQuality_) {
        ActionType a = std::get<0>(actionQual);
        if (a == action) {
            float& quality = std::get<2>(actionQual);
            float predictionError = 1.0f - computePatternSimilarity(predicted, actual);
            quality += pImpl->learningRate * (reward - predictionError);
            quality = std::clamp(quality, 0.0f, pImpl->maxActionQuality);
            foundAction = true;
            break;
        }
    }
    
    if (!foundAction) {
        actionQuality_.emplace_back(action, predicted, pImpl->learningRate * reward);
    }
    
    ++pImpl->totalUpdatesApplied;
    
    NLM_LOG_INFO("Updated action prediction: action=" + std::to_string(static_cast<int>(action)) +
                 " error=" + std::to_string(1.0f - computePatternSimilarity(predicted, actual)));
}

float ActionConsequencePredictor::getActionQuality(ActionType action) const {
    for (const auto& actionQual : actionQuality_) {
        ActionType a = std::get<0>(actionQual);
        if (a == action) {
            return std::get<2>(actionQual);
        }
    }
    
    return 0.0f;
}

void ActionConsequencePredictor::clear() {
    patternNeurons_.clear();
    actionConsequences_.clear();
    actionQuality_.clear();
    NLM_LOG_INFO("ActionConsequencePredictor cleared");
}

NeuronId ActionConsequencePredictor::findOrCreatePatternNeuron(const std::vector<float>& pattern) {
    // Find existing neuron for this pattern
    for (const auto& patternNeuron : patternNeurons_) {
        if (computePatternSimilarity(pattern, patternNeuron.second) >= 0.8f) {
            return patternNeuron.first;
        }
    }
    
    // Create new neuron for pattern
    if (patternNeurons_.size() >= 100) {  // Limit to prevent explosion
        return INVALID_NEURON_ID;
    }
    
    NeuronId newNeuronId = brain_->getTotalNeuronCount() + patternNeurons_.size();
    patternNeurons_.emplace_back(newNeuronId, pattern);
    
    NLM_LOG_INFO("Created new pattern neuron with ID: " + std::to_string(newNeuronId));
    return newNeuronId;
}

float ActionConsequencePredictor::computePatternSimilarity(const std::vector<float>& a,
                                                          const std::vector<float>& b) const {
    if (a.empty() || b.empty() || a.size() != b.size()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / std::sqrt(normA * normB);
}

} // namespace nlm
