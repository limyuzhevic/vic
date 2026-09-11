#include "../src/memory/NeuralAssociativeMemory.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Neural-based associative memory with Hebbian learning
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::unordered_map<NeuronId, std::vector<NeuronId>> forwardAssociations;
    std::unordered_map<NeuronId, std::vector<NeuronId>> backwardAssociations;
    std::unordered_map<NeuronId, float> neuronExcitability;
    float maxStrength;
    float minStrength;
    size_t maxAssociations;
    
    Impl() 
        : maxStrength(1.0f), minStrength(-1.0f), maxAssociations(10000) {
        // Initialize with zero excitability for all neurons
        neuronExcitability.reserve(100000);  // Pre-allocate for performance
    }
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl) {
    // Reserve space for better performance
    pImpl->associations.reserve(pImpl->maxAssociations);
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    // Initialize with brain reference for neural access
    // Currently a placeholder for neural Hebbian learning
}

void NeuralAssociativeMemory::setMaxAssociations(size_t max) {
    pImpl->maxAssociations = max;
    // Trim associations if necessary
    if (pImpl->associations.size() > max) {
        pImpl->associations.resize(max);
    }
}

void NeuralAssociativeMemory::setStrengthLimits(float min, float max) {
    pImpl->minStrength = min;
    pImpl->maxStrength = max;
}

void NeuralAssociativeMemory::createAssociation(NeuronId a, NeuronId b, float strength) {
    if (!validateNeuronId(a) || !validateNeuronId(b)) {
        return;
    }
    
    // Clamp strength to valid range
    strength = std::clamp(strength, pImpl->minStrength, pImpl->maxStrength);
    
    // Remove existing association if present (for re-learning)
    auto it = std::find_if(pImpl->associations.begin(), pImpl->associations.end(),
        [a, b](const auto& assoc) {
            auto& [a_id, b_id, _] = assoc;
            return (a_id == a && b_id == b) || (a_id == b && b_id == a);
        });
    
    if (it != pImpl->associations.end()) {
        pImpl->associations.erase(it);
    }
    
    // Add new association
    pImpl->associations.emplace_back(a, b, strength);
    
    // Update forward and backward maps for efficient lookup
    pImpl->forwardAssociations[a].push_back(b);
    pImpl->backwardAssociations[b].push_back(a);
    
    // Track neuron excitability for later use
    if (pImpl->neuronExcitability.find(a) == pImpl->neuronExcitability.end()) {
        pImpl->neuronExcitability[a] = 0.0f;
    }
    if (pImpl->neuronExcitability.find(b) == pImpl->neuronExcitability.end()) {
        pImpl->neuronExcitability[b] = 0.0f;
    }
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    if (!validateNeuronId(a) || !validateNeuronId(b)) {
        return 0.0f;
    }
    
    for (const auto& assoc : pImpl->associations) {
        auto& [a_id, b_id, strength] = assoc;
        if ((a_id == a && b_id == b) || (a_id == b && b_id == a)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    if (!validateNeuronId(a) || !validateNeuronId(b)) {
        return;
    }
    
    for (auto& assoc : pImpl->associations) {
        auto& [a_id, b_id, strength] = assoc;
        if ((a_id == a && b_id == b) || (a_id == b && b_id == a)) {
            strength = std::clamp(strength + delta, pImpl->minStrength, pImpl->maxStrength);
            
            // Also update excitability based on association strength
            float influence = strength * 0.1f;  // Scale influence
            pImpl->neuronExcitability[a] += influence;
            pImpl->neuronExcitability[b] += influence;
            
            // Clamp excitability
            pImpl->neuronExcitability[a] = std::clamp(pImpl->neuronExcitability[a], -1.0f, 1.0f);
            pImpl->neuronExcitability[b] = std::clamp(pImpl->neuronExcitability[b], -1.0f, 1.0f);
            
            return;
        }
    }
}

void NeuralAssociativeMemory::applyHebbianLearning(const std::vector<NeuronId>& activePattern) {
    if (activePattern.size() < 2) {
        return;
    }
    
    // Apply Hebbian learning: co-activation strengthens associations
    for (size_t i = 0; i < activePattern.size(); ++i) {
        for (size_t j = i + 1; j < activePattern.size(); ++j) {
            NeuronId a = activePattern[i];
            NeuronId b = activePattern[j];
            
            if (!validateNeuronId(a) || !validateNeuronId(b)) {
                continue;
            }
            
            // Incrementally strengthen association
            float currentStrength = getAssociationStrength(a, b);
            float delta = 0.01f * (1.0f - std::abs(currentStrength));  // Saturates at ±1.0
            
            if (delta > 0.0f) {
                updateAssociation(a, b, delta);
            }
        }
    }
}

float NeuralAssociativeMemory::getNeuronExcitability(NeuronId neuron) const {
    if (!validateNeuronId(neuron)) {
        return 0.0f;
    }
    
    auto it = pImpl->neuronExcitability.find(neuron);
    if (it != pImpl->neuronExcitability.end()) {
        return it->second;
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateExcitabilityFromPattern(const std::vector<NeuronId>& pattern, float magnitude) {
    for (NeuronId neuron : pattern) {
        if (validateNeuronId(neuron)) {
            // Update excitability based on pattern activation
            float current = pImpl->neuronExcitability[neuron];
            current += magnitude * 0.1f;  // Scale effect
            pImpl->neuronExcitability[neuron] = std::clamp(current, -1.0f, 1.0f);
        }
    }
}

void NeuralAssociativeMemory::decay(float rate) {
    if (rate <= 0.0f) return;
    
    // Decay association strengths towards zero
    for (auto& assoc : pImpl->associations) {
        auto& [a, b, strength] = assoc;
        strength *= (1.0f - rate);
    }
    
    // Decay neuron excitabilities
    for (auto& [neuron, exc] : pImpl->neuronExcitability) {
        exc *= (1.0f - rate * 0.5f);  // Slower decay
    }
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->forwardAssociations.clear();
    pImpl->backwardAssociations.clear();
    pImpl->neuronExcitability.clear();
}

std::vector<std::tuple<NeuronId, NeuronId, float>> NeuralAssociativeMemory::getAllAssociations() const {
    return pImpl->associations;
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociatedNeurons(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Get from forward associations
    auto it = pImpl->forwardAssociations.find(neuron);
    if (it != pImpl->forwardAssociations.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    
    // Get from backward associations
    it = pImpl->backwardAssociations.find(neuron);
    if (it != pImpl->backwardAssociations.end()) {
        result.insert(result.end(), it->second.begin(), it->second.end());
    }
    
    // Remove duplicates
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

bool NeuralAssociativeMemory::validateNeuronId(NeuronId id) const {
    // Basic validation - should be checked for actual existence in brain
    return id.index() != std::numeric_limits<size_t>::max();
}

} // namespace nlm