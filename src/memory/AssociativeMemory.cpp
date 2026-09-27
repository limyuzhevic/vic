#include "AssociativeMemory.hpp"
#include <algorithm>

namespace nlm {

struct AssociativeMemory::Impl {
    struct Association {
        NeuronId a;
        NeuronId b;
        float strength;
        
        Association(NeuronId a, NeuronId b, float strength) 
            : a(a), b(b), strength(strength) {}
    };
    
    std::vector<Association> associations;
    std::unordered_map<NeuronId, std::vector<NeuronId>> forwardMap;
    std::unordered_map<NeuronId, std::vector<NeuronId>> backwardMap;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl()) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Remove existing associations for this pair
    auto& forward = pImpl->forwardMap;
    auto& backward = pImpl->backwardMap;
    
    // Remove existing a->b association
    if (auto it = forward.find(a); it != forward.end()) {
        auto& list = it->second;
        list.erase(std::remove(list.begin(), list.end(), b), list.end());
    }
    
    // Remove existing b->a association
    if (auto it = backward.find(b); it != backward.end()) {
        auto& list = it->second;
        list.erase(std::remove(list.begin(), list.end(), a), list.end());
    }
    
    // Add new association
    pImpl->associations.emplace_back(a, b, strength);
    forward[a].push_back(b);
    backward[b].push_back(a);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Get forward associations (neuron -> others)
    if (auto it = pImpl->forwardMap.find(neuron); it != pImpl->forwardMap.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    
    // Get backward associations (others -> neuron)
    if (auto it = pImpl->backwardMap.find(neuron); it != pImpl->backwardMap.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    // Check forward association
    if (auto it = pImpl->forwardMap.find(a); it != pImpl->forwardMap.end()) {
        auto& list = it->second;
        auto it2 = std::find(list.begin(), list.end(), b);
        if (it2 != list.end()) {
            // Find the corresponding association to get strength
            for (const auto& assoc : pImpl->associations) {
                if (assoc.a == a && assoc.b == b) {
                    return assoc.strength;
                }
            }
        }
    }
    
    // Check backward association
    if (auto it = pImpl->backwardMap.find(b); it != pImpl->backwardMap.end()) {
        auto& list = it->second;
        auto it2 = std::find(list.begin(), list.end(), a);
        if (it2 != list.end()) {
            // Find the corresponding association to get strength
            for (const auto& assoc : pImpl->associations) {
                if (assoc.a == a && assoc.b == b) {
                    return assoc.strength;
                }
            }
        }
    }
    
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.a == a && assoc.b == b) || (assoc.a == b && assoc.b == a)) {
            assoc.strength += delta;
            break;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->forwardMap.clear();
    pImpl->backwardMap.clear();
}

} // namespace nlm
