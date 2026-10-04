#include "AssociativeMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <functional>

namespace nlm {

struct AssociativeMemory::Impl {
    // Hebbian-like associative memory with synaptic weights
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float learningRate;
    float decayRate;
    float maxAssociationStrength;
    float minAssociationStrength;
    
    Impl() : learningRate(0.1f), decayRate(0.01f), 
             maxAssociationStrength(1.0f), minAssociationStrength(-1.0f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Real associative memory implementation with Hebbian learning
    
    // Check if association already exists (bidirectional)
    bool exists = false;
    size_t index = 0;
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        NeuronId x = std::get<0>(assoc);
        NeuronId y = std::get<1>(assoc);
        
        if ((x == a && y == b) || (x == b && y == a)) {
            exists = true;
            index = i;
            break;
        }
    }
    
    if (exists) {
        // Update existing association with new learning
        auto& assoc = pImpl->associations[index];
        float& oldStrength = std::get<2>(assoc);
        
        // Hebbian-like update: strengthen if co-activation detected
        float delta = pImpl->learningRate * (strength - oldStrength);
        oldStrength += delta;
        
        // Apply bounds
        oldStrength = std::clamp(oldStrength, pImpl->minAssociationStrength, 
                                pImpl->maxAssociationStrength);
    } else {
        // Create new association
        pImpl->associations.emplace_back(a, b, strength);
        
        // Apply synaptic strengthening rules
        strengthenAssociatedAssociations(a, b, strength);
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if (a == neuron) {
            result.push_back(b);
        } else if (b == neuron) {
            result.push_back(a);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        NeuronId x = std::get<0>(assoc);
        NeuronId y = std::get<1>(assoc);
        
        if ((x == a && y == b) || (x == b && y == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        NeuronId x = std::get<0>(assoc);
        NeuronId y = std::get<1>(assoc);
        
        if ((x == a && y == b) || (x == b && y == a)) {
            float& strength = std::get<2>(assoc);
            strength += delta;
            
            // Apply synaptic bounds
            strength = std::clamp(strength, pImpl->minAssociationStrength, 
                                 pImpl->maxAssociationStrength);
            break;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

void AssociativeMemory::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible associations
    pImpl->associations.clear();
    
    // Create realistic neural associations based on biology
    // Motor-sensory associations
    size_t numAssociations = rng.uniformInt(50, 200);
    
    for (size_t i = 0; i < numAssociations; ++i) {
        // Generate biologically realistic neuron IDs
        NeuronId a = static_cast<NeuronId>(rng.uniformInt(1, 5000));
        NeuronId b = static_cast<NeuronId>(rng.uniformInt(1, 5000));
        
        // Different types of associations
        float strength;
        int assocType = rng.uniformInt(0, 3);
        
        switch (assocType) {
            case 0: // Excitatory association
                strength = rng.uniformReal(0.2f, 0.8f);
                break;
            case 1: // Inhibitory association  
                strength = rng.uniformReal(-0.8f, -0.2f);
                break;
            case 2: // Neutral/weak association
                strength = rng.uniformReal(-0.1f, 0.1f);
                break;
            default: // Strong association
                strength = rng.uniformReal(0.7f, 1.0f);
                break;
        }
        
        associate(a, b, strength);
    }
}

void AssociativeMemory::strengthenAssociatedAssociations(NeuronId a, NeuronId b, float newStrength) {
    // Hebbian-like strengthening of related associations
    // When A and B form a strong association, other associations involving A or B get strengthened
    
    for (auto& assoc : pImpl->associations) {
        NeuronId x = std::get<0>(assoc);
        NeuronId y = std::get<1>(assoc);
        float& strength = std::get<2>(assoc);
        
        // Strengthen associations that share either neuron
        if ((x == a || y == a) || (x == b || y == b)) {
            // Don't strengthen the association we just created
            if (!((x == a && y == b) || (x == b && y == a))) {
                // Apply proportional strengthening
                float enhancement = pImpl->learningRate * std::abs(newStrength) * 0.5f;
                strength = std::clamp(strength + enhancement, 
                                    pImpl->minAssociationStrength, 
                                    pImpl->maxAssociationStrength);
            }
        }
    }
}

void AssociativeMemory::applySynapticDecay() {
    // Apply synaptic decay to maintain stability
    for (auto& assoc : pImpl->associations) {
        float& strength = std::get<2>(assoc);
        strength *= (1.0f - pImpl->decayRate);
        
        // Remove very weak associations
        if (std::abs(strength) < 0.01f) {
            strength = 0.0f;
        }
    }
}

} // namespace nlm
