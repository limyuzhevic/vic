// Associative Memory - Simple implementation
// This delegates to NeuralAssociativeMemory for distributed representations

#include "Memory.hpp"
#include "NeuralAssociativeMemory.hpp"
#include <algorithm>

namespace nlm {

struct AssociativeMemory::Impl {
    NeuralAssociativeMemory neuralAssociativeMemory;
    
    Impl() {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Convert neuron IDs to patterns for neural associative memory
    std::vector<float> patternA{static_cast<float>(a.getId()) / 1000.0f};
    std::vector<float> patternB{static_cast<float>(b.getId()) / 1000.0f};
    
    pImpl->neuralAssociativeMemory.associate(patternA, patternB, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    // Convert neuron ID to pattern
    std::vector<float> pattern{static_cast<float>(neuron.getId()) / 1000.0f};
    
    // Retrieve patterns from neural associative memory
    std::vector<std::vector<float>> associatedPatterns = 
        pImpl->neuralAssociativeMemory.retrieve(pattern, 10);
    
    std::vector<NeuronId> result;
    for (const auto& pattern : associatedPatterns) {
        if (!pattern.empty()) {
            // Convert pattern back to NeuronId
            NeuronId assoc(static_cast<size_t>(pattern[0] * 1000));
            result.push_back(assoc);
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    std::vector<float> patternA{static_cast<float>(a.getId()) / 1000.0f};
    std::vector<float> patternB{static_cast<float>(b.getId()) / 1000.0f};
    
    return pImpl->neuralAssociativeMemory.getAssociationStrength(patternA, patternB);
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    std::vector<float> patternA{static_cast<float>(a.getId()) / 1000.0f};
    std::vector<float> patternB{static_cast<float>(b.getId()) / 1000.0f};
    
    pImpl->neuralAssociativeMemory.updateAssociation(patternA, patternB, delta);
}

void AssociativeMemory::clear() {
    pImpl->neuralAssociativeMemory.clear();
}

} // namespace nlm