#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for associative recall
    std::vector<NeuronId> patternNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.action.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.action, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& pattern) const {
    std::vector<std::vector<float>> results;
    
    NeuronId patternNeuron = findPatternNeuron(pattern);
    if (patternNeuron == INVALID_NEURON_ID) {
        return results;
    }
    
    for (const auto& assoc : associations_) {
        if (std::get<0>(assoc) == patternNeuron) {
            NeuronId relatedNeuron = std::get<1>(assoc);
            // Find pattern that corresponds to related neuron
            for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                if (pImpl->patternNeurons[i] == relatedNeuron) {
                    results.push_back({});
                    break;
                }
            }
        } else if (std::get<1>(assoc) == patternNeuron) {
            NeuronId relatedNeuron = std::get<0>(assoc);
            // Find pattern that corresponds to related neuron
            for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                if (pImpl->patternNeurons[i] == relatedNeuron) {
                    results.push_back({});
                    break;
                }
            }
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return;
    }
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
    
    // Create new association if not exists
    associations_.emplace_back(neuronA, neuronB, std::clamp(delta, 0.0f, 1.0f));
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& pattern) const {
    std::vector<std::vector<float>> results;
    
    NeuronId patternNeuron = findPatternNeuron(pattern);
    if (patternNeuron == INVALID_NEURON_ID) {
        return results;
    }
    
    // Find associated neurons
    std::vector<NeuronId> relatedNeurons;
    for (const auto& assoc : associations_) {
        if (std::get<0>(assoc) == patternNeuron) {
            relatedNeurons.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == patternNeuron) {
            relatedNeurons.push_back(std::get<0>(assoc));
        }
    }
    
    // Convert neuron IDs back to patterns
    for (NeuronId relatedNeuron : relatedNeurons) {
        for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
            if (pImpl->patternNeurons[i] == relatedNeuron) {
                results.push_back({});
                break;
            }
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) const {
    if (pattern.empty()) {
        return INVALID_NEURON_ID;
    }
    
    // Simple pattern matching - in a real implementation, this would use neural similarity
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        // For now, just return the first neuron as placeholder
        return pImpl->patternNeurons.empty() ? INVALID_NEURON_ID : pImpl->patternNeurons[0];
    }
    
    return INVALID_NEURON_ID;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.empty() || b.empty()) {
        return 0.0f;
    }
    
    // Simple cosine similarity as placeholder
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / std::sqrt(normA * normB);
}

} // namespace nlm

