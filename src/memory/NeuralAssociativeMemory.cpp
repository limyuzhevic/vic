#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    class Brain* brain;
    
    // Action-outcome associations
    struct ActionOutcome {
        class Action action;
        float outcome;
        float value;
        float lastUpdated;
    };
    std::vector<ActionOutcome> actionOutcomes;
    
    // Pattern associations
    struct PatternAssociation {
        std::vector<NeuronId> pattern1;
        std::vector<NeuronId> pattern2;
        float strength;
        float duration;
        float lastUpdated;
    };
    std::vector<PatternAssociation> patternAssociations;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(class Brain* brain) {
    pImpl->brain = brain;
}

void NeuralAssociativeMemory::update(TimestepDuration dt, const class Action& action, 
                                    float outcome, float value) {
    if (!pImpl->brain) return;
    
    // Update action-outcome association
    for (auto& ao : pImpl->actionOutcomes) {
        if (ao.action.type == action.type) {
            // Update existing association with decay
            ao.outcome = ao.outcome * 0.9f + outcome * 0.1f;
            ao.value = ao.value * 0.9f + value * 0.1f;
            ao.lastUpdated = pImpl->brain->getCurrentTime();
            return;
        }
    }
    
    // Create new association
    ActionOutcome newAo;
    newAo.action = action;
    newAo.outcome = outcome;
    newAo.value = value;
    newAo.lastUpdated = pImpl->brain->getCurrentTime();
    pImpl->actionOutcomes.push_back(newAo);
}

void NeuralAssociativeMemory::learnAssociation(const std::vector<NeuronId>& pattern1,
                                               const std::vector<NeuronId>& pattern2,
                                               float strength, float duration) {
    PatternAssociation assoc;
    assoc.pattern1 = pattern1;
    assoc.pattern2 = pattern2;
    assoc.strength = strength;
    assoc.duration = duration;
    assoc.lastUpdated = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0f;
    pImpl->patternAssociations.push_back(assoc);
}

std::vector<NeuronId> NeuralAssociativeMemory::completePattern(const std::vector<NeuronId>& inputPattern) const {
    if (pImpl->patternAssociations.empty()) return inputPattern;
    
    std::vector<NeuronId> completedPattern = inputPattern;
    float maxStrength = 0.0f;
    
    for (const auto& assoc : pImpl->patternAssociations) {
        // Simple pattern completion based on similarity
        float similarity = 0.0f;
        for (size_t i = 0; i < std::min(assoc.pattern1.size(), inputPattern.size()); ++i) {
            if (assoc.pattern1[i] == inputPattern[i]) {
                similarity += 1.0f;
            }
        }
        
        similarity /= std::max(static_cast<float>(assoc.pattern1.size()), 1.0f);
        
        if (similarity > maxStrength) {
            maxStrength = similarity;
            if (similarity > 0.5f) {  // Threshold for completion
                // Add complementary pattern neurons
                for (NeuronId neuron : assoc.pattern2) {
                    if (std::find(completedPattern.begin(), completedPattern.end(), neuron) == completedPattern.end()) {
                        completedPattern.push_back(neuron);
                    }
                }
            }
        }
    }
    
    return completedPattern;
}

float NeuralAssociativeMemory::getActionValue(const class Action& action) const {
    for (const auto& ao : pImpl->actionOutcomes) {
        if (ao.action.type == action.type) {
            return ao.value;
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::updateActionValue(const class Action& action, float outcome,
                                               float learningRate) {
    for (auto& ao : pImpl->actionOutcomes) {
        if (ao.action.type == action.type) {
            ao.value += learningRate * (outcome - ao.outcome);
            ao.outcome = outcome;
            return;
        }
    }
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<NeuronId>& pattern1,
                                                     const std::vector<NeuronId>& pattern2) const {
    for (const auto& assoc : pImpl->patternAssociations) {
        bool match1 = (assoc.pattern1.size() == pattern1.size());
        bool match2 = (assoc.pattern2.size() == pattern2.size());
        if (match1 && match2) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::clear() {
    pImpl->actionOutcomes.clear();
    pImpl->patternAssociations.clear();
}

const std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>>& 
NeuralAssociativeMemory::getAssociations() const {
    static std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>> empty;
    
    // Convert pattern associations to pair format
    std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>> result;
    for (const auto& assoc : pImpl->patternAssociations) {
        result.push_back({assoc.pattern1, assoc.pattern2});
    }
    return result;
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->patternAssociations.size();
}

float NeuralAssociativeMemory::getAverageAssociationStrength() const {
    if (pImpl->patternAssociations.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (const auto& assoc : pImpl->patternAssociations) {
        totalStrength += assoc.strength;
    }
    return totalStrength / static_cast<float>(pImpl->patternAssociations.size());
}

} // namespace nlm