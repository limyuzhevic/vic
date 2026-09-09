#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Store associations between neuron patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> patternAssociations;
    
    // Store pattern vectors for fast lookup
    std::vector<NeuronId> patternNeurons;
    std::vector<float> patternActivations;
    
    // Pattern strength and decay
    std::vector<float> patternStrength;
    std::vector<SimulationStep> patternAge;
    
    // Association statistics
    size_t totalAssociations;
    size_t maxAssociations;
    
    Impl() : totalAssociations(0), maxAssociations(1000) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("NeuralAssociativeMemory created");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() {
    NLM_LOG_INFO("NeuralAssociativeMemory destroyed");
}

void NeuralAssociativeMemory::initialize(Brain* brain) {
    // For now, no initialization needed
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associatePattern(const std::vector<NeuronId>& neurons,
                                               const std::vector<float>& activations,
                                               float strength) {
    if (neurons.empty() || neurons.size() != activations.size()) return;
    
    // Check if this pattern already exists
    float existingStrength = getPatternStrength(neurons);
    if (existingStrength > 0.0f) {
        // Update existing pattern
        for (size_t i = 0; i < neurons.size() && i < pImpl->patternNeurons.size(); ++i) {
            if (pImpl->patternNeurons[i] == neurons[0]) {
                pImpl->patternStrength[i] = std::max(pImpl->patternStrength[i], strength);
                pImpl->patternAge[i] = 0;
                for (size_t j = 0; j < neurons.size(); ++j) {
                    if (i < neurons.size()) {
                        pImpl->patternActivations[i * neurons.size() + j] = activations[j];
                    }
                }
                NLM_LOG_INFO("Updated existing pattern association with strength " + std::to_string(strength));
                return;
            }
        }
    }
    
    // Create new pattern
    if (pImpl->patternNeurons.size() < pImpl->maxAssociations * 10) {
        // Store pattern
        pImpl->patternNeurons.push_back(neurons[0]);
        pImpl->patternStrength.push_back(strength);
        pImpl->patternAge.push_back(0);
        
        // Store activation vector
        size_t baseIndex = pImpl->patternNeurons.size() - 1;
        for (float activation : activations) {
            pImpl->patternActivations.push_back(activation);
        }
        
        // Create pairwise associations
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                float assocStrength = strength * std::sqrt(activations[i] * activations[j]);
                pImpl->patternAssociations.emplace_back(neurons[i], neurons[j], assocStrength);
                pImpl->totalAssociations++;
            }
        }
        
        NLM_LOG_INFO("Created new pattern association with " + std::to_string(neurons.size()) + 
                    " neurons and strength " + std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->patternAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::max(std::get<2>(assoc), strength);
            return;
        }
    }
    
    // Create new association
    pImpl->patternAssociations.emplace_back(a, b, strength);
    pImpl->totalAssociations++;
    NLM_LOG_INFO("Created new association between neurons with strength " + std::to_string(strength));
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->patternAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->patternAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociatedNeurons(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->patternAssociations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternAssociations.clear();
    pImpl->patternNeurons.clear();
    pImpl->patternActivations.clear();
    pImpl->patternStrength.clear();
    pImpl->patternAge.clear();
    pImpl->totalAssociations = 0;
    NLM_LOG_INFO("Cleared all associative memory");
}

void NeuralAssociativeMemory::update(TimestepDuration dt) {
    // Age patterns and associations
    for (auto& age : pImpl->patternAge) {
        age++;
    }
    
    // Decay weak associations over time
    std::vector<size_t> toRemove;
    for (size_t i = 0; i < pImpl->patternAssociations.size(); ++i) {
        if (std::get<2>(pImpl->patternAssociations[i]) < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak associations (in reverse order)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        pImpl->totalAssociations--;
        pImpl->patternAssociations.erase(pImpl->patternAssociations.begin() + *it);
    }
}

void NeuralAssociativeMemory::retrievePattern(const std::vector<NeuronId>& pattern,
                                              std::vector<float>& activations,
                                              float& strength) const {
    activations.clear();
    strength = 0.0f;
    
    // Simple pattern matching - find most similar stored pattern
    float maxSimilarity = 0.0f;
    size_t bestMatch = -1;
    
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, i);
        if (similarity > maxSimilarity) {
            maxSimilarity = similarity;
            bestMatch = i;
        }
    }
    
    if (bestMatch != -1 && maxSimilarity > 0.5f) {
        strength = pImpl->patternStrength[bestMatch];
        
        // Extract activation vector for matched pattern
        size_t numNeuronsInPattern = pattern.size();
        size_t baseIndex = bestMatch * numNeuronsInPattern;
        
        for (size_t j = 0; j < numNeuronsInPattern && baseIndex + j < pImpl->patternActivations.size(); ++j) {
            activations.push_back(pImpl->patternActivations[baseIndex + j]);
        }
    }
}

float NeuralAssociativeMemory::getPatternStrength(const std::vector<NeuronId>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        if (pImpl->patternNeurons[i] == pattern[0]) {
            return pImpl->patternStrength[i];
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::reinforcePattern(const std::vector<NeuronId>& pattern, float factor) {
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        if (pImpl->patternNeurons[i] == pattern[0]) {
            pImpl->patternStrength[i] = std::min(1.0f, pImpl->patternStrength[i] * factor);
            pImpl->patternAge[i] = 0;
            break;
        }
    }
}

float NeuralAssociativeMemory::computePatternSimilarity(const std::vector<NeuronId>& pattern, size_t patternIndex) const {
    if (patternIndex >= pImpl->patternNeurons.size()) return 0.0f;
    
    // Simple cosine similarity between patterns
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    size_t numNeuronsInPattern = pattern.size();
    size_t baseIndex = patternIndex * numNeuronsInPattern;
    
    for (size_t i = 0; i < numNeuronsInPattern && i < pattern.size() && baseIndex + i < pImpl->patternActivations.size(); ++i) {
        float valA = (i < pattern.size()) ? 1.0f : 0.0f;  // Pattern is binary for simplicity
        float valB = pImpl->patternActivations[baseIndex + i];
        
        dotProduct += valA * valB;
        normA += valA * valA;
        normB += valB * valB;
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

} // namespace nlm
