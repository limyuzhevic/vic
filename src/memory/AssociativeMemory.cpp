#include "AssociativeMemory.hpp"

namespace nlm {

struct AssociativeMemory::Impl {
    std::unordered_map<uint64_t, std::vector<std::pair<NeuronId, float>>> associations;
    
    Impl() {
        // Pre-populate with some basic associations
        // Create some synthetic neuron IDs for testing
        NeuronId sensoryId = NeuronId(1);
        NeuronId motorId = NeuronId(2);
        
        associations[static_cast<uint64_t>(sensoryId.value)].push_back(
            std::make_pair(motorId, 0.8f)
        );
        
        NeuronId internalId = NeuronId(3);
        associations[static_cast<uint64_t>(internalId.value)].push_back(
            std::make_pair(sensoryId, 0.6f)
        );
    }
};

AssociativeMemory::AssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Associate a with b
    associations[static_cast<uint64_t>(a.value)].push_back(
        std::make_pair(b, strength)
    );
    
    // Also associate b with a (symmetric association)
    associations[static_cast<uint64_t>(b.value)].push_back(
        std::make_pair(a, strength)
    );
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    auto it = pImpl->associations.find(static_cast<uint64_t>(neuron.value));
    if (it != pImpl->associations.end()) {
        for (const auto& pair : it->second) {
            result.push_back(pair.first);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    float maxStrength = 0.0f;
    
    // Check a -> b
    auto itA = pImpl->associations.find(static_cast<uint64_t>(a.value));
    if (itA != pImpl->associations.end()) {
        for (const auto& pair : itA->second) {
            if (pair.first == b) {
                maxStrength = std::max(maxStrength, pair.second);
            }
        }
    }
    
    // Check b -> a
    auto itB = pImpl->associations.find(static_cast<uint64_t>(b.value));
    if (itB != pImpl->associations.end()) {
        for (const auto& pair : itB->second) {
            if (pair.first == a) {
                maxStrength = std::max(maxStrength, pair.second);
            }
        }
    }
    
    return maxStrength;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Update a -> b
    auto itA = pImpl->associations.find(static_cast<uint64_t>(a.value));
    if (itA != pImpl->associations.end()) {
        for (auto& pair : itA->second) {
            if (pair.first == b) {
                pair.second += delta;
                pair.second = std::max(0.0f, std::min(1.0f, pair.second));
                break;
            }
        }
    }
    
    // Update b -> a
    auto itB = pImpl->associations.find(static_cast<uint64_t>(b.value));
    if (itB != pImpl->associations.end()) {
        for (auto& pair : itB->second) {
            if (pair.first == a) {
                pair.second += delta;
                pair.second = std::max(0.0f, std::min(1.0f, pair.second));
                break;
            }
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
