#include "AssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

struct AssociativeMemory::Impl {
    // Direct neuron associations with strengths
    std::unordered_map<NeuronId, std::unordered_map<NeuronId, float>> associations;
    // Bidirectional mapping for quick lookup
    std::unordered_map<NeuronId, std::unordered_set<NeuronId>> forwardMap;
    std::unordered_map<NeuronId, std::unordered_set<NeuronId>> backwardMap;
    // Association history for learning tracking
    std::vector<std::pair<NeuronId, NeuronId>> associationHistory;
    // Similarity-based association discovery
    std::unordered_map<size_t, std::vector<NeuronId>> similarityClusters;
    
    Impl() : nextId_(1000) {}
};

AssociativeMemory::AssociativeMemory()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("AssociativeMemory initialized");
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    if (strength <= 0.0f) {
        // Remove weak associations
        removeAssociation(a, b);
        return;
    }
    
    // Create/ensure direct association
    if (!pImpl->associations[a].count(b)) {
        pImpl->associationHistory.push_back({a, b});
        if (pImpl->associationHistory.size() > 10000) {
            pImpl->associationHistory.erase(pImpl->associationHistory.begin());
        }
    }
    
    pImpl->associations[a][b] = strength;
    pImpl->forwardMap[a].insert(b);
    pImpl->backwardMap[b].insert(a);
    
    // Bidirectional association strength (symmetrical)
    if (a != b) {
        pImpl->associations[b][a] = strength;
        pImpl->forwardMap[b].insert(a);
        pImpl->backwardMap[a].insert(b);
    }
    
    NLM_LOG_INFO("AssociativeMemory: Associated neuron " + std::to_string(a) + 
                 " with " + std::to_string(b) + " (strength: " + std::to_string(strength) + ")");
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Add forward associations
    auto forwardIt = pImpl->forwardMap.find(neuron);
    if (forwardIt != pImpl->forwardMap.end()) {
        result.insert(result.end(), forwardIt->second.begin(), forwardIt->second.end());
    }
    
    // Add backward associations (excluding self)
    auto backwardIt = pImpl->backwardMap.find(neuron);
    if (backwardIt != pImpl->backwardMap.end()) {
        for (NeuronId associated : backwardIt->second) {
            if (associated != neuron) {
                result.push_back(associated);
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    auto forwardIt = pImpl->associations.find(a);
    if (forwardIt != pImpl->associations.end()) {
        auto bIt = forwardIt->second.find(b);
        if (bIt != forwardIt->second.end()) {
            return bIt->second;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    float strength = getAssociationStrength(a, b);
    float newStrength = strength + delta;
    
    if (newStrength > 0.0f) {
        associate(a, b, newStrength);
    } else {
        removeAssociation(a, b);
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->forwardMap.clear();
    pImpl->backwardMap.clear();
    pImpl->associationHistory.clear();
    pImpl->similarityClusters.clear();
    NLM_LOG_INFO("AssociativeMemory: Cleared all associations");
}

void AssociativeMemory::removeAssociation(NeuronId a, NeuronId b) {
    auto forwardIt = pImpl->forwardMap.find(a);
    if (forwardIt != pImpl->forwardMap.end()) {
        forwardIt->second.erase(b);
        if (forwardIt->second.empty()) {
            pImpl->forwardMap.erase(a);
        }
    }
    
    auto backwardIt = pImpl->backwardMap.find(b);
    if (backwardIt != pImpl->backwardMap.end()) {
        backwardIt->second.erase(a);
        if (backwardIt->second.empty()) {
            pImpl->backwardMap.erase(b);
        }
    }
    
    auto aIt = pImpl->associations.find(a);
    if (aIt != pImpl->associations.end()) {
        aIt->second.erase(b);
        if (aIt->second.empty()) {
            pImpl->associations.erase(a);
        }
    }
}

} // namespace nlm