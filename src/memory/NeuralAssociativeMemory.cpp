#include "../memory/NeuralAssociativeMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Sparse representation of associations
    // Format: (neuron_a, neuron_b, strength, last_updated)
    std::vector<std::tuple<NeuronId, NeuronId, float, float>> associations;
    
    // Maximum number of associations to store (for performance)
    size_t maxAssociations;
    
    // Association decay rate
    float decayRate;
    
    // Current timestamp for tracking association age
    float currentTime;
    
    Impl(size_t maxAssoc, float decay) 
        : maxAssociations(maxAssoc), decayRate(decay), currentTime(0.0f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl(10000, 0.0001f)) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(std::shared_ptr<Brain> brain, float sparsity) {
    // Initialize with brain connection for real neural integration
    setBrain(brain);
    
    // Set sparsity and capacity based on brain size
    size_t maxAssoc = brain ? brain->getTotalNeuronCount() * sparsity * 0.5f : 10000;
    pImpl->maxAssociations = maxAssoc;
    pImpl->decayRate = 0.0001f / sparsity;  // Higher sparsity = faster decay
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with " +
                 std::to_string(maxAssoc) + " max associations");
    
    // Pre-populate with some random associations if brain is available
    if (brain && sparsity > 0.0f) {
        initializeRandomAssociations(*brain, sparsity);
    }
}

void NeuralAssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Remove existing association if it exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            std::get<3>(assoc) = pImpl->currentTime;
            return;
        }
    }
    
    // Add new association if under capacity
    if (pImpl->associations.size() < pImpl->maxAssociations) {
        pImpl->associations.emplace_back(a, b, strength, pImpl->currentTime);
    }
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Decay based on age
            float age = pImpl->currentTime - std::get<3>(assoc);
            float decayedStrength = std::get<2>(assoc) * std::pow(1.0f - pImpl->decayRate, age);
            return decayedStrength;
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            std::get<3>(assoc) = pImpl->currentTime;
            return;
        }
    }
    
    // Create new association if strength is positive
    if (delta > 0.0f) {
        associate(a, b, delta);
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociatedNeurons(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float NeuralAssociativeMemory::getActivationBasedAssociation(NeuronId neuron, float threshold) const {
    float totalWeight = 0.0f;
    size_t count = 0;
    
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            float strength = std::get<2>(assoc);
            if (strength > threshold) {
                totalWeight += strength;
                count++;
            }
        } else if (std::get<1>(assoc) == neuron) {
            float strength = std::get<2>(assoc);
            if (strength > threshold) {
                totalWeight += strength;
                count++;
            }
        }
    }
    
    return count > 0 ? totalWeight / count : 0.0f;
}

void NeuralAssociativeMemory::decayAll(float rate) {
    pImpl->decayRate = rate;
}

void NeuralAssociativeMemory::cleanup() {
    // Remove very weak associations
    pImpl->associations.erase(
        std::remove_if(pImpl->associations.begin(), pImpl->associations.end(),
            [this](const auto& assoc) {
                float age = pImpl->currentTime - std::get<3>(assoc);
                float strength = std::get<2>(assoc) * std::pow(1.0f - pImpl->decayRate, age);
                return strength < 0.001f;  // Remove very weak associations
            }
        ),
        pImpl->associations.end()
    );
}

void NeuralAssociativeMemory::advanceTime(float dt) {
    pImpl->currentTime += dt;
}

void NeuralAssociativeMemory::initializeRandomAssociations(const Brain& brain, float sparsity) {
    // Create initial associations between sensory and motor neurons
    // This simulates biological pre-wiring
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t minNeuronsForAssoc = std::min(size_t(50), totalNeurons / 10);
    
    // Get random generator from brain
    const auto* rng = brain.getRandomGenerator();
    if (!rng) return;
    
    // Create sparse random associations
    size_t attempts = 0;
    size_t targetAssociations = minNeuronsForAssoc;
    
    while (pImpl->associations.size() < targetAssociations && attempts < targetAssociations * 100) {
        // Pick two different neurons
        NeuronId a(rng->uniformInt(0, totalNeurons - 1));
        NeuronId b;
        do {
            b = NeuronId(rng->uniformInt(0, totalNeurons - 1));
        } while (b.value == a.value);
        
        // Check if already associated
        bool exists = false;
        for (const auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            // Create association with random strength
            float strength = rng->uniformReal(0.1f, 0.5f);
            pImpl->associations.emplace_back(a, b, strength, pImpl->currentTime);
        }
        
        attempts++;
    }
    
    NLM_LOG_INFO("Initialized " + std::to_string(pImpl->associations.size()) + 
                 " random associations");
}

} // namespace nlm