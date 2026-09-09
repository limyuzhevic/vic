#include "AssociativeMemory.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AssociativeMemory::Impl {
    std::vector<std::pair<NeuronId, std::vector<std::pair<NeuronId, float>>>> associations;
    std::unordered_map<NeuronId, std::vector<NeuronId>> forwardIndex;
    std::unordered_map<std::pair<NeuronId, NeuronId>, float> strengthMap;
    
    Impl() : associations(), forwardIndex(), strengthMap() {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl()) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Remove existing association if present
    for (auto it = pImpl->associations.begin(); it != pImpl->associations.end(); ++it) {
        if (it->first == a && std::find_if(it->second.begin(), it->second.end(),
                                          [b](const auto& p) { return p.first == b; }) != it->second.end()) {
            // Remove from forward index
            pImpl->forwardIndex[a].erase(
                std::remove(pImpl->forwardIndex[a].begin(), pImpl->forwardIndex[a].end(), b),
                pImpl->forwardIndex[a].end()
            );
            if (pImpl->forwardIndex[a].empty()) {
                pImpl->forwardIndex.erase(a);
            }
            
            // Remove from strength map
            pImpl->strengthMap.erase(std::make_pair(a, b));
            
            // Remove from associations list
            it->second.erase(
                std::remove_if(it->second.begin(), it->second.end(),
                             [b](const auto& p) { return p.first == b; }),
                it->second.end()
            );
            if (it->second.empty()) {
                pImpl->associations.erase(it);
            }
            break;
        }
    }
    
    // Add new association
    pImpl->associations.push_back({a, std::vector<std::pair<NeuronId, float>>()});
    auto& association = pImpl->associations.back();
    association.second.push_back({b, strength});
    
    // Update forward index
    pImpl->forwardIndex[a].push_back(b);
    
    // Update strength map
    pImpl->strengthMap[std::make_pair(a, b)] = strength;
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    auto it = pImpl->forwardIndex.find(neuron);
    if (it == pImpl->forwardIndex.end()) {
        return {};
    }
    
    return it->second;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    auto it = pImpl->strengthMap.find(std::make_pair(a, b));
    if (it == pImpl->strengthMap.end()) {
        return 0.0f;
    }
    return it->second;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    auto key = std::make_pair(a, b);
    auto it = pImpl->strengthMap.find(key);
    if (it != pImpl->strengthMap.end()) {
        it->second += delta;
        
        // Clamp to reasonable range
        if (it->second < 0.0f) it->second = 0.0f;
        if (it->second > 1.0f) it->second = 1.0f;
        
        // Update in associations list
        for (auto& assoc : pImpl->associations) {
            if (assoc.first == a) {
                for (auto& pair : assoc.second) {
                    if (pair.first == b) {
                        pair.second = it->second;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->forwardIndex.clear();
    pImpl->strengthMap.clear();
}

} // namespace nlm
